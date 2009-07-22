//-----------------------------------------------------------------------//
//                                                                       //
//  Artemis Project: CoreLauncher_v2                                     //
//  solution provided by jimmikaelkael                                   //
//  rev 0.7	                                                         //
//                                                                       //
//-----------------------------------------------------------------------//

	The program is patching some generic scePadRead calls and hooking
 IOP resets then running the Game elf. When inGame IOP reset is hooked,
 it does the expected job and reload a few modules: dev9, smap, ps2ip and
 ntpbserver. ntpbserver IRX is for communication with the Win32 App.  

	I don't take any credits for the dev9 driver, this is just the 
 original Marcus R. Brown dev9 driver shrinked off from poweroff support.

 	A lot of credits goes to Polo and to ffgriever for the all part of
 code in the EE core that hooks IOP reset and allow to rewrite some IRX 
 from the Game IOP replacement image, or/and reload additional modules 
 after that (code borrowed to HDProject).
 
 	Credits goes to dlanor & EP for the function the parses the .CNF
 files reliably (code borrowed to uLaunchElf).
	
	Netlog code integrated in ntpbserver.irx is the work of misfire
 (code borrowed to his netlog.irx module), so credits to misfire.

	Many thanks to ffgriever & misfire for sharing sources and
 knowledge. 

 	A lot of thanks to LBGSHI for meticulous tests. 

//-----------------------------------------------------------------------//
//                                                                       //
//  Usage - Technical specifications                                     //
//                                                                       //
//-----------------------------------------------------------------------//
 
 The PS2 program comes in several compiled versions :

	- "core_launcher.elf": this is the normal one. 	

	- "core_launcher_altelfload": uses an alternate method to load the
 game elf (doesn't work with backups).

	- "core_launcher_disable_after_IOP_reset": the syscalls are unHooked
 when the 1st IOP reset has been done, so no way to catch a 2nd IOP reset,
 but this may allow some games to work.

 Start the game with CoreLauncher elf then use either command-line or Win32 
 client. 

 During the game launch, you'll see colored debug screens:
	- If the first one is White, then a fatal error occured to read disc.
	- Blue when the core hook IOP reset.
	- Purple while searching for free ram to put IOPRP images.
	- Red while patching IOPRP.
	- Green while resetting IOP with the modified IOPRP image.
	- Cyan while loading additional modules.
	- Magenta while searching padRead patterns. (followed by white if no pattern found, not fatal)
	- Lime Green while searching padRead Calls. (followed by white if no call found, not fatal)		

 The program uses TCP port 4234. It's planned to switch to UDP.

 The protocol used for communication with the PC side is fairly simple, here's
 a pseudo code (invalid as data block is of undefined size) to help you to
 figure out how a message is structured.

 struct ntpb_msg_t {  
	unsigned char  magic[6]; // must be "\xff\x00NTPB"
	unsigned short size; 	 // size in bytes of the data block below
	unsigned short cmd; 	 // the command to be sent to the Win32 app
	unsigned char  data[];   // the data block
 };
 
 Check sources to see availabable commands, on PC side the command line client
 sources are easier to understand.


//-----------------------------------------------------------------------//
//                                                                       //
//  current revision compatibility                                       //
//                                                                       //
//  Note that I've not tested core_launcher_altelfload with non working  //
//  games as my 3 original discs are working, and this one doesn't work  //
//  with swap.                                                           //
//                                                                       //
//  I've tested on Slim PS2 scph-70004, booted with FreeMcBoot 1.8b.     //
//  For Backups I start FMCB with an original disc inserted then swap    //
//  when the core_launcher ask for it.                                   //
//                                                                       //
//  Here are the results for ~50 games I have tested myself               //
//                                                                       //
//-----------------------------------------------------------------------//

 Tested working with normal core_launcher:
	- Godzilla Unleashed NTSC-U
	- Rogue Galaxy PAL
	- Rayman Ravin Rabbids PAL	
	- Van Helsing PAL
	- Meet the Robinsons PAL
	- Worms 4: Mayhem PAL
	- Hot Wheels: Beat That! PAL
	- Dragon Ball Z Infinite World PAL
	- Asterix & Obelix XXL 2 PAL
	- Euro 2004 PAL
	- Harry Potter: the Order of the Phenix PAL
	- FIFA 2003 PAL
	- Madagascar 2 PAL
	- Bee Movie PAL
	- Sega SuperStars Tennis PAL
	- Crash of the Titans PAL
	- Pirates of the Carribean: At World's End PAL
	- Narnia Chronicles PAL
	- SpongeBob Squarepants: Battle for Volcano Island PAL
	- George of the Jungle PAL
	- Burnout Dominator PAL
	- Spiderwick Chronicles PAL
	- Sonic Riders Zero Gravity PAL
	- Lego Star Wars PAL
	- Lego Star Wars 2 PAL
	- Lego Indiana Jones PAL
	- Lego Batman PAL
	- Ratatouille PAL
	- The Dog Island PAL
	- Shrek 3 PAL
 	- Azur and Asmar PAL
	- Final Fantasy 12 PAL
	- Sims: Pets PAL
	- Naruto Shippuden: Ultimate Ninja 4 PAL

 Game Launching & partially working
	- Ratchet Gladiator PAL (I can dump during menu, but the padRead hook is loosen inGame, the elf is originally packed) 

 Game Launching & working but the win32 app refuse to connect(TCP), but netlog(UDP) still active:
	- Sniper Elite PAL
	- Medal of Honor Sunrise PAL (the core catches all 3 IOP reset and reload modules each time without problem, then locate the padRead the 3rd time)  	

 Tested incompatible with the current additional module set:
	- Tomb Raider Underworld PAL (smap, ntpbserver) 
	- Ratchet & Clank: Size matters PAL (ntpbserver)
	- FIFA 2004 PAL (ntpbserver)
	- Teenage Mutant Ninja Turtle: the movie PAL (ntpbserver)
	- Soul Calibur 3 PAL (ntpbserver)
	- Tomb Raider Legend PAL
	- Tomb Raider Anniversary PAL
	- Cars Matter PAL (freeze when you select a game)
	- The Golden Compass PAL

 Not working (I'm unable to start those game backups with CogSwap as well, there having the same symptoms):
	- Spyro: The Eternal Knight PAL (pass loading screen, you can select language, sounds are garbage in video and freezes)
	- Call of Duty 3: March to Paris PAL (launch fine, I can dump during menu, but freeze as soon as you want to play)

 NOTE: Most of network enabled games will not connect with the Win32 app, 
the reason why I want to switch to UDP. 
 