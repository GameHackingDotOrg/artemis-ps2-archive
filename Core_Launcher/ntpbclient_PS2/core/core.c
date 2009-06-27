
#include "core.h"

#define DEBUG
#define MANUAL_ELF_LOAD
//#define NOADDITIONAL_IRX
//#define NOSYSCALL_HOOK

int set_reg_hook;
static int first_hook = 1;

// for IRX from ram
typedef struct {
	void *irxaddr;
	int irxsize;
} irxptr_t;

#define IRX_NUM 		6

u8 EEmembuffer[16384];
u8 IOPmembuffer[16384];

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

u32 old_pad;

static u32 padReadpattern0[] = {
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
  0x00000000, 			// ...
  0x00000000, 			// ...
  0x00000000, 			// ...
  0x00000000, 			// ...
  0x00000000, 			// ...
  0x00000000, 			// ...
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

static u32 padReadpattern1[] = {
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
  0x00000000, 			// ...
  0x00000000, 			// ...
  0x00000000, 			// ...
  0x00000000, 			// ...
  0x00000000, 			// ...
  0x00000000, 			// ...
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

static u32 padReadpattern2[] = {
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

static u32 padReadpattern3[] = {
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

static u32 padReadpattern4[] = { // for libpad2: this one just doesn't work, probably not the padRead function!!!
	0x27bdffc0,			// addiu sp, sp, $ffc0
	0xffb10010,			// sd 	 s1, $0010(sp)
	0xffb20020,			// sd 	 s2, $0020(sp)
	0x0080882d,			// daddu s1, a0, zero
	0xffbf0030,			// sd 	 ra, $0030(sp)
	0x00a0902d,			// daddu s2, a1, zero
	0x0c000000,			// jal   ...
	0xffb00000,			// sd 	 s0, $0000(sp)
	0x24030334, 		// li 	 v1, $00000334
	0x3c020000, 		// lui 	 v0, $XXXX
	0x02231818,			// mult  v1, s1, v1
	0x24420000, 		// addiu v0, v0, $XXXX
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000,			// ...
	0x00000000,			// ...
	0x0c000000			// jal   ...	
};
static u32 padReadpattern4_mask[] = {
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xfc000000,
	0xffffffff,
	0xffffffff,
	0xffff0000,
	0xffffffff,
	0xffff0000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0xfc000000
};

struct padButtonStatus
{
    unsigned char ok;
    unsigned char mode;
    unsigned short btns;
    // joysticks
    unsigned char rjoy_h;
    unsigned char rjoy_v;
    unsigned char ljoy_h;
    unsigned char ljoy_v;
    // pressure mode
    unsigned char right_p;
    unsigned char left_p;
    unsigned char up_p;
    unsigned char down_p;
    unsigned char triangle_p;
    unsigned char circle_p;
    unsigned char cross_p;
    unsigned char square_p;
    unsigned char l1_p;
    unsigned char r1_p;
    unsigned char l2_p;
    unsigned char r2_p;
    unsigned char unkn16[12];
} __attribute__((packed));

static u8 (*scePadRead)(int port, int slot, struct padButtonStatus *data);
static u8 (*scePadRead2)(int port, struct padButtonStatus *data);
static int scePadRead_style = 1;


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
int HookIopReset(const char *arg, int flag)
{
 	void   *iopdest_img;
	int     i, r, fd, ret;
	ioprp_t ioprp_img;
	char    ioprp_path[0x50];
	char    update_path[0x50];
	u32  	qid;
	SifDmaTransfer_t dmat;	 
	int iopreset_update = 0;
	int iopreset_ioprp = 0;
 
	SifInitRpc(0);
	while (!IopReset(NULL, 0)) {;}
	while (!IopSync()){;}
	SifExitRpc();
	SifExitIopHeap();
	LoadFileExit();
	SifInitRpc(0);
	
#ifdef DEBUG	
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

#ifdef DEBUG				
		scr_printf("\t Catched InGame IOP reset with IOPRPxxx.IMG !!!\n");	
		scr_printf("\t ioprp path: %s\n", ioprp_path);
#endif		
  
		fioInit();
		fd = open(ioprp_path, O_RDONLY);
		if (fd < 0) {
			GS_BGCOLOUR = 0x800000;
			while (1){;}
		}

		ioprp_img.size_in = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);

		ioprp_img.data_in  = (char*)0x01700000;
		ioprp_img.data_out = (char*)0x017C0000;

		read(fd, ioprp_img.data_in , ioprp_img.size_in);
		close(fd);
		fioExit();

		memcpy(ioprp_img.data_out, ioprp_img.data_in, ioprp_img.size_in); 

		r = PatchIOPRPimg(&ioprp_img);
		if (r == 0) {
			GS_BGCOLOUR = 0x00FF00;
			while (1){;}
		}

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
			
			// Patching memdisk irx
			u8 *memdisk_drv = (u8 *)memdisk_irx;
			*(u32*)(&memdisk_drv[0x19C]) = (u32)iopdest_img;
			*(u32*)(&memdisk_drv[0x1A8]) = ioprp_img.size_out;
			FlushCache(0);

			// Load module asynchronously with memdisk drv
			LoadModuleBuffer(memdisk_irx, size_memdisk_irx, 0, NULL);
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
		else {
			GS_BGCOLOUR = 0xFF0000;
			while (1){;}
		}
    
		while (!IopSync()) {;}
  
		// we can load additional modules here
		SifExitIopHeap();
		SifInitRpc(0);
		SifInitIopHeap();
		LoadFileInit();

#ifdef DEBUG								 		
		scr_printf("\t loading additional modules... ");
#endif    
		
#ifndef NOADDITIONAL_IRX
		LoadModuleBuffer(ps2dev9_irx, size_ps2dev9_irx, 0, NULL);		  				
		LoadModuleBuffer(ps2ip_irx, size_ps2ip_irx, 0, NULL);		  				
		LoadModuleBuffer(ps2smap_irx, size_ps2smap_irx, 0, NULL);		  				
		
		LoadModuleBuffer(ntpbclient_irx, size_ntpbclient_irx, 0, NULL);		  				
#endif		

#ifdef DEBUG								 		
		scr_printf("done\n");
#endif    

		FlushCache(0);

#ifndef NOADDITIONAL_IRX				
		rpcNTPBinit();

#ifdef DEBUG								 		
		scr_printf("\t rpcNTPBconnect to IP %s on port %d - ", SERVER_IP, SERVER_PORT);
#endif    
    	rpcNTPBconnect(SERVER_IP, SERVER_PORT);
    	rpcSync(0, NULL, &ret);
#ifdef DEBUG								 		
		if (ret)
			scr_printf("connection failed %d\n", ret);
		else	
			scr_printf("connection OK\n");
#endif		    	
#endif				
		// Check it's the first IOP reset hook	
		if (first_hook)			
			first_hook = 0;
/*
#ifdef DEBUG
    	for (i=0; i<100; i++)
    		nopdelay();
#endif    		
*/
			
		FlushCache(0);
															
		SifExitRpc();
		SifExitIopHeap();
		LoadFileExit();
				
#ifdef DEBUG		
		scr_printf("\t IOP reset done\n");
		//SleepThread();			
#endif	
	
	}
	else if (iopreset_update) {  
		while (!IopReset(arg, flag)) {;}		
	}
	else {
		while (!IopReset(arg, flag)) {;}
	}
 
	set_reg_hook = 4;
	
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
			memcpy((void*)((u32)ioprp_img->data_out+offset_out), eesync_irx, size_eesync_irx);
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
int sendEEdump(void)
{
	int r, len, sndSize, rpos;	
	u32 dump_start, dump_end;
	
	rpcNTPBsendCmd(NTPBCMD_GET_EEDUMP_START, NULL, 0);
	rpcSync(0, NULL, &dump_start);
	rpcNTPBsendCmd(NTPBCMD_GET_EEDUMP_END, NULL, 0);
	rpcSync(0, NULL, &dump_end);
						
	len = dump_end - dump_start;						
	dump_mem((void *)dump_start, len, EEmembuffer);
			
	// reducing send size for rpc if needed
	if (len > 8192)
		sndSize = 8192;
	else		
		sndSize = len;
				
	rpos = 0;
	while (rpos < len) {
		rpcNTPBsendCmd(NTPBCMD_PRINT_EEDUMP, &EEmembuffer[rpos], sndSize);
		rpcSync(0, NULL, &r);										
		rpos += sndSize;
		if ((len - rpos) < 8192)
			sndSize = len - rpos;				
	}	
	
	return rpos;
}

//--------------------------------------------------------------
int sendIOPdump(void)
{
	int r, len, sndSize, rpos;	
	u32 dump_start, dump_end;
	
	rpcNTPBsendCmd(NTPBCMD_GET_IOPDUMP_START, NULL, 0);
	rpcSync(0, NULL, &dump_start);
	rpcNTPBsendCmd(NTPBCMD_GET_IOPDUMP_END, NULL, 0);
	rpcSync(0, NULL, &dump_end);
					
	dump_start |= 0xbc000000;
	dump_end   |= 0xbc000000;
		
	len = dump_end - dump_start;						
	dump_mem((void *)dump_start, len, IOPmembuffer);
						
	// reducing send size for rpc if needed
	if (len > 8192)
		sndSize = 8192;
	else		
		sndSize = len;
				
	rpos = 0;
	while (rpos < len) {			
		rpcNTPBsendCmd(NTPBCMD_PRINT_IOPDUMP, &IOPmembuffer[rpos], sndSize);			
		rpcSync(0, NULL, &r);
		rpos += sndSize;
		if ((len - rpos) < 8192)
			sndSize = len - rpos;				
	}				
	
	return rpos;	
}

//--------------------------------------------------------------
int sendU32Value(u32 value) // for debug purpose
{	
	int r;
	
	*((u32 *)&EEmembuffer[0]) = value;						
				
	rpcNTPBsendCmd(NTPBCMD_PRINT_EEDUMP, &EEmembuffer[0], 4);
	rpcSync(0, NULL, &r);										
	
	return 1;
}

//--------------------------------------------------------------
void start_screen(void)
{
	int i;
	for(i = 0; i<6; i++)
	{
		// init debug screen
		init_scr();
	}
	for(i = 0; i<500; i++)
	{
		scr_printf("SCREEN STARTED!!\n");
	}
}

//--------------------------------------------------------------
static u8 Hook_scePadRead(int port, int slot, struct padButtonStatus *data)
{
	u8 ret;
	u32 paddata, new_pad;

	ret = scePadRead(port, slot, data);
	
	if (ret != 0) {
		paddata = 0xffff ^ data->btns;
        new_pad = paddata & ~old_pad;
        old_pad = paddata;
        
        //if (new_pad)
        //	sendU32Value(new_pad);
      
#ifdef NOADDITIONAL_IRX
		if ((new_pad) && (new_pad == (PAD_UP | PAD_R1 | PAD_CROSS))) {	
			//data->btns = 0 ^ 0xffff;
			start_screen();
		}
		else if ((new_pad) && (new_pad == (PAD_DOWN | PAD_R1 | PAD_CROSS))) {
			//data->btns = 0 ^ 0xffff;
			start_screen();
		}	
#else          	
		if ((new_pad) && (new_pad == (PAD_UP | PAD_R1 | PAD_CROSS))) {	
			//data->btns = 0 ^ 0xffff;
			sendEEdump();
		}
		else if ((new_pad) && (new_pad == (PAD_DOWN | PAD_R1 | PAD_CROSS))) {
			//data->btns = 0 ^ 0xffff;
			sendIOPdump();
		}	
#endif		
	}
				
	return ret;
}

//--------------------------------------------------------------
static u8 Hook_scePadRead2(int port, struct padButtonStatus *data)
{
	u8 ret;
	u32 paddata, new_pad;

	ret = scePadRead2(port, data);
		
	return ret;
}

//--------------------------------------------------------------
int patch_padRead(void *epc)
{
	u8 *ptr;
	u32 memscope, inst, fncall;	
	u32 pattern[1], mask[1];

#ifdef DEBUG			    		    	
    scr_printf("\t patch_padRead start!\n");
#endif    		
		
	memscope = 0x01fe0000 - (u32)epc;
	
	// First try to locate the orginal libpad's scePadRead function
    ptr = find_bytes_with_mask((u8 *)epc, memscope, (u8 *)padReadpattern0, (u8 *)padReadpattern0_mask, sizeof(padReadpattern0));	
    if (!ptr) {
	    ptr = find_bytes_with_mask((u8 *)epc, memscope, (u8 *)padReadpattern1, (u8 *)padReadpattern1_mask, sizeof(padReadpattern1));	
	    if (!ptr) {
		    ptr = find_bytes_with_mask((u8 *)epc, memscope, (u8 *)padReadpattern2, (u8 *)padReadpattern2_mask, sizeof(padReadpattern2));	
	    	if (!ptr) {
		    	ptr = find_bytes_with_mask((u8 *)epc, memscope, (u8 *)padReadpattern3, (u8 *)padReadpattern3_mask, sizeof(padReadpattern3));
		    	if (!ptr) {
		    		//ptr = find_bytes_with_mask((u8 *)epc, memscope, (u8 *)padReadpattern4, (u8 *)padReadpattern4_mask, sizeof(padReadpattern4));
		    		//if (!ptr) {
#ifdef DEBUG			    		    	
    					scr_printf("\t padReadpattern not found...\n");
#endif    		
    					return 0;
					}
					//else
					//	scePadRead_style = 2;
				//}		    	
	    	}
    	}
    }
#ifdef DEBUG			    	
	scr_printf("\t Found scePadRead at 0x%08x\n", (int)ptr);
#endif

	// Save original scePadRead ptr
	if (scePadRead_style == 2)
		scePadRead2 = (void *)ptr;
	else
		scePadRead = (void *)ptr;

	// Retrieve scePadRead call Instruction code
	inst = 0x0c000000;
	inst |= 0x03ffffff & ((u32)ptr >> 2);

#ifdef DEBUG			    		
	scr_printf("\t scePadRead Call Instruction: 0x%08x\n", (int)inst);
#endif
	
	// Make pattern with function call code saved above
	pattern[0] = inst;
	mask[0] = 0xffffffff;

	// Get Hook_scePadRead call Instruction code
	if (scePadRead_style == 2) {
		inst = 0x0c000000;
		inst |= 0x03ffffff & ((u32)Hook_scePadRead2 >> 2);		
	}
	else {	
		inst = 0x0c000000;
		inst |= 0x03ffffff & ((u32)Hook_scePadRead >> 2);
	}
		
	// Search & patch for calls to scePadRead
	ptr = (u8 *)epc;
	while (ptr) {
		memscope = 0x01fe0000 - (u32)ptr;	
		ptr = find_bytes_with_mask(ptr, memscope, (u8 *)pattern, (u8 *)mask, sizeof(pattern));
		if (ptr) {
			fncall = (u32)ptr;
			_sw(inst, fncall); // overwrite the original scePadRead function call with our function call		
#ifdef DEBUG			    		    	
			scr_printf("\t scePadRead call detected at: 0x%08x changed into: 0x%08x\n", (int)fncall, (int)inst);
#endif			
			ptr += 8;
		}
	}
	
#ifdef DEBUG			    		    	
    scr_printf("\t patch_padRead complete!\n");
#endif    		
		
	return 1;			    	   	
}

//-------------------------------------------------------------- 
void GetIrxRAM(void)
{
	u32 *total_irxsize = (u32 *)0x01a00000;
	void *irx_tab = (void *)0x01a00010;
	irxptr_t irxptr_tab[IRX_NUM];
	
	memcpy(&irxptr_tab[0], irx_tab, sizeof(irxptr_tab));
	
	size_memdisk_irx = irxptr_tab[0].irxsize; 
	size_eesync_irx = irxptr_tab[1].irxsize; 
	size_ntpbclient_irx = irxptr_tab[2].irxsize; 	
	size_ps2dev9_irx = irxptr_tab[3].irxsize; 
	size_ps2ip_irx = irxptr_tab[4].irxsize; 
	size_ps2smap_irx = irxptr_tab[5].irxsize; 
	
	memdisk_irx = malloc(size_memdisk_irx);
	eesync_irx = malloc(size_eesync_irx);
	ntpbclient_irx = malloc(size_ntpbclient_irx);	
	ps2dev9_irx = malloc(size_ps2dev9_irx);
	ps2ip_irx = malloc(size_ps2ip_irx);
	ps2smap_irx = malloc(size_ps2smap_irx);
	
	memcpy(memdisk_irx, irxptr_tab[0].irxaddr, size_memdisk_irx);
	memcpy(eesync_irx, irxptr_tab[1].irxaddr, size_eesync_irx);
	memcpy(ntpbclient_irx, irxptr_tab[2].irxaddr, size_ntpbclient_irx);	
	memcpy(ps2dev9_irx, irxptr_tab[3].irxaddr, size_ps2dev9_irx);
	memcpy(ps2ip_irx, irxptr_tab[4].irxaddr, size_ps2ip_irx);
	memcpy(ps2smap_irx, irxptr_tab[5].irxaddr, size_ps2smap_irx);
		
#ifdef DEBUG
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

#ifdef DEBUG
	init_scr();
	scr_clear();
	scr_printf("\t Core start !\n");
#endif	
	
	// Get back needed modules pointers from EE ram				
	GetIrxRAM();
				
	// Clearing user mem, so better not to have anything valuable on stack
	for (i = 0x100000; i < 0x2000000; i += 64) {
		asm (
			"\tsq $0, 0(%0) \n"
			"\tsq $0, 16(%0) \n"
			"\tsq $0, 32(%0) \n"
			"\tsq $0, 48(%0) \n"
			:: "r" (i)
		);
	}	
				
	// Get elf path
	strcpy(ElfPath, argv[0]);
	
#ifdef DEBUG		
	scr_printf("\t loading %s... ", ElfPath);	
#endif	

#ifdef MANUAL_ELF_LOAD
	// We load the elf manually
	fioInit();
 	fd = open(ElfPath, O_RDONLY);
 	if (fd < 0) {
	 	//can't open file, exiting...
		GS_BGCOLOUR = 0xff0000;
		goto error;
 	}
 
	if (!lseek(fd, 0, SEEK_END)) {
		close(fd);
		// Zero size ? something wrong !
		GS_BGCOLOUR = 0xffff00;
		goto error;
	}
	
	// Read the Elf Header
	lseek(fd, 0, SEEK_SET);
	read(fd, &elf_header, sizeof(elf_header));
	
	// Check Elf Magic
	if ( (*(u32*)elf_header.ident) != ELF_MAGIC) {
		close(fd);
		//not an elf file
		GS_BGCOLOUR = 0x0000ff;
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
	
#ifdef DEBUG		
	scr_printf("done \n");		
#endif			
#ifndef NOSYSCALL_HOOK		
	// Hooking SifSetDma & SifSetReg Syscalls
	Old_SifSetDma  = GetSyscall(119);
	SetSyscall(119, &HookSifSetDma);

	Old_SifSetReg  = GetSyscall(121);
	SetSyscall(121, &HookSifSetReg);
#endif		
	fioExit();
	LoadFileExit();
	SifExitRpc();

	FlushCache(0);
	FlushCache(2);

	//if (!patch_padRead((void *)elf_header.entry))
	//	goto error;	    		
	patch_padRead((void *)elf_header.entry);

#ifdef DEBUG			
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
#ifdef DEBUG		
		scr_printf("done \n");		
#endif			
#ifndef NOSYSCALL_HOOK		
		// Hooking SifSetDma & SifSetReg Syscalls
		Old_SifSetDma  = GetSyscall(119);
		SetSyscall(119, &HookSifSetDma);

		Old_SifSetReg  = GetSyscall(121);
		SetSyscall(121, &HookSifSetReg);
#endif
		fioExit();
		SifExitRpc();

		FlushCache(0);
		FlushCache(2);

		//if (!patch_padRead((void *)exec.epc))
		//	goto error;
		patch_padRead((void *)exec.epc);	    		
				
#ifdef DEBUG			
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
	while (1){;}
}

