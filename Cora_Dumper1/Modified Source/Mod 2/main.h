#include <tamtypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <loadfile.h>
#include <fileio.h>
#include <debug.h>
#include "libpad.h"
#include "malloc.h"
#include "libcdvd.h"
#include "gsKit.h"
#include "dmaKit.h"

#define _RESIDENT_	__attribute__((section(".text")))

#define ERROR_HAX0R				-4
#define ERROR_CDVDFSV_INIT		-5
#define ERROR_SYSTEMCNF_PARSE	-6
#define ERROR_SYSTEMCNF_FILEIO	-7
#define ERROR_SYSTEMCNF_MEMORY	-8

//PAD VARIABLES
	//check for multiple definitions
	#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
	#define ROM_PADMAN
	#endif

	#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
	#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
	#endif

	#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
	#error ROM_PADMAN or NEW_PADMAN must be defined!
	#endif
	//pad buffer
	static char padBuf[256] __attribute__((aligned(64)));
	//rumblers
	static char actAlign[6];
	static int actuators;
	//button status
	struct padButtonStatus buttons;
		u32 paddata;
		u32 old_pad;
		u32 new_pad;
	int port, slot;

//CD / DVD VARIABLES
	//file name to boot elf from
	char *bootFileName;

//CHEAT VARIABLES
	//HookSyscall vars
	_RESIDENT_ u32 vector;
	//system call table address
	_RESIDENT_ static u32 *p_vtbl;// = 0x800144c0;
	//jal to cheat writer
	_RESIDENT_ u32 jal_cheats;


//MACROS
	#define MAKE_J(addr)   (u32)(0x08000000 | (0x03FFFFFF & ((u32)addr >> 2)))
	#define MAKE_JAL(addr)   (u32)(0x0C000000 | (0x03FFFFFF & ((u32)addr >> 2)))
	#define KSEG0(x)   ((void*)(((u32)(x)) | 0x80000000))
