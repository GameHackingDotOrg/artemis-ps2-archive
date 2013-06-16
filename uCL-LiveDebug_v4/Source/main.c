#include "main.h"

//Don't mind this...
int PointerToFunction(int addr, int **ptr) {
	
	*(u8*)addr = (u8)((int)ptr >> 24);
	addr++;
	*(u8*)addr = (u8)((int)ptr >> 16);
	addr++;
	*(u8*)addr = (u8)((int)ptr >> 8);
	addr++;
	*(u8*)addr = (u8)(int)ptr;
	return addr;
}
 
/*
 * main function
 */
int main(int argc, char *argv[])
{
	int fdn;
	u64 WaitTime = 0;
	int i;
	int slowDown_Dpad = 0;
	static int slowDown_amount = 40;
	
	/* get boot path */
    run_path[0] = 0;
	strcpy(run_path, argv[0]);

	/* Transform the boot path to homebrew standards */	
	if (!strncmp(run_path, "mass0:", 6)) { /* SwapMagic boot path for usb_mass */
		run_path[4] = ':';
		strcpy(&run_path[5], &run_path[6]);

		for (i=0; run_path[i]!=0; i++) {
			if (run_path[i] == '\\')
				run_path[i] = '/';
		}
	}

	/* Reset IOP unless in DEBUG mode */
	if (!DEBUG)
		IOP_Reset();
	
	SifInitRpc(0);
	
	/* Load modules & apply sbv patches */
	int ret = LoadModules();
	if (DEBUG)
		printf("LoadModules return %d\n", ret);
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();
	
	/* Load settings into global variables */
	mkdir("mc0:/LiveDebug");
	config_build(&config);
	ret = LoadSettings();
	if (DEBUG)
		printf("LoadSettings return %d\n", ret);
	
	/* Defaults to NTSC mode */
	TV_mode = GS_MODE_NTSC;
	
	/* Reading ROMVER */
	if((fdn = open("rom0:ROMVER", O_RDONLY)) > 0) {
		read(fdn, romver, sizeof romver);
		close(fdn);
	}

	/* Set Video PAL mode if needed */
	if (romver[4] == 'E') TV_mode = GS_MODE_PAL;
	//TV_mode = GS_MODE_NTSC; // to force NTSC on PAL console
	
	/* Set default screen values to use with GsKit depending on TV mode */
	if (TV_mode == GS_MODE_PAL) { /* PAL values fit perfectly on my TV */
		SCREEN_WIDTH  = 640;
		SCREEN_HEIGHT = 512;
		SCREEN_X	  = 692;
		SCREEN_Y	  = 72;
		Y_RATIO	  	  = 1.0f;
	} else { /* NTSC values can be adjusted, although it fit fine too on my TV */
		SCREEN_WIDTH  = 640;
		SCREEN_HEIGHT = 448;
		SCREEN_X 	  = 672;
		SCREEN_Y	  = 50;
		Y_RATIO	  	  = 0.875f;
	}

	/* needed before to init pads */
	TimerInit();

	/* setup GS */
	Setup_GS(TV_mode);

	/* Set the max vram pointer */
	vramMax = 0x00400000; // + gsGlobal->TexturePointer;
	
	/* Initializes pads */
	setupPad();
	
	/* Load Textures into VRAM */
	LoadTextures(lmainM);
	
	/* Initialize the keyboards X and Y values */
	/* Standard QWERTY keyboard */
	kb_yoff = 80 + kb_frame_y;
	kb_yrate = 30;
	kb_yspace = 200 + kb_frame_y;
	kb_xoff = 84 + kb_frame_x;
	kb_xrate = 35;
	kb_xspace = 270 + kb_frame_x;
	/* Hexadecimal keyboard */
	kb_yoffH = 110 + kb_frameH_y;
	kb_yrateH = 35;
	kb_yenterH = (3 * kb_yrateH) + kb_yoffH - 10;
	kb_xoffH = 180 + kb_frameH_x;
	kb_xrateH = 35;
	kb_xenterH = 220 + kb_frameH_x;
	kb_xcancelH = 100 + kb_xenterH;
	
	/* Prior to call any of the gfx functions */
	gfx_set_defaults();
		
	/* Playing INTRO part 1 */
	while (!Draw_INTRO_part1())
		Render_GUI();
	Render_GUI();

	/* delay */
	#ifndef PCSX2_DEBUG
	WaitTime = Timer();
	while (Timer() < (WaitTime + 500))
		Render_GUI();
	#endif	
	
	/* Playing INTRO part 2 */
	while (!Draw_INTRO_part2())
		Render_GUI();
	Render_GUI();		
			
	/* Set first button selected and highlight pulse ON */
	selected_button = 1;
	highlight_pulse = 1;

	/* main GUI pad loop */
	while (1) {
		
		waitAnyPadReady();
		if (readPad()) {
			/* Set Highlight pulse ON */
			highlight_pulse = 1;

			if (new_pad & PAD_CROSS) {
				/* Shut-off highlight pulse when X is pressed */
				highlight_pulse = 0;
				
				/* Render while Waiting highlight pulse is at max */
				while (!Draw_MainMenu(selected_button, highlight_pulse))
					Render_GUI();
				/* Render again to take in account last changes by Draw_GUI */
				Render_GUI();
				
				/* The sounds have been placed here for timing */
				/*
				//Currently disabled.
				if (selected_button == 1) { play_sound(&start_game, size_start_game); }
				else if (selected_button == 2) { play_sound(&stop_disc, size_stop_disc); }
				else if (selected_button > 2) { play_sound(&menu_trans, size_menu_trans); } //Play sound if it's not Start Game or Stop Disc
				*/
				
				char cnf[1024];
				
				/* Taking action for button press */
				switch (selected_button) {
					
					case 1: /* start game */
						//launch_OSDSYS();
						
						ParseSYSTEMCNF(sys_cnf, cnf);
						load_elf(cnf);
						break;
						
					case 2: /* stop disc */
						StopDisc();				
						break;
					
					case 3: /* patches menu */
						LoadTextures(lPatch);
						
						patches_image_alpha = 0;
						menu_bar_x = 0;
						about_menu_header_alpha = 0;
						while (1) {
							//waitAnyPadReady();
							if (readPad()) {
								if (new_pad & PAD_TRIANGLE) {
									//play_sound((void*)&menu_trans, size_menu_trans);
									break;
								}
							}
							Update_PatchesMenu();
						}
						patchret = "";
						break;
						
					case 4: /* options menu */
						LoadTextures(loptM);
						
						options_text_alpha = 0;
						menu_bar_x = 0;
						about_menu_header_alpha = 0;
						LoadSettings();
						if (jokCombo > 0)
							jokCombo = ~jokCombo;
						while (1) {
							//waitAnyPadReady();
							if (readPad()) {
								if (new_pad & PAD_TRIANGLE) {
									StoreSett(NULL, NULL, NULL);
									//play_sound(&menu_trans, size_menu_trans);
									break;
								}
							}
							Update_OptionsMenu();	
						}					
						break;
					
					case 5: /* about menu */
						LoadTextures(labout);
						
						Init_AboutMenu();
						menu_bar_x = 0;
						while (1) {
							waitAnyPadReady();
							if (readPad()) {
								if (new_pad) {
									//play_sound(&menu_trans, size_menu_trans);
									break;
								}
							}
							Update_AboutMenu();	
						}
						break;
					
					default:
						break;
				}
				
				/* reload main menu textures to vram */
				LoadTextures(lmainM);
			}
			else if(new_pad & PAD_LEFT)	{
				/* Slow Down D-pads response without blocking */
				#ifndef PCSX2_DEBUG
				if (Timer() > WaitTime + slowDown_amount) 
					slowDown_Dpad = 0;		
				#endif	
					
				if (!slowDown_Dpad) {
					/* Getting time for slow down */
					#ifndef PCSX2_DEBUG
					WaitTime = Timer();
					slowDown_Dpad = 1;
					#endif		
								
					selected_button--;
					if (selected_button < 1) selected_button = 5;
				}				
			}
			else if(new_pad & PAD_RIGHT) {
				/* Slow Down D-pads response without blocking */
				#ifndef PCSX2_DEBUG
				if (Timer() > WaitTime + slowDown_amount) 
					slowDown_Dpad = 0;		
				#endif	
					
				if (!slowDown_Dpad) {
					/* Getting time for slow down */
					#ifndef PCSX2_DEBUG
					WaitTime = Timer();
					slowDown_Dpad = 1;		
					#endif
								
					selected_button++;
					if (selected_button > 5) selected_button = 1;				
				}
			}
			else if (new_pad & PAD_SELECT) {
				launch_OSDSYS();
			}
			else if (new_pad & PAD_START) {
				char uLE[200];
				GetValidPath();
				memcpy((char*)uLE, ulePath, strlen((const char*)ulePath));
				load_elf((char*)uLE);
			}
		}
		
		/* Update GUI and render */
		Update_MainMenu();
	}

	/* We won't get here */
//err_out:
	scr_printf("\t CL-LiveDebug GUI Init failed...\n");

	SleepThread();

	return 0;
}

