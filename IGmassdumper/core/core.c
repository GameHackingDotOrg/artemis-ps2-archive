#include "core.h"

//#define VERBOSE
#define MANUAL_ELF_LOAD
//#define NOADDITIONAL_IRX
//#define NOSYSCALL_HOOKS
//#define DISABLE_AFTER_IOPRESET

int set_reg_hook;

// for IRX from ram
typedef struct {
	void *irxaddr;
	int irxsize;
} irxptr_t;

#define IRX_NUM 		5

u8 membuffer[65536];
void *irx_mem;

static int eedump_index = 0;
static int iopdump_index = 0;
static int kerneldump_index = 0;
static int scratchpaddump_index = 0;

char dump_filename[1024];

#define PAD_LEFT      0x0080
#define PAD_DOWN      0x0040
#define PAD_RIGHT     0x0020
#define PAD_UP        0x0010
#define PAD_START     0x0008
#define PAD_R3        0x0004
#define PAD_L3        0x0002
#define PAD_SELECT    0x0001
#define PAD_SQUARE    0x8000
#define PAD_CROSS     0x4000
#define PAD_CIRCLE    0x2000
#define PAD_TRIANGLE  0x1000
#define PAD_R1        0x0800
#define PAD_L1        0x0400
#define PAD_R2        0x0200
#define PAD_L2        0x0100


// ------------------------------------------------------------------------
// For libpad support
// 
// libpad		2.1.1.0
// libpad		2.1.3.0
// libpad		2.2.0.0
// libpad		2.3.0.0
// libpad		2.4.0.0
// libpad		2.4.1.0
// libpad		2.5.0.0
// libpad		2.6.0.0
// libpad		2.7.0.0
//
static u32 padReadpattern0[] = { // type 4
	0x0080382d,			// daddu a3, a0, zero
	0x24030070,			// li 	 v1, $00000070
	0x2404001c,			// li  	 a0, $0000001c
	0x70e31818,			// mult1 v1, a3, v1
	0x00a42018,			// mult	 a0, a1, a0
	0x27bdff00,			// addiu sp, sp, $ffXX
	0x3c020000,			// lui 	 v0, $XXXX
	0xff000000,			// sd 	 XX, $XXXX(XX)
	0xffbf0000,			// sd  	 ra, $XXXX(sp)
	0x24420000,			// addiu v0, v0, $XXXX 
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x0c000000			// jal   scePadGetDmaStr
};
static u32 padReadpattern0_mask[] = {
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffff00,
	0xffff0000,
	0xff000000,
	0xffff0000,
	0xffff0000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0xfc000000	   
};

// ------------------------------------------------------------------------
// For libpad support
// 
// libpad		2.1.0.0
//
static u32 padReadpattern1[] = { // type 3
	0x0080382d,			// daddu a3, a0, zero
	0x24020060,			// li 	 v0, $00000060
	0x24040180,			// li  	 a0, $00000180
	0x00a21018,			// mult  v0, a1, v0
	0x70e42018,			// mult1 a0, a3, a0
	0x27bdff00,			// addiu sp, sp, $ffXX
	0x3c030000,			// lui 	 v1, $XXXX
	0xff000000,			// sd 	 XX, $XXXX(XX)
	0xffbf0000,			// sd  	 ra, $XXXX(sp)
	0x24630000,			// addiu v1, v1, $XXXX 
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x00000000, 		// ...
	0x0c000000			// jal   scePadGetDmaStr
};
static u32 padReadpattern1_mask[] = {
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffff00,
	0xffff0000,
	0xff000000,
	0xffff0000,
	0xffff0000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0xfc000000	   
};

// ------------------------------------------------------------------------
// For libpad support
// 
// libpad		2.7.1.0
// libpad		2.8.0.0
//
static u32 padReadpattern2[] = { // type 5
	0x27bdff00,			// addiu sp, sp, $ffXX
	0x24030070,			// li 	 v1, $00000070
	0xffb10000,			// sd 	 s1, $XXXX(sp)
	0x3c020000,			// lui	 v0, $XXXX
	0xffb20000,			// sd 	 s2, $XXXX(sp)
	0x0080882d,			// daddu s1, a0, zero 
	0x00a0902d,			// daddu s2, a1, zero
	0x2404001c,			// li 	 a0, $0000001c
	0x72231818,			// mult1 v1, s1, v1	
	0x02442018,			// mult  a0, s2, a0	
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x0c000000			// jal   DIntr
};
static u32 padReadpattern2_mask[] = {
	0xffffff00,
	0xffffffff,
	0xffff0000,
	0xffff0000,
	0xffff0000,
	0xffffffff,
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,
	0x00000000,
	0x00000000,    
	0xfc000000	   
};

