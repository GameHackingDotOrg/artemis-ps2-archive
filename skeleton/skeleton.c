/*skeleton.c

Very badly designed skeleton GUI for Artemis (raw, hacky, scr_printf text on a black background, and
	screen transitions via init_scr(). Just something to get the ball rolling.) Borrowed
	scandalously (heh) from Pukko's padlib - many thanks to him for providing it, to Lukasz for his improvements
	to it, and for everyone else's work involving it. Enjoy, and please - make something better!
	
					- Lazy Bastard
					
					*/

#include <tamtypes.h>
#include <kernel.h>
#include <loadfile.h>
#include <stdio.h>
#include <sifrpc.h>
#include <graph_registers.h>
#include <libpad.h>
#include <string.h>
#include <debug.h>

#define DEBUG_BGCOLOR(col) *((u64 *) 0x120000e0) = (u64) (col)

char* padTypeStr[] = {	"Unsupported controller", "Mouse", "Nejicon",
                                                "Konami Gun", "Digital", "Analog", "Namco Gun",
                                                "DualShock"};

static char *padBuf[2]; 
u32 portConnected[2];
u32 paddata[2];
u32 old_pad[2];
u32 new_pad[2];
u8 actDirect[2][6] = { {0,0,0,0,0,0}, {0,0,0,0,0,0}};

void wait_vsync() 
{
        // Enable the vsync interrupt.
        GS_REG_CSR |= GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);

        // Wait for the vsync interrupt.
        while (!(GS_REG_CSR & (GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0)))) { }

        // Disable the vsync interrupt.
        GS_REG_CSR &= ~GS_SET_CSR(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
                        
}


char currentMenu[25];

void mainMenuS() {
	init_scr();
    scr_printf("\n\n\n\n\n          Project Artemis\n\n"
               "          Welcome to the Project Artemis (terrible) skeleton GUI.\n\n\n\n"
               "         *Start Game\n\n"
               "          Options\n\n");
						strcpy(currentMenu,"mainMenuS");
               
			}
			
void mainMenuO() {
	init_scr();

    scr_printf("\n\n\n\n\n          Project Artemis\n\n"
               "          Welcome to the Project Artemis (terrible) skeleton GUI.\n\n\n\n"
               "          Start Game\n\n"
               "         *Options\n\n");
               strcpy(currentMenu,"mainMenuO");
                
			}

void startMenu1() {
	init_scr();
	scr_printf("\n\n\n\n\n          Welcome to the (terrible) Start menu.\n\n\n\n"
				"         *Choice 1\n\n"
				"          Choice 2\n\n"
				"          Choice 3\n\n");
				strcpy(currentMenu,"startMenu1");
						
			}	
			
void startMenu2() {
	init_scr();
	scr_printf("\n\n\n\n\n          Welcome to the (terrible) Start menu.\n\n\n\n"
				"          Choice 1\n\n"
				"         *Choice 2\n\n"
				"          Choice 3\n\n");
				strcpy(currentMenu,"startMenu2");
				
			}
			
void startMenu3() {
	init_scr();
	scr_printf("\n\n\n\n\n          Welcome to the (terrible) Start menu.\n\n\n\n"
				"          Choice 1\n\n"
				"          Choice 2\n\n"
				"         *Choice 3\n\n");
				strcpy(currentMenu,"startMenu3");
				
			}
			
void optionsMenu1() {
	init_scr();
	scr_printf("\n\n\n\n\n          Welcome to the (terrible) Options menu.\n\n\n\n"
				"         *Choice 1\n\n"
				"          Choice 2\n\n"
				"          Choice 3\n\n");
				strcpy(currentMenu,"optionsMenu1");
				
			}
			
void optionsMenu2() {
	init_scr();
	scr_printf("\n\n\n\n\n          Welcome to the (terrible) Options menu.\n\n\n\n"
				"          Choice 1\n\n"
				"         *Choice 2\n\n"
				"          Choice 3\n\n");
				strcpy(currentMenu,"optionsMenu2");
				
			}
			
void optionsMenu3() {
	init_scr();
	scr_printf("\n\n\n\n\n          Welcome to the (terrible) Options menu.\n\n\n\n"
				"          Choice 1\n\n"
				"          Choice 2\n\n"
				"         *Choice 3\n\n");
				strcpy(currentMenu,"optionsMenu3");
				
			}