/*
 * Loads in all the modules and the qwerty mod
 */
int LoadModules(void) {
	int ret = 0;

	SifLoadFileInit();
	SifInitRpc(0);

	// Load modules off ROM
	ret += SifLoadModule("rom0:SIO2MAN", 0, NULL);
	ret += SifLoadModule("rom0:MCMAN", 0, NULL);
	ret += SifLoadModule("rom0:MCSERV", 0, NULL);
	ret += SifLoadModule("rom0:PADMAN", 0, NULL);
	//ret = SifLoadModule("rom0:LIBSD", 0, NULL);

	// Modules from PS2SDK
	ret += SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, NULL);
	ret += SifExecModuleBuffer(&usb_mass_irx, size_usb_mass_irx, 0, NULL, NULL);
	ret += SifExecModuleBuffer(&freesd_irx, size_freesd_irx, 0, NULL, NULL);
	ret += SifExecModuleBuffer(&audsrv_irx, size_audsrv_irx, 0, NULL, NULL);
	
	//audsvr_start(-1);
	
	// QWERTY Module from CogSwap
	loadmodulemem(iopmod, 1865, 0, 0);

	SifLoadFileExit();
	
	return ret;
}

/*
 * Loads the array at ptr into the memory at addr
 */
int InstallArray(int addr, void ** ptr, int size) {
	int i = 0;
	u32 Store = addr;
	u32 Read = (u32)ptr;
	
	if (DEBUG)
		printf("addr = %08x, ptr = %08x, size = %x (%d)\n", (int)addr, (int)ptr, (int)size, (int)size);
	
	if (addr >= 0x80000000) { /* Kernel */
		for (i = 0; i < size; i += 1)
		{
			DI();
			ee_kmode_enter();
			*(u8*)Store = *(u8*)Read;
			ee_kmode_exit();
			EI();
			Store += 1;
			Read += 1;
		}
	} else { /* EE */
		for (i = 0; i < size; i += 1)
		{
			*(u8*)Store = *(u8*)Read;
			Store += 1;
			Read += 1;
		}
	}
	return 1;
}
 
/*
 * load an elf file using embedded elf loader
 * this fixes the memory overlapping problem
 */