// ------------------------------------------------------------------------
// For libpad support
// 
// libpad      3.0.0.0
//
static u32 padReadpattern3[] = { //type 6
	0x27bdff00,			// addiu sp, sp, $ffXX
	0x24030070,			// li 	 v1, $00000070
	0xffb10000,			// sd 	 s1, $XXXX(sp)
	0x3c020000,			// lui	 v0, $XXXX
	0xffb20000,			// sd 	 s2, $XXXX(sp)
	0x0080882d,			// daddu s1, a0, zero 
	0x00a0902d,			// daddu s2, a1, zero
	0x2404001c,			// li 	 a0, $0000001c
	0x72231818,			// mult1 v1, s1, v1	
	0x02442018,			// mult  a0, s2, a0	
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x00000000,			// ...  
	0x0c000000			// jal   DIntr
};
static u32 padReadpattern3_mask[] = {
	0xffffff00,
	0xffffffff,
	0xffff0000,
	0xffff0000,
	0xffff0000,
	0xffffffff,
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,  
	0x00000000,
	0x00000000,
	0x00000000,    
	0xfc000000	   
};

// ------------------------------------------------------------------------
// For libpad2 support
// 
// libpad2		2.4.0.0
// libpad2		2.5.0.0
// libpad2		2.7.0.0
// libpad2		2.7.1.0
// libpad2		2.8.0.0
// libpad2      3.0.0.0
// libpad2      3.0.2.0
// 
static u32 pad2Readpattern0[] = {
	0x27bdffc0,			// addiu sp, sp, $ffc0
	0x24020000, 		// li 	 v0, $XXXX
	0xffb10010,			// sd 	 s1, $0010(sp)
	0x3c030000, 		// lui 	 v1, $XXXX
	0x0080882d,			// daddu s1, a0, zero
	0xffb20020,			// sd 	 s2, $0020(sp)
	0x02222018,			// mult  a0, s1, v0
	0x24660000, 		// addiu a2, v1, $XXXX	
	0xffbf0030,			// sd 	 ra, $0030(sp)
	0x00a0902d,			// daddu s2, a1, zero		
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000,			// ...	
	0x0c000000			// jal   scePad2LinkDriver	
};
static u32 pad2Readpattern0_mask[] = {
	0xffffffff,
	0xffff0000,
	0xffffffff,
	0xffff0000,	
	0xffffffff,	
	0xffffffff,	
	0xffffffff,
	0xffff0000,	
	0xffffffff,
	0xffffffff,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,	
	0xfc000000
};

// ------------------------------------------------------------------------
// For libpad support
// 
// libpad		1.5.0.0
//
static u32 padReadpattern4[] = { //type 0
	0x27bdffc0,			// addiu	sp, sp, $ffc0
	0x00052900,			// sll		a1, a1, 4
	0x000421c0,			// sll		a0, a0, 7
	0x3c020000,			// lui		v0, PadDataAddrHi
	0x00a42821,			// addu		a1, a1, a0
	0xffb20020,			// sd		s2, $0020(sp)
	0xffb10010,			// sd		s1, $0010(sp)
	0x24420000,			// addiu	v0, v0, PadDataAddrLo
	0xffbf0030,			// sd		ra, $0030(sp)
	0x00451021,			// addu		v0, v0, a1
	0xffb00000,			// sd		s0, $0000(sp)
	0x00c0902d,			// daddu	s2, a2, zero
	0x8c50000c,			// lw		s0, $000c(v0)
	0x24050100,			// li		a1, $00000100
	0x0c000000			// jal		sceSifWriteBackDCache
};
static u32 padReadpattern4_mask[] = {
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffff0000,
	0xffffffff,	
	0xffffffff,
	0xffffffff,  
	0xffff0000,	  
	0xffffffff,  
	0xffffffff,
	0xffffffff,  
	0xffffffff,
	0xffffffff,  
	0xffffffff,
	0xfc000000	   
};

// ------------------------------------------------------------------------
// For libpad support
// 
// libpad		1.6.2.0
// libpad		1.6.3.0
//
static u32 padReadpattern5[] = { //type 1	
	0x24020060,			// li		v0, $00000060
	0x24070300,			// li		a3, $00000300
	0x00a21818,			// mult		v1, a1, v0
	0x70872018,			// mult1	a0, a0, a3
	0x27bdffc0,			// addiu	sp, sp, $ffc0
	0x3c020000,			// lui		v0, PadDataAddrHi
	0xffb20020,			// sd		s2, $0020(sp)
	0x24420000,			// addiu	v0, v0, PadDataAddrLo
	0xffb10010,			// sd		s1, $0010(sp)
	0x00c0902d,			// daddu	s2, a2, zero
	0x00641821,			// addu		v1, v1, a0
	0xffbf0030,			// sd		ra, $0030(sp)
	0xffb00000,			// sd		s0, $0000(sp)
	0x00621821,			// addu		v1, v1, v0
	0x8c70000c,			// lw		s0, $000c(v1)
	0x24050100,			// li		a1, $00000100
	0x0c000000,			// jal		sceSifWriteBackDCache
};
static u32 padReadpattern5_mask[] = {
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffff0000,
	0xffffffff,  
	0xffff0000,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xfc000000	   
};

