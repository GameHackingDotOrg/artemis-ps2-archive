/*
====================================================================================
  _____               __     __                _____                  _       
 / ____|              \ \   / /               / ____|                (_)      
| |     __ _ _ __ _____\ \_/ /__  _   _ _____| (___  _   _ _ ____   _____   _____ 
| |    / _` | '_ \______\   / _ \| | | |______\___ \| | | | '__\ \ / / \ \ / / _ \
| |___| (_| | | | |      | | (_) | |_| |      ____) | |_| | |   \ V /| |\ V /  __/
 \_____\__,_|_| |_|      |_|\___/ \__,_|     |_____/ \__,_|_|    \_/ |_| \_/ \___|
 ====================================================================================
			=|	|				 .--.								|	|=
			=|	|				:									|	|=
			=|	|				|    .-. .--..-.  ©					|	|=
			=|	|				:   (   )|  (   )					|	|=
			=|	|				 `--'`-' '   `-'`-					|	|=
			=|	|					Cheat Device					|	|=
 ====================================================================================
 = Author: Jordan Hamill / cYs Driver
 = Notes:
 ==
 ==
 */

#include "main.h"
#include "ctype.h"
//PREDEFINED VARIABLES
	//version number:
	char *_VERSION_ = "1.0a";

	//Debug mode:
	//#define DEBUG

	//syscall number to hook
	int syscall_num = 0x10;
	u32 jal_scr_init;
//FNC Protos
static void pad_wait_button(u32 button);
static void buttonStatts(int port, int slot);
static int initializePad(int port, int slot);
static int waitPadReady(int port, int slot);
static void loadModules(void);

extern int j_defhandler;
extern void write_cheats(void) _RESIDENT_;
extern void cheat_hook(void) _RESIDENT_;

//FNCS
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

/////////////////////////////////////////////////////////////////////
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

   // Read vector from syscall vector table
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
//							MAIN FUNCTION
/////////////////////////////////////////////////////////////////////
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


int main(void)
{
	int i;
	initalise();

	//welcome message
	scr_printf("				Welcome to cYs Cora %s writen by cYs Driver\n\n", _VERSION_);
	scr_printf("	Press Square to load Game\n\n");
	
	//get vector table address
	if(getVTblAddr() !=0){
		scr_printf("	== Fatal Error ==\n");
		SleepThread();
	}
	jal_cheats = MAKE_JAL(&write_cheats);
	jal_scr_init = MAKE_JAL(&start_screen);

	#if defined DEBUG
			printf("jal_cheats: %0x\n",jal_cheats);
			scr_printf("	jal_cheats: %0x\n",jal_cheats);
	#endif
	//hook the syscall - systemcall number, write_cheat address where it places jal on padread - location at the end of my function
	HookSyscall(syscall_num, &cheat_hook, &j_defhandler);
	scr_printf("	Done\n\n");

	//main loop
	while(1)
	{
		//check to see if the pad is still connected
		checkPadConnected();
		//read pad 1
		buttonStatts(0, 0);
		//square is pressed
		if(new_pad & PAD_SQUARE){
			//stop Disk spinning
			cdStop();

			scr_printf("	Swap CD/DVD's now and close tray\n	Press X to start game\n\n");
			//eject tray
			cdTrayReq(0,0x00490000);
			//wait for X to be pressed
			pad_wait_button(PAD_CROSS);
			//close tray
			if(_lw(0x00490000) == 0)
			{
				cdTrayReq(1,0x00490000);
			}

			scr_printf("	Loading...\n");
			//wait for CD to spin up
			waitCdReady();
			//get elf name and check for an error
			if(strlen(bootFileName = parseSystemCnf()) <= 0)
			{
				scr_printf("	== Fatal Error ==\n");
				SleepThread();
			}
			#if defined DEBUG
				scr_printf("	%s\n",bootFileName);
			#endif
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
	return 0;
}
