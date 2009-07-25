
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <fileio.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sbv_patches.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <debug.h>
#include <libmc.h>
#include <libcdvd.h>
#include <libpad.h>
#include <graph_registers.h>

extern void core_elf;
extern int  size_core_elf;
extern void memdisk_irx;
extern int  size_memdisk_irx;
extern void eesync_irx;
extern int  size_eesync_irx;
extern void ntpbserver_irx;
extern int  size_ntpbserver_irx;
extern void ps2dev9_irx;
extern int  size_ps2dev9_irx;
extern void ps2ip_irx;
extern int  size_ps2ip_irx;
extern void ps2smap_irx;
extern int  size_ps2smap_irx;

#define MAX_PATH 1024

char elf_path[MAX_PATH];	   
  
#define ELF_MAGIC		0x464c457f
#define ELF_PT_LOAD		1

typedef struct {
	void *irxaddr;
	int irxsize;
} irxptr_t;

#define IRX_NUM 		6

//------------------------------
typedef struct
{
	u8	ident[16];			// struct definition for ELF object header
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

typedef struct
{
	u32	type;				// struct definition for ELF program section header
	u32	offset;
	void	*vaddr;
	u32	paddr;
	u32	filesz;
	u32	memsz;
	u32	flags;
	u32	align;
} elf_pheader_t;

static char padBuf[256] __attribute__((aligned(64)));
static struct padButtonStatus buttons;

#define WAIT_VSYNC \
	do { GS_REG_CSR |= GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0); \
	while (!(GS_REG_CSR & (GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0)))); \
	GS_REG_CSR &= ~GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0); } while (0)

//--------------------------------------------------------------
int Launch_Core(char *boot_path)
{ 
	char *argv[1];
	u8 *boot_elf; 
	elf_header_t *eh; 
	elf_pheader_t *eph; 
	int i;

	argv[0] = boot_path;
	
	// The core elf is embedded
	boot_elf = (u8 *)&core_elf; 
	eh = (elf_header_t *)boot_elf;
	
	if (_lw((u32)&eh->ident) != ELF_MAGIC) 
	   while (1); 

	eph = (elf_pheader_t *)(boot_elf + eh->phoff); 

	for (i = 0; i < eh->phnum; i++) { 
		if (eph[i].type != ELF_PT_LOAD) 
			continue; 

		memcpy(eph[i].vaddr, (void *)(boot_elf + eph[i].offset), eph[i].filesz); 

		if (eph[i].memsz > eph[i].filesz) 
			memset(eph[i].vaddr + eph[i].filesz, 0, eph[i].memsz - eph[i].filesz); 
	}

	fioExit(); 
	SifInitRpc(0); 
	SifExitRpc(); 
	FlushCache(0); 
	FlushCache(2); 

	ExecPS2((void *)eh->entry, 0, 1, argv); 
	
	return 0;
}

//----------------------------------------------------------------
int	get_CNF_string(u8 **CNF_p_p, u8 **name_p_p, u8 **value_p_p)
{	// taken from uLaunchElf !
	
	u8 *np, *vp, *tp = *CNF_p_p;

start_line:
	while((*tp<=' ') && (*tp>'\0')) tp+=1;  //Skip leading whitespace, if any
	if(*tp=='\0') return 0;            		//but exit at EOF
	np = tp;                                //Current pos is potential name
	if(*tp<'A')                             //but may be a comment line
	{                                       //We must skip a comment line
		while((*tp!='\r')&&(*tp!='\n')&&(*tp>'\0')) tp+=1;  //Seek line end
		goto start_line;                    //Go back to try next line
	}

	while((*tp>='A')||((*tp>='0')&&(*tp<='9'))) tp+=1;  //Seek name end
	if(*tp=='\0') return -1;          		//but exit at EOF

	while((*tp<=' ') && (*tp>'\0'))
		*tp++ = '\0';                       //zero&skip post-name whitespace
	if(*tp!='=') return -2;	                //exit (syntax error) if '=' missing
	*tp++ = '\0';                           //zero '=' (possibly terminating name)

	while((*tp<=' ') && (*tp>'\0')          //Skip pre-value whitespace, if any
		&& (*tp!='\r') && (*tp!='\n')		//but do not pass the end of the line
		)tp+=1;								
	if(*tp=='\0') return -3;          		//but exit at EOF
	vp = tp;                                //Current pos is potential value

	while((*tp!='\r')&&(*tp!='\n')&&(*tp!='\0')) tp+=1;  //Seek line end
	if(*tp!='\0') *tp++ = '\0';             //terminate value (passing if not EOF)
	while((*tp<=' ') && (*tp>'\0')) tp+=1;  //Skip following whitespace, if any

	*CNF_p_p = tp;                          //return new CNF file position
	*name_p_p = np;                         //return found variable name
	*value_p_p = vp;                        //return found variable value
	return 1;                           	//return control to caller
}	//Ends get_CNF_string

