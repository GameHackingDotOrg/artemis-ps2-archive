//---------------------------------------------------------------------------
// agui.c
// Author: Lazy Bastard, LBGSHI@gmail.com
//
// Modules in this source are based on a demo by Tony Saveski, and I utilize his g2 graphics
// library and his BMP2C application. Without his work, I would have been forced to use gsKit or gslib,
// or to just manually force the PS2 to draw letters. A big thanks to him.
//---------------------------------------------------------------------------
#include "inc/tamtypes.h"
#include "inc/kernel.h"
#include "inc/libpad.h"
#include "inc/loadfile.h"
#include <stdio.h>
#include "inc/sifrpc.h"
#include "inc/graph_registers.h"
#include <string.h>


#include "g2.h"
#include "gs.h"

extern uint32 ssplash_w;
extern uint32 ssplash_h;
extern uint32 ssplash[];
extern uint32 sMainS_w;
extern uint32 sMainS_h;
extern uint32 sMainS[];
extern uint32 sMainO_w;
extern uint32 sMainO_h;
extern uint32 sMainO[];
extern uint32 sStartS_w;
extern uint32 sStartS_h;
extern uint32 sStartS[];
extern uint32 sStartC_w;
extern uint32 sStartC_h;
extern uint32 sStartC[];
extern uint32 sCodeA_w;
extern uint32 sCodeA_h;
extern uint32 sCodeA[];
extern uint32 sOptions1_w;
extern uint32 sOptions1_h;
extern uint32 sOptions1[];
extern uint32 sOptions2_w;
extern uint32 sOptions2_h;
extern uint32 sOptions2[];
extern uint32 sOptions3_w;
extern uint32 sOptions3_h;
extern uint32 sOptions3[];


static uint16 maxx, maxy;

char* padTypeStr[] = {	"Unsupported controller", "Mouse", "Nejicon",
                                                "Konami Gun", "Digital", "Analog", "Namco Gun",
                                                "DualShock"};

static char *padBuf[2]; 
u32 portConnected[2];
u32 paddata[2];
u32 old_pad[2];
u32 new_pad[2];
u8 actDirect[2][6] = { {0,0,0,0,0,0}, {0,0,0,0,0,0}};

char currentMenu[25] = "Splash";
int delaycounter = 3;

//--------------------------------------------------------------------------
void wait_vsync() 
{
        // Enable the vsync interrupt.
        GS_REG_CSR |= GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);

        // Wait for the vsync interrupt.
        while (!(GS_REG_CSR & (GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0)))) { }

        // Disable the vsync interrupt.
        GS_REG_CSR &= ~GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
                        
}

//---------------------------------------------------------------------------
void a_clear_screen(void)		
{
		// clear the screen
		g2_set_fill_color(0, 0, 0);
		g2_fill_rect(0, 0, maxx, maxy);
}		
		
//---------------------------------------------------------------------------
void a_g2_init(void)
{
	if(gs_is_ntsc())
		g2_init(NTSC_512_224_32);
	else
		g2_init(PAL_512_256_32);

	maxx = g2_get_max_x();
	maxy = g2_get_max_y();

	// clear the screen
	g2_set_fill_color(0, 0, 0);
	g2_fill_rect(0, 0, maxx, maxy);

	// draw next frame to hidden buffer
	g2_set_active_frame(1);
	g2_set_fill_color(0, 0, 0);
	g2_fill_rect(0, 0, maxx, maxy);
}


//---------------------------------------------------------------------------
void flip_buffers(void)
{
	g2_wait_vsync();
	g2_set_visible_frame(1 - g2_get_visible_frame());
	g2_set_active_frame(1 - g2_get_active_frame());
}

//---------------------------------------------------------------------------

void Splash(void)
{

		g2_put_image(0, 0, ssplash_w, ssplash_h, ssplash);
		flip_buffers();
		strcpy(currentMenu,"MainS");
		
}

