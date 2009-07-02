//-----------------------------------------------------------------------//
//                                                                       //
//  Artemis Project: Task 2308/2341                                      //
//  solution provided by jimmikaelkael                                   //
//  rev 0.94                                                             //
//                                                                       //
//-----------------------------------------------------------------------//

	The program is patching some generic scePadRead calls and hooking
 IOP resets then running the Game elf. When inGame IOP reset is hooked,
 it does the expected job and reload a few modules: dev9, smap, ps2ip and
 ntpbclient. ntpclient IRX is for communication with the Win32 App.  

	I don't take any credits for the dev9 driver, this is just the 
 original Marcus R. Brown dev9 driver shrinked off from poweroff support.

 	A lot of credits goes to Polo and to ffgriever for the all part of
 code in the EE core that hooks IOP reset and allow to rewrite some IRX 
 from the Game IOP replacement image, or/and reload additional modules 
 after that (code borrowed to HDProject).
 
 	Credits goes to dlanor & EP for the function the parses the .CNF
 files reliably (code borrowed to uLaunchElf).
	
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

	- "core_launcher_silentcore": the core doesn't display onscreen
 debug.

	- "core_launcher_noadditionalirx": this one doesn't reload any
 additional modules after IOP resets. So there's no communication with the
 Win32 app, the pad combo is just triggering a quick test of onscreen debug.
 This one allow to check that the game fails because of confilcts with
 IOP modules that are reloaded by ourselves.

	- "core_launcher_nosyscallhook": doesn't use any syscall hook, so
 IOP resets are not hooked (and no modules reloaded of course so no
 communication with the Win32 app...), it allow to check that's the syscall
 hooks that cause problems, and to solve this we should unhook it after the 
 1st IOP reset.

 The first thing to do is to start the Win32 app (ntpbserver.exe, must be
 executed with administrator privileges under Vista), then start the
 core_launcher. (there's actually no timeouts on connect, recv, and not
 enough checks).

 The program uses TCP port 4234, and connect to the following IP: 192.168.0.2

 To trigger EE mem dump, wait the pad is listened (avoid dumps in memcards
 checks or save) and hit UP+CROSS+R1.
 To trigger IOP mem dump, wait the pad is listened (avoid dumps in memcards
 checks or save) and hit DOWN+CROSS+R1.
 To trigger Kernel mem dump, wait the pad is listened (avoid dumps in memcards
 checks or save) and hit UP+CROSS+R2.
 To trigger ScratchPad mem dump, wait the pad is listened (avoid dumps in memcards
 checks or save) and hit DOWN+CROSS+R2.

 This is just a solution and can be enhanced, like performing all dumps on 
 one combo shot, or add combos to trigger some mem values changes...

 The protocol used for communication with the Win32 is fairly simple, here's
 a pseudo code (invalid as data block is of undefined size) to help you to
 figure out how a message is structured.

 struct ntpb_msg_t {  
	unsigned char  magic[6]; // must be "\xff\x00NTPB"
	unsigned short size; 	 // size in bytes of the data block below
	unsigned short cmd; 	 // the command to be sent to the Win32 app
	unsigned char  data[];   // the data block
 };

 At the moment only a few commands are implemented:
	- 0x101: Send request to get EE dump start address.
	- 0x102: Send request to get IOP dump start address.
	- 0x103: Send request to get Kernel dump start address.
	- 0x104: Send request to get ScratchPad dump start address.
	- 0x201: Send request to get EE dump end address.
	- 0x202: Send request to get IOP dump end address.
	- 0x203: Send request to get Kernel dump end address.
	- 0x204: Send request to get ScratchPad dump end address.
	- 0x301: Send request for printing EE dump.
	- 0x302: Send request for printing IOP dump.
	- 0x301: Send request for printing Kernel dump.
	- 0x302: Send request for printing ScratchPad dump.
	- 0x400: Send request to get remote dump request.

 The data block is only used for these 4 requests:   
	- 0x301: must be filled with raw EE dump datas.
	- 0x302: must be filled with raw IOP dump datas.
	- 0x303: must be filled with raw Kernel dump datas.
	- 0x304: must be filled with raw ScratchPad dump datas.

 For others commands it stay empty with a data block size to 0.

 For responses, the Win32 app fills the data block depending on the
 commands.

 Commands 0x301 to 0x304 use this data block struct:  	
 struct data_block_t {  
	unsigned short reply; 	 // always 1
 };

 Commands 0x101 to 0x104 and 0x201 to 0x204 use this data block struct:  	
 struct data_block_t {  
	unsigned short reply; 	 // always 1
	unsigned int   address;	 // address given 
 };

 Commands 0x400 use this data block struct:  	
 struct data_block_t {  
	unsigned short reply; 	  // always 1
	unsigned int   dump_type; // 0 = no dump request, 1 = EE dump, 2 = IOP dump, 3 = Kernel dump, 4 = ScratchPad dump
 };

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
//  Here are the results for 42 games I have tested myself               //
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
	- Bee Movie PAL
	- Sega SuperStars Tennis PAL
	- Crash of the Titans PAL
	- Pirates of the Carribean: At World's End PAL
	- Narnia Chronicles PAL
	- SpongeBob Squarepants: Battle for Volcano Island PAL
	- George of the Jungle PAL
	- Burnout Dominator PAL

 Tested working with core_launcher_silentcore:
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

 Tested working with core_launcher_noadditionalirx:
	- Tomb Raider Underworld PAL
	- Ratchet & Clank: Size matters PAL
	- FIFA 2004 PAL
	- Teenage Mutant Ninja Turtle: the movie PAL
	- Soul Calibur 3 PAL

 Tested working with core_launcher_nosyscallhook:
	- The Golden Compass PAL
	- Sims: Pets PAL

 Not working (I'm unable to start those game backups with CogSwap as well, there having the same symptoms):
	- Spyro: The Eternal Knight PAL (pass loading screen, you can select language, sounds are garbage in video and freezes)
	- Tomb Raider Legend PAL (connect to PC, but after that it's BSOD)
	- Tomb Raider Anniversary PAL (connect to PC, seems to load a few but after that it's BSOD)
	- Call of Duty 3: March to Paris PAL (launch fine, I can dump during menu, but freeze as soon as you want to play)
	- Final Fantasy 12 PAL (connect to PC, seems to load a few but after that it's BSOD)

 NOTE: About those games which works but not responding to pad combo, 
 those games are using libpad2, and I still didn't find the corresponding
 padRead function in it. 
 