//----------------------------------------------------------------
int ParseSYSTEMCNF(char *system_cnf, char *boot_path)
{
	int r, entrycnt, cnfsize;
	u8 *pcnf, *pcnf_start, *name, *value;
	int fd = -1;
	
	fd = open(system_cnf, O_RDONLY);
	if (fd < 0)
		return -1;		

	cnfsize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	pcnf = (char *)malloc(cnfsize);
	pcnf_start = pcnf;
	if (!pcnf) {
		close(fd);
		return -2;
	}
	
	r = read(fd, pcnf, cnfsize);    // Read CNF in one pass
	if (r != cnfsize)
		return -3;
		
	close(fd);
	pcnf[cnfsize] = 0; 				// Terminate the CNF string
	
	for (entrycnt = 0; get_CNF_string(&pcnf, &name, &value); entrycnt++) {
		if (!strcmp(name,"BOOT2"))  // check for BOOT2 entry
			strcpy(boot_path, value);			
	}
			
	pcnf = pcnf_start;
	free(pcnf);
	
	return 1;
}

//-------------------------------------------------------------- 
void SendIrxKernelRAM(void)
{
	u32 *total_irxsize = (u32 *)0x80030000;
	void *irxtab = (void *)0x80030010;
	void *irxptr = (void *)0x80030100;
	irxptr_t irxptr_tab[IRX_NUM];
	void *irxsrc[IRX_NUM];
	int i;
	u32 irxsize;

	irxptr_tab[0].irxsize = size_memdisk_irx; 
	irxptr_tab[1].irxsize = size_eesync_irx; 
	irxptr_tab[2].irxsize = size_ntpbserver_irx; 	
	irxptr_tab[3].irxsize = size_ps2dev9_irx; 
	irxptr_tab[4].irxsize = size_ps2ip_irx; 
	irxptr_tab[5].irxsize = size_ps2smap_irx; 
	
	irxsrc[0] = (void *)&memdisk_irx;
	irxsrc[1] = (void *)&eesync_irx;
	irxsrc[2] = (void *)&ntpbserver_irx;	
	irxsrc[3] = (void *)&ps2dev9_irx;
	irxsrc[4] = (void *)&ps2ip_irx;
	irxsrc[5] = (void *)&ps2smap_irx;
	
	irxsize = 0;		

	DIntr();
	ee_kmode_enter();
				
	for (i = 0; i <	IRX_NUM; i++) {
		irxptr_tab[i].irxaddr = irxptr; 
		
		memcpy((void *)irxptr_tab[i].irxaddr, (void *)irxsrc[i], irxptr_tab[i].irxsize);
		
		irxptr += irxptr_tab[i].irxsize;
		irxsize += irxptr_tab[i].irxsize;
	}
	
	memcpy((void *)irxtab, (void *)&irxptr_tab[0], sizeof(irxptr_tab));	
	
	*total_irxsize = irxsize;
	
	ee_kmode_exit();
	EIntr();
}	

//-------------------------------------------------------------- 
void waitPadReady(int port, int slot)
{
	int state;
	
	do {
		state = padGetState(port, slot);
	} while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1));
}

//-------------------------------------------------------------- 
void wait_X_press(void)
{
	static u32 paddata, new_pad, old_pad = 0;

	while (1) {
		WAIT_VSYNC;
		waitPadReady(0, 0);
		
		if (!padRead(0, 0, &buttons))
			continue;
			
		paddata = 0xffff ^ buttons.btns;
		new_pad = paddata & ~old_pad;
		old_pad = paddata;
		
		if (new_pad & PAD_CROSS)
			break;
	}
}

//-------------------------------------------------------------- 
int main(int argc, char *argv[])
{	
	init_scr();
	scr_clear();
	scr_printf("\t CORE LAUNCHER V2 - v0.7d\n\n");
	
	SifInitRpc(0);
	
	// Perform a full IOP reset
	while(!SifIopReset("rom0:UDNL rom0:EELOADCNF",0));
  	while(!SifIopSync());
  	fioExit();
  	SifExitIopHeap();
  	SifLoadFileExit();
  	SifExitRpc();
  	SifExitCmd();
  	
  	SifInitRpc(0);
  	FlushCache(0);
  	FlushCache(2);
 	    		  	
	SifLoadFileInit();
	SifInitIopHeap();
	
    sbv_patch_enable_lmb();
    sbv_patch_disable_prefix_check();
	              	    
    SifLoadModule("rom0:SIO2MAN", 0, 0);
    SifLoadModule("rom0:MCMAN", 0, 0);
    SifLoadModule("rom0:MCSERV", 0, 0);
    SifLoadModule("rom0:PADMAN", 0, 0);

	padInit(0);
	padPortOpen(0, 0, padBuf);
            	
    // Send all IRX modules to Kernel ram for the Core elf to get it
	SendIrxKernelRAM();
        	
	cdInit(CDVD_INIT_INIT);
	//cdSetMediaMode(2);

	cdDiskReady(0);
	cdStop();
	cdSync(0);	

	scr_printf("\t You can swap Disc if needed...\n");
	scr_printf("\t Press X to continue.\n");
	wait_X_press();	

	padPortClose(0, 0);
	padReset();
		
	cdDiskReady(0);
	cdStandby();
	cdSync(0);
	
    // SYSTEM.CNF parsing to find main game boot elf	
	ParseSYSTEMCNF("cdrom0:\\SYSTEM.CNF;1", elf_path);
	
	//strcpy(elf_path, "mc0:/SLUS_201.44");
	scr_printf("\t Game Elf path = \"%s\"\n", elf_path);
			
	// Execute core elf
    Launch_Core(elf_path);    
      	  
   	SleepThread();
   	return 0;
}