//---------------------------------------------------------------------------

void MainS(void)
{

		a_clear_screen();
		g2_put_image(0, 0, sMainS_w, sMainS_h, sMainS);
		flip_buffers();
		strcpy(currentMenu,"MainS");
		
}

//---------------------------------------------------------------------------

void MainO(void)
{

		a_clear_screen();
		g2_put_image(0, 0, sMainO_w, sMainO_h, sMainO);
		flip_buffers();
		strcpy(currentMenu,"MainO");
		
}

//---------------------------------------------------------------------------

void StartS(void)
{

		a_clear_screen();
		g2_put_image(0, 0, sStartS_w, sStartS_h, sStartS);
		flip_buffers();
		strcpy(currentMenu,"StartS");
}

//---------------------------------------------------------------------------

void StartC(void)
{

		a_clear_screen();
		g2_put_image(0, 0, sStartC_w, sStartC_h, sStartC);
		flip_buffers();
		strcpy(currentMenu,"StartC");
		
}

//---------------------------------------------------------------------------

void CodeA(void)
{

		a_clear_screen();
		g2_put_image(0, 0, sCodeA_w, sCodeA_h, sCodeA);
		flip_buffers();
		strcpy(currentMenu,"CodeA");
		
}

//---------------------------------------------------------------------------

void Options1(void)
{

		a_clear_screen();
		g2_put_image(0, 0, sOptions1_w, sOptions1_h, sOptions1);
		flip_buffers();
		strcpy(currentMenu,"Options1");
}

//---------------------------------------------------------------------------

void Options2(void)
{

		a_clear_screen();
		g2_put_image(0, 0, sOptions2_w, sOptions2_h, sOptions2);
		flip_buffers();
		strcpy(currentMenu,"Options2");
}

//---------------------------------------------------------------------------

void Options3(void)
{

		a_clear_screen();
		g2_put_image(0, 0, sOptions3_w, sOptions3_h, sOptions3);
		flip_buffers();
		strcpy(currentMenu,"Options3");
}

//---------------------------------------------------------------------------


void backgroundcall(void)
{

		if ((strcmp(currentMenu,"MainS") == 0))
		MainS();
		if ((strcmp(currentMenu,"MainO") == 0))
		MainO();
		if ((strcmp(currentMenu,"StartS") == 0))
		StartS();
		if ((strcmp(currentMenu,"StartC") == 0))
		StartC();
		if ((strcmp(currentMenu,"CodeA") == 0))
		CodeA();
		if ((strcmp(currentMenu,"Options1") == 0))
		Options1();
		if ((strcmp(currentMenu,"Options2") == 0))
		Options2();
		if ((strcmp(currentMenu,"Options3") == 0))
		Options3();
		
		if ((strcmp(currentMenu,"Splash") == 0))
		{
		Splash();
		for(delaycounter=0; delaycounter<0x1FFFFFF; delaycounter++) {} // delay
		
		}
}

//---------------------------------------------------------------------------


void loadmodules(int free)
{
        s32 ret;
        
        SifExitIopHeap ();
    	SifLoadFileExit();
    	SifExitRpc     ();
    	SifIopReset ( "rom0:UDNL rom0:EELOADCNF", 0 );
    	while (  SifIopSync ()  );
    	SifInitRpc ( 0 ); 
        
        if(free == 1)
        {
                if((ret = SifLoadModule("host0:freesio2.irx", 0, NULL)) < 0)
                {
                        SleepThread();
                }

                if((ret = SifLoadModule("host0:freepad.irx", 0, NULL)) < 0)
                {
                        SleepThread();
                }
        }
        else
        {
                if((ret = SifLoadModule("rom0:XSIO2MAN", 0, NULL)) < 0)
                {
                        SleepThread();
                }
                
                if((ret = SifLoadModule("rom0:XPADMAN", 0, NULL)) < 0)
                {
                        SleepThread();
                }
        }	
}

