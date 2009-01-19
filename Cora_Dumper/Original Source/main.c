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
#include "r5900_regs.h"

#define _RESIDENT_	__attribute__((section(".resident")))
#define DEBUG
char *bootFileName;
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

	#define MAKE_J(addr)   (u32)(0x08000000 | (0x03FFFFFF & ((u32)addr >> 2)))
	#define MAKE_JAL(addr)   (u32)(0x0C000000 | (0x03FFFFFF & ((u32)addr >> 2)))
	#define KSEG0(x)   ((void*)(((u32)(x)) | 0x80000000))

	static u32 vector;
	static u32 *p_vtbl;
	_RESIDENT_ u32 jal_cheats;

	extern void systemHook(void);
	extern void readPad(void);
	extern int j_defhandler;




void breakPoint(void);
//FUNCTIONS
/////////////////////////////////////////////////////////////////////
//loadModules
/////////////////////////////////////////////////////////////////////
static void loadModules(void)
{
    int ret;
#ifdef ROM_PADMAN
    ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
#else
    ret = SifLoadModule("rom0:XSIO2MAN", 0, NULL);
#endif
    if (ret < 0) {
        SleepThread();
    }
	ret = SifLoadModule("rom0:CDVDMAN", 0, NULL);
	if(ret < 0) {
		SleepThread();
	}

#ifdef ROM_PADMAN
    ret = SifLoadModule("rom0:PADMAN", 0, NULL);
#else
    ret = SifLoadModule("rom0:XPADMAN", 0, NULL);
#endif
    if (ret < 0) {
        SleepThread();
    }
}
///////////////////////////////////////////////////////////////////
//initalise
/////////////////////////////////////////////////////////////////////
void initalise(void)
{
	int ret;

	SifInitRpc(0);
	// init debug screen
	init_scr();
	// load all modules
	loadModules();
	// init pad
	padInit(0);
	if((ret = padPortOpen(0, 0, padBuf)) == 0) {
		#if defined DEBUG
			scr_printf("padOpenPort failed: %d\n", ret);
		#endif
		SleepThread();
	}

	if(!initializePad(0, 0)) {
		#if defined DEBUG
			scr_printf("pad initalization failed!\n");
		#endif
		SleepThread();
	}
}

/////////////////////////////////////////////////////////////////////
//waitPadReady
/////////////////////////////////////////////////////////////////////
static int waitPadReady(int port, int slot)
{
    int state;
    int lastState;
    char stateString[16];

    state = padGetState(port, slot);
    lastState = -1;
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
        }
        lastState = state;
        state=padGetState(port, slot);
    }
    // Were the pad ever 'out of sync'?
    if (lastState != -1) {

    }
    return 0;
}

/////////////////////////////////////////////////////////////////////
//initalizePad
/////////////////////////////////////////////////////////////////////
static int initializePad(int port, int slot)
{

    int ret;
    int modes;
    int i;

    waitPadReady(port, slot);
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
    if (modes > 0) {
        for (i = 0; i < modes; i++) {
        }

    }
    if (modes == 0) {
        return 1;
    }

    i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);
    if (i >= modes) {
        return 1;
    }

    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
        return 1;
    }
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
    padInfoPressMode(port, slot);

    waitPadReady(port, slot);
    padEnterPressMode(port, slot);

    waitPadReady(port, slot);
    actuators = padInfoAct(port, slot, -1, 0);

    if (actuators != 0) {
        actAlign[0] = 0;
        actAlign[1] = 1;
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady(port, slot);

       padSetActAlign(port, slot, actAlign);
    }
    else {
        //printf("Did not find any actuators.\n");
    }
    return 1;
}

/////////////////////////////////////////////////////////////////////
//buttonStatts
/////////////////////////////////////////////////////////////////////
static void buttonStatts(int port, int slot)
{
	int ret;
		ret = padRead(port, slot, &buttons);

        if (ret != 0) {
            paddata = 0xffff ^ buttons.btns;

            new_pad = paddata & ~old_pad;
            old_pad = paddata;
		}
}

