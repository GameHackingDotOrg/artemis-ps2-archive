/*skeleton.c

Very badly designed skeleton GUI for Artemis (raw, hacky, scr_printf text on a black background, and
	screen transitions via init_scr(). Just something to get the ball rolling.) Borrowed
	scandalously (heh) from Pukko's padlib - many thanks to him for providing it, and for everyone else's
	work in improving it. Enjoy, and please - make something better!
	
					- Lazy Bastard
																				*/
				

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <debug.h>
#include <string.h>

#include "libpad.h"

#define ROM_PADMAN

#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
#endif

#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#error ROM_PADMAN or NEW_PADMAN must be defined!
#endif

static char padBuf[256] __attribute__((aligned(64)));

static char actAlign[6];
static int actuators;



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
			
			


 
void
loadModules(void)
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

#ifdef ROM_PADMAN
    ret = SifLoadModule("rom0:PADMAN", 0, NULL);
#else
    ret = SifLoadModule("rom0:XPADMAN", 0, NULL);
#endif 
    if (ret < 0) {
  
        SleepThread();
    }
}


int waitPadReady(int port, int slot)
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
    // Was the pad ever 'out of sync'?
    if (lastState != -1) {
     
    }
    return 0;
}


int
initializePad(int port, int slot)
{

    int ret;
    int modes;
    int i;

    waitPadReady(port, slot);

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
   

    if (modes > 0) {
      
        for (i = 0; i < modes; i++) {
            
        }
       
    }

  

    // If modes == 0, this is not a Dual shock controller 
    // (it has no actuator engines)
    if (modes == 0) {
	    
        return 1;
    }

    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);
    if (i >= modes) {
        
        return 1;
    }

    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
       
        return 1;
    }

 

    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
 

    actuators = padInfoAct(port, slot, -1, 0);
 

    if (actuators != 0) {
        actAlign[0] = 0;   // Enable small engine
        actAlign[1] = 1;   // Enable big engine
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady(port, slot);
        
    }
    else {
        
    }

    waitPadReady(port, slot);

    return 1;
}





int
main()
{
    int ret;
    int port, slot;
    int i;
    struct padButtonStatus buttons;
    u32 paddata;
    u32 old_pad = 0;
    u32 new_pad;


    SifInitRpc(0);
    strcpy(currentMenu,"mainMenuS");

    
    mainMenuS(); /*Load main menu text*/
    


    loadModules();

    padInit(0);



    port = 0; // 0 -> Connector 1, 1 -> Connector 2
    slot = 0; // Always zero if not using multitap

   

    
    if((ret = padPortOpen(port, slot, padBuf)) == 0) {
        
        SleepThread();
    }
    
    if(!initializePad(port, slot)) {

	         SleepThread();
    }
    
    for (;;) {                /*Infinite loop to detect pad state*/
            
        i=0;
        ret=padGetState(port, slot);
        while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) {
            if(ret==PAD_STATE_DISCONN) {

	                        }
            ret=padGetState(port, slot);
        }
        if(i==1) {
        }
            
        ret = padRead(port, slot, &buttons); // port, slot, buttons
            
        if (ret != 0) {
            paddata = 0xffff ^ buttons.btns;
                
            new_pad = paddata & ~old_pad;
            old_pad = paddata;
                
            
            
            
            // Directions
            if(new_pad & PAD_LEFT) {
            }
            
            if(new_pad & PAD_RIGHT) {
            }           
                        
            if((new_pad & PAD_UP) && (strcmp(currentMenu,"mainMenuO") == 0)) {
	            mainMenuS();

	    
            }
                       
            if((new_pad & PAD_UP) && (strcmp(currentMenu,"startMenu2") == 0)) {
	            startMenu1();			

	            
            }
            
            if((new_pad & PAD_UP) && (strcmp(currentMenu,"startMenu3") == 0)) {
	            startMenu2();

	            
	            
	            
	            
            }
                       
            if((new_pad & PAD_UP) && (strcmp(currentMenu,"optionsMenu2") == 0)) {
	            optionsMenu1();	

	            
	            
	                    
            }
            
            if((new_pad & PAD_UP) && (strcmp(currentMenu,"optionsMenu3") == 0)) {
	            optionsMenu2();

	            
	            
	            
            }
                        
            if((new_pad & PAD_DOWN) && (strcmp(currentMenu,"mainMenuS") == 0)) {
	            mainMenuO();

	            
	            
	            
            }
                       
            if((new_pad & PAD_DOWN) && (strcmp(currentMenu,"startMenu1") == 0)) {
	            startMenu2();

	           
	            
	            
            }
            
            if((new_pad & PAD_DOWN) && (strcmp(currentMenu,"startMenu2") == 0)) {
	            startMenu3();

	            
	            
	            
            }
                       
            if((new_pad & PAD_DOWN) && (strcmp(currentMenu,"optionsMenu1") == 0)) {
	            optionsMenu2();

	            
	            
	            
            }
            
            if((new_pad & PAD_DOWN) && (strcmp(currentMenu,"optionsMenu2") == 0)) {
	            optionsMenu3();
	            
	            
	            
            }
                       
            if(new_pad & PAD_START) {
            }
            
            if(new_pad & PAD_R3) {
            }
            
            if(new_pad & PAD_L3) {
            }
            
            if(new_pad & PAD_SELECT) {
            }
            
            if(new_pad & PAD_SQUARE) {  
            }            
            
            if((new_pad & PAD_CROSS) && (strcmp(currentMenu,"mainMenuS") == 0)) {
               startMenu1();

	            
	            
	            
            }
            
            if((new_pad & PAD_CROSS) && (strcmp(currentMenu,"mainMenuO") == 0)) {
               optionsMenu1();

	            
	            
	            
            }           
            
            if(new_pad & PAD_CIRCLE) {
               
            }
            
            if((new_pad & PAD_TRIANGLE) && (strcmp(currentMenu,"mainMenuS") != 0) && (strcmp(currentMenu,"mainMenuO") != 0)) {
               mainMenuS();

	            
	            
	            
            }
            
            if(new_pad & PAD_R1) {
            }
            
            if(new_pad & PAD_L1) {
            }
            
            if(new_pad & PAD_R2) {
            }
            
            if(new_pad & PAD_L2) {
            }
            
        

            // Test the press mode
            /* Calling SetActAlign repetedly will kill the iop :P
             * (guess the EE is too fast for IOP to handle..)
             * So I'd recommend to change the actuator values only once per
             * vblank or so..
             */
            if(buttons.triangle_p) {
#if 0
                actAlign[1] = (i >> 3); buttons.triangle_p;  // Big engine
                padSetActDirect(port, slot, actAlign);
#else
#endif
            }
            // Start little engine if we move right analog stick right
            if(buttons.rjoy_h > 0xf0)
            {
                // Stupid check to see if engine is already running,
                // just to prevent overloading the IOP with requests
                if (actAlign[0] == 0) {
                    actAlign[0] = 1;
                    padSetActDirect(port, slot, actAlign);
                }
                
               
                
            }
        } 
    }

    SleepThread();

    return 0;
}