//---------------------------------------------------------------------------


void padWait(int port)
{
        /* Wait for request to complete. */
        while(padGetReqState(port, 0) != PAD_RSTAT_COMPLETE)
                wait_vsync();

        /* Wait for pad to be stable. */
        while(padGetState(port, 0) != PAD_STATE_STABLE)
                wait_vsync();
}

void padStartAct(int port, int act, int speed)
{
        if(actDirect[port][act] != speed)
        {
                actDirect[port][act] = speed;	
        
                padSetActDirect(port, 0, actDirect[port]);
                padWait(port);
        }
}

void padStopAct(int port, int act)
{
        padStartAct(port, act, 0);
}


//---------------------------------------------------------------------------


int main(int argc, char **argv)
{

	
	u32 port;
        struct padButtonStatus buttons;
        int dualshock[2];
        int acts[2];

        SifInitRpc(0);

        
        
	a_g2_init();
		
	
	
	if((argc == 2) && (strncmp(argv[1], "free", 4) == 0))
        {
                loadmodules(1);	
        }
        else
        {
                loadmodules(0);
        }
        
        
        
	padInit(0);
        
        padBuf[0] = memalign(64, 256);
        padBuf[1] = memalign(64, 256);

        old_pad[0] = 0;
        old_pad[1] = 0;	

        portConnected[0] = 0;
        portConnected[1] = 0;

        dualshock[0] = 0;
        dualshock[1] = 0;

        acts[0] = 0;
        acts[1] = 0;

        padPortOpen(0, 0, padBuf[0]);
        padPortOpen(1, 0, padBuf[1]);
        
        
        
        
        
        while(1)
        {	
	        
	        
	        backgroundcall();

	        
	        
	        
                for(port=0; port < 2; port++)
                {
                        s32 state = padGetState(port, 0);

                        if((state == PAD_STATE_STABLE) && (portConnected[port] == 0)) 
                        {
                                u32 i;
                                u8 mTable[8];
                                u32 ModeCurId;
                                u32 ModeCurOffs;
                                u32 ModeCurExId;
                                u32 ModeTableNum = padInfoMode(port, 0, PAD_MODETABLE, -1);
                                

                                /* Check if dualshock and if so, activate analog mode */
                                for(i = 0; i < ModeTableNum; i++)
                                        mTable[i] = padInfoMode(port, 0, PAD_MODETABLE, i);
                                
                                /* Works for dualshock2 */
                                if((mTable[0] == 4) && (mTable[1] == 7) && (ModeTableNum == 2))
                                        dualshock[port] = 1;

                                /* Active and lock analog mode */
                                if(dualshock[port] == 1)
                                {
                                        padSetMainMode(port, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);
                                        padWait(port);
                                }
                                
                                ModeCurId = padInfoMode(port, 0, PAD_MODECURID, 0);
                                ModeCurOffs = padInfoMode(port, 0, PAD_MODECUROFFS, 0);
                                ModeCurExId = padInfoMode(port, 0, PAD_MODECUREXID, 0);
                                ModeTableNum = padInfoMode(port, 0, PAD_MODETABLE, -1);
                                acts[port] = padInfoAct(port, 0, -1, 0);		

                
                                for(i = 0; i < ModeTableNum; i++)
                                {
                                        mTable[i] = padInfoMode(port, 0, PAD_MODETABLE, i);
                                }	


        
                                if(acts[port] > 0)
                                {
                                        u8 actAlign[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                                        u32 i;

                                        /* Set offsets for motor parameters for SetActDirect. */
                                        for(i=0; i < acts[port]; i++)
                                                actAlign[i] = i;

                                        padSetActAlign(port, 0, actAlign);
                                        padWait(port);
                                }

                                padWait(port);


                                portConnected[port] = 1;
                        }

                        if((state == PAD_STATE_DISCONN) && (portConnected[port] == 1))
                        { 
                                portConnected[port] = 0;
                        }

                        if(portConnected[port] == 1)
                        {
                                s32 ret = padRead(port, 0, &buttons);

                                if(ret != 0)
                                {
                                        paddata[port] = 0xffff ^ buttons.btns;
        
                                        new_pad[port] = paddata[port] & ~old_pad[port];
                                        old_pad[port] = paddata[port];
                                        
                                        
                                        
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"MainS") == 0)) {   
	                                        strcpy(currentMenu,"MainO");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"MainS") == 0)) {   
	                                        strcpy(currentMenu,"MainO");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_CROSS) && (strcmp(currentMenu,"MainS") == 0)) {   
	                                        strcpy(currentMenu,"StartS");
	                                        continue;
                                        }
                                        
                                        
                                       if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"MainO") == 0)) {   
	                                        strcpy(currentMenu,"MainS");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"MainO") == 0)) {   
	                                        strcpy(currentMenu,"MainS");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_CROSS) && (strcmp(currentMenu,"MainO") == 0)) {   
	                                        strcpy(currentMenu,"Options1");
	                                        continue;
                                        }
                                        
                                        
                                        
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"StartS") == 0)) {   
	                                        strcpy(currentMenu,"StartC");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"StartS") == 0)) {   
	                                        strcpy(currentMenu,"StartC");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_TRIANGLE) && (strcmp(currentMenu,"StartS") == 0)) {   
	                                        strcpy(currentMenu,"MainS");
	                                        continue;
                                        }
                                        
                                        
                                      
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"StartC") == 0)) {   
	                                        strcpy(currentMenu,"StartS");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"StartC") == 0)) {   
	                                        strcpy(currentMenu,"StartS");
	                                        continue;
                                        }
                                        
                                        if((new_pad[port] & PAD_CROSS) && (strcmp(currentMenu,"StartC") == 0)) {   
	                                        strcpy(currentMenu,"CodeA");
	                                        continue;
                                        }
                                        
                                        if((new_pad[port] & PAD_TRIANGLE) && (strcmp(currentMenu,"StartC") == 0)) {   
	                                        strcpy(currentMenu,"MainS");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_TRIANGLE) && (strcmp(currentMenu,"CodeA") == 0)) {   
	                                        strcpy(currentMenu,"StartS");
	                                        continue;
                                        }
                                        
                                      
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"Options1") == 0)) {   
	                                        strcpy(currentMenu,"Options3");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"Options1") == 0)) {   
	                                        strcpy(currentMenu,"Options2");
	                                        continue;
                                        }
                                        
                                       
                                        
                                        if((new_pad[port] & PAD_TRIANGLE) && (strcmp(currentMenu,"Options1") == 0)) {   
	                                        strcpy(currentMenu,"MainS");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"Options2") == 0)) {   
	                                        strcpy(currentMenu,"Options1");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"Options2") == 0)) {   
	                                        strcpy(currentMenu,"Options3");
	                                        continue;
                                        }
                                        
                                       
                                        
                                        if((new_pad[port] & PAD_TRIANGLE) && (strcmp(currentMenu,"Options2") == 0)) {   
	                                        strcpy(currentMenu,"MainS");
	                                        continue;
                                        }
                                        
                                        
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"Options3") == 0)) {   
	                                        strcpy(currentMenu,"Options2");
	                                        continue;
                                        }
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"Options3") == 0)) {   
	                                        strcpy(currentMenu,"Options1");
	                                        continue;
                                        }
                                        
                                       
                                        
                                        if((new_pad[port] & PAD_TRIANGLE) && (strcmp(currentMenu,"Options3") == 0)) {   
	                                        strcpy(currentMenu,"MainS");
	                                        continue;
                                        }
                                        
                                        
                                        
                                        

                                        if(new_pad[port]);
                                        
	}	

                        }
                }
                wait_vsync();
        }
	

	g2_end();
	return(0);
}