void load_elf(char *elf_path)
{

	if (elf_path == NULL) { return; }
	
	if (jokCombo > 0) { //Odd bug...
		jokCombo = ~jokCombo;
		StoreSett(NULL, NULL, NULL);
	}
	
	if (elf_path[strlen(elf_path) - 1] == '\n')
		elf_path[strlen(elf_path) - 1] = '\0'; //A simple fix to a bug with booting uLE
	
	if (DEBUG)
		printf("boot path check\n");
	
	u8 *boot_elf;
	elf_header_t *boot_header;
	elf_pheader_t *boot_pheader;
	int i;
	char *args[2], *elfp = malloc(7);
	memcpy(elfp, elf_path, 7);
	elfp[7] = '\0';
	
	//If it is booting from the disc, then AutoJoker is the deciding factor
	if (strncmp(elfp, "cdrom0:", 7) == 0) { uLEJoker = 1; }
	free(elfp);
	
	//This is for the finding the appropriate joker and for the safeguard loop fix
	char *tempJoker = malloc(50), *tempVer = malloc(10);
	int x = 0;
	ParseSYSTEMCNF(sys_cnf, tempJoker);
	ParseVerNumber(sys_cnf, tempVer);
	while (tempJoker[0] != '\\') { tempJoker++; } //Finds '\' in cdrom0:\\....
	tempJoker++; //Skips '\'
	while (tempJoker[x] != ';') { x++; } //Finds the ';' in ????_???.??;1
	tempJoker[x] = '\0'; //Ends string at S???_???.??
	
	if (DEBUG)
		printf("Beginning elf_loader loading\n");
	
	/* The loader is embedded */
	boot_elf = (u8 *)&elf_loader;

	/* Get Elf header */
	boot_header = (elf_header_t *)boot_elf;

	/* Check elf magic */
	if ((*(u32*)boot_header->ident) != ELF_MAGIC)
		return;

	/* Get program headers */
	boot_pheader = (elf_pheader_t *)(boot_elf + boot_header->phoff);

	/* Scan through the ELF's program headers and copy them into appropriate RAM
	 * section, then pad with zeros if needed.
	 */
	for (i = 0; i < boot_header->phnum; i++) {
		if (boot_pheader[i].type != ELF_PT_LOAD)
			continue;

		memcpy(boot_pheader[i].vaddr, boot_elf + boot_pheader[i].offset, boot_pheader[i].filesz);

		if (boot_pheader[i].memsz > boot_pheader[i].filesz)
			memset(boot_pheader[i].vaddr + boot_pheader[i].filesz, 0, boot_pheader[i].memsz - boot_pheader[i].filesz);
	}
	
	if (DEBUG)
		printf("Installing the debugger into the kernel\n");
	
	InstallArray(0x8002FE00, (void**)&LDSpace, sizeof(LDSpace)); 		/* Install the spacing into the kernel */
	InstallArray(0x80030000, (void**)&LDFont, sizeof(LDFont)); 			/* Install the font into the kernel */
	InstallArray(0x80078250, (void**)&LDHook, sizeof(LDHook)); 			/* Install the hook filter into the kernel */
	InstallArray(0x80060000, (void**)&LDEngine, sizeof(LDEngine)); 		/* Install the LiveDebug v3 Engine into the kernel */
	InstallArray(0x8007F800, (void**)&LDCheats, sizeof(LDCheats)); 		/* Install the LiveDebug v3 Cheat Engine into the kernel */
	InstallArray(0x8007E880, (void**)&LDScanner, sizeof(LDScanner)); 	/* Install the Joker Scanner into the kernel */
	InstallArray(0x80045000, (void**)&LDRegD, sizeof(LDRegD)); 			/* Install the Register Dumper into the kernel */
	//if (ExcHand)														/* Install the Exception Handler into the Kernel if ExcHand is 1 */
	//	InstallArray(0x80051000, (void**)&LDExcept, sizeof(LDExcept));  /* The address 0x80051000 should be changed to a free area! */
	
	if (DEBUG) {
		float uSize = (float)(sizeof(LDSpace) + sizeof(LDFont) + sizeof(LDHook) + sizeof(LDEngine) \
					  + sizeof(LDCheats) + sizeof(LDScanner) + sizeof(LDRegD) + (0x0004C000 - 0x00030000)) / 1024;
		float kSize = (float)((0x00074FFF - 0x00030000) + (0x0007FFFF - 0x00078250)) / 1024; //This isn't entirely accurate
		printf("Total estimated kernel usage: %.02f kb / %.0f kb (%.02f%c)\n", (float)uSize, (float)kSize, (float)(uSize / kSize)*100, (char)'%');
	}
	
	u32 joker = 0;
	if (AutoJoker == 1 && uLEJoker == 1) {
	
		LoadSettings();
		//Load joker from conf file
		const char *jok_temp = LoadJoker(tempJoker, tempVer);
		if (jok_temp == NULL) {
			jok_temp = LoadJokerArray(tempJoker, tempVer);
		}
		
		//If it finds a joker, convert it to base 16
		if (jok_temp != NULL)
			joker = strtol(jok_temp, NULL, 16);
		
	}
	
	if (DEBUG)
		printf("Joker is = %08x\nConfiguring engine\n", joker);
	
	/* Install the Scanner Config into the kernel */
	DI();
	ee_kmode_enter();
	*(u32*)0x8007E7FC = (u32)joker; //If != 0 then joker scanner isn't called
	*(u32*)0x8007E800 = 0x00100000; //Start Point
	*(u32*)0x8007E804 = 0x00000100; //Scan Size
	*(u32*)0x8007E808 = 0x00000000; //Index
	*(u32*)0x8007E80C = 0x8007e820; //Define Patterns Address
	*(u32*)0x8007E810 = (u32)joker; //Storage 1
	*(u32*)0x8007E814 = 0x00000000; //Storage 2
	*(u32*)0x8007E818 = 0x00000000; //Storage 3
	*(u32*)0x8007E81C = 0x00000000; //Storage 4
	
	/* Install the Joker patterns into the kernel */
	*(u32*)0x8007E820 = 0x04121601;
	*(u32*)0x8007E824 = 0x00000001;
	*(u32*)0x8007E828 = 0x04060a01;
	*(u32*)0x8007E82C = 0x00000001;
	*(u32*)0x8007E830 = 0x5a000000;
	*(u32*)0x8007E834 = 0xffff0000;
	*(u32*)0x8007E838 = 0x0000ffff;
	*(u32*)0x8007E83C = 0x5a000000;
	*(u32*)0x8007E840 = 0x00000000;
	*(u32*)0x8007E844 = 0x00000000;
	*(u32*)0x8007E848 = 0xffff0100;
	
	/* Configure the engine */
	*(u32*)0x80050018 = 0x80050130; //Pointer to linetohex region
	*(u32*)0x8005001C = 0x80030000; //Pointer to font
	*(u32*)0x80050020 = 0x00100000; //Current address (starting address)
	*(u32*)0x80050028 = 1; //!ExcHand;   //Exception handler install boolean (1 = off, 0 = on)
	*(u32*)0x8005002C = jokCombo;	//Joker combo to open the debugger
	
	/* Search pre-configuration */
	*(u32*)0x80044440 = 0x00100000; //Start value
	*(u32*)0x80044444 = 0x00100000; //Stop value
	*(u32*)0x80044420 = 0x52415453; //Value 1
	*(u32*)0x80044424 = 0x6F742054; //Value 2
	*(u32*)0x80044428 = 0x73657220; //Value 3
	*(u32*)0x8004442C = 0x00007465; //Value 4
	 *(u8*)0x80044410 = 0x00;       //Search op
	 *(u8*)0x80044411 = 0x00;       //Next search op
	 *(u8*)0x80044412 = 0x02;       //Bits (0 = 8, 1 = 16, 2 = 32, 3 = 64, 4 = 128)
	
	//Default allocation
	//If FindFreeRegion can't find a region for these then this will be used instead
	*(u32*)0x80050060 = (int)pktAlloc;  //Packet
	*(u32*)0x80050064 = (int)ehAlloc;   //Exception handler
	*(u32*)0x80050068 = (int)regdAlloc; //Register dumper
	ee_kmode_exit();
	EI();
	
	free(tempJoker);
	
	if (DEBUG)
		printf("Writing labels and patches\n");
		
	/* Install string table into the kernel */
	WriteLabels();

	/* Install Patches into the kernel */
	WritePatches();
	
	if (DEBUG)
		printf("Installing the hook\n");
	
	DI();
	ee_kmode_enter();
		*(u32*)HookAddr = HookValue; // Install the kernel hook
	ee_kmode_exit();
	EI();
	
	args[0] = elf_path;
	args[1] = (char*)ulePath;
	
	if (DEBUG)
		printf("args[0] = %s\nargs[1] = %s\nCleaning up and saying good bye!\n", args[0], args[1]);
	
	/* Cleanup before launching elf loader */
	CleanUp();
	
	/* Execute Elf Loader */
	ExecPS2((void *)boot_header->entry, 0, 2, args);
}