/////////////////////////////////////////////////////////////////////
//checkPadConnected
/////////////////////////////////////////////////////////////////////
void checkPadConnected(void)
{
	int ret, i;
	ret=padGetState(0, 0);
	while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) {
		if(ret==PAD_STATE_DISCONN) {
			#if defined DEBUG
	           scr_printf("	Pad(%d, %d) is disconnected\n", 0, 0);
			#endif
		}
		ret=padGetState(0, 0);
	}
	if(i==1) {
		//scr_printf("	Pad: OK!\n");
	}
}

/////////////////////////////////////////////////////////////////////
//pad_wat_button
/////////////////////////////////////////////////////////////////////
void pad_wait_button(u32 button)
{
	while(1)
	{
		buttonStatts(0, 0);
		if(new_pad & button) return;
	}
}



/////////////////////////////////////////////////////////////////////
//getSyscallVTblAddr
/////////////////////////////////////////////////////////////////////
u32 getSyscallVTblAddr(void)
{
   const s32 syscall_num[2] = { 0xFE, 0xFF };
   const u32 magic[2] = { 0x1337C0DE, 0xB16B00B5 };
   u32 addr = -1;
   u32 i;

   SetSyscall(syscall_num[0], (void*)magic[0]);
   SetSyscall(syscall_num[1], (void*)magic[1]);
   ee_kmode_enter();

   for (i = 0x80000000; i < 0x80080000; i += 4) {
      if (!memcmp((u32*)i, magic, sizeof(magic))) {
         addr = i - syscall_num[0] * 4;
         break;
      }
   }

   ee_kmode_exit();
   SetSyscall(syscall_num[0], (void*)0);
   SetSyscall(syscall_num[1], (void*)0);

   return addr;
}

/////////////////////////////////////////////////////////////////////
//getVTblAddr
/////////////////////////////////////////////////////////////////////
int getVTblAddr(void)
{
	//GET SYSCALL VECTOR ADDRESS
	p_vtbl = getSyscallVTblAddr();
	//check for errors
	if(p_vtbl == NULL || p_vtbl == 0){
		scr_printf("	ERROR! Could not get vector address!! Cheat device will not run!\n");
		return -1;
	}
	#if defined DEBUG
		printf("vector table: %0x\n", p_vtbl);
		//scr_printf("	vector table: %0x\n", p_vtbl);
	#endif
	return 0;
}

/////////////////////////////////////////////////////////////////////
//KernelRead
/////////////////////////////////////////////////////////////////////
u32 KernelRead(void *addr, void *buf, u32 size)
{
   DI(); // Disable interrupts
   ee_kmode_enter(); // Enter kernel mode
   memcpy(buf, addr, size);
   ee_kmode_exit(); // Leave kernel mode
   EI(); // Enable interrupts

   return size;
}

/////////////////////////////////////////////////////////////////////
//HookSyscall
/////////////////////////////////////////////////////////////////////
void HookSyscall(int syscall_num, void *myhandler, u32 *j_defhandler)
{
   u32 vector;

   // Read origional handler from syscall vector table
   KernelRead(p_vtbl + syscall_num, &vector, sizeof(vector));

   // Insert jump to the syscall's default handler
   *j_defhandler = MAKE_J(vector);

   // Patch default vector to call my handler first
   SetSyscall(syscall_num, KSEG0(myhandler));
	#if defined DEBUG
   printf("Hooked syscall 0x%02X (old vector: 0x%08X, current: 0x%08X)\n",
      syscall_num, vector, (u32)KSEG0(myhandler));
	#endif
}
/////////////////////////////////////////////////////////////////////
//Dump_memory
/////////////////////////////////////////////////////////////////////
void dump_memory(void)
{
	//int i;
	//for(i = 0; i<10; i++)
	//{
	//	init_scr();
	//	scr_printf("	Hello world!! %i\n", i);
	//}
	//while(1);


	//u32 *memory;
	//int fd;
	//fd = fioOpen("mc0:dump.bin", O_RDONLY);
	//if(fd < 0) return;
	//memcpy(memory, 0x00100000, 0x20000);
	//fioWrite(fd, memory, sizeof(memory));
	//fioClose(fd);
	//free(memory);

}
/////////////////////////////////////////////////////////////////////
//Print_regs
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//Set_break
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//Breakpoint
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//WaitCdReady
/////////////////////////////////////////////////////////////////////
void waitCdReady()
{
	// Block until the dvdrom is ready to take commands
	cdDiskReady(0);

	// Spin up the cd/dvd
	cdStandby();
}