// ------------------------------------------------------------------------
// For libpad support
// 
// libpad		2.0.0.0
// libpad		2.0.5.0
//
static u32 padReadpattern6[] = { //type 2		
	0x24020060,			// li		v0, $00000060
	0x24030180,			// li		v1, $00000180
	0x00a22818,			// mult		a1, a1, v0
	0x70832018,			// mult1	a0, a0, v1
	0x27bdffe0,			// addiu	sp, sp, $ffe0
	0x3c020000,			// lui		v0, PadDataAddrHi
	0xffbf0010,			// sd		ra, $0010(sp)
	0x24420000,			// addiu	v0, v0, PadDataAddrLo
	0xffb00000,			// sd		s0, $0000(sp)
	0x00a42821,			// addu		a1, a1, a0
	0x00a22821,			// addu		a1, a1, v0
	0x8cb0000c,			// lw		s0, $000c(a1)
	0x0200202d,			// daddu	a0, s0, zero
	0x0c000000,			// jal		SyncDCache
};
static u32 padReadpattern6_mask[] = {
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffff0000,
	0xffffffff,  
	0xffff0000,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xffffffff,  
	0xfc000000	   
};

// ------------------------------------------------------------------------
// For libpad support
// 
// libpad          3.0.1.0
// libpad          3.0.2.0
//
static u32 padReadpattern7[] = { //type 7			
	0x27bdffb0,		// addiu	sp, sp, $ffb0
	0xffb20020,		// sd		s2, $0020(sp)
	0xffb10010,		// sd		s1, $0010(sp)
	0x00c0902d,		// daddu	s2, a2, zero
	0xffb00000,		// sd		s0, $0000(sp)
	0x0080882d,		// daddu	s1, a0, zero
	0xffb30030,		// sd		s3, $0030(sp)
	0xffbf0040,		// sd		ra, $0040(sp)
	0x0c000000,		// jal		DI
	0x00a0802d,		// daddu	s0, a1, zero
	0x00000000,		// ...
	0x00000000,		// ...
	0x00000000,		// ...
	0x00000000,		// ...
	0x00000000,		// ...
	0x00000000,		// ...
	0x00000000,		// ...
	0x00000000,		// ...
	0x00000000,		// ...
	0x00000000,		// ...
	0x00000000,		// ...
	0x0c000000,		// jal		scePadGetDmaStr
};
static u32 padReadpattern7_mask[] = {
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffff0000,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xfc000000,
	0xffffffff,
	0x00000000,  	
	0x00000000,  
	0x00000000,  	
	0x00000000,  
	0x00000000,  	
	0x00000000,  
	0x00000000,  	
	0x00000000,  
	0x00000000,  	
	0x00000000,  
	0x00000000,  	
	0xfc000000	   
};

// ------------------------------------------------------------------------
struct padButtonStat {
    u8 ok;
    u8 mode;
    u16 btns;
} __attribute__((packed));

struct pad2ButtonStat {
    u16 btns;
} __attribute__((packed));

int patch_padRead(void);

static int (*scePadRead)(int port, int slot, struct padButtonStat *data);
static int (*scePad2Read)(int socket, struct pad2ButtonStat *data);
static int scePadRead_style = 1;

static u32 old_pad;


//--------------------------------------------------------------
void *find_free_ram(void *addr_start, void *addr_end, u32 len)
{
	u32 i, j;

	addr_start = (void *)(((u32)addr_start) & 0xffffffc0); // 64bytes align check
	
	u32 memsize = (u32)addr_end - (u32)addr_start;
	u8 *buf = (u8 *)addr_start;
	
	for (i = 0; i < memsize - len; ) {
		for (j = 0; j < len; j++) {
			if (buf[i + j] != 0)
				break;
		}
		if (j == len)
			return &buf[i];
		else
			i += (j + 0x40) & 0xffffffc0; // 64bytes align check
	}
	
	return NULL;
}

// ------------------------------------------------------------------------
u32 NewSifSetDma(SifDmaTransfer_t *sdd, s32 len)
{
	SifCmdResetData *reset_pkt = (SifCmdResetData*)sdd->src;
	HookIopReset(reset_pkt->arg, reset_pkt->flag);

	return 1;
}

// ------------------------------------------------------------------------
u32 HookSifSetDma(SifDmaTransfer_t *sdd, s32 len)
{
	if ((sdd->attr == 0x44) && (sdd->size==0x68)) {
		SifCmdResetData *reset_pkt = (SifCmdResetData*)sdd->src;
		if (((reset_pkt->chdr.psize == 0x68)) && (reset_pkt->chdr.fcode == 0x80000003))	{			
			__asm__(
				"la $v1, NewSifSetDma\n\t"
				"sw $v1, 8($sp)\n\t"
				"jr $ra\n\t"
				"nop\n\t"
			);
		}
	}
		
	__asm__(
		"move $a0, %1\n\t"
		"move $a1, %2\n\t"
		"jr   %0\n\t"
		::"r"((u32)Old_SifSetDma), "r"((u32)sdd), "r"((u32)len)
	);

	return 1;
}

