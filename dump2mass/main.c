/*
 * main.c - Dump PS2 memory to USB flash drive
 *
 * Solution for Project Artemis' BountySource task #2267:
 * https://artemis.bountysource.com/task/show/2267
 *
 * The solution's page can be found here:
 * https://artemis.bountysource.com/solution/show/1074
 *
 * Copyright (C) 2008-2009 misfire <misfire@xploderfreax.de>
 *
 * Licensed under the Academic Free License version 2.0.  See file LICENSE.
 */

#include <tamtypes.h>
#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <loadfile.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <sifrpc.h>
#include <sbv_patches.h>
#include <debug.h>

#ifdef _DEBUG
	#define dbgprintf(args...)	printf(args)
#else
	#define dbgprintf(args...)	do {} while (0)
#endif

#define APP_NAME	"dump2mass"
#define APP_VERSION	"0.2"
#define APP_BUILD_DATE	__DATE__" "__TIME__

#define WELCOME_STRING	"Welcome to "APP_NAME" "APP_VERSION"\n\n"

extern u32 usbd_irx;
extern u32 size_usbd_irx;
extern u32 usb_mass_irx;
extern u32 size_usb_mass_irx;

#ifdef _IOPRESET
/**
 * reset_iop - Resets the IOP.
 * @img: filename of IOP replacement image
 */
static void reset_iop(const char *img)
{
	dbgprintf("%s: IOP says goodbye...\n", __FUNCTION__);

	SifInitRpc(0);

	/* Exit RPC services */
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();

	SifIopReset(img, 0);

	while (!SifIopSync())
		;
	SifInitRpc(0);
}
#endif /* _IOPRESET */

/**
 * kmem_read - Reads data from kernel memory.
 * @addr: memory address to read from
 * @buf: buffer to read data into
 * @size: size of data to read
 * @return: number of bytes read
 */
static u32 kmem_read(void *addr, void *buf, u32 size)
{
	DI();
	ee_kmode_enter();
	memcpy(buf, addr, size);
	ee_kmode_exit();
	EI();

	return size;
}

/**
 * memregion_t - describes a memory region
 * @name: short name
 * @desc: description
 * @addr: start address
 * @size: size of region in bytes
 */
typedef struct {
	char	*name;
	char	*desc;
	u32	addr;
	u32	size;
} memregion_t;

/* Supported memory regions */
static const memregion_t g_memregions[] = {
	{
		.name = "ram",
		.desc = "RAM (cached) (31 MiB)",
		.addr = 0x00100000,
		.size = 0x01F00000,
	},
	{
		.name = "ram2",
		.desc = "RAM (uncached) (31 MiB)",
		.addr = 0x20100000,
		.size = 0x01F00000,
	},
	{
		.name = "ram3",
		.desc = "RAM (uncached & accelerated) (31 MiB)",
		.addr = 0x30100000,
		.size = 0x01F00000,
	},
	{
		.name = "kseg0",
		.desc = "KSEG0 (32 MiB)",
		.addr = 0x80000000,
		.size = 0x02000000,
	},
	{
		.name = "kseg1",
		.desc = "KSEG1 (32 MiB)",
		.addr = 0xA0000000,
		.size = 0x02000000,
	},
	{
		.name = "iop",
		.desc = "IOP RAM (2 MiB)",
		.addr = 0xBC000000,
		.size = 0x00200000,
	},
	{
		.name = "scratch",
		.desc = "Scratch Pad (16 KiB)",
		.addr = 0x70000000,
		.size = 0x00004000,
	},
	{
		.name = "rom0",
		.desc = "ROM0 (uncached) (4 MiB)",
		.addr = 0x1FC00000,
		.size = 0x00400000,
	},
	{
		.name = "rom09",
		.desc = "ROM0 (cached) (4 MiB)",
		.addr = 0x9FC00000,
		.size = 0x00400000,
	},
	{
		.name = "rom0b",
		.desc = "ROM0 (uncached) (4 MiB)",
		.addr = 0xBFC00000,
		.size = 0x00400000,
	},
	{
		.name = "rom1",
		.desc = "ROM1 (256 KiB)",
		.addr = 0xBE000000,
		.size = 0x00040000,
	},
	{
		.name = "rom2",
		.desc = "ROM2 (256 KiB)",
		.addr = 0xBE400000,
		.size = 0x00040000,
	},
	/* terminator */
	{ NULL, NULL, 0, 0 }
};

