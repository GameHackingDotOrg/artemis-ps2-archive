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
#include "disassembler_regs.h"
#include "defines.h"

#define VERSION "v0.1a"

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
static void read_pad(int port, int slot)
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
		read_pad(0, 0);
		if(new_pad & button) return;
	}
}

int main(void)
{
	initalise();
	scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
	u32 address = 0x0010013c;
	u32 history = address;
	opPacket opp;
	int count = 1;
	history = (u32*)malloc(count * sizeof(u32));
	scr_printf("	pointer: %8.8x\n",&history);
	opp = execOp(address);
	while(1){
		checkPadConnected();
		read_pad(0, 0);
		if(new_pad & PAD_UP){
			address +=4;
			scr_clear();
			scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
			opp = execOp(address);
		}else if(new_pad & PAD_DOWN){
			address -=4;
			scr_clear();
			scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
			opp = execOp(address);
		}else if(new_pad & PAD_RIGHT){
			if(opp.isTargetUsed){
				count++;
				realloc(history, count * sizeof(u32));
				_sw(address,history+(count*4));
				address = opp.target;
				scr_clear();
				scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
				opp = execOp(address);
			}
		}else if(new_pad & PAD_LEFT){
			u32 t_address = _lw(history+(count*4));
			if(t_address > 0x00100000){
				address = t_address;
				count--;
				if(count < 1){count = 1;}
				realloc(history, count * sizeof(u32));
				scr_clear();
				scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
				opp = execOp(address);
			}
		}
	}

    return 0;
}