void WriteLabels(void)
{
DI();
ee_kmode_enter();
*(u32*)0x800444C0 = 0x656C6544; //"Dele"
*(u32*)0x800444C4 = 0x00006574; //"te"
*(u32*)0x800444C8 = 0x636E6143; //"Canc"
*(u32*)0x800444CC = 0x00006C65; //"el"
*(u32*)0x800444D0 = 0x20646441; //"Add "
*(u32*)0x800444D4 = 0x43206F74; //"to C"
*(u32*)0x800444D8 = 0x7365646F; //"odes"
*(u32*)0x800444DC = 0x006B4F00; //"Ok"
*(u32*)0x800444E0 = 0x003A704F; //"Op:"
*(u32*)0x800444E4 = 0x3D21003D; //next 4 addresses are for operators =, !=, <, >, <=, >=
*(u32*)0x800444E8 = 0x3E003C00;
*(u32*)0x800444EC = 0x003D3C00;
*(u32*)0x800444F0 = 0x00003D3E; 
*(u32*)0x800444F4 = 0x7478654E; //"Next"
*(u32*)0x800444F8 = 0x61655320; //" Sea"
*(u32*)0x800444FC = 0x00686372; //"rch"
*(u32*)0x80044500 = 0x4C2D4C43; //CL-LiveDebug v4
*(u32*)0x80044504 = 0x44657669; 
*(u32*)0x80044508 = 0x67756265;
*(u32*)0x8004450C = 0x00347620;
*(u32*)0x80044510 = 0x61657243;
*(u32*)0x80044514 = 0x20646574;
*(u32*)0x80044518 = 0x203A7962;
*(u32*)0x8004451C = 0x636C7447;
*(u32*)0x80044520 = 0x706D6970;
*(u32*)0x80044524 = 0x00000000;
*(u32*)0x80044528 = 0x69736956;
*(u32*)0x8004452C = 0x73752074;
*(u32*)0x80044530 = 0x20746120;
*(u32*)0x80044534 = 0x2E777777;
*(u32*)0x80044538 = 0x61656843;
*(u32*)0x8004453C = 0x73726574;
*(u32*)0x80044540 = 0x756F4C2D;
*(u32*)0x80044544 = 0x2E65676E;
*(u32*)0x80044548 = 0x0074656E;
*(u32*)0x8004454C = 0x6961462A;
*(u32*)0x80044550 = 0x61532D6C;
*(u32*)0x80044554 = 0x6D206566;
*(u32*)0x80044558 = 0x2A65646F;
*(u32*)0x8004455C = 0x65725020;
*(u32*)0x80044560 = 0x27207373;
*(u32*)0x80044564 = 0x52415453;
*(u32*)0x80044568 = 0x74202754;
*(u32*)0x8004456C = 0x6E65206F;
*(u32*)0x80044570 = 0x20726574;
*(u32*)0x80044574 = 0x20656874;
*(u32*)0x80044578 = 0x756E656D;
*(u32*)0x8004457C = 0x0000002E;
*(u32*)0x80044580 = 0x6E654D3D;
*(u32*)0x80044584 = 0x6F432075;
*(u32*)0x80044588 = 0x6F72746E;
*(u32*)0x8004458C = 0x003D736C;
*(u32*)0x80044590 = 0x41207055;
*(u32*)0x80044594 = 0x776F7272;
*(u32*)0x80044598 = 0x6441203A;
*(u32*)0x8004459C = 0x73657264;
*(u32*)0x800445A0 = 0x202D2073;
*(u32*)0x800445A4 = 0x00000034;
*(u32*)0x800445A8 = 0x6E776F44;
*(u32*)0x800445AC = 0x72724120;
*(u32*)0x800445B0 = 0x203A776F;
*(u32*)0x800445B4 = 0x72646441;
*(u32*)0x800445B8 = 0x20737365;
*(u32*)0x800445BC = 0x0034202B;
*(u32*)0x800445C0 = 0x7466654C;
*(u32*)0x800445C4 = 0x72724120;
*(u32*)0x800445C8 = 0x203A776F;
*(u32*)0x800445CC = 0x65676150;
*(u32*)0x800445D0 = 0x00705520;
*(u32*)0x800445D4 = 0x68676952;
*(u32*)0x800445D8 = 0x72412074;
*(u32*)0x800445DC = 0x3A776F72;
*(u32*)0x800445E0 = 0x67615020;
*(u32*)0x800445E4 = 0x6F442065;
*(u32*)0x800445E8 = 0x00006E77;
*(u32*)0x800445EC = 0x203A3152;
*(u32*)0x800445F0 = 0x54206F47;
*(u32*)0x800445F4 = 0x0000006F;
*(u32*)0x800445F8 = 0x61697254;
*(u32*)0x800445FC = 0x656C676E;
*(u32*)0x80044600 = 0x7845203A;
*(u32*)0x80044604 = 0x4D207469;
*(u32*)0x80044608 = 0x00756E65;
*(u32*)0x8004460C = 0x6F6D654D;
*(u32*)0x80044610 = 0x42207972;
*(u32*)0x80044614 = 0x73776F72;
*(u32*)0x80044618 = 0x00007265;
*(u32*)0x8004461C = 0x75746552;
*(u32*)0x80044620 = 0x54206E72;
*(u32*)0x80044624 = 0x6147206F;
*(u32*)0x80044628 = 0x0000656D;

*(u32*)0x8004462C = 0x65646F43; // Codes
*(u32*)0x80044630 = 0x00000073;

*(u32*)0x80044634 = 0x63746150;
*(u32*)0x80044638 = 0x00736568;
*(u32*)0x8004463C = 0x7466654C;
*(u32*)0x80044640 = 0x72724120;
*(u32*)0x80044644 = 0x2D20776F;
*(u32*)0x80044648 = 0x66654C20;
*(u32*)0x8004464C = 0x20312074;
*(u32*)0x80044650 = 0x69676944;
*(u32*)0x80044654 = 0x00000074;
*(u32*)0x80044658 = 0x68676952;
*(u32*)0x8004465C = 0x72412074;
*(u32*)0x80044660 = 0x20776F72;
*(u32*)0x80044664 = 0x6952202D;
*(u32*)0x80044668 = 0x20746867;
*(u32*)0x8004466C = 0x69442031;
*(u32*)0x80044670 = 0x00746967;
*(u32*)0x80044674 = 0x41207055;
*(u32*)0x80044678 = 0x776F7272;
*(u32*)0x8004467C = 0x49202D20;
*(u32*)0x80044680 = 0x6572636E;
*(u32*)0x80044684 = 0x746E656D;
*(u32*)0x80044688 = 0x42203120;
*(u32*)0x8004468C = 0x00007469;
*(u32*)0x80044690 = 0x6E776F44;
*(u32*)0x80044694 = 0x72724120;
*(u32*)0x80044698 = 0x2D20776F;
*(u32*)0x8004469C = 0x63654420;
*(u32*)0x800446A0 = 0x656D6572;
*(u32*)0x800446A4 = 0x3120746E;
*(u32*)0x800446A8 = 0x74694220;
*(u32*)0x800446AC = 0x00000000;
*(u32*)0x800446B0 = 0x736F7243;
*(u32*)0x800446B4 = 0x202D2073;
*(u32*)0x800446B8 = 0x54206F47;
*(u32*)0x800446BC = 0x6441206F;
*(u32*)0x800446C0 = 0x73657264;
*(u32*)0x800446C4 = 0x00000073;
*(u32*)0x800446C8 = 0x63726943;
*(u32*)0x800446CC = 0x2D20656C;
*(u32*)0x800446D0 = 0x6E614320;
*(u32*)0x800446D4 = 0x006C6563;
*(u32*)0x800446D8 = 0x61757153;
*(u32*)0x800446DC = 0x203A6572;
*(u32*)0x800446E0 = 0x656E694C;
*(u32*)0x800446E4 = 0x6C655320;
*(u32*)0x800446E8 = 0x6F746365;
*(u32*)0x800446EC = 0x00000072;
*(u32*)0x800446F0 = 0x2D203152;
*(u32*)0x800446F4 = 0x76615320;
*(u32*)0x800446F8 = 0x61442065;
*(u32*)0x800446FC = 0x00006174;
*(u32*)0x80044700 = 0x61697254;
*(u32*)0x80044704 = 0x656C676E;
*(u32*)0x80044708 = 0x43202D20;
*(u32*)0x8004470C = 0x65636E61;
*(u32*)0x80044710 = 0x0000006C;
*(u32*)0x80044714 = 0x41207055;
*(u32*)0x80044718 = 0x776F7272;
*(u32*)0x8004471C = 0x694C203A;
*(u32*)0x80044720 = 0x5520656E;
*(u32*)0x80044724 = 0x00312070;
*(u32*)0x80044728 = 0x6E776F44;
*(u32*)0x8004472C = 0x72724120;
*(u32*)0x80044730 = 0x203A776F;
*(u32*)0x80044734 = 0x656E694C;
*(u32*)0x80044738 = 0x776F4420;
*(u32*)0x8004473C = 0x0031206E;
*(u32*)0x80044740 = 0x736F7243;
*(u32*)0x80044744 = 0x45203A73;
*(u32*)0x80044748 = 0x20746964;
*(u32*)0x8004474C = 0x656E694C;
*(u32*)0x80044750 = 0x00000000;
*(u32*)0x80044754 = 0x63726943;
*(u32*)0x80044758 = 0x203A656C;
*(u32*)0x8004475C = 0x42206F47;
*(u32*)0x80044760 = 0x006B6361;
*(u32*)0x80044764 = 0x6E727554;
*(u32*)0x80044768 = 0x004E4F20;
*(u32*)0x8004476C = 0x6E727554;
*(u32*)0x80044770 = 0x46464F20;
*(u32*)0x80044774 = 0x00000000;
*(u32*)0x80044778 = 0x42206F47;
*(u32*)0x8004477C = 0x006B6361;
*(u32*)0x80044780 = 0x63746150;
*(u32*)0x80044784 = 0x00000068;
*(u32*)0x80044788 = 0x6769724F;
*(u32*)0x8004478C = 0x6C616E69;
*(u32*)0x80044790 = 0x00000000;
*(u32*)0x80044794 = 0x72616553; // Search
*(u32*)0x80044798 = 0x00006863;

*(u32*)0x8004479C = 0x72617453; // Start:
*(u32*)0x800447A0 = 0x00003A74;

*(u32*)0x800447A4 = 0x6F745320; // Stop:
*(u32*)0x800447A8 = 0x00003A70;

*(u32*)0x800447AC = 0x756C6156; // Value:
*(u32*)0x800447B0 = 0x00003A65;

*(u32*)0x800447B4 = 0x74694220; // Bits:
*(u32*)0x800447B8 = 0x00003A73;

*(u32*)0x800447BC = 0x36310038; // 8 / 16 / 32 / 64 / 128
*(u32*)0x800447C0 = 0x00323300;
*(u32*)0x800447C4 = 0x31003436;
*(u32*)0x800447C8 = 0x00003832;

*(u32*)0x800447CC = 0x75736552; // Results
*(u32*)0x800447D0 = 0x0073746C;

*(u32*)0x800447D4 = 0x42206F47; // Go Back
*(u32*)0x800447D8 = 0x006B6361;

*(u32*)0x800447DC = 0x72616553; // Searching...
*(u32*)0x800447E0 = 0x6E696863;
*(u32*)0x800447E4 = 0x2E2E2E67;
*(u32*)0x800447E8 = 0x00000000;

*(u32*)0x800447EC = 0x72616553; // Search process completed.
*(u32*)0x800447F0 = 0x70206863;
*(u32*)0x800447F4 = 0x65636F72;
*(u32*)0x800447F8 = 0x63207373;
*(u32*)0x800447FC = 0x6C706D6F;
*(u32*)0x80044800 = 0x64657465;
*(u32*)0x80044804 = 0x0000002E;

*(u32*)0x80044808 = 0x61746F54; // Total Results:
*(u32*)0x8004480C = 0x6552206C;
*(u32*)0x80044810 = 0x746C7573;
*(u32*)0x80044814 = 0x00003A73;

*(u32*)0x80044818 = 0x73657250; // Press (X) To Continue.
*(u32*)0x8004481C = 0x58282073;
*(u32*)0x80044820 = 0x6F542029;
*(u32*)0x80044824 = 0x6E6F4320;
*(u32*)0x80044828 = 0x756E6974;
*(u32*)0x8004482C = 0x00002E65;

*(u32*)0x80044830 = 0x72616553; // Search Query:
*(u32*)0x80044834 = 0x51206863;
*(u32*)0x80044838 = 0x79726575;
*(u32*)0x8004483C = 0x0000003A;

*(u32*)0x80044840 = 0x7478654E; //"Next Search:"
*(u32*)0x80044844 = 0x61655320;
*(u32*)0x80044848 = 0x3A686372;
*(u32*)0x8004484C = 0x00000000;

*(u32*)0x80044850 = 0x7265704F; //"Operator"
*(u32*)0x80044854 = 0x726F7461;
*(u32*)0x80044858 = 0x00000000;

*(u32*)0x8004485C = 0x6E616843; //"Changed"
*(u32*)0x80044860 = 0x00646567;

*(u32*)0x80044864 = 0x68636E55; //"Unchanged"
*(u32*)0x80044868 = 0x65676E61;
*(u32*)0x8004486C = 0x00000064;

*(u32*)0x80044870 = 0x65646F43; //"Code Name:"
*(u32*)0x80044874 = 0x6D614E20;
*(u32*)0x80044878 = 0x00003A65;

*(u32*)0x8004487C = 0x74203152; //"R1 to store 00"
*(u32*)0x80044880 = 0x7473206F;
*(u32*)0x80044884 = 0x2065726F;
*(u32*)0x80044888 = 0x00003030;

*(u32*)0x8004488C = 0x7420314C; //"L1 to store 20"
*(u32*)0x80044890 = 0x7473206F;
*(u32*)0x80044894 = 0x2065726F;
*(u32*)0x80044898 = 0x00003032;

*(u32*)0x8004489C = 0x69676552; //Register Dumper
*(u32*)0x800448A0 = 0x72657473;
*(u32*)0x800448A4 = 0x6D754420;
*(u32*)0x800448A8 = 0x00726570;

//Registers at - ra (zero skipped because it is a constant)
*(u32*)0x800448AC = 0x003A7461; //at
*(u32*)0x800448B0 = 0x003A3076; //v0
*(u32*)0x800448B4 = 0x003A3176; //v1
*(u32*)0x800448B8 = 0x003A3061; //a0
*(u32*)0x800448BC = 0x003A3161; //a1
*(u32*)0x800448C0 = 0x003A3261; //a2
*(u32*)0x800448C4 = 0x003A3361; //a3
*(u32*)0x800448C8 = 0x003A3074; //t0
*(u32*)0x800448CC = 0x003A3174; //t1
*(u32*)0x800448D0 = 0x003A3274; //t2
*(u32*)0x800448D4 = 0x003A3374; //t3
*(u32*)0x800448D8 = 0x003A3474; //t4
*(u32*)0x800448DC = 0x003A3574; //t5
*(u32*)0x800448E0 = 0x003A3674; //t6
*(u32*)0x800448E4 = 0x003A3774; //t7
*(u32*)0x800448E8 = 0x003A3073; //s0
*(u32*)0x800448EC = 0x003A3173; //s1
*(u32*)0x800448F0 = 0x003A3273; //s2
*(u32*)0x800448F4 = 0x003A3373; //s3
*(u32*)0x800448F8 = 0x003A3473; //s4
*(u32*)0x800448FC = 0x003A3573; //s5
*(u32*)0x80044900 = 0x003A3673; //s6
*(u32*)0x80044904 = 0x003A3773; //s7
*(u32*)0x80044908 = 0x003A3874; //t8
*(u32*)0x8004490C = 0x003A3974; //t9
*(u32*)0x80044910 = 0x003A306B; //k0
*(u32*)0x80044914 = 0x003A316B; //k1
*(u32*)0x80044918 = 0x003A7067; //gp
*(u32*)0x8004491C = 0x003A7066; //fp
*(u32*)0x80044920 = 0x003A7073; //sp
*(u32*)0x80044924 = 0x003A6172; //ra

*(u32*)0x80044928 = 0x74736E49; //Install Dumper at
*(u32*)0x8004492C = 0x206C6C61;
*(u32*)0x80044930 = 0x706D7544;
*(u32*)0x80044934 = 0x61207265;
*(u32*)0x80044938 = 0x00000074;

*(u32*)0x8004493C = 0x72646441; //Address not between 00080000 and 02000000!
*(u32*)0x80044940 = 0x20737365;
*(u32*)0x80044944 = 0x20746F6E;
*(u32*)0x80044948 = 0x77746562;
*(u32*)0x8004494C = 0x206E6565;
*(u32*)0x80044950 = 0x38303030;
*(u32*)0x80044954 = 0x30303030;
*(u32*)0x80044958 = 0x646E6120;
*(u32*)0x8004495C = 0x30323020;
*(u32*)0x80044960 = 0x30303030;
*(u32*)0x80044964 = 0x00002130;

*(u32*)0x80044968 = 0x6F6D6552; //Remove dumper
*(u32*)0x8004496C = 0x64206576;
*(u32*)0x80044970 = 0x65706D75;
*(u32*)0x80044974 = 0x00000072;

*(u32*)0x80044978 = 0x69676552; //Register Dumper hooked to:
*(u32*)0x8004497C = 0x72657473;
*(u32*)0x80044980 = 0x6D754420;
*(u32*)0x80044984 = 0x20726570;
*(u32*)0x80044988 = 0x6B6F6F68;
*(u32*)0x8004498C = 0x74206465;
*(u32*)0x80044990 = 0x00003A6F;

*(u32*)0x80044994 = 0x203A314C; //L1: Install Reg Dumper
*(u32*)0x80044998 = 0x74736E49;
*(u32*)0x8004499C = 0x206C6C61;
*(u32*)0x800449A0 = 0x20676552;
*(u32*)0x800449A4 = 0x706D7544;
*(u32*)0x800449A8 = 0x00007265;

*(u32*)0x800449AC = 0x005E0076; //v, ^ (Up and Down)

*(u32*)0x800449B0 = 0x7366664F; //Offset:
*(u32*)0x800449B4 = 0x003A7465;

*(u32*)0x800449B8 = 0x00310030; //0, 1
*(u32*)0x800449BC = 0x00330032; //2, 3

*(u32*)0x800449C0 = 0x74786554; //Text
*(u32*)0x800449C4 = 0x0000003A; //:

*(u32*)0x800449C8 = 0x656B6F4A; //Joke
*(u32*)0x800449CC = 0x003A7372; //rs:

ee_kmode_exit();
EI();
}

