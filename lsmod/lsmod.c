/*
 * lsmod.c - Shows what IOP modules and libraries are currently loaded.
 *
 * Copyright (C) 2007-2008 misfire <misfire@xploderfreax.de>
 *
 * Licensed under the Academic Free License version 2.0.  See file LICENSE.
 */

#include <tamtypes.h>
#include "irx_imports.h"
#include <iopmgr.h>	/* for smod_mod_info_t */
#include "xprintf.h"	/* for sprintf() */

#define MODNAME	"lsmod"
#define VER_MAJ	1
#define VER_MIN	1

IRX_ID(MODNAME, VER_MAJ, VER_MIN);

#define M_PRINTF(format, args...) \
	printf(MODNAME ": " format, ## args)

#define MODLIST_FILE	"mc0:modlist" /* default output file */

static u8 g_buf[1024] __attribute__((aligned(16))); /* global buffer */

/* Writes formatted string to stdout and fd. */
#define A_PRINTF(fd, format, args...) \
	sprintf(g_buf, format, ## args); \
	printf("%s", g_buf); \
	if (fd > 0) write(fd, g_buf, strlen(g_buf))

/*
 * list_modules - Lists all loaded IOP modules.
 */
static int list_modules(int fd)
{
	smod_mod_info_t *info = (smod_mod_info_t*)0x800; /* head of module list */
	int count = 0;

	A_PRINTF(fd, "ID %-25s VERS FLGS NFLG ENTRY    GP      TEXTSTART TEXTSIZE " \
		"DATASIZE BSS_SIZE\n", "MODULE_NAME");

	do {
		A_PRINTF(fd, "%2i %-25s %04X %04X %04X %08X %08X %08X %08X %08X %08X\n",
			info->id, info->name, info->version, info->flags, info->newflags,
			(unsigned int)info->entry, (unsigned int)info->gp,
			(unsigned int)info->text_start, (unsigned int)info->text_size,
			(unsigned int)info->data_size, (unsigned int)info->bss_size);

		info = info->next;
		count++;
	} while (info != NULL);

	A_PRINTF(fd, "Total: %i modules loaded\n", count);
	return count;
}

/*
 * list_libs - Lists all loaded IOP libraries.
 */
static int list_libs(int fd)
{
	iop_library_table_t *libtable;
	iop_library_t *libptr;
	struct irx_import_table *imptab;
	unsigned int *exptab;
	int count = 0;
	char name[9];

	A_PRINTF(fd, "LIB_NAME VERS FLGS ENTRY    IMPTABLE EXPTABLE\n");

	libtable = GetLibraryEntryTable();
	libptr = libtable->tail;
	name[8] = '\0';

	while (libptr != NULL) {
		imptab = libptr->caller;
		exptab = (unsigned int*)libptr->exports;
		memcpy(name, libptr->name, 8);
		A_PRINTF(fd, "%-8s %04X %04X %08X %08X %08X\n", name, libptr->version, libptr->flags,
			(unsigned int)libptr, (unsigned int)imptab, (unsigned int)exptab);

		libptr = libptr->prev;
		count++;
	}

	A_PRINTF(fd, "Total: %i libs loaded\n", count);
	return count;
}

int _start(int argc, char *argv[])
{
	const char *filename;
	int fd;

	SifInitRpc(0);

	M_PRINTF("Version %d.%d by misfire\n", VER_MAJ, VER_MIN);

	filename = (argc > 1) ? argv[1] : MODLIST_FILE;
	fd = open(filename, O_CREAT | O_WRONLY);

	M_PRINTF("filename = %s (fd = %i)\n", filename, fd);

	M_PRINTF("Listing modules...\n");
	list_modules(fd);
	M_PRINTF("Listing libraries...\n");
	list_libs(fd);

	if (fd > 0)
		close(fd);

	M_PRINTF("Done.\n");

	return MODULE_NO_RESIDENT_END;
}