/////////////////////////////////////////////////////////////////////
//ParseSystemCnf
/////////////////////////////////////////////////////////////////////
char *parseSystemCnf()
{
	char *buffer;
	int fd, fdSize;
	int i;

	// Open SYSTEM.CNF on the cdrom, allocate memory for it, terminate the array
	fd = fioOpen("cdrom0:\\SYSTEM.CNF;1", O_RDONLY);
	if(fd < 0) return (char *)ERROR_SYSTEMCNF_FILEIO;

	fdSize = fioLseek(fd, 0, SEEK_END);
	fioLseek(fd, 0, SEEK_SET);

	buffer = malloc(fdSize + 1);
	if(!buffer) return (char *)ERROR_SYSTEMCNF_MEMORY;

	if(fioRead(fd, buffer, fdSize) != fdSize) return (char *)ERROR_SYSTEMCNF_FILEIO;
	fioClose(fd);
	buffer[fdSize] = '\0';

	// Find boot file substring
	buffer = strstr(buffer, "BOOT2");
	buffer += 5;
	while((*buffer == ' ') || (*buffer == '\t')) buffer++;
	buffer++; // bypass '=' character
	while((*buffer == ' ') || (*buffer == '\t')) buffer++;

	i = 0;
	while((buffer[i] != '\n') && (buffer[i] != '\r')) i++;

	// Terminate string at end of boot elf filename
	buffer[i] = '\0';

	// Return pointer to boot elf filename string
	return buffer;
}


int main(void)
{
	initalise();
	scr_printf("				Welcome to cYs Cora Memory Dumper! writen by cYs Driver\n\n");

	if(getVTblAddr() !=0){
		scr_printf("	== Fatal Error ==\n");
		SleepThread();
	}

	while(1)
	{
		//check to see if the pad is still connected
		checkPadConnected();
		//read pad 1
		buttonStatts(0, 0);

		if(new_pad & PAD_CROSS){
			__asm("cross:");
			jal_cheats = MAKE_JAL(&readPad);
			#if defined DEBUG
					printf("jal_cheats: %0x\n",jal_cheats);
			#endif
			//hook the syscall - systemcall number, write_cheat address where it places jal on padread - location at the end of my function
			HookSyscall(0x10, &systemHook, &j_defhandler);
			scr_printf("	Done\n\n");

		}

		if(new_pad & PAD_SQUARE){

			__asm("square:");
			cdStop();

			scr_printf("	Swap CD/DVD's now and close tray\n	Press X to start game\n\n");
			cdTrayReq(0,0x00490000);

			pad_wait_button(PAD_CROSS);

			scr_printf("	Loading...\n");

			//wait for CD to spin up
			waitCdReady();
			if(strlen(bootFileName = parseSystemCnf()) <= 0)
			{
				scr_printf("	== Fatal Error ==\n");
				SleepThread();
			}

			scr_printf("	Loaded Game!");

			// Shutdown
			padPortClose(0, 0);
			SifExitRpc();
			LoadExecPS2((const char *)bootFileName, 0, NULL);

			// Should never get here
			SleepThread();
			return 0;
		}
	}
	

	
	//SifExitRpc();
	return 0;
}