
#include <stdio.h>
#include <loadcore.h>
#include <ioman.h>
#include <sifman.h>
#include <sysmem.h>
#include <errno.h>
#include <io_common.h>
#include "loadcore_add.h"

#define MODNAME "SyncEE"
IRX_ID(MODNAME, 2, 2);
//IRX_ID(MODNAME, 1, 1);

struct irx_export_table _exp_eesync;

// functions prototypes
int PostResetcb(void);
void CheckSecrmanLength(void);

//-------------------------------------------------------------------------
int _start(int argc, char** argv)
{	
	register int *r;
	register int bootmode; 
	
	r = QueryBootMode(3);
	
	if (r) {
		bootmode = *((int *)r + 4);
		
		if (bootmode & 1)
			return MODULE_NO_RESIDENT_END;
		
		if (bootmode & 2)
			return MODULE_NO_RESIDENT_END;
	}
	
	// Register eesync dummy export table	
	if (RegisterLibraryEntries(&_exp_eesync) < 0)
		return MODULE_NO_RESIDENT_END;
	
	CheckSecrmanLength();
	
	SetPostResetcb(PostResetcb, 2, NULL);
	
	return MODULE_RESIDENT_END;
}

//-------------------------------------------------------------------------
int PostResetcb(void)
{
	sceSifSetSMFlag(0x40000);
	
	return 0;
}

//-------------------------------------------------------------------------
void CheckSecrmanLength(void)
{
	u8 buf[16];
	register int i, fd, secrman_length, alloc_size;
	
	i = 0;
	do {
		buf[i] = 0x70 - ((i + 1) << 2);	// writes 0x6C 0x68 0x64 0x60 0x5C 0x58 0x54 0x50 0x4C 0x48 0x44 0x40 to buf
	} while (++i < 12);
	
	// descramble it to "rom0:sercman"
	*((u32 *)&buf[12]) = 0;
	*((u32 *)&buf[0]) ^= 0x5009071e;
	*((u32 *)&buf[4]) ^= 0x13110b66;
	*((u32 *)&buf[8]) ^= 0x0e05051e;
	
	// Get secrman size
	fd = open(buf, O_RDONLY);
	if (fd < 0) 
		return;
		
	secrman_length = lseek(fd, 0, SEEK_END);
	
	if (close(fd) < 0)	
		return;
		
	if (secrman_length < 0)
		return;
		
	if (secrman_length == 0x2731)
		alloc_size = 0x1900;
	else 
		alloc_size = 0x100;		
		
	AllocSysMemory(ALLOC_FIRST, alloc_size, NULL);	
}