// ------------------------------------------------------------------------
int HookSifSetReg(u32 register_num, int register_value)
{
	if (set_reg_hook) {
		set_reg_hook--;
		
		if (set_reg_hook == 0) {
			__asm__(
				"la $v1, PostReset_Hook\n\t"
				"sw $v1, 8($sp)\n\t"
				"jr $ra\n\t"
				"nop\n\t"
			);
		}
		return 1;
	}

	__asm__(
		"move $a0, %1\n\t"
		"move $a1, %2\n\t"
		"jr   %0\n\t"
		::"r"((u32)Old_SifSetReg), "r"((u32)register_num), "r"((u32)register_value)
	);

	return 1;
}

// ------------------------------------------------------------------------
int Hook_Syscalls(void)
{
	Old_SifSetDma = GetSyscall(__NR_SifSetDma);
	SetSyscall(__NR_SifSetDma, &HookSifSetDma);

	Old_SifSetReg = GetSyscall(__NR_SifSetReg);
	SetSyscall(__NR_SifSetReg, &HookSifSetReg);
	
	return 1;
}

// ------------------------------------------------------------------------
int UnHook_Syscalls(void)
{
	SetSyscall(__NR_SifSetDma, Old_SifSetDma);
	SetSyscall(__NR_SifSetReg, Old_SifSetReg);	 
	
	return 1;
}

// ------------------------------------------------------------------------
int PostReset_Hook(void)
{	
	patch_padRead();

#ifdef DISABLE_AFTER_IOPRESET	
	UnHook_Syscalls();
#endif
		
	return 1;
}

// ------------------------------------------------------------------------
int LoadIRXfromKernel(void *irxkernelmem, int irxsize, int arglen, char *argv)
{	
	DIntr();
	ee_kmode_enter();
	
	memcpy(irx_mem, irxkernelmem, irxsize);
	
	ee_kmode_exit();
	EIntr();
		
	return LoadModuleBuffer(irx_mem, irxsize, arglen, argv);
}

