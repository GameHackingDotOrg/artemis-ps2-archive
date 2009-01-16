/*skeleton.c

Very badly designed skeleton GUI for Artemis (raw, hacky, scr_printf text on a black background,
	screen transitions via init_scr() and button-initiated menu choices with no ability to move about
	and highlight specific items before choosing them. Just something to get the ball rolling.) Borrowed
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



void mainMenu() {
	init_scr();

    scr_printf("\n\n\n\n\n          Project Artemis\n\n"
               "          Welcome to the Project Artemis (terrible) skeleton GUI.\n\n"
               "          Press X to view the Codes menu.\n\n"
               "          Press O to view the Options menu.\n\n"
               "          Press Triangle to return to this menu.\n");
			}

void codesMenu() {
	init_scr();
	scr_printf("\n\n\n\n\n          Welcome to the (terrible) Code menu.\n\n"
				"          Choice 1\n\n"
				"          Choice 2\n\n"
				"          Choice 3\n\n");
			}	
			
void optionsMenu() {
	init_scr();
	scr_printf("\n\n\n\n\n          Welcome to the (terrible) Options menu.\n\n"
				"          Choice 1\n\n"
				"          Choice 2\n\n"
				"          Choice 3\n\n");
			}
			
			


/*
 * Local functions
 */

/*
 * loadModules()
 */
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

/*
 * waitPadReady()
 */
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
    // Were the pad ever 'out of sync'?
    if (lastState != -1) {
     
    }
    return 0;
}


/*
 * initializePad()
 */
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
    
    mainMenu(); /*Load main menu text*/


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
    
    for (;;) {
            
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
            if(new_pad & PAD_DOWN) {
            }
            if(new_pad & PAD_RIGHT) {
                /*
                       padSetMainMode(port, slot, 
                                      PAD_MMODE_DIGITAL, PAD_MMODE_LOCK));
                */
            }
            if(new_pad & PAD_UP) {
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
            if(new_pad & PAD_CROSS) {
                codesMenu();
            }
            if(new_pad & PAD_CIRCLE) {
                optionsMenu();
            }
            if(new_pad & PAD_TRIANGLE) {
                mainMenu();
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
             * (guess the EE is to fast for IOP to handle..)
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
            // Start little engine if we move right analogue stick right
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
    } // for

    SleepThread();

    return 0;
}
