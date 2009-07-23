//-----------------------------------------------------------------------//
//                                                                       //
//  Artemis Project: InGame Mass Dumper                                  //
//  solution provided by jimmikaelkael                                   //
//  rev 0.1		                                                 //
//                                                                       //
//-----------------------------------------------------------------------//

	The program is patching some generic scePadRead calls and hooking
 IOP resets then running the Game elf. When inGame IOP reset is hooked,
 it does the expected job and reload a few modules.  

 	A lot of credits goes to Polo and to ffgriever for the all part of
 code in the EE core that hooks IOP reset and allow to rewrite some IRX 
 from the Game IOP replacement image, or/and reload additional modules 
 after that (code borrowed to HDProject).
 
 	Credits goes to dlanor & EP for the function the parses the .CNF
 files reliably (code borrowed to uLaunchElf).
	
	Many thanks to ffgriever & misfire for sharing sources and
 knowledge. 

//-----------------------------------------------------------------------//
//                                                                       //
//  Usage - Technical specifications                                     //
//                                                                       //
//-----------------------------------------------------------------------//
 
 The PS2 program comes in several compiled versions :

	- "IGmassdumper.elf": this is the normal one. 	

	- "IGmassdumper_altelfload": uses an alternate method to load the
 game elf (doesn't work with backups).

	- "IGmassdumper_disable_after_IOP_reset": the syscalls are unHooked
 when the 1st IOP reset has been done, so no way to catch a 2nd IOP reset,
 but this may allow some games to work.

 Start the game with IGmassdumper elf.
 
 Press UP + R1 + CROSS for EE dump to mass. 
 Press UP + R1 + TRIANGLE for IOP dump to mass. 
 Press UP + R1 + SQUARE for Kernel dump to mass. 
 Press UP + R1 + CIRCLE for ScratchPad dump to mass. 

 