// ------------------------------------------------------------------------
int HookIopReset(const char *arg, int flag)
{
 	void   *iopdest_img;
	int     i, r, fd;
	ioprp_t ioprp_img;
	char    ioprp_path[0x50];
	char    update_path[0x50];
	u32  	qid;
	SifDmaTransfer_t dmat;	 
	int iopreset_update = 0;
	int iopreset_ioprp = 0;
	u8 *p;
 
	GS_BGCOLOUR = 0xff0000; // Blue for 1st IOP reset
	
	SifInitRpc(0);
	while (!IopReset(NULL, 0)) {;}
	while (!IopSync()){;}
	SifExitRpc();
	SifExitIopHeap();
	LoadFileExit();
	SifInitRpc(0);
		
#ifdef VERBOSE	
	init_scr();
	scr_clear();
#endif		

	if (arg != NULL) {
		memset(update_path, 0, 0x50);
		// check for EELOADCNF or OSDCNF update
		for (i=0; i<0x50; i++) { 
			if ((!memcmp(&arg[i], "rom0:UDNL rom0:EELOADCNF", 24)) || (!memcmp(&arg[i], "rom0:UDNL rom0:OSDCNF", 21))) {
				sprintf(update_path, "%s", arg+i);
				iopreset_update = 1;
				break;
			}
		}
		// if not found check for IOPRP Image
		if (i == 0x50) {
			memset(ioprp_path, 0, 0x50);  
			for (i=0; i<0x50; i++) {
				if ((!memcmp(&arg[i], "rom0:UDNL cdrom", 15)) || (!memcmp(&arg[i], "rom0:UDNL mc", 12))){
					sprintf(ioprp_path, "%s", arg+i+10);
					iopreset_ioprp = 1;
					break;
				}
			}			
		}  	 
	} 
	 
	if (iopreset_ioprp) {
		
		GS_BGCOLOUR = 0x800080; // Purple for reset with IOPRP catched, and free mem location search
		
#ifdef VERBOSE				
		scr_printf("\t Catched InGame IOP reset with IOPRPxxx.IMG !!!\n");	
		scr_printf("\t ioprp path: %s\n", ioprp_path);
#endif		
  
		fioInit();
		fd = open(ioprp_path, O_RDONLY);
		if (fd < 0)
			while (1){;}

		ioprp_img.size_in = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);

		p = find_free_ram((void *)0x01700000, (void *)0x01f00000, (ioprp_img.size_in << 1) + 16384);
		if (!p) {
#ifdef VERBOSE			
			scr_printf("\t No free mem found for ioprp...\n"); 		
#endif		
			// force default adresses
			ioprp_img.data_in  = (void *)0x01700000;
			ioprp_img.data_out = (void *)0x017C0000;		
		}	
		else {
			ioprp_img.data_in  = (void *)p;
			ioprp_img.data_out = (void *)&p[(ioprp_img.size_in + 0x40) & 0xffffffc0]; // 64bytes align check
		}
				
#ifdef VERBOSE					
		scr_printf("\t ioprp_in: 0x%08x ioprp_out: 0x%08x\n", (int)ioprp_img.data_in, (int)ioprp_img.data_out); 
#endif						
		
		GS_BGCOLOUR = 0x0000ff; // Red for IOPRP patching
		
		read(fd, ioprp_img.data_in , ioprp_img.size_in);
		close(fd);
		fioExit();

		memcpy(ioprp_img.data_out, ioprp_img.data_in, ioprp_img.size_in); 

		r = PatchIOPRPimg(&ioprp_img);
		if (r == 0) 
			while (1){;}

		GS_BGCOLOUR = 0x008000; // Green for IOP reset with patched IOPRP
		
		// Reseting IOP			  		
		while (!IopReset(NULL, 0)) {;}
		while (!IopSync()){;}
		SifInitRpc(0);
		SifInitIopHeap();
		LoadFileInit();
		sbv_patch_enable_lmb();
		sbv_patch_disable_prefix_check();
      
		// allocate mem & get pointer to iop ram
		iopdest_img = SifAllocIopHeap(ioprp_img.size_out);
		if (iopdest_img) {
			// Send IOPRP IMG on IOP mem
			dmat.src = (void *)ioprp_img.data_out;
			dmat.dest = iopdest_img;
			dmat.size = ioprp_img.size_out;
			dmat.attr = 0;
			
			qid = SifSetDma(&dmat, 1);
			while (SifDmaStat(qid) >= 0);
			
			// get back memdisk drv from kernel ram		
			DIntr();
			ee_kmode_enter();
			memcpy(irx_mem, memdisk_irx, size_memdisk_irx);
			ee_kmode_exit();
			EIntr();
			
			// Patching memdisk irx
			u8 *memdisk_drv = (u8 *)irx_mem;
			*(u32*)(&memdisk_drv[0x19C]) = (u32)iopdest_img;
			*(u32*)(&memdisk_drv[0x1A8]) = ioprp_img.size_out;
			FlushCache(0);

			// Load module asynchronously with memdisk drv
			LoadModuleBuffer(memdisk_drv, size_memdisk_irx, 0, NULL);			
			LoadModuleAsync("rom0:UDNL", 7, "MDISK0:");

			SifExitRpc();
			LoadFileExit();

			DIntr();
			ee_kmode_enter();
			Old_SifSetReg(SIF_REG_SMFLAG, 0x10000);
			Old_SifSetReg(SIF_REG_SMFLAG, 0x20000);
			Old_SifSetReg(0x80000002, 0);
			Old_SifSetReg(0x80000000, 0);
			ee_kmode_exit();
			EIntr();
		}
		else 
			while (1){;}
    
		while (!IopSync()) {;}
				
		GS_BGCOLOUR = 0xffff00; // Cyan for loading additional IRX
  
		// we can load additional modules here
		SifExitIopHeap();
		SifInitRpc(0);
		SifInitIopHeap();
		LoadFileInit();

#ifdef VERBOSE								 		
		scr_printf("\t loading additional modules... ");
#endif    
		
#ifndef NOADDITIONAL_IRX

		r = LoadIRXfromKernel(usbd_irx, size_usbd_irx, 0, NULL);
		if (r < 0)
			while (1) {;}
											
		r = LoadIRXfromKernel(usb_mass_irx, size_usb_mass_irx, 0, NULL);			
		if (r < 0)
			while (1) {;}
			
		r = LoadIRXfromKernel(usbserv_irx, size_usbserv_irx, 0, NULL);			
		if (r < 0)
			while (1) {;}
				
#endif		

#ifdef VERBOSE								 		
		scr_printf("done\n");
#endif    

		FlushCache(0);

#ifndef NOADDITIONAL_IRX				
		rpcUSBInit();
#endif				
		FlushCache(0);		
																					
		SifExitRpc();
		SifExitIopHeap();
		LoadFileExit();
				
#ifdef VERBOSE		
		scr_printf("\t IOP reset done\n");
		//while(1) {;}			
#endif	
	
	}
	else if (iopreset_update) {  
		while (!IopReset(arg, flag)) {;}		
	}
	else {
		while (!IopReset(arg, flag)) {;}
	}
 
	set_reg_hook = 4;
	
	GS_BGCOLOUR = 0x000000; // Black, IOP reset done
		
	return 1;
}

// ------------------------------------------------------------------------
int PatchIOPRPimg(ioprp_t *ioprp_img)
{
	int       offset_in, offset_out;
	romdir_t *romdir_in, *romdir_out;

	offset_in  = 0;
	offset_out = 0;

	romdir_in  = (romdir_t*)ioprp_img->data_in;
	romdir_out = (romdir_t*)ioprp_img->data_out;

	while(strlen(romdir_in->fileName) > 0) {
		// replacing some modules from .img file
		if (!strcmp(romdir_in->fileName, "EESYNC")) {
			DIntr(); // get back eesync drv from kernel ram		
			ee_kmode_enter();
			memcpy((void*)((u32)ioprp_img->data_out+offset_out), eesync_irx, size_eesync_irx);
			ee_kmode_exit();
			EIntr();
			romdir_out->fileSize = size_eesync_irx;
		}
		// leave module from .img file as it is
	  	else {
			memcpy((void*)((u32)ioprp_img->data_out+offset_out), (void*)((u32)ioprp_img->data_in+offset_in), romdir_in->fileSize);
		}
		// arrange irx size to next 16 bytes multiple 
		if ((romdir_in->fileSize % 0x10)==0)
			offset_in += romdir_in->fileSize;
		else
			offset_in += (romdir_in->fileSize + 0x10) & 0xfffffff0;

		if ((romdir_out->fileSize % 0x10)==0)
			offset_out += romdir_out->fileSize;
		else
			offset_out += (romdir_out->fileSize + 0x10) & 0xfffffff0;

		romdir_in++;
		romdir_out++;
	}
	
	ioprp_img->size_out = offset_out;

	return 1;
}