int WritePatches(void)
{
	int i, n, added;
	char c;
	u32 textAddr = 0x00F00000;
	u32 WriteAddr = 0x80047810;
	u8 b1, b2, b3, b4, b5, b6, b7, b8;
	u32 val, szLimit;

	szLimit = textAddr + *(u32*)0x00EFFFFC;

	for (i = 0x80047810; i < 0x8004B810; i += 4)
	{
		DI();
		ee_kmode_enter();
		*(u32*)i = 0;
		ee_kmode_exit();
		EI();
	}


	if (PatchesLoaded == 0)
		return 0;


	n = 0;
	added = 0;
	while (textAddr < szLimit)
	{
		if (added == 512) { goto WC_Done; }

		if (*(u8*)textAddr != 0x0D) {

		// Copy Label
		for (n = 0; n < 16; n++)
		{
			c = *(u8*)textAddr;

			if (c == 0x0D)
				goto WP_CodeConvert;
			if (c == 0x0A)
				goto WP_CodeConvert;

			DI();
			ee_kmode_enter();
			*(u8*)(WriteAddr + n) = c;
			*(u8*)(WriteAddr + (n + 1)) = 0x00;
			ee_kmode_exit();
			EI();
			
			textAddr++;
		}
		WP_CodeConvert:

		while (*(u8*)textAddr != 0x0A)
			textAddr++;

		textAddr++;
		WriteAddr += 0x10;

		for (n = 0; n < 3; n++)
		{
			b1 = *(u8*)textAddr; textAddr++;
			b2 = *(u8*)textAddr; textAddr++;
			b3 = *(u8*)textAddr; textAddr++;
			b4 = *(u8*)textAddr; textAddr++;
			b5 = *(u8*)textAddr; textAddr++;
			b6 = *(u8*)textAddr; textAddr++;
			b7 = *(u8*)textAddr; textAddr++;
			b8 = *(u8*)textAddr; textAddr++;
			textAddr++;

			b1 -= 0x30;
			b2 -= 0x30;
			b3 -= 0x30;
			b4 -= 0x30;
			b5 -= 0x30;
			b6 -= 0x30;
			b7 -= 0x30;
			b8 -= 0x30;

			// Lower Case                          Upper Case
			if (b1 > 0x30) { b1 -= 0x27; } else if (b1 > 0x10) { b1 -= 0x07; }
			if (b2 > 0x30) { b2 -= 0x27; } else if (b2 > 0x10) { b2 -= 0x07; }
			if (b3 > 0x30) { b3 -= 0x27; } else if (b3 > 0x10) { b3 -= 0x07; }
			if (b4 > 0x30) { b4 -= 0x27; } else if (b4 > 0x10) { b4 -= 0x07; }
			if (b5 > 0x30) { b5 -= 0x27; } else if (b5 > 0x10) { b5 -= 0x07; }
			if (b6 > 0x30) { b6 -= 0x27; } else if (b6 > 0x10) { b6 -= 0x07; }
			if (b7 > 0x30) { b7 -= 0x27; } else if (b7 > 0x10) { b7 -= 0x07; }
			if (b8 > 0x30) { b8 -= 0x27; } else if (b8 > 0x10) { b8 -= 0x07; }

			val = 0;
			val += (b8 * 0x1);
			val += (b7 * 0x10);
			val += (b6 * 0x100);
			val += (b5 * 0x1000);
			val += (b4 * 0x10000);
			val += (b3 * 0x100000);
			val += (b2 * 0x1000000);
			val += (b1 * 0x10000000);
			
			DI();
			ee_kmode_enter();
			*(u32*)WriteAddr = val;
			ee_kmode_exit();
			EI();
			WriteAddr += 4;
		}
		DI();
		ee_kmode_enter();
		*(u32*)WriteAddr = 0;
		ee_kmode_exit();
		EI();
		WriteAddr += 4;

		added++;

		while (*(u8*)textAddr != 0x0A)
			textAddr++;

		textAddr += 1;

		}
		else
		{
			textAddr += 1;
		}
	}
	WC_Done:

	return 1;
}

