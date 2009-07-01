#ifndef _CORE_H_
#define _CORE_H_

#include <kernel.h>
#include <stdio.h>
#include <iopheap.h>
#include <sifrpc.h>
#include <string.h>
#include <sbv_patches.h>
#include <syscallnr.h>
#include <libcdvd.h>
#include <debug.h>

#include "iopcontrol.h"
#include "loadfile.h"

#define SERVER_IP		"192.168.0.2"
#define SERVER_PORT 	4234

void *memdisk_irx;
int  size_memdisk_irx;
void *eesync_irx;
int  size_eesync_irx;
void *ntpbclient_irx;
int  size_ntpbclient_irx;
void *ps2dev9_irx;
int  size_ps2dev9_irx;
void *ps2ip_irx;
int  size_ps2ip_irx;
void *ps2smap_irx;
int  size_ps2smap_irx;


#define GS_BGCOLOUR *((volatile unsigned long int*)0x120000E0)
#define MAX_PATH     0x100

typedef struct _romdir {
	char           fileName[10];
	unsigned short extinfo_size;
	int            fileSize;
} romdir_t;

typedef struct _ioprp {
	void        *data_in;
	void        *data_out;
	unsigned int size_in;
	unsigned int size_out;
} ioprp_t;

#define ELF_MAGIC	0x464c457f
#define ELF_PT_LOAD	1

typedef struct {
	u8	ident[16];
	u16	type;
	u16	machine;
	u32	version;
	u32	entry;
	u32	phoff;
	u32	shoff;
	u32	flags;
	u16	ehsize;
	u16	phentsize;
	u16	phnum;
	u16	shentsize;
	u16	shnum;
	u16	shstrndx;
} elf_header_t;

typedef struct {
	u32	type;
	u32	offset;
	void	*vaddr;
	u32	paddr;
	u32	filesz;
	u32	memsz;
	u32	flags;
	u32	align;
} elf_pheader_t;

typedef struct {
	u32 epc;
	u32 gp;
	u32 sp;
	u32 pad;  
} t_ExecData;

extern int set_reg_hook;

int HookIopReset(const char *arg, int flag);
int PatchIOPRPimg(ioprp_t *ioprp_img);

u32 (*Old_SifSetDma)(SifDmaTransfer_t *sdd, s32 len);
int (*Old_SifSetReg)(u32 register_num, int register_value);

// loadfile.c
int LoadFileInit();
void LoadFileExit();
int LoadModuleAsync(const char *path, int arg_len, const char *args);
int LoadModuleBuffer(void *modptr, unsigned int modsize, int arg_len, const char *args);
int LoadElf(const char *path, t_ExecData *data);

// iopcontrol.c
int IopReset(const char *arg, int flag);
int IopSync(void);

// rpc.c
int rpcNTPBinit(void);
int rpcNTPBreset(void);
int rpcNTPBconnect(const char *ServerIP, int ServerPort);
int rpcNTPBsendCmd(u16 cmd, u8 *buf, int size);
int rpcNTPBdisconnect(void);
int rpcSync(int mode, int *cmd, int *result);

#define NTPBCMD_GET_EEDUMP_START 			0x101
#define NTPBCMD_GET_IOPDUMP_START			0x102
#define NTPBCMD_GET_KERNELDUMP_START		0x103
#define NTPBCMD_GET_SCRATCHPADDUMP_START	0x104
#define NTPBCMD_GET_EEDUMP_END 				0x201
#define NTPBCMD_GET_IOPDUMP_END				0x202
#define NTPBCMD_GET_KERNELDUMP_END			0x203
#define NTPBCMD_GET_SCRATCHPADDUMP_END		0x204
#define NTPBCMD_PRINT_EEDUMP 				0x301
#define NTPBCMD_PRINT_IOPDUMP				0x302
#define NTPBCMD_PRINT_KERNELDUMP 			0x303
#define NTPBCMD_PRINT_SCRATCHPADDUMP		0x304
#define NTPBCMD_GET_REMOTE_DUMPREQUEST		0x400

#define REMOTE_DUMPREQUEST_NONE			0
#define REMOTE_DUMPREQUEST_EE			1
#define REMOTE_DUMPREQUEST_IOP			2
#define REMOTE_DUMPREQUEST_KERNEL		3
#define REMOTE_DUMPREQUEST_SCRATCHPAD	4

#define GET_DUMP_START					0x100
#define GET_DUMP_END					0x200
#define PRINT_DUMP						0x300

#define EE_DUMP							1
#define IOP_DUMP						2
#define KERNEL_DUMP						3
#define SCRATCHPAD_DUMP					4

#endif