//--------------------------------------------------------------
u8 *find_bytes_with_mask(u8 *buf, u32 bufsize, u8 *bytes, u8 *mask, u32 len)
{
	u32 i, j;

	for (i = 0; i < bufsize - len; i++) {
		for (j = 0; j < len; j++) {
			if ((buf[i + j] & mask[j]) != bytes[j])
				break;
		}
		if (j == len)
			return &buf[i];
	}
	
	return NULL;
}

//--------------------------------------------------------------
int dump_mem(void *addr, int size, void *buf)
{
	int i;
	u8 *p1 = (u8 *)addr;
	u8 *p2 = (u8 *)buf;
						
	DIntr();
	ee_kmode_enter();
		
	for (i=0; i<size; i++)
		p2[i] = p1[i];

	ee_kmode_exit();	
	EIntr();
	
	return 0;
}

//--------------------------------------------------------------
int sendDump(char *filename, u32 dump_start, u32 dump_end)
{
	int r, fd, len, sndSize, dumpSize, dpos, rpos;	
										
	len = dump_end - dump_start;

	if (len < 0)
		return 0;
	
	rpcUSBcreate(filename);
	rpcUSBSync(0, NULL, &fd);	
	
	if (fd < 0)
		return 0;									
		
	// reducing dump size to fit in buffer
	if (len > sizeof(membuffer))
		dumpSize = sizeof(membuffer);
	else		
		dumpSize = len;
		
	dpos = 0;	
	while (dpos < len) {
									
		dump_mem((void *)(dump_start + dpos), dumpSize, membuffer);
			
		// reducing send size for rpc if needed
		if (dumpSize > 16376)
			sndSize = 16376;
		else		
			sndSize = dumpSize;
				
		rpos = 0;
		while (rpos < dumpSize) {
			rpcUSBwrite(fd, &membuffer[rpos], sndSize);
			rpcUSBSync(0, NULL, &r);
			if (r != sndSize)
				return 0; 										
			rpos += sndSize;
			if ((dumpSize - rpos) < 16376)
				sndSize = dumpSize - rpos;				
		}
		
		dpos += dumpSize;	
		if ((len - dpos) < sizeof(membuffer))
			dumpSize = len - dpos;				
	}

	rpcUSBclose(fd);
	rpcUSBSync(0, NULL, &r);		
				
	return len;
}

//--------------------------------------------------------------
void padReadHook_job(void *data)
{
	u32 paddata, new_pad;
	
	if (scePadRead_style == 2)
		paddata = 0xffff ^ ((struct pad2ButtonStat *)data)->btns;
	else	
		paddata = 0xffff ^ ((struct padButtonStat *)data)->btns;
		
	new_pad = paddata & ~old_pad;
	old_pad = paddata;
              
	if ((new_pad) && (new_pad == (PAD_UP | PAD_R1 | PAD_CROSS))) {	
		sprintf(dump_filename, "EEdump@00100000-02000000-%03d.raw", eedump_index);
		sendDump(dump_filename, 0x00100000, 0x02000000);
		eedump_index++;
	}
	else if ((new_pad) && (new_pad == (PAD_UP | PAD_R1 | PAD_TRIANGLE))) {	
		sprintf(dump_filename, "IOPdump@00000000-00200000-%03d.raw", iopdump_index);		
		sendDump(dump_filename, 0xbc000000, 0xbc200000);
		iopdump_index++;
	}
	else if ((new_pad) && (new_pad == (PAD_UP | PAD_R1 | PAD_SQUARE))) {
		sprintf(dump_filename, "Kerneldump@00000000-02000000-%03d.raw", kerneldump_index);			
		sendDump(dump_filename, 0x80000000, 0x82000000);
		kerneldump_index++;
	}
	else if ((new_pad) && (new_pad == (PAD_UP | PAD_R1 | PAD_CIRCLE))) {	
		sprintf(dump_filename, "ScratchPaddump@0000-4000-%03d.raw", scratchpaddump_index);					
		sendDump(dump_filename, 0x70000000, 0x70004000);
		scratchpaddump_index++;
	}	
}

//--------------------------------------------------------------
static int Hook_scePadRead(int port, int slot, struct padButtonStat *data)
{
	int ret;

	ret = scePadRead(port, slot, data);
	padReadHook_job(data);
				
	return ret;
}

//--------------------------------------------------------------
static int Hook_scePad2Read(int socket, struct pad2ButtonStat *data)
{
	int ret;

	ret = scePad2Read(socket, data);
	padReadHook_job(data);	
				
	return ret;
}