//----------------------------------------------------------------
int     get_CNF_string(u8 **CNF_p_p, u8 **name_p_p, u8 **value_p_p) //Taken from uLaunchElf
{
        
        u8 *np, *vp, *tp = *CNF_p_p;

start_line:
        while((*tp<=' ') && (*tp>'\0')) tp+=1;  //Skip leading whitespace, if any
        if(*tp=='\0') return 0;                         //but exit at EOF
        np = tp;                                //Current pos is potential name
        if(*tp<'A')                             //but may be a comment line
        {                                       //We must skip a comment line
                while((*tp!='\r')&&(*tp!='\n')&&(*tp>'\0')) tp+=1;  //Seek line end
                goto start_line;                    //Go back to try next line
        }

        while((*tp>='A')||((*tp>='0')&&(*tp<='9'))) tp+=1;  //Seek name end
        if(*tp=='\0') return -1;                        //but exit at EOF

        while((*tp<=' ') && (*tp>'\0'))
                *tp++ = '\0';                       //zero&skip post-name whitespace
        if(*tp!='=') return -2;                 //exit (syntax error) if '=' missing
        *tp++ = '\0';                           //zero '=' (possibly terminating name)

        while((*tp<=' ') && (*tp>'\0')          //Skip pre-value whitespace, if any
                && (*tp!='\r') && (*tp!='\n')           //but do not pass the end of the line
                )tp+=1;                                                         
        if(*tp=='\0') return -3;                        //but exit at EOF
        vp = tp;                                //Current pos is potential value

        while((*tp!='\r')&&(*tp!='\n')&&(*tp!='\0')) tp+=1;  //Seek line end
        if(*tp!='\0') *tp++ = '\0';             //terminate value (passing if not EOF)
        while((*tp<=' ') && (*tp>'\0')) tp+=1;  //Skip following whitespace, if any

        *CNF_p_p = tp;                          //return new CNF file position
        *name_p_p = np;                         //return found variable name
        *value_p_p = vp;                        //return found variable value
        return 1;                               //return control to caller
}       //Ends get_CNF_string

