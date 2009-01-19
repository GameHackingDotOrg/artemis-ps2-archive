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
#include "disassembler_regs.h"
#include "defines.h"

#define VERSION "v0.1a"
//#define _useGS 1
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
#ifdef _useGS
	u64 White, Black, BlackFont, WhiteFont, RedFont, GreenFont, BlueFont, BlueTrans, RedTrans, GreenTrans, WhiteTrans;
	GSGLOBAL *gsGlobal = gsKit_init_global(GS_MODE_NTSC);

	GSFONT *gsFont = gsKit_init_font(GSKIT_FTYPE_FONTM, NULL);

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);
	
	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
	
	WhiteFont = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x80,0x00);
	BlackFont = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
	RedFont = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x80,0x00);
	GreenFont = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x80,0x00);
	BlueFont = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x80,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

	gsGlobal->PrimAlpha = GS_BLEND_FRONT2BACK;

	gsKit_init_screen(gsGlobal);

	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

	gsKit_font_upload(gsGlobal, gsFont);

	gsFont->FontM_Spacing = 0.9f;
	
	gsKit_clear(gsGlobal, Black);


	char *welcome = "";
	welcome = (char*)malloc(sizeof(char)*50);
	sprintf(welcome,"Welcome to Driver's disassembler %s\n\n",VERSION);
	gsKit_font_print_scaled(gsGlobal, gsFont, 10, 10, 3, 0.6f, WhiteFont, welcome);
	gsKit_sync_flip(gsGlobal);
	gsKit_queue_exec(gsGlobal);
#else 
	scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
#endif

	u32 address = 0x0010013c;
	u32 history = address;
	opPacket opp;
	int count = 1;
	history = (u32*)malloc(count * sizeof(u32));

	opp = execOp(address);
#ifdef _useGS
	gsKit_font_print_scaled(gsGlobal, gsFont, 10, 45, 3, 0.3f, WhiteFont, opp.opString);
	gsKit_sync_flip(gsGlobal);
	gsKit_queue_exec(gsGlobal);
#else
	scr_printf("%s",opp.opString);
#endif
	while(1){
		memset(&opp.opString, 0, sizeof(opp.opString));
		checkPadConnected();
		read_pad(0, 0);
		if(new_pad & PAD_UP){
			address +=4;
			opp = execOp(address);
#ifdef _useGS
			gsKit_clear(gsGlobal, Black);
			gsKit_font_print_scaled(gsGlobal, gsFont, 10, 10, 3, 0.6f, WhiteFont, welcome);
			printf("opp.opString = %s",opp.opString);
			gsKit_font_print_scaled(gsGlobal, gsFont, 10, 45, 3, 0.4f, WhiteFont, opp.opString);

			gsKit_sync_flip(gsGlobal);
			gsKit_queue_exec(gsGlobal);
#else
			scr_clear();
			scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
			scr_printf("%s",opp.opString);
#endif

		}else if(new_pad & PAD_DOWN){
			address -=4;
			opp = execOp(address);
#ifdef _useGS
			gsKit_clear(gsGlobal, Black);
			gsKit_font_print_scaled(gsGlobal, gsFont, 10, 10, 3, 0.6f, WhiteFont, welcome);
			gsKit_font_print_scaled(gsGlobal, gsFont, 10, 45, 3, 0.4f, WhiteFont, opp.opString);
			gsKit_sync_flip(gsGlobal);
			gsKit_queue_exec(gsGlobal);
#else
			scr_clear();
			scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
			scr_printf("%s",opp.opString);
#endif
		}else if(new_pad & PAD_RIGHT){
			if(opp.isTargetUsed){
				count++;
				realloc(history, count * sizeof(u32));
				_sw(address,history+(count*4));
				address = opp.target;
				opp = execOp(address);
#ifdef _useGS
				gsKit_clear(gsGlobal, Black);
				gsKit_font_print_scaled(gsGlobal, gsFont, 10, 10, 3, 0.6f, WhiteFont, welcome);
				gsKit_font_print_scaled(gsGlobal, gsFont, 10, 45, 3, 0.4f, WhiteFont, opp.opString);
				gsKit_sync_flip(gsGlobal);
				gsKit_queue_exec(gsGlobal);
#else
				scr_clear();
				scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
				scr_printf("%s",opp.opString);
#endif
			}
		}else if(new_pad & PAD_LEFT){
			u32 t_address = _lw(history+(count*4));
			if(t_address > 0x00100000){
				address = t_address;
				count--;
				if(count < 1){count = 1;}
				realloc(history, count * sizeof(u32));
				opp = execOp(address);
#ifdef _useGS
				gsKit_clear(gsGlobal, Black);
				gsKit_font_print_scaled(gsGlobal, gsFont, 10, 10, 3, 0.6f, WhiteFont, welcome);
				gsKit_font_print_scaled(gsGlobal, gsFont, 10, 45, 3, 0.4f, WhiteFont, opp.opString);
				gsKit_sync_flip(gsGlobal);
				gsKit_queue_exec(gsGlobal);
#else
				scr_clear();
				scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
				scr_printf("%s",opp.opString);
#endif
			}
		}
	}

    return 0;
}