/**
 * get_memregion - Gets a memory region by its name.
 * @name: name to search for (see g_memregions)
 * @return: ptr to memregion, or NULL if it could not be found
 */
static const memregion_t *get_memregion(const char *name)
{
	if (name != NULL) {
		const memregion_t *p = &g_memregions[0];

		while (p->name != NULL) {
			if (!strcmp(p->name, name))
				return p;
			p++;
		}
	}

	return NULL;
}

/* Device to dump to */
#define DUMPDEV		"mass:"

#define DUMPBUFSIZE	(128 * 1024)

/* Global buffer for dumping */
static u8 g_buf[DUMPBUFSIZE] __attribute__((aligned(16)));

/**
 * get_dumpfile - Gets the name of the file to dump memory to.
 * @name: memory region name (see g_memregions)
 * @return: filename
 */
static char *get_dumpfile(const char *name)
{
	static char dumpfile[256];

	if (name == NULL)
		return NULL;

	strcpy(dumpfile, DUMPDEV);
	strcat(dumpfile, name);
	strcat(dumpfile, ".bin");

	return dumpfile;
}

/**
 * dump_callback - Sample callback function for dump_to_file().
 * @done: number of bytes already dumped
 * @total: total number of bytes
 */
static void dump_callback(u32 done, u32 total)
{
	/* Whatever... */
	dbgprintf("%u/%u\n", done, total);

	if (!done)
		scr_printf("Progress: ");
	else if (done == total)
		scr_printf("\nDone.\n");
	else
		scr_printf(".");
}

/**
 * dump_to_file - Dumps data from memory to file.
 * @fd: file descriptor to write data to
 * @addr: memory address to start dumping from
 * @size: size of data to dump
 * @callback: callback function
 * @return: 0: success, <0: error
 */
static int dump_to_file(int fd, void *addr, u32 size, void(*callback)(u32, u32))
{
	const u32 total = size;

	if (callback != NULL)
		callback(0, total);

	while (size > 0) {
		u32 bytes_read = (size > DUMPBUFSIZE) ? DUMPBUFSIZE : size;
		kmem_read(addr, g_buf, bytes_read);

		if (write(fd, g_buf, bytes_read) < bytes_read)
			return -1;

		addr += bytes_read;
		size -= bytes_read;

		if (callback != NULL)
			callback(total - size, total);
	}

	return 0;
}

/* Default args, used if no region name is provided */
static int def_argc = 2;
static char *def_argv[] = {
	NULL, /* ignored */
	"ram"
};

int main(int argc, char *argv[])
{
	int ret, i;

#ifdef _IOPRESET
	reset_iop("rom0:UDNL rom0:EELOADCNF");
#endif
	init_scr();
	scr_printf(WELCOME_STRING);
	dbgprintf(WELCOME_STRING);
	dbgprintf("Build date: "APP_BUILD_DATE"\n");

	if (argc < 2) {
		argc = def_argc;
		argv = def_argv;
	}

	dbgprintf("Applying SBV patches...\n");
	sbv_patch_enable_lmb();
//	sbv_patch_disable_prefix_check();

	SifLoadModule("rom0:SIO2MAN", 0, NULL);
//	SifLoadModule("rom0:MCMAN", 0, NULL);
//	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, &ret);
	SifExecModuleBuffer(&usb_mass_irx, size_usb_mass_irx, 0, NULL, &ret);

	i = 1;
	while (i < argc) {
		/* Clear screen every once in a while */
		if (i % 4 == 0) {
			scr_clear();
			scr_printf(WELCOME_STRING);
		}

		char *regname = argv[i++];
		if (regname == NULL)
			continue;

		const memregion_t *reg = get_memregion(regname);
		if (reg == NULL) {
			scr_printf("Error: unknown region name %s\n", regname);
			continue;
		}

		char *dumpfile = get_dumpfile(reg->name);
		scr_printf("[%i] Dumping %s to file %s\n", i-1, reg->desc, dumpfile);

		int fd = open(dumpfile, O_CREAT | O_WRONLY);
		if (fd < 0) {
			scr_printf("Error: can't create file %s\n", dumpfile);
			continue;
		}

		ret = dump_to_file(fd, (void*)reg->addr, reg->size, dump_callback);
		if (ret < 0) {
			scr_printf("Error: memory dump failed\n");
		}

		close(fd);
	}

	scr_printf("Exit.\n");
	dbgprintf("Exit.\n");
	SleepThread();
	return 0;
}