//----------------------------------------------------------------
int ParseSYSTEMCNF(char *system_cnf, char *boot_path)
{
        int r, entrycnt, cnfsize;
        u8 *pcnf, *pcnf_start, *name, *value;
        int fd = -1;
        
        fd = open(system_cnf, O_RDONLY);
        if (fd < 0)
                return -1;              

        cnfsize = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
        
        pcnf = (char *)malloc(cnfsize);
        pcnf_start = pcnf;
        if (!pcnf) {
                close(fd);
                return -2;
        }
        
        r = read(fd, pcnf, cnfsize);    // Read CNF in one pass
        if (r != cnfsize)
                return -3;
                
        close(fd);
        pcnf[cnfsize] = 0;                              // Terminate the CNF string
        
        for (entrycnt = 0; get_CNF_string(&pcnf, &name, &value); entrycnt++) {
                if (!strcmp(name,"BOOT2"))  // check for BOOT2 entry
                        strcpy(boot_path, value);                       
        }
                        
        pcnf = pcnf_start;
        free(pcnf);
        
        return 1;
}

/*
 * Reset IOP
 */
void IOP_Reset(void)
{
  	while(!SifIopReset("rom0:UDNL rom0:EELOADCNF",0));
  	while(!SifIopSync());
  	fioExit();
  	SifExitIopHeap();
  	SifLoadFileExit();
  	SifExitRpc();

  	SifInitRpc(0);
  	FlushCache(0);
  	FlushCache(2);
}

/*
 * deInit pads, timers, exit services & clear screen
 */
void CleanUp(void)
{
	padPortClose(0,0);
	padPortClose(1,0);
	padEnd();

    TimerEnd();

  	fioExit();
  	SifExitIopHeap();
  	SifLoadFileExit();
  	SifExitRpc();

  	FlushCache(0);
  	FlushCache(2);

	Clear_Screen();
}

/*
 * asm delay function
 */
void delay(int count)
{
	int i;
	int ret;
	
	for (i  = 0; i < count; i++) {
		ret = 0x01000000;
		while(ret--)
			asm("nop\nnop\nnop\nnop");
	}
}

/*
 * reload OSDSYS, so reloads FMCB if installed
 */
void launch_OSDSYS(void)
{
	CleanUp();

	__asm__ __volatile__(
		"	li $3, 0x04;"
		"	syscall;"
		"	nop;"
	);
}

/*
 * Draw & render Main Menu
 */
void Update_MainMenu(void)
{
	/* Update GUI and render */
	Draw_MainMenu(selected_button, highlight_pulse);
	Render_GUI();
}

/*
 * Draw & render Options Menu
 */
void Update_OptionsMenu()
{
	/* Update GUI and render */
	Draw_OptionsMenu();
	Render_GUI();
}

/*
 * Draw & render About Menu
 */
void Update_AboutMenu(void)
{
	/* Update GUI and render */
	Draw_AboutMenu(PROGRAM_VERSION, &about_text[0]);
	Render_GUI();
}

/*
 * Draw & render Patches Menu
 */
void Update_PatchesMenu()
{
	Draw_PatchesMenu();
	Render_GUI();
	
}

void ResetCNF(void)
{
	fioOpen("cdrom0:\\SYSTEM.CNF;1",1);
	SifIopReset("rom0:UDNL cdrom0:\\SYSTEM.CNF;1", 0);

	while ( SifIopSync()) ;

	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();
	EI();

	SifInitRpc(0);

	FlushCache(0);
	FlushCache(2);
}

void StopDisc(void)
{
	fioOpen("qwerty:r",1); // Wait for the disc to be ready
	int fd = fioOpen(sys_cnf, O_RDONLY); //Just to get the disc spinning
	fioClose(fd);
	
	ResetCNF();
	IOP_Reset();

	SifInitRpc(0);
	LoadModules();

	fioOpen("qwerty:c",1); // Set LBA to 0
	fioOpen("qwerty:s",1); // Stop the disc

	setupPad();
}

int LoadSettings(void) {
	
	int fd = fioOpen(sfile, O_RDONLY);
	if (fd < 0) { NewSett(); return 0; }
	
	fioClose(fd);
	
	if (config_read_file(&config, sfile) != CONFIG_TRUE && DEBUG)
		printf("config error: %s\n", config_error_text(&config));
	config_print(&config);
	
	AutoJoker = config_get_bool(&config, SET_PRESET_JOKER);
	uLEJoker = config_get_bool(&config, SET_PRESET_ULE);
	#ifdef EH
		ExcHand = config_get_bool(&config, SET_INSTALL_EHANDLER);
	#endif
	ulePath = config_get_string(&config, (char*)SET_ULE_PATH);
	pktAlloc = config_get_int(&config, SET_PACKET_ALLOC);
	ehAlloc = config_get_int(&config, SET_EXC_HAND_ALLOC);
	regdAlloc = config_get_int(&config, SET_REG_DUMP_ALLOC);
	jokCombo = config_get_int(&config, SET_JOK_COMBO);
	
	return 0;
}

void NewSett(void) {
	FILE *fd;
	
	mkdir("mc0:/LiveDebug");
	
	fd = fopen(sfile, "w");
	fwrite(SettLayout, 1, strlen(SettLayout), fd);
	fclose(fd);
	
	//Set settings to default
	AutoJoker = 0;
	uLEJoker = 0;
	ExcHand = 0;
	pktAlloc = 0;
	ehAlloc = 0;
	regdAlloc = 0;
	ulePath = NULL;
}