//--------------------------------------------------------------
int patch_padRead(void)
{
	u8 *ptr;
	u32 memscope, inst, fncall;	
	u32 pattern[1], mask[1];
	u32 start = 0x00100000;
	int pattern_found = 0;

	GS_BGCOLOUR = 0xff00ff; // Magenta while padRead pattern search	
			
	memscope = 0x01f00000 - start;
	
	// First try to locate the orginal libpad's scePadRead function
    ptr = find_bytes_with_mask((u8 *)start, memscope, (u8 *)padReadpattern0, (u8 *)padReadpattern0_mask, sizeof(padReadpattern0));	
    if (!ptr) {
	    ptr = find_bytes_with_mask((u8 *)start, memscope, (u8 *)padReadpattern1, (u8 *)padReadpattern1_mask, sizeof(padReadpattern1));	
	    if (!ptr) {
		    ptr = find_bytes_with_mask((u8 *)start, memscope, (u8 *)padReadpattern2, (u8 *)padReadpattern2_mask, sizeof(padReadpattern2));	
	    	if (!ptr) {
		    	ptr = find_bytes_with_mask((u8 *)start, memscope, (u8 *)padReadpattern3, (u8 *)padReadpattern3_mask, sizeof(padReadpattern3));
		    	if (!ptr) {
		    		ptr = find_bytes_with_mask((u8 *)start, memscope, (u8 *)pad2Readpattern0, (u8 *)pad2Readpattern0_mask, sizeof(pad2Readpattern0));
		    		if (!ptr) {
		    			ptr = find_bytes_with_mask((u8 *)start, memscope, (u8 *)padReadpattern4, (u8 *)padReadpattern4_mask, sizeof(padReadpattern4));
		    			if (!ptr) {
		    				ptr = find_bytes_with_mask((u8 *)start, memscope, (u8 *)padReadpattern5, (u8 *)padReadpattern5_mask, sizeof(padReadpattern5));
		    				if (!ptr) {
		    					ptr = find_bytes_with_mask((u8 *)start, memscope, (u8 *)padReadpattern6, (u8 *)padReadpattern6_mask, sizeof(padReadpattern6));
		    					if (!ptr) {
		    						ptr = find_bytes_with_mask((u8 *)start, memscope, (u8 *)padReadpattern7, (u8 *)padReadpattern7_mask, sizeof(padReadpattern7));
		    						if (!ptr) {
    									//while(1) {;}
    									GS_BGCOLOUR = 0xffffff; // White, pattern not found	   
    									return 0;
									}
								}
							}
						}
					}
					else // If found scePad2Read pattern
						scePadRead_style = 2;
				}		    	
	    	}
    	}
    }
    
 	GS_BGCOLOUR = 0x00ff00; // Lime while padRead patches	   

	// Save original scePadRead ptr
	if (scePadRead_style == 2)
		scePad2Read = (void *)ptr;
	else
		scePadRead = (void *)ptr;

	// Retrieve scePadRead call Instruction code
	inst = 0x0c000000;
	inst |= 0x03ffffff & ((u32)ptr >> 2);
	
	// Make pattern with function call code saved above
	pattern[0] = inst;
	mask[0] = 0xffffffff;

	// Get Hook_scePadRead call Instruction code
	if (scePadRead_style == 2) {
		inst = 0x0c000000;
		inst |= 0x03ffffff & ((u32)Hook_scePad2Read >> 2);		
	}
	else {	
		inst = 0x0c000000;
		inst |= 0x03ffffff & ((u32)Hook_scePadRead >> 2);
	}
		
	// Search & patch for calls to scePadRead
	ptr = (u8 *)start;
	while (ptr) {
		memscope = 0x01f00000 - (u32)ptr;	
		ptr = find_bytes_with_mask(ptr, memscope, (u8 *)pattern, (u8 *)mask, sizeof(pattern));
		if (ptr) {
			pattern_found = 1;
			fncall = (u32)ptr;
			_sw(inst, fncall); // overwrite the original scePadRead function call with our function call		
			ptr += 8;
		}
	}
	
	if (!pattern_found)
		GS_BGCOLOUR = 0xffffff; // White, pattern not found
	else	
		GS_BGCOLOUR = 0x000000; // Black, done
			
	return 1;			    	   	
}

//-------------------------------------------------------------- 
void GetIrxKernelRAM(void)
{
	int i, irx_maxsize;
	u32 *total_irxsize = (u32 *)0x80030000;
	void *irx_tab = (void *)0x80030010;
	irxptr_t irxptr_tab[IRX_NUM];

	DIntr();
	ee_kmode_enter();
	
	memcpy(&irxptr_tab[0], irx_tab, sizeof(irxptr_tab));

	ee_kmode_exit();
	EIntr();
		
	irx_maxsize = 0;
	for (i=0; i<IRX_NUM; i++) {
		if (irxptr_tab[i].irxsize > irx_maxsize)
			irx_maxsize = irxptr_tab[i].irxsize;
	}
	
	size_memdisk_irx = irxptr_tab[0].irxsize; 
	size_eesync_irx = irxptr_tab[1].irxsize; 
	size_usbd_irx = irxptr_tab[2].irxsize; 	
	size_usb_mass_irx = irxptr_tab[3].irxsize; 
	size_usbserv_irx = irxptr_tab[4].irxsize; 
	
	irx_mem = malloc(irx_maxsize);
	
	memdisk_irx = (void *)irxptr_tab[0].irxaddr;
	eesync_irx = (void *)irxptr_tab[1].irxaddr;
	usbd_irx = (void *)irxptr_tab[2].irxaddr;
	usb_mass_irx = (void *)irxptr_tab[3].irxaddr;
	usbserv_irx = (void *)irxptr_tab[4].irxaddr;
					
#ifdef VERBOSE
	scr_printf("\t total IRX size = %d\n", *total_irxsize); 
#endif
}	