void loadmodules(int free)
{
        s32 ret;
        
        if(free == 1)
        {
                if((ret = SifLoadModule("host0:freesio2.irx", 0, NULL)) < 0)
                {
                        scr_printf("Failed to load freesio2.irx module (%d)\n", ret);
                        SleepThread();
                }

                if((ret = SifLoadModule("host0:freepad.irx", 0, NULL)) < 0)
                {
                        scr_printf("Failed to load freepad.irx module (%d)\n", ret);
                        SleepThread();
                }
        }
        else
        {
                if((ret = SifLoadModule("rom0:XSIO2MAN", 0, NULL)) < 0)
                {
                        scr_printf("Failed to load XSIO2MAN module (%d)\n", ret);
                        SleepThread();
                }
                
                if((ret = SifLoadModule("rom0:XPADMAN", 0, NULL)) < 0)
                {
                        scr_printf("Failed to load XPADMAN module (%d)\n", ret);
                        SleepThread();
                }
        }	
}

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

int main(int argc, char **argv)
{
        u32 port;
        struct padButtonStatus buttons;
        int dualshock[2];
        int acts[2];

        SifInitRpc(0);
        init_scr();
        
        mainMenuS(); /*Load main menu text*/
        
        strcpy(currentMenu,"mainMenuS");
        
        
 
        
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
                                scr_printf("Controller (%i) disconnected\n", port);
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

                                        // Values 50 and 200 used because my controllers are worn out :-)

                                
                            			if(new_pad[port] & PAD_LEFT);		
                            			
                            			
                                        if(new_pad[port] & PAD_RIGHT);		
                                        
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"mainMenuO") == 0))   mainMenuS();		
                                        
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"startMenu2") == 0))   startMenu1();
                                        
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"startMenu3") == 0))   startMenu2();
                                        
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"optionsMenu2") == 0))   optionsMenu1();
 
                                        
                                        if((new_pad[port] & PAD_UP) && (strcmp(currentMenu,"optionsMenu3") == 0))   optionsMenu2();
                                       
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"mainMenuS") == 0))   mainMenuO();
                                                                               
                                        	
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"startMenu1") == 0))   startMenu2();	
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"startMenu2") == 0))   startMenu3();
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"optionsMenu1") == 0))   optionsMenu2();
                                        
                                        
                                        if((new_pad[port] & PAD_DOWN) && (strcmp(currentMenu,"optionsMenu2") == 0))   optionsMenu3();
                                        
                                        
                                        if(new_pad[port] & PAD_START); 	
                                        
                                        
                                        if(new_pad[port] & PAD_SELECT); 		
                                        
                                        
                                        if(new_pad[port] & PAD_SQUARE); 		
                                        
                                        
                                        if((new_pad[port] & PAD_TRIANGLE) && (strcmp(currentMenu,"mainMenuS") != 0) && (strcmp(currentMenu,"mainMenuO") != 0))  mainMenuS();	
                                        
                                        
                                        if(new_pad[port] & PAD_CIRCLE);		
                                        
                                        
                                        if((new_pad[port] & PAD_CROSS) && (strcmp(currentMenu,"mainMenuS") == 0))   startMenu1();
                                        
                                        
                                        if((new_pad[port] & PAD_CROSS) && (strcmp(currentMenu,"mainMenuO") == 0))   optionsMenu1();		
		
                                        
                                        
                                        if(new_pad[port] & PAD_L1)			
                                        {	
                                                padStartAct(port, 0, 1); 
                                        }
                                        if(new_pad[port] & PAD_L2)			
                                        {	
                                                padStartAct(port, 0, 0); 
                                        }
                                        
                                        if(new_pad[port] & PAD_L3);
                                        			
                                        if(new_pad[port] & PAD_R1)
                                        {
                                                padStartAct(port, 1, 255);
                                        }
                                        
                                        if(new_pad[port] & PAD_R2)			
                                        {
                                                padStopAct(port, 1);
                                        }
                                        
                                        if(new_pad[port] & PAD_R3);		

                                        if(new_pad[port]);
                                }	
        
                        }
                }
                wait_vsync();
        }

        return 0;
}