/* config_write_file() outputs in a weird format... So I made my own version - Dnawrkshp */
int StoreSett(char *add_joker, char *add_jokerV, char *add_ver) {
	FILE *fd;
	char *settings;
	
	//Intead of recompiling the joker array stuff, I think it is faster to just copy it from the file...
	char *jarray = NULL;
	int fp = fioOpen(sfile, O_RDONLY); //Open settings file
	if (fp < 0) { goto nojarray; } //File doesn't exist
	
	int jsize = fioLseek(fp, 0, SEEK_END); //Go to end of file
	fioLseek(fp, 0, SEEK_SET); //Return to beginning
		
	//Allocate memory for jarray according to size of file and the potential arguments
	if (add_joker == NULL) {
		jarray = malloc(jsize + 1); //Arguments are invalid
	} else {
		jarray = malloc(jsize + strlen(add_joker) + strlen(add_jokerV) + strlen(add_ver) + 50); //Arguments are valid
	}
	
	memset(jarray, 0, jsize + 1);
	fioRead(fp, jarray, jsize); //Read contents of file into jarray
	fioClose(fp); //Close file
	
	int i = 0;
	while ((jarray[i] != 'j' || jarray[i + 5] != ':') && i < strlen(jarray)) { i++; } //Find the joker: {
	if (i >= strlen(jarray)) { jarray = NULL; } else { jarray += i; } //*jarray = *jarray + i
	jarray[jsize - i] = '\0';
	
	if (add_joker != NULL) {
		i = 0;
		while (jarray[i] != '[' && i < strlen(jarray)) { i++; } //Get to '[' in jarray
		i++; //Skip that char
		
		char *jtemp = malloc(strlen(jarray) - i + 1);
		strcpy(jtemp, jarray + i + 1); //Get last bit of string
		
		int x = 0;
		while (jarray[x] != '\"' && x < strlen(jarray)) { x++; } //Looks for another array, if yes then it adds a comma
		char *jcomma = ",";
		if (DEBUG)
			printf("x = %d/%d, %s\n", x, strlen(jarray), (x == strlen(jarray)) ? "There is no other array" : "Found another array; appending");
		if (x == strlen(jarray))
			jcomma = " ";
		
		sprintf(jarray + i, "\n\t\t\"%s\", \"%s\", \"%s\"%s\n\t%s", add_joker, add_jokerV, add_ver, jcomma, jtemp); //Insert new joker
		free(jtemp);
		//jarray[strlen(jarray)] = '\0'; //End string
	}
	
	goto donejarray;
	
	nojarray: ;
	jarray = malloc(60 + strlen(add_joker));
	sprintf(jarray, "joker: {\n\t" \
					"joker_array = [\n\t"
					"\"%s\", \"%s\"\n\t" \
					"];\n"
					"};", add_joker, add_jokerV);
	printf("No j array\njarray = %s\n", jarray);
	
	donejarray: ;
	
	if (jarray == NULL) {
		sprintf(jarray, "joker: {\n\t" \
					"joker_array = [\n\t"
					"];\n"
					"};");
	}
	
	//The main bit
	settings = malloc(100 + (jsize ? jsize : 200));
	
	sprintf(settings,
					"# CL-LiveDebug config file\n\n" \
					"main: {\n\t" \
						"preset_joker = %s;\n\t" \
						"preset_ule = %s;\n\t" \
						"install_ehandler = %s;\n\t" \
						"ule_path = \"%s\";\n" \
					"};\n\n" \
					"debugger: {\n\t" \
						"pkt_alloc = 0x%8.8x;\n\t" \
						"eh_alloc = 0x%8.8x;\n\t" \
						"regd_alloc = 0x%8.8x;\n" \
						"jok_combo = 0x%8.8x;\n" \
					"};\n\n" \
					"%s", AutoJoker ? "true" : "false", uLEJoker ? "true" : "false", ExcHand ? "true" : "false", 
						( ((char*)ulePath == NULL) ? "" : (char*)ulePath ),
						pktAlloc, ehAlloc, regdAlloc, (jokCombo << 16) >> 16, jarray);

	fd = fopen(sfile, "w");
	if (fd == NULL) { return -1; }
	fwrite(settings, 1, strlen(settings), fd);
	fclose(fd);
	
	free(settings);
	free(jarray);
	return 0;
}

void GetValidPath(void) {
	char *uLE = malloc(strlen((const char*)ulePath) + 1);
	memset(uLE, 0, sizeof(uLE));
	strcpy(uLE, ulePath);
	
	int fd = -1;
	if (uLE != NULL) { fd = fioOpen(uLE, O_RDONLY); }
	if (fd < 0) {
		uLE = "mc0:/BOOT/BOOT.ELF";
		fd = fioOpen(uLE, O_RDONLY);
		if (fd < 0) {
			uLE = "mc1:/BOOT/BOOT.ELF";
			fd = fioOpen(uLE, O_RDONLY);
			if (fd < 0) {
				uLE = "mass:/BOOT/BOOT.ELF";
				fd = fioOpen(uLE, O_RDONLY);
				if (fd < 0) {
					uLE = NULL;
				}
			}
		}
	}
	fioClose(fd);
	if (uLE != NULL) {
		strcpy((char*)ulePath, uLE);
	} else {
		ulePath = NULL;
	}
	if (DEBUG)
		printf("Valid path = %s\n", uLE);
	free(uLE);
}

/* Modified version of the ParseSYSTEMCNF() function */
int ParseVerNumber(char *cnf, char *ret) {
	
	int r, entrycnt, cnfsize;
    u8 *pcnf, *pcnf_start;
    int fd = -1;
        
    fd = open(cnf, O_RDONLY);
    if (fd < 0)
        return -1;              

    cnfsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
        
    pcnf = (char *)malloc(cnfsize);
    pcnf_start = pcnf;
    if (!pcnf) {
        close(fd);
        return -2;
    }
        
    r = read(fd, pcnf, cnfsize);    // Read CNF in one pass
    if (r != cnfsize)
                return -3;
                
    close(fd);
    pcnf[cnfsize] = 0;                              // Terminate the CNF string
        
    for (entrycnt = 0; entrycnt < cnfsize; entrycnt++) {
        if (pcnf[entrycnt-1] == 'R' && (pcnf[entrycnt] == ' ' || pcnf[entrycnt] == '=')) {
			pcnf += entrycnt; entrycnt = 0;
			while (pcnf[0] == ' ' || pcnf[0] == '=')
				pcnf++;
			while (((char)pcnf[entrycnt] >= 0x30 && (char)pcnf[entrycnt] <= 0x39) || (char)pcnf[entrycnt] == '.')
				entrycnt++;
			pcnf[entrycnt] = 0;
            strcpy(ret, pcnf);
		}
    }
	
    pcnf = pcnf_start;
    free(pcnf);
    
    return 1;
	
}

/*
 * An "intelligent" function that will load textures depending on whether
 * they have already been loaded or there is enough room in the allocated VRAM
 */
int LoadTextures(int mode) {
	
	int cBit = 1;
	int cnt = 0;
	int tSize = 0;
	
	/* Calculate the total size of the textures */
	for (cnt = 0; cnt < 8; cnt++) {
		if ((~mode & cBit) == 0) {
			switch (cnt) {
				case 0:	tSize += mmText_size; break;
				case 1:	tSize += bgText_size; break;
				case 2:	tSize += ptText_size; break;
				case 3:	tSize += ftText_size; break;
				case 4:	tSize += smText_size; break;
				case 5:	tSize += abText_size; break;
				case 6:	tSize += ynText_size; break;
				case 7:	tSize += kbText_size; break;
			}
		}
		cBit = cBit << 1;
	}
	
	if ((tSize + gsGlobal->CurrentPointer) >= vramMax) {
		if (DEBUG == 2)
			printf("Clearing VRAM\n");
		clearVRAM();
	}
	
	cBit = 1;
	for (cnt = 0; cnt < 8; cnt++) {
		if ((~mode & cBit) == 0) {
			switch (cnt) {
				case 0: /* Mainmenu */
					if (mmText) //Already loaded
						break;
					
					load_mainmenu_Textures();
					mmText = 1;
					break;
				case 1: /* Background */
					if (bgText) //Already loaded
						break;
					
					load_background_Textures();
					bgText = 1;
					break;
				case 2: /* Patches */
					if (ptText) //Already loaded
						break;
					
					load_patches_Texture();
					ptText = 1;
					break;
				case 3: /* Font */
					if (ftText) //Already loaded
						break;
					
					load_Font();
					ftText = 1;
					break;
				case 4: /* Submenu */
					if (smText) //Already loaded
						break;
					
					load_submenu_Textures();
					smText = 1;
					break;
				case 5: /* About */
					if (abText) //Already loaded
						break;
					
					load_about_Textures();
					abText = 1;
					break;
				case 6: /* YorN */
					if (ynText) //Already loaded
						break;
					
					load_YorN_Textures();
					ynText = 1;
					break;
				case 7: /* Keyboard */
					if (kbText) //Already loaded
						break;
					
					load_keyboard_Textures();
					kbText = 1;
					break;
			}
		}
		cBit = cBit << 1; //Next bit
	}
	return 1;
}

int clearVRAM(void) {
	vram_free();
	
	mmText = 0; /* load_mainmenu_Textures() */
	bgText = 0; /* load_background_Textures() */
	ptText = 0; /* load_patches_Texture() */
	ftText = 0; /* load_Font() */
	smText = 0; /* load_submenu_Textures() */
	abText = 0; /* load_about_Textures() */
	ynText = 0; /* load_YorN_Textures() */
	kbText = 0; /* load_keyboard_Textures() */
	
	return 1;
}