//--------------------------------------------------------------
int main(int argc, char *argv[1])
{
	char ElfPath[MAX_PATH];
	int i, fd;
	t_ExecData exec;
	elf_header_t elf_header;
	elf_pheader_t elf_pheader;
	

	SifInitRpc(0);
	SifInitIopHeap();

#ifdef VERBOSE
	init_scr();
	scr_clear();
	scr_printf("\t Core start !\n");
#endif	
	
	// Get back needed modules pointers from Kernel ram				
	GetIrxKernelRAM();
					
	// Clearing user mem, so better not to have anything valuable on stack
	for (i = 0x100000; i < 0x02000000; i += 64) {
		asm (
			"\tsq $0, 0(%0) \n"
			"\tsq $0, 16(%0) \n"
			"\tsq $0, 32(%0) \n"
			"\tsq $0, 48(%0) \n"
			:: "r" (i)
		);
	}	

#ifndef NOSYSCALL_HOOKS
	// Hooking SifSetDma & SifSetReg Syscalls
	Hook_Syscalls();
#endif
					
	// Get elf path
	strcpy(ElfPath, argv[0]);
/*	
	sbv_patch_user_mem_clear(0x00100000);
	
	SifExitIopHeap();
    SifExitRpc();
    LoadExecPS2(ElfPath, 0, NULL);	
*/		
#ifdef VERBOSE		
	scr_printf("\t loading %s... ", ElfPath);	
#endif	

#ifdef MANUAL_ELF_LOAD
	// We load the elf manually
	fioInit();
 	fd = open(ElfPath, O_RDONLY);
 	if (fd < 0) {
	 	//can't open file, exiting...
		goto error;
 	}
 
	if (!lseek(fd, 0, SEEK_END)) {
		close(fd);
		// Zero size ? something wrong !
		goto error;
	}
	
	// Read the Elf Header
	lseek(fd, 0, SEEK_SET);
	read(fd, &elf_header, sizeof(elf_header));
	
	// Check Elf Magic
	if ( (*(u32*)elf_header.ident) != ELF_MAGIC) {
		close(fd);
		//not an elf file
		goto error;
	}
	
	// Scan through the ELF's program headers and copy them into apropriate RAM
	// section, then padd with zeros if needed.
	for (i = 0; i < elf_header.phnum; i++) {
		lseek(fd, elf_header.phoff+(i*sizeof(elf_pheader)), SEEK_SET);
		read(fd, &elf_pheader, sizeof(elf_pheader));
		if (elf_pheader.type != ELF_PT_LOAD)
			continue;

		lseek(fd, elf_pheader.offset, SEEK_SET);
		read(fd, elf_pheader.vaddr, elf_pheader.filesz);
		
		if (elf_pheader.memsz > elf_pheader.filesz)
			memset(elf_pheader.vaddr + elf_pheader.filesz, 0,
					elf_pheader.memsz - elf_pheader.filesz);
	}
	close(fd);
	
#ifdef VERBOSE		
	scr_printf("done \n");		
#endif
			
	fioExit();
	LoadFileExit();
	SifExitRpc();

	FlushCache(0);
	FlushCache(2);
	
#ifdef VERBOSE			
    scr_printf("\t Executing...\n");
#endif    
	// temporary solution, some elfs fails if the path is not passed as argument
	argc = 1;
	argv[0] = ElfPath;
	argv[1] = NULL;
		
	// Execute...
	ExecPS2((void *)elf_header.entry, 0, argc, argv);
	
		
				
#else // ELF LOAD
	exec.epc = 0;
	exec.gp = 0;
	exec.sp = 0;
	
	LoadFileInit();
	LoadElf(ElfPath, &exec);

	if (exec.epc) {
#ifdef VERBOSE		
		scr_printf("done \n");		
#endif			
	
		fioExit();
		SifExitRpc();

		FlushCache(0);
		FlushCache(2);
		
#ifdef VERBOSE			
    	scr_printf("\t Executing...\n");
#endif    
		// temporary solution, some elfs fails if the path is not passed as argument
		argc = 1;
		argv[0] = ElfPath;
		argv[1] = NULL;
		
		// Execute...
		ExecPS2((void *)exec.epc, (void *)exec.gp, argc, argv);
	}						
#endif


		
	return 0;
 
error:
	GS_BGCOLOUR = 0xffffff; // white: fatal error
	while (1){;}
}


