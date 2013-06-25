#include "gui.h"
#include "textures.h"

/* main.c */
extern int PatchesLoaded;
extern int AutoJoker;
extern int uLEJoker;
extern int ExcHand;
extern int jokCombo;
extern const char *ulePath;
extern int StoreSett(char *add_joker, char *add_jokerV, char *add_ver);
extern int ParseSYSTEMCNF(char *system_cnf, char *boot_path);
extern int ParseVerNumber(char *cnf, char *ret);
extern char *sys_cnf;
extern int ParseJokerVal(int val, char *ret);
extern int LoadTextures(int mode);
extern int clearVRAM(void);
extern int lmainM;
extern int lPatch;
extern int loptM;
extern int labout;
extern int lyorn;
extern int lkeyb;

/* From pad.c */
extern u32 new_pad;
extern u32 old_pad;
extern int  readPad(void);
extern void waitAnyPadReady(void);
extern u32 paddata;

/*
 * Clears VRAM
 */
void vram_free(void) {
	gsKit_vram_clear(gsGlobal);
}

/*
 * Careful !!! Must be called after Load_Texture function, and before drawing anything !
 * Init default for many gfx functions
 */
void gfx_set_defaults(void)
{
	/* Init defaults for GUI */
	logo_alpha = 0;
	icon_start_y = SCREEN_HEIGHT + (240 * Y_RATIO);
	icon_stop_y = SCREEN_HEIGHT + (180 * Y_RATIO);
	icon_patches_y = SCREEN_HEIGHT + (120 * Y_RATIO);
	icon_options_y = SCREEN_HEIGHT + (60 * Y_RATIO);	
	icon_about_y = SCREEN_HEIGHT;
	icon_start_alpha = 0;
	icon_stop_alpha = 0;
	icon_about_alpha = 0;
	icon_options_alpha = 0;	
	icon_patches_alpha = 0;	
	desc_start_alpha = 0;
	desc_stop_alpha = 0;
	desc_about_alpha = 0;
	desc_options_alpha = 0;	
	desc_patches_alpha = 0;	
	background_alpha = 128;
	highlight_alpha = 32;
	amount = 6;
	pause_pulse = 0;

}

/*
 * setup GS
 */
void Setup_GS(int gs_vmode)
{
	/* GS Init */
	gsGlobal = gsKit_init_global_custom(
		GS_RENDER_QUEUE_OS_POOLSIZE, //+GS_RENDER_QUEUE_OS_POOLSIZE/2, /* eliminates overflow */
		GS_RENDER_QUEUE_PER_POOLSIZE);

	/* Clear Screen */
	gsKit_clear(gsGlobal, Black);

	/* Mode & screen width/height init */
	gsGlobal->Mode = gs_vmode;
    gsGlobal->Width = SCREEN_WIDTH;
    gsGlobal->Height = SCREEN_HEIGHT;

	/* Screen Position Init */
	gsGlobal->StartX = SCREEN_X;
	gsGlobal->StartY = SCREEN_Y;

	/* Buffer Init */
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsGlobal->PrimAAEnable = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_OFF;
	gsGlobal->ZBuffering      = GS_SETTING_OFF;
	gsGlobal->PSM = GS_PSM_CT32;
	
	/* Force Interlace and Field mode */
	gsGlobal->Interlace = GS_INTERLACED;
	gsGlobal->Field     = GS_FIELD;

	/* DMAC Init */
	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);

	/* Screen Init */
	gsKit_init_screen(gsGlobal);
	gsKit_clear(gsGlobal, Black);
		
	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);
}

/*
 * render GUI
 */
void Render_GUI(void)
{				     
	/* Flips Framebuffers on VSync */
	gsKit_sync_flip(gsGlobal);

	/* Normal User Draw Queue "Execution" (Kicks Oneshot and Persistent Queues) */
	gsKit_queue_exec(gsGlobal);	
		
	gsKit_queue_reset(gsGlobal->Per_Queue);
}

/*
 * clear screen
 */
void Clear_Screen(void)
{
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);
}

/*
 * Draw intro scene part 1
 * return 1 if intro is finished
 * otherwise return 0
 */
int Draw_INTRO_part1(void)
{
	int intro_done = 0;
	int logo_fadein_done = 0;
	
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Alpha calculation to control logo fade-in */
	if (logo_alpha < 128) {
		logo_alpha += 2;
		if (logo_alpha >= 128) {
			logo_alpha = 128;
			logo_fadein_done = 1;
		}
	}
	else
		logo_fadein_done = 1;

	/* Draw Background */
	draw_background(background_alpha, 0);
	
	/* Draw ghost logo */
	draw_logo_ghost(logo_alpha);
				
	/* Draw logo */
	draw_logo(logo_alpha);
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    if (logo_fadein_done)
    	intro_done = 1;

    /* Return 1 if intro have finished playing */
    if (intro_done) {
    	if (DEBUG != 0) {
	    	printf("Intro part 1 is done\n");
	    }

	    return 1;
    }
    return 0;
}

/*
 * Draw intro scene part 2
 * return 1 if intro is finished
 * otherwise return 0
 */
int Draw_INTRO_part2(void)
{
	int intro_done = 0;
	int icon_start_fadein_done = 0;
	int icon_stop_fadein_done = 0;
	int icon_options_fadein_done = 0;			
	int icon_about_fadein_done = 0;	
	int icon_patches_fadein_done = 0;		
	int desc_start_fadein_done = 0;
	int desc_stop_fadein_done = 0;
	int desc_options_fadein_done = 0;	
	int desc_about_fadein_done = 0;	
	int desc_patches_fadein_done = 0;
	int icon_start_move_done = 0;
	int icon_stop_move_done = 0;
	int icon_options_move_done = 0;		
	int icon_about_move_done = 0;	
	int icon_patches_move_done = 0;
	
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha, 0);

	/* Draw ghost logo */
	draw_logo_ghost(logo_alpha);
				
	/* Draw logo */
	draw_logo(logo_alpha);

	int min_y = 300 * Y_RATIO;
	/* Y coordinate calculation to start icon moving */
	if (icon_start_y > min_y) {
		icon_start_y -= 6;
		if (icon_start_y <= min_y) {
			icon_start_y = min_y;
			icon_start_move_done = 1;
		}
	}
	else
		icon_start_move_done = 1;
		
	/* Y coordinate calculation to stop icon moving */
	if (icon_stop_y > min_y) {
		icon_stop_y -= 6;
		if (icon_stop_y <= min_y) {
			icon_stop_y = min_y;
			icon_stop_move_done = 1;
		}
	}
	else
		icon_stop_move_done = 1;

	/* Y coordinate calculation to options icon moving */
	if (icon_options_y > min_y) {
		icon_options_y -= 6;
		if (icon_options_y <= min_y) {
			icon_options_y = min_y;
			icon_options_move_done = 1;
		}
	}
	else
		icon_options_move_done = 1;		
		
	/* Y coordinate calculation to about icon moving */
	if (icon_about_y > min_y) {
		icon_about_y -= 6;
		if (icon_about_y <= min_y) {
			icon_about_y = min_y;
			icon_about_move_done = 1;
		}
	}
	else
		icon_about_move_done = 1;

	/* Y coordinate calculation to patches icon moving */
	if (icon_patches_y > min_y) {
		icon_patches_y -= 6;
		if (icon_patches_y <= min_y) {
			icon_patches_y = min_y;
			icon_patches_move_done = 1;
		}
	}
	else
		icon_patches_move_done = 1;
		
	/* Alpha calculation to control icon start fade-in */
	if (icon_start_y <= SCREEN_HEIGHT) {
		if (icon_start_alpha < 128) {
			icon_start_alpha += 3;
			if (icon_start_alpha >= 128) {
				icon_start_alpha = 128;
				icon_start_fadein_done = 1;
			}
		}
		else
			icon_start_fadein_done = 1;
	}
				
	/* Alpha calculation to control icon stop fade-in */
	if (icon_stop_y <= SCREEN_HEIGHT) {	
		if (icon_stop_alpha < 128) {
			icon_stop_alpha += 3;
			if (icon_stop_alpha >= 128) {
				icon_stop_alpha = 128;
				icon_stop_fadein_done = 1;
			}
		}
		else
			icon_stop_fadein_done = 1;
	}

	/* Alpha calculation to control icon options fade-in */
	if (icon_options_y <= SCREEN_HEIGHT) {	
		if (icon_options_alpha < 128) {
			icon_options_alpha += 3;
			if (icon_options_alpha >= 128) {
				icon_options_alpha = 128;
				icon_options_fadein_done = 1;
			}
		}
		else
			icon_options_fadein_done = 1;
	}
						
	/* Alpha calculation to control icon about fade-in */
	if (icon_about_y <= SCREEN_HEIGHT) {
		if (icon_about_alpha < 128) {
			icon_about_alpha += 3;
			if (icon_about_alpha >= 128) {
				icon_about_alpha = 128;
				icon_about_fadein_done = 1;
			}
		}
		else
			icon_about_fadein_done = 1;
	}
	
	if (icon_patches_y <= SCREEN_HEIGHT) {
		if (icon_patches_alpha < 128) {
			icon_patches_alpha += 3;
			if (icon_patches_alpha >= 128) {
				icon_patches_alpha = 128;
				icon_patches_fadein_done = 1;
			}
		}
		else
			icon_patches_fadein_done = 1;
	}
				
	/* Draw icons */
	draw_image(tex_icon_start, icon_start_x, icon_start_y, icon_start_alpha); //148
	draw_image(tex_icon_stop, icon_stop_x, icon_stop_y - 5, icon_stop_alpha); //242
	draw_image(tex_icon_options, icon_options_x, icon_options_y, icon_options_alpha);	//344
	draw_image(tex_icon_about, icon_about_x, icon_about_y, icon_about_alpha); //442
	draw_image(tex_icon_patches, icon_patches_x, icon_patches_y, icon_patches_alpha); //442
	
	/* Alpha calculation to control desc start fade-in */
	if (icon_start_fadein_done) {
		if (desc_start_alpha < 32) {
			desc_start_alpha += 2;
			if (desc_start_alpha >= 32) {
				desc_start_alpha = 32;
				desc_start_fadein_done = 1;
			}
		}
		else
			desc_start_fadein_done = 1;
	}	

	/* Alpha calculation to control desc stop fade-in */
	if (desc_start_alpha > 8) {
		if (desc_stop_alpha < 32) {
			desc_stop_alpha += 2;
			if (desc_stop_alpha >= 32) {
				desc_stop_alpha = 32;
				desc_stop_fadein_done = 1;
			}
		}
		else
			desc_stop_fadein_done = 1;
	}	

	/* Alpha calculation to control desc options fade-in */
	if (desc_stop_alpha > 8) {
		if (desc_options_alpha < 32) {
			desc_options_alpha += 2;
			if (desc_options_alpha >= 32) {
				desc_options_alpha = 32;
				desc_options_fadein_done = 1;
			}
		}
		else
			desc_options_fadein_done = 1;
	}	
	
	/* Alpha calculation to control desc about fade-in */
	if (desc_options_alpha > 8) {
		if (desc_about_alpha < 32) {
			desc_about_alpha += 2;
			if (desc_about_alpha >= 32) {
				desc_about_alpha = 32;
				desc_about_fadein_done = 1;
			}
		}
		else
			desc_about_fadein_done = 1;
	}
	
	if (desc_about_alpha > 8) {
		if (desc_patches_alpha < 32) {
			desc_patches_alpha += 2;
			if (desc_patches_alpha >= 32) {
				desc_patches_alpha = 32;
				desc_patches_fadein_done = 1;
			}
		}
		else
			desc_patches_fadein_done = 1;
	}
			
	/* Draw descs */
	draw_image(tex_desc_start, desc_start_x, desc_stop_y * Y_RATIO, desc_start_alpha);
	draw_image(tex_desc_stop, desc_stop_x, desc_stop_y * Y_RATIO, desc_stop_alpha);
	draw_image(tex_desc_options, desc_options_x, desc_stop_y * Y_RATIO, desc_options_alpha);	
	draw_image(tex_desc_about, desc_about_x, desc_stop_y * Y_RATIO, desc_about_alpha);
	draw_image(tex_desc_patches, desc_patches_x, desc_stop_y * Y_RATIO, desc_patches_alpha);
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    if ((icon_start_move_done) && (icon_stop_move_done) && (icon_about_move_done) && \
    	(icon_start_fadein_done) && (icon_stop_fadein_done) && (icon_about_fadein_done) && (icon_options_fadein_done) && \
    	(desc_start_fadein_done) && (desc_stop_fadein_done) && (desc_about_fadein_done) && (desc_options_fadein_done) && \
		(icon_patches_move_done) && (icon_patches_fadein_done) && (desc_patches_fadein_done))
    	intro_done = 1;

    /* Return 1 if intro have finished playing */
    if (intro_done) {
    	if (DEBUG != 0) {
	    	printf("Intro part 2 is done\n");
	    }

	    return 1;
    }
    return 0;
}

/*
 * Draw MainMenu
 */
int Draw_MainMenu(int selected_button, int highlight_pulse)
{
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha, 0);

	/* Draw ghost logo */
	draw_logo_ghost(logo_alpha);
				
	/* Draw logo */
	draw_logo(logo_alpha);
				
	/* Draw icons */		
	draw_image(tex_icon_start, icon_start_x, icon_start_y, icon_start_alpha); //148
	draw_image(tex_icon_stop, icon_stop_x, icon_stop_y - 5, icon_stop_alpha); //242
	draw_image(tex_icon_options, icon_options_x, icon_options_y, icon_options_alpha);	//344
	draw_image(tex_icon_about, icon_about_x, icon_about_y, icon_about_alpha); //442
	draw_image(tex_icon_patches, icon_patches_x, icon_patches_y, icon_patches_alpha); //442
						
	/* Alpha calculation to control Highlight pulse	*/
	if (highlight_pulse) {
		highlight_alpha += amount;
		if (highlight_alpha > 128) {
			highlight_alpha = 128;
			pause_pulse++;
			if (pause_pulse >= 12) {
				amount = -3;
				pause_pulse = 0;
			}
		} else if (highlight_alpha < 32) {
			amount = 3;
			highlight_alpha = 32;
		}
	}
	else {
		highlight_alpha = 128;	
	}
		
	desc_start_alpha = 32;
	desc_stop_alpha = 32;
	desc_options_alpha = 32;	
	desc_about_alpha = 32;
	desc_patches_alpha = 32;
	
	switch (selected_button) {
		case 1:
			desc_start_alpha = highlight_alpha;
			break;
		case 2:
			desc_stop_alpha = highlight_alpha;
			break;
		case 3:
			desc_patches_alpha = highlight_alpha;
			break;
		case 4:
			desc_options_alpha = highlight_alpha;
			break;
		case 5:
			desc_about_alpha = highlight_alpha;
			break;			
	}
	
	/* Draw descs */
	draw_image(tex_desc_start, desc_start_x, desc_stop_y * Y_RATIO, desc_start_alpha);
	draw_image(tex_desc_stop, desc_stop_x, desc_stop_y * Y_RATIO, desc_stop_alpha);
	draw_image(tex_desc_options, desc_options_x, desc_stop_y * Y_RATIO, desc_options_alpha);	
	draw_image(tex_desc_about, desc_about_x, desc_stop_y * Y_RATIO, desc_about_alpha);
	draw_image(tex_desc_patches, desc_patches_x, desc_stop_y * Y_RATIO, desc_patches_alpha);
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
    
    return 1;
}

/*
 * Checks if value si within range, if not it loops
 */
int CheckVal(int val, int min, int max) {
	int low, high;
	low = min; high = max;
	if (low > high) { low = max; high = min; }
	if (low == high) { return -1; }
	
	if (val < low) { return high; }
	if (val > high) { return low; }
	return val;
}
 
/*
 * Draw OptionsMenu
 */
int Draw_OptionsMenu(void)
{	
	int retval = 0;
	if (new_pad & PAD_RIGHT) { //Right
		switch (optselbutt) {
			case 1: AutoJoker++; break;
			case 2: uLEJoker++; break;
			case 3: ExcHand++; break;
		}
	}
	if (new_pad & PAD_LEFT) { //Left
		switch (optselbutt) {
			case 1: AutoJoker--; break;
			case 2: uLEJoker--; break;
			case 3: ExcHand--; break;
		}
	}
	if (new_pad & PAD_UP) 
		optselbutt--; //Up
	if (new_pad & PAD_DOWN) 
		optselbutt++; //Down
	
	if (new_pad & PAD_CROSS) {
			if (optselbutt == 5) { /* Joker combo to open the debugger */
				retval = 0x20000;
				while (retval == 0x20000) {
					retval = draw_jokerCombo("Press the buttons you wish to set as the joker", "Then press the Cross (X) button");
					Render_GUI();
				}
				
				if (retval == 0x10000 || retval == 0x20000)
					goto opt_5_exit;
				
				if (retval > 0)
					jokCombo = ~retval;
				else
					jokCombo = retval;
				jokUpdate = 1;
				StoreSett(NULL, NULL, NULL);
				
				opt_5_exit: ;
			} else if (optselbutt == 4) { /* Set the uLE boot path */
			
			//Load the keyboard and font textures into the vram
			LoadTextures(lkeyb);
			
			char *ret = malloc(100);
			memset(ret, 0, 100);
			strcpy(ret, ulePath);
			kb_ptr = strlen(ret);
			kb_sel = 27; //0
			while (retval <= 0) {
				retval = draw_keyBoard(ret, 0, "uLaunchELF boot path");
				Render_GUI();
			}
		
			if (retval == 1) //Enter was selected
				strcpy((char*)ulePath, (char*)ret);
			
			if (DEBUG)
				printf("Keyboard returned %s\n", (char*)ret);
			
			free(ret);
			
			//Reload the textures into the vram
			LoadTextures(loptM);
			
		} else if (optselbutt == (int)opt_max) { /* Add custom preset joker */
			
			char *ret = malloc(50);
			char *ret2 = malloc(11);
			int loadType = 0;
			
			//Load the keyboard and font textures into the vram
			LoadTextures(lyorn);
			
			while (retval <= 0) {
				retval = draw_YorN("Would you like to load the Sony ID from the disc?", "Load from disc", "Enter manually");
				Render_GUI();
			}
			
			LoadTextures(lkeyb);
			
			if (retval == 3)
				goto exit_opt_max;
			
			loadType = retval;
			if (retval == 1) { //Load from SYSTEM.CNF
				int x = 0;
				ParseSYSTEMCNF(sys_cnf, ret);
				while (ret[0] != '\\') { ret++; } //Finds '\' in cdrom0:\\....
				ret++; //Skips '\'
				while (ret[x] != ';') { x++; } //Finds the ';' in ????_???.??;1
				ret[x] = '\0'; //Ends string at S???_???.??
				
			} else {
				memset(ret, 0, 12);
				kb_ptr = 0;
				kb_sel = 27; //A
				retval = 0;
				while (retval <= 0) {
					retval = draw_keyBoard(ret, 0, "ELF ID");
					Render_GUI();
				}
			}
			if (retval == 1) { //Enter was selected
			
				memset(ret2, 0, 11);
				strcpy(ret2, "0x");
				kb_ptr = 2;
				kb_sel = 0; //0
				retval = 0;
				while (retval <= 0) {
					retval = draw_keyBoard(ret2, 1, "Joker address");
					Render_GUI();
				}
			
				if (DEBUG)
					printf("ret = %s\nret2 = %s\n", (char*)ret, (char*)ret2);
				
				if (ret != NULL && ret2 != NULL && strcmp(ret2, "0x") != 0 && retval == 1) { //Enter was selected
					char *ret3 = malloc(10);
					if (loadType == 1) { /* Load from CNF */
						ParseVerNumber(sys_cnf, ret3);
					} else { /* Enter manually */
						memset(ret3, 0, 11);
						strcpy(ret3, "");
						kb_ptr = 0;
						kb_sel = 0; //0
						retval = 0;
						while (retval <= 0) {
							retval = draw_keyBoard(ret3, 0, "Version number");
							Render_GUI();
						}
					}
					
					if (ret3 == NULL) {
						free(ret2);
						goto exit_opt_max;
					}
					
					StoreSett(ret, ret2, ret3);
					free(ret3);
				}
				
				free(ret2);
			}
			exit_opt_max: ;
			free(ret);
			
			//Reload the textures into the vram
			LoadTextures(loptM);
		}
	}
	
	AutoJoker = CheckVal(AutoJoker, 0, 1);
	uLEJoker = CheckVal(uLEJoker, 0, 1);
	ExcHand = CheckVal(ExcHand, 0, 1);
	optselbutt = CheckVal(optselbutt, 1, (int)opt_max);
	
	if (jokUpdate == 1 || strcmp(jokComStr, "") == 0) {
		if (jokCombo != 0)
			ParseJokerVal(~jokCombo, jokComStr);
		else {
			jokCombo = 0x0000FFF9;
			strcpy((char*)jokComStr, "L3+R3");
			StoreSett(NULL, NULL, NULL);
		}
		jokUpdate = 0;
	}
	
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha, 0);
		
	/* Alpha calculation for options menu header */
	if (about_menu_header_alpha < 128) { //Don't mind the name... :P
		about_menu_header_alpha += 2;
		if (about_menu_header_alpha > 128) {
			about_menu_header_alpha = 128;
		}
	}
		
	/* Draw options mini icon */ 
	draw_image(tex_icon_options_mini, 34, 34 * Y_RATIO, about_menu_header_alpha);
	
	/* Draw text */
	drawString_neuropol(61, 33 * Y_RATIO, about_menu_header_alpha, 22, 0, "Options");
	
	DrawMenuBar(menu_bar_x);
	
	if (menu_bar_x == SCREEN_WIDTH) {
	
		if (options_text_alpha < 128) {
			options_text_alpha += 2;		
			if (options_text_alpha >= 128) {
				options_text_alpha = 128;
				highlight_alpha = 32;
			}
		}
	
		drawString_neuropol(40, 80 * Y_RATIO, options_text_alpha, 20, 0, "Auto Load Preset Joker on Boot?");
		drawString_neuropol(40, 110 * Y_RATIO, options_text_alpha, 20, 0, "Auto Load Preset Joker with uLE?");
		drawString_neuropol(40, 140 * Y_RATIO, options_text_alpha, 20, 0, "Install the Exception Handler?");
		drawString_neuropol(40, 170 * Y_RATIO, options_text_alpha, 20, 0, "uLaunchELF Boot Path:");
		drawString_neuropol(40, 200 * Y_RATIO, options_text_alpha, 20, 0, "Debugger Combo:");
		centerString_neuropol((SCREEN_HEIGHT - 40) * Y_RATIO, add_pj_alpha, 20, 0, "-> Add New Preset Joker <-");
		
			highlight_alpha += amount;
			if (highlight_alpha > 128) {
				highlight_alpha = 128;
				pause_pulse++;
				if (pause_pulse >= 12) {
					amount = -3;
					pause_pulse = 0;
				}
			} else if (highlight_alpha < 32) {
				amount = 3;
				highlight_alpha = 32;
			}
	
		joker_alpha = 32;
		ule_alpha = 32;
		exc_alpha = 32;
		uboot_alpha = 32;
		dc_alpha = 32;
		add_pj_alpha = 32;
	
		switch (optselbutt) {
			case 1:
				joker_alpha = highlight_alpha;
				break;
			case 2:
				ule_alpha = highlight_alpha;
				break;
			case 3:
				exc_alpha = highlight_alpha;
				break;
			case 4:
				uboot_alpha = highlight_alpha;
				break;
			case 5:
				dc_alpha = highlight_alpha;
				break;
			case (int)opt_max:
				add_pj_alpha = highlight_alpha;
				break;
		}
		
		if (options_text_alpha == 128) {
			drawString_neuropol(500, 80 * Y_RATIO, joker_alpha, 20, 0, AutoJoker ? "Yes": "No");
			drawString_neuropol(500, 110 * Y_RATIO, ule_alpha, 20, 0, uLEJoker ? "Yes": "No");
			drawString_neuropol(500, 140 * Y_RATIO, exc_alpha, 20, 0, ExcHand ? "Yes": "No");
			drawStringRight_neuropol(540, 200 * Y_RATIO, dc_alpha, 20, 0, (char*)jokComStr);
			
			char *uTemp = malloc(100);
			strcpy(uTemp, ulePath);
			while (getStringWidth_neuropol(uTemp, 25, 0) > 200)
				uTemp++;
				
			if (strlen(uTemp) != strlen(ulePath)) {
				uTemp -= 3;
				memcpy(uTemp, "...", 3);
			}
			
			drawString_neuropol(400, 170 * Y_RATIO, uboot_alpha, 20, 0, uTemp);
			free(uTemp);
		}
		
	}
	
	if (menu_bar_x < SCREEN_WIDTH) { menu_bar_x += rate; }
	if (menu_bar_x > SCREEN_WIDTH) { menu_bar_x = SCREEN_WIDTH; }
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);

    return 1;
}

/*
 * Draw PatchesMenu
 */
int Draw_PatchesMenu(void)
{
	int retval = 0;
	
	if (new_pad & PAD_RIGHT) { selbutt++; patchret = NULL; } 
	if (new_pad & PAD_LEFT) { selbutt--; patchret = NULL; }

	if (selbutt > 3) { selbutt = 1; }
	if (selbutt < 1) { selbutt = 3; }
	
	if (new_pad & PAD_CROSS) {
		retval = LoadPatches(selbutt - 1);
		if (retval < 0) {
			patchret = "Failed To Load Patch File";
			patch_x = 120;
			*(u32*)0x00EFFFFC = 0; //Set the amount of patches to 0
			PatchesLoaded = 0; //WritePatches() won't write patches
		} else { 
			patchret = "Successfully Loaded Patch File";
			patch_x = 70;
		}
	}
	
	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);

	/* Draw Background */
	draw_background(background_alpha, 0);
	
	/* Alpha calculation for patches menu header */
	if (about_menu_header_alpha < 128) { //Don't mind the name... :P
		about_menu_header_alpha += 2;
		if (about_menu_header_alpha > 128) {
			about_menu_header_alpha = 128;
		}
	}
	
	/* Draw patches mini icon */ 
	draw_image(tex_icon_patches_mini, 31, 37 * Y_RATIO, about_menu_header_alpha);
	
	/* Draw text */
	drawString_neuropol(61, 33 * Y_RATIO, about_menu_header_alpha, 22, 0, "Patches");

	/* Draw LD logo */
	draw_logo(logo_alpha);
	
	/* draw menu delimeter */
	DrawMenuBar(menu_bar_x);
	
	if (menu_bar_x == SCREEN_WIDTH) {
			
		if (patches_image_alpha < 128) {
			patches_image_alpha += 2;		
			if (patches_image_alpha >= 128) {
				patches_image_alpha = 128;
				highlight_alpha = 32;
			}
		}
	
		/* Draw images */
		draw_image(tex_icon_mc, 100, 300 * Y_RATIO, patches_image_alpha);
		draw_image(tex_icon_mc, 250, 300 * Y_RATIO, patches_image_alpha);
		draw_image(tex_icon_mass, 400, 300 * Y_RATIO, patches_image_alpha);
	
		if (patchret != NULL) { drawString_neuropol(patch_x, 440 * Y_RATIO, 128, 30, 0, patchret); }
	
		/* Alpha calculation to control Highlight pulse	*/
		if (patches_image_alpha == 128) {
			highlight_alpha += amount;
			if (highlight_alpha > 128) {
				highlight_alpha = 128;
				pause_pulse++;
				if (pause_pulse >= 12) {
					amount = -3;
					pause_pulse = 0;
				}
			} else if (highlight_alpha < 32) {
				amount = 3;
				highlight_alpha = 32;
			}
		}
	
		desc_mc0_alpha = 32;
		desc_mc1_alpha = 32;
		desc_mass_alpha = 32;
	
		switch (selbutt) {
			case 1:
				desc_mc0_alpha = highlight_alpha;
				break;
			case 2:
				desc_mc1_alpha = highlight_alpha;
				break;
			case 3:
				desc_mass_alpha = highlight_alpha;
				break;		
		}
		
		/* Draw text descriptions */
		if (patches_image_alpha == 128) {
			draw_image(tex_desc_mc0, 100, 400 * Y_RATIO, desc_mc0_alpha);
			draw_image(tex_desc_mc1, 250, 400 * Y_RATIO, desc_mc1_alpha);
			draw_image(tex_desc_mass, 400, 400 * Y_RATIO, desc_mass_alpha);
		}
		
	}
	if (menu_bar_x < SCREEN_WIDTH) { menu_bar_x += rate; }
	if (menu_bar_x > SCREEN_WIDTH) { menu_bar_x = SCREEN_WIDTH; }
	
	gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
	
    return 0;
}

/*
 * Init AboutMenu
 */
int Init_AboutMenu(void)
{
	about_menu_header_alpha = 0;
	about_menu_thx_alpha = 0;
	about_menu_content_alpha = 0;

	return 1;
}

/*
 * Draw AboutMenu
 */
int Draw_AboutMenu(char *version, struct about_content *about_text)
{
	char ver[16];
	int about_menu_header_fadein_done = 0;
	int about_menu_thx_fadein_done = 0;
	int about_menu_content_fadein_done = 0;

	/* Clear screen	*/
	gsKit_clear(gsGlobal, Black);

	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);
	
	/* Draw Background */
	draw_background(background_alpha, 0);

	/* Alpha calculation for about menu header */
	if (about_menu_header_alpha < 128) {
		about_menu_header_alpha += 2;
		if (about_menu_header_alpha >= 128) {
			about_menu_header_alpha = 128;
			about_menu_header_fadein_done = 1;
		}
	}
	else
		about_menu_header_fadein_done = 1;
					
	/* Draw about mini icon */ 
	draw_image(tex_icon_about_mini, 34, 37 * Y_RATIO, about_menu_header_alpha);
	
	/* Draw text */
	drawString_neuropol(61, 30 * Y_RATIO, about_menu_header_alpha, 29, 2, "About");
	sprintf(ver, "v%s", version);
	drawString_neuropol(550, 39 * Y_RATIO, about_menu_header_alpha, 16, 0, ver);
	
	DrawMenuBar(menu_bar_x);
	if (menu_bar_x < SCREEN_WIDTH) { menu_bar_x += rate; }
	if (menu_bar_x > SCREEN_WIDTH) { menu_bar_x = SCREEN_WIDTH; }
	
	if (menu_bar_x == SCREEN_WIDTH) {
		/* Alpha calculation for about menu header */
		if (about_menu_thx_alpha < 128) {
			about_menu_thx_alpha += 5;
			if (about_menu_thx_alpha >= 128) {
				about_menu_thx_alpha = 128;
				about_menu_thx_fadein_done = 1;
			}
		}
		else
			about_menu_thx_fadein_done = 1;
		
		centerString_neuropol(99 * Y_RATIO, about_menu_thx_alpha, 25, 1, "Thank you for using CL-LiveDebug!");	
		centerString_neuropol(124 * Y_RATIO, about_menu_thx_alpha, 20, 1, "a PlayStation 2 In-Game Debugger");
	}

	if (about_menu_thx_fadein_done) {
		/* Alpha calculation for about menu content */
		if (about_menu_content_alpha < 128) {
			about_menu_content_alpha += 5;
			if (about_menu_content_alpha >= 128) {
				about_menu_content_alpha = 128;
				about_menu_content_fadein_done = 1;
			}
		}
		else
			about_menu_content_fadein_done = 1;
		
		int y = 180 * Y_RATIO;
		while (about_text->name) {
			drawString_neuropol(34, y, about_menu_content_alpha, 25, 1, about_text->name);
			y += getStringHeight_neuropol(about_text->name, 25);
			drawString_neuropol(34, y, about_menu_content_alpha, 20, 1, about_text->desc);
			y += getStringHeight_neuropol(about_text->desc, 20);
			y += 20 * Y_RATIO;
			about_text++;
		}
	}
	
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
    
    if (about_menu_content_fadein_done)
    	return 1;

    return 0;
}

int LoadPatches(int m)
{
	int f = 0, i;
	u32 textAddr = 0x00F00000;

	memset((u32*)textAddr, 0, 0x00100000);
	
	//for (i = 0; i < 0x00100000; i += 4)
	//	*(u32*)(&textAddr + i) = 0;

	if (m == 0) { f = fioOpen("mc0:/LiveDebug/LDPatches.txt", O_RDONLY); }
	if (m == 1) { f = fioOpen("mc1:/LiveDebug/LDPatches.txt", O_RDONLY); }
	if (m == 2) { f = fioOpen("mass:/LiveDebug/LDPatches.txt", O_RDONLY); }

	if (f < 0) { return -1; }

	i = fioLseek(f, 0, SEEK_END);
	fioLseek(f, 0, SEEK_SET);

	if (i <= 0) { fioClose(f); return -2; }

	fioRead(f, (u32*)textAddr, i);
	fioClose(f);

	*(u32*)0x00EFFFFC = i;

	PatchesLoaded = 1;

	return 1;
}

/*
 * Draws menu bar from 0 to X
 */
int DrawMenuBar(int X) {
	int cnt = 0;
	
	while (cnt <= X) {
		draw_image(tex_menu_bar, cnt, menu_bar_y * Y_RATIO, menu_bar_a);
		cnt++;
	}
	return 0;
}

int draw_keyBoard(char *ret, int mode, char *title) {
	kb_box_max = kb_frame_x + 490;
	
	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);
	
	/* Calculate the title string's x and y */
	int tStringFontSize = 20;
	int tString_x = ((kb_frame_x + kb_frame_w) / 2) - (getStringWidth_neuropol(title, tStringFontSize, 0) / 2);
	int tString_y = 0;
	if (mode == 0)
		tString_y = (kb_frame_y + kb_frame_h) - getStringHeight_neuropol(title, tStringFontSize) - 25;
	else
		tString_y = (kb_frame_y + kb_frame_h) - getStringHeight_neuropol(title, tStringFontSize) - 70;
	
	switch (mode) {
		case 0:
			draw_kb_frame(); /* Order matters! */
			draw_kb_chars();
			ret[100] = '\0'; //Just to make sure
			break;
		case 1:
			draw_kb_frame_hex(); /* Order matters! */
			draw_kb_hex();
			ret[11] = '\0'; //Just to make sure
			break;
	}
	
	/* This part is long, stupid, and horribly put together. I just wanted something that worked */
	char *selSizetemp = malloc(100);
	memset(selSizetemp, 0, 100);
	strcpy(selSizetemp, ret);
	//End string at selector
	selSizetemp[kb_ptr] = '\0';
	
	if (getStringWidth_neuropol(ret, 25, 0) <= kb_box_max) { //The string is within the box, skip the long calculation
		//Draw the string
		drawString_neuropol(kb_xoff - 20, (kb_yoff - 45) * Y_RATIO, 128, 25, 0, ret);
		
		//Calculate the x coordinate of the selector
		int selSize = getStringWidth_neuropol(selSizetemp, 25, 0);
		//Draw the selector
		draw_image(tex_sel_bar2, selSize + kb_xoff - 20, (kb_yoff - 50) * Y_RATIO, highlight_alpha); //Draw the selector
	} else {
	
	int ret_off = 0;
	
	//Get the last bit fo the string that can fit within the box
	while (getStringWidth_neuropol(selSizetemp, 25, 0) > kb_box_max) { selSizetemp++; ret_off++; }
	if (!ret_off) { //If it fits within the box
		int i = kb_ptr;
		while (getStringWidth_neuropol(selSizetemp, 25, 0) < 480 && i < strlen(ret)) { //Get the first bit of the string that fits in the box
			memcpy(selSizetemp + i, ret + i, 1); //Replace old NULL byte with proper byte
			i++;
			selSizetemp[i] = '\0';
		}
		//Draw first bit of the string
		drawString_neuropol(kb_xoff - 20, (kb_yoff - 45) * Y_RATIO, 128, 25, 0, selSizetemp);
	} else {
		//Draw the last bit of the string
		drawString_neuropol(kb_xoff - 20, (kb_yoff - 45) * Y_RATIO, 128, 25, 0, selSizetemp);
	}
	
	//Get the x coordinate of the selector
	selSizetemp[kb_ptr] = '\0';
	int selSize = getStringWidth_neuropol(selSizetemp, 25, 0);
	//Draw the selector
	draw_image(tex_sel_bar2, selSize + kb_xoff - 20, (kb_yoff - 50) * Y_RATIO, highlight_alpha);
	
	}
	
	/* Draw the title string */
	drawString_neuropol(tString_x, tString_y * Y_RATIO, 128, tStringFontSize, 0, title);
	
	free(selSizetemp);
	
	gsKit_set_test(gsGlobal, GS_ATEST_ON);
    
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
	
	
	/* Moving away from that complicated stuff onto a huge tornado of if statements... */
	char kchar[10];
	
	waitAnyPadReady();
	if (readPad() && !mode) {
		if (new_pad & PAD_TRIANGLE) {
			return 2;
		} else if (new_pad & PAD_CROSS) {
			if (kb_sel == ENT)
				return 1;
			if (kb_sel == CANC)
				return 2;
			
			if (kb_caps || kb_shift) //Uppercase
				strcpy(kchar, keysU[kb_sel]);
			if (!kb_caps && !kb_shift) //Lowercase
				strcpy(kchar, keysL[kb_sel]);
			
			kb_shift = 0; //Since shift only is on once, it can be turned off afterwards
			
			if (strlen(kchar) != 1) {
				if (strcmp(kchar, "TAB") == 0) {
					strcpy(kchar, " "); //This font doesn't have a tab character...
				} else if (strcmp(kchar, "SPACE") == 0) {
					strcpy(kchar, " ");
				} else if (strcmp(kchar, "SHIFT") == 0) {
					strcpy(kchar, "");
					kb_shift = (kb_shift ? 0 : 1);
					if (kb_shift)
						kb_caps = 0;
				} else if (strcmp(kchar, "CAPS") == 0) {
					strcpy(kchar, "");
					kb_caps = (kb_caps ? 0 : 1);
					if (kb_caps)
						kb_shift = 0;
				}
			}
			
			if (strcmp(kchar, "") != 0 && kb_ptr < 99) {
				char *kb_temp0 = malloc(100);
				strcpy(kb_temp0, ret);
			
				kb_temp0 += kb_ptr; //End string
				
				sprintf(ret + kb_ptr, "%s%s", kchar, kb_temp0);
				free(kb_temp0);
				kb_ptr++;
				if (kb_sel == SPEC1) //SPEC1 = ':/'
					kb_ptr++;
			}
			
		} else if (new_pad & PAD_LEFT) { //Move left
			if ((kb_sel % 13) == 0 && kb_sel != 52) {
				kb_sel += 12; //Loop back around
			} else if (kb_sel != 0)
				kb_sel--;
		} else if (new_pad & PAD_RIGHT) { //Move right
			if ((kb_sel % 13) == 12 && kb_sel != 52) {
				kb_sel -= 12; //Loop back around
			} else if (kb_sel != 52)
				kb_sel++;
		} else if (new_pad & PAD_DOWN) { //Move down
			if (kb_sel < (row * 4) - 1) {
				if (kb_sel >= (row * 3)) {
					old_sel = kb_sel + row; //Since PAD_UP subtracts a row, I counter that with adding a row
					kb_sel = 52;
				} else {
					kb_sel += row;
				}
			}
		} else if (new_pad & PAD_UP) { //Move up
			if (kb_sel == 52)
				kb_sel = old_sel;
			if (kb_sel > 12)
				kb_sel -= row;
		} else if (new_pad & PAD_START) { //Jump to ENTER
			kb_sel = ENT;
		} else if (new_pad & PAD_SQUARE && kb_ptr > 0) { //Delete
			char *kb_temp1 = malloc(100);
			strcpy(kb_temp1, ret);
			
			ret[kb_ptr - 1] = '\0';
			kb_temp1 += kb_ptr;
			strcpy(ret + (kb_ptr - 1), kb_temp1);
			free(kb_temp1);
			kb_ptr--;
		} else if (new_pad & PAD_R1) { //Move ptr right
			if (kb_ptr < strlen(ret) && kb_ptr < 99)
				kb_ptr++;
		}else if (new_pad & PAD_L1) { //Move ptr left
			if (kb_ptr > 0)
				kb_ptr--;
		} else if (new_pad & PAD_L2) { //Turn on/off caps lock
			kb_caps = !kb_caps;
		} else if (new_pad & PAD_CIRCLE) { //Add a space
			char *kb_temp0 = malloc(100);
			strcpy(kb_temp0, ret);
			kb_temp0 += kb_ptr; //End part of string
				
			sprintf(ret + kb_ptr, " %s", kb_temp0);
			free(kb_temp0);
			kb_ptr++;
		}
	} else if (mode == 1) { /* Hexadecimal mode */
		if (new_pad & PAD_TRIANGLE) {
			return 2;
		} else if (new_pad & PAD_CROSS) {
			if (kb_sel == H_ENT)
				return 1;
			if (kb_sel == H_CANC)
				return 2;
			strcpy(kchar, keysH[kb_sel]);
			if (strcmp(kchar, "") != 0 && kb_ptr < 10 && strlen(ret) != 10) {
				char *kb_temp0 = malloc(11);
				strcpy(kb_temp0, ret);
			
				kb_temp0 += kb_ptr; //End string
				
				sprintf(ret + kb_ptr, "%s%s", kchar, kb_temp0);
				free(kb_temp0);
				kb_ptr++;
			}
		} else if (new_pad & PAD_LEFT) { //Move left
			if (kb_sel == 17) {
				kb_sel = 18; //Loop back around
			} else if ((kb_sel % 8) == 0) {
				kb_sel += 7; //Loop back around
			} else if (kb_sel != 0)
				kb_sel--;
		} else if (new_pad & PAD_RIGHT) { //Move right
			if (kb_sel == 18) {
				kb_sel = 17; //Loop back around
			} else if ((kb_sel % 8) == 7) {
				kb_sel -= 7; //Loop back around
			} else if (kb_sel != 18)
				kb_sel++;
		} else if (new_pad & PAD_DOWN) { //Move down
			if (kb_sel < 17) {
				if (kb_sel >= 8) { //If it is on the 2nd to last row...
					old_sel = kb_sel + 8; //Since PAD_UP subtracts 8, I counter that with adding a row
					if ((kb_sel % 8) < 4) //If on left side of kb, goto enter
						kb_sel = 17;
					if ((kb_sel % 8) > 3) //If on right side of kb, goto cancel
						kb_sel = 18;
				} else {
					kb_sel += 8;
				}
			}
		} else if (new_pad & PAD_UP) { //Move up
			if (kb_sel > 16)
				kb_sel = old_sel;
			if (kb_sel >= 8)
				kb_sel -= 8;
		} else if (new_pad & PAD_R1) { //Move ptr right
			if (kb_ptr < strlen(ret) && kb_ptr < 10)
				kb_ptr++;
		} else if (new_pad & PAD_L1) { //Move ptr left
			if (kb_ptr > 2)
				kb_ptr--;
		} else if (new_pad & PAD_SQUARE && kb_ptr > 2) { //Delete
			char *kb_temp1 = malloc(11);
			strcpy(kb_temp1, ret);
			
			ret[kb_ptr - 1] = '\0';
			kb_temp1 += kb_ptr;
			strcpy(ret + (kb_ptr - 1), kb_temp1);
			free(kb_temp1);
			kb_ptr--;
		} else if (new_pad & PAD_START) {
			kb_sel = H_ENT;
		}
	}
	return -1;
}

int draw_kb_frame(void) {
	int i = 0;
	
	highlight_alpha += amount;
	if (highlight_alpha > 128) {
		highlight_alpha = 128;
		pause_pulse++;
		if (pause_pulse >= 12) {
			amount = -3;
			pause_pulse = 0;
		}
	} else if (highlight_alpha < 32) {
		amount = 3;
		highlight_alpha = 32;
	}
	
	/* Clear the alpha of each key */
	for (i = 0; i <= 52; i++ ) {
		kb_highl[i] = 32;
	}
	/* Set the selected key equal to the highlight_alpha */
	kb_highl[kb_sel] = highlight_alpha;
	
	//Draw kb_frame
	draw_image(tex_kb_frame, kb_frame_x, kb_frame_y * Y_RATIO, 128);
	
	int keyoffy = kb_yoff - key_yoff; //In order to center around the character
	int keyoffx = kb_xoff - key_xoff;
	
	//Draw keys (column 1 - 12)
	for (i = 1; i < (row - 1); i++) {
		draw_image(tex_key_sqr, (i * kb_xrate) + keyoffx, keyoffy * Y_RATIO, kb_highl[i]);
		draw_image(tex_key_sqr, (i * kb_xrate) + keyoffx, (keyoffy + (kb_yrate * 1)) * Y_RATIO, kb_highl[i + (1 * row)]);
		draw_image(tex_key_sqr, (i * kb_xrate) + keyoffx, (keyoffy + (kb_yrate * 2)) * Y_RATIO, kb_highl[i + (2 * row)]);
		draw_image(tex_key_sqr, (i * kb_xrate) + keyoffx, (keyoffy + (kb_yrate * 3)) * Y_RATIO, kb_highl[i + (3 * row)]);
	}
	//Draw the rest of the keys
	
	/* Column 1 */
	draw_image(tex_key_sqr, (0 * kb_xrate) + keyoffx, keyoffy * Y_RATIO, kb_highl[0]); // '`' or '\'
	draw_image(tex_key_rect, (0 * kb_xrate) + keyoffx - 30, (keyoffy + (kb_yrate * 1)) * Y_RATIO, kb_highl[13]); // TAB
	draw_image(tex_key_rect, (0 * kb_xrate) + keyoffx - 37, (keyoffy + (kb_yrate * 2)) * Y_RATIO, kb_highl[26]); // CAPS
	draw_image(tex_key_rect, (0 * kb_xrate) + keyoffx - 37, (keyoffy + (kb_yrate * 3)) * Y_RATIO, kb_highl[39]); // SHIFT
	
	/* Column 13 */
	draw_image(tex_key_sqr, (12 * kb_xrate) + keyoffx, keyoffy * Y_RATIO, kb_highl[12]); // '=' or '+'
	draw_image(tex_key_sqr, (12 * kb_xrate) + keyoffx, (keyoffy + (kb_yrate * 1)) * Y_RATIO, kb_highl[25]); // ']' or '}'
	draw_image(tex_key_enter, (12 * kb_xrate) + keyoffx, (keyoffy + (kb_yrate * 2)) * Y_RATIO, kb_highl[38]); // ENTER
	draw_image(tex_key_enter, (12 * kb_xrate) + keyoffx, (keyoffy + (kb_yrate * 3)) * Y_RATIO, kb_highl[51]); // CANCEL
	
	/* Space */
	draw_image(tex_key_enter, kb_xspace - 5, (kb_yspace - 5) * Y_RATIO, kb_highl[52]); // SPACE
	
	return 1;
}

int draw_kb_chars(void) {
	int i = 0;
	
	/* For Columns 2 - 13 (1 - 12) */
	if (kb_shift == 1 || kb_caps == 1) {
		for (i = 1; i < row; i++) {
			drawString_neuropol((i * kb_xrate) + kb_xoff, kb_yoff * Y_RATIO, 128, kb_char_size, 1, keysU[i]);
			drawString_neuropol((i * kb_xrate) + kb_xoff, (kb_yoff + (kb_yrate * 1)) * Y_RATIO, 128, kb_char_size, 1, keysU[i + (1 * row)]);
			drawString_neuropol((i * kb_xrate) + kb_xoff, (kb_yoff + (kb_yrate * 2)) * Y_RATIO, 128, kb_char_size, 1, keysU[i + (2 * row)]);
			drawString_neuropol((i * kb_xrate) + kb_xoff, (kb_yoff + (kb_yrate * 3)) * Y_RATIO, 128, kb_char_size, 1, keysU[i + (3 * row)]);
		}
		/* Draw Column 1 */
		drawString_neuropol(kb_xoff, kb_yoff * Y_RATIO, 128, kb_char_size, 1, keysU[0]);
		drawString_neuropol(kb_xoff - 30, (kb_yoff + (kb_yrate * 1)) * Y_RATIO, 128, kb_char_size, 1, keysU[1 * row]);
		drawString_neuropol(kb_xoff - 40, (kb_yoff + (kb_yrate * 2)) * Y_RATIO, 128, kb_char_size, 1, keysU[2 * row]);
		drawString_neuropol(kb_xoff - 40, (kb_yoff + (kb_yrate * 3)) * Y_RATIO, 128, kb_char_size, 1, keysU[3 * row]);
		
		/* Draw the space bar */
		drawString_neuropol(kb_xspace, kb_yspace * Y_RATIO, 128, kb_spaceSize, 1, "SPACE");
		
		return 1;
	} else {
		for (i = 1; i < row; i++) {
			drawString_neuropol((i * kb_xrate) + kb_xoff, kb_yoff * Y_RATIO, 128, kb_char_size, 1, keysL[i]);
			drawString_neuropol((i * kb_xrate) + kb_xoff, (kb_yoff + (kb_yrate * 1)) * Y_RATIO, 128, kb_char_size, 1, keysL[i + (1 * row)]);
			drawString_neuropol((i * kb_xrate) + kb_xoff, (kb_yoff + (kb_yrate * 2)) * Y_RATIO, 128, kb_char_size, 1, keysL[i + (2 * row)]);
			drawString_neuropol((i * kb_xrate) + kb_xoff, (kb_yoff + (kb_yrate * 3)) * Y_RATIO, 128, kb_char_size, 1, keysL[i + (3 * row)]);
		}
		/* Draw Column 1 */
		drawString_neuropol(kb_xoff, kb_yoff * Y_RATIO, 128, kb_char_size, 1, keysL[0]);
		drawString_neuropol(kb_xoff - 30, (kb_yoff + (kb_yrate * 1)) * Y_RATIO, 128, kb_char_size, 1, keysL[1 * row]);
		drawString_neuropol(kb_xoff - 40, (kb_yoff + (kb_yrate * 2)) * Y_RATIO, 128, kb_char_size, 1, keysL[2 * row]);
		drawString_neuropol(kb_xoff - 40, (kb_yoff + (kb_yrate * 3)) * Y_RATIO, 128, kb_char_size, 1, keysL[3 * row]);

		/* Draw the space bar */
		drawString_neuropol(kb_xspace, kb_yspace * Y_RATIO, 128, kb_spaceSize, 1, "SPACE");
		
		return 1;
	}
	return -1;
}

int draw_kb_frame_hex(void) {
	int i = 0;
	
	highlight_alpha += amount;
	if (highlight_alpha > 128) {
		highlight_alpha = 128;
		pause_pulse++;
		if (pause_pulse >= 12) {
			amount = -3;
			pause_pulse = 0;
		}
	} else if (highlight_alpha < 32) {
		amount = 3;
		highlight_alpha = 32;
	}
	
	/* Clear the alpha of each key */
	for (i = 0; i <= 52; i++ ) {
		kb_highl[i] = 32;
	}
	/* Set the selected key equal to the highlight_alpha */
	kb_highl[kb_sel] = highlight_alpha;
	
	//Draw kb_frame
	draw_image(tex_kb_frame, kb_frameH_x, kb_frameH_y * Y_RATIO, 128);
	
	int keyoffy = kb_yoffH - key_yoffH; //In order to center around the character
	int keyoffx = kb_xoffH - key_xoffH;
	
	//Draw keys (column 0 - 1)
	for (i = 0; i < 8; i++) {
		draw_image(tex_key_sqr, (i * kb_xrateH) + keyoffx, keyoffy * Y_RATIO, kb_highl[i]);
		draw_image(tex_key_sqr, (i * kb_xrateH) + keyoffx, (keyoffy + (kb_yrateH * 1)) * Y_RATIO, kb_highl[i + (1 * 8)]);
	}
	
	/* Column 2 */
	draw_image(tex_key_enter, kb_xenterH, kb_yenterH * Y_RATIO, kb_highl[17]); // ENTER
	draw_image(tex_key_enter, kb_xcancelH, kb_yenterH * Y_RATIO, kb_highl[18]); // CANCEL
	
	return 1;
}

int draw_kb_hex(void) {
	int i = 0;
	
	for (i = 0; i < 8; i++) {
		drawString_neuropol((i * kb_xrateH) + kb_xoffH - 2, kb_yoffH * Y_RATIO, 128, kb_charH_size, 1, keysH[i]);
		drawString_neuropol((i * kb_xrateH) + kb_xoffH - 2, (kb_yoffH + (kb_yrateH * 1)) * Y_RATIO, 128, kb_charH_size, 1, keysH[i + 8]);
	}
	
	/* Draw the enter key */
	drawString_neuropol((kb_xenterH + 9), (kb_yenterH + 7) * Y_RATIO, 128, kb_spaceSize - 3, 1, "ENTER");
	
	/* Draw the cancel key */
	drawString_neuropol((kb_xcancelH + 4), (kb_yenterH + 7) * Y_RATIO, 128, kb_spaceSize - 3, 1, "CANCEL");
	
	return 1;
}

int draw_YorN(char *Q, char *Yes, char *No) {
	static int x, y, z;
	
	if (x == 0)
		while (getStringWidth_neuropol(Q, x, 0) < SCREEN_WIDTH - 20) { x++; }
	if (y == 0)
		while (getStringWidth_neuropol(Yes, y, 0) < SCREEN_WIDTH - 440) { y++; }
	if (z == 0)
		while (getStringWidth_neuropol(No, z, 0) < SCREEN_WIDTH - 440) { z++; }
	
	if (y < 20)
		y = 20;
	if (z < 20)
		z = 20;
	
	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);
	
	draw_background(yn_alpha, 1);
	centerString_neuropol(120 * Y_RATIO, yn_alpha, x, 0, Q);
	
	if (yn_alpha < 128) 
		yn_alpha += 2;
	if (yn_alpha > 128)
		yn_alpha = 128;
	
	if (yn_alpha == 128) {
		highlight_alpha += amount;
		if (highlight_alpha > 128) {
			highlight_alpha = 128;
			pause_pulse++;
			if (pause_pulse >= 12) {
				amount = -3;
				pause_pulse = 0;
			}
		} else if (highlight_alpha < 32) {
			amount = 3;
			highlight_alpha = 32;
		}
		
		/* Clear the alpha */
		y_alpha = 32;
		n_alpha = 32;
		
		/* Set the selected text equal to the highlight_alpha */
		switch (yn_sel) {
			case 0:
				y_alpha = highlight_alpha;
				break;
			case 1:
				n_alpha = highlight_alpha;
				break;
		}
	
		centerString_neuropol(200 * Y_RATIO, y_alpha, y, 0, Yes);
		centerString_neuropol(240 * Y_RATIO, n_alpha, z, 0, No);
	}
	
	gsKit_set_test(gsGlobal, GS_ATEST_ON);
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
	
	waitAnyPadReady();
	if(readPad()) {
		if (new_pad & PAD_CROSS) {
			if (yn_sel == 0) {
				yn_alpha = 0;
				x = 0; y = 0; z = 0;
				return 1;
			} else if (yn_sel == 1) {
				yn_alpha = 0;
				x = 0; y = 0; z = 0;
				return 2;
			}
		} else if (new_pad & PAD_UP || new_pad & PAD_DOWN) {
			yn_sel = !yn_sel;
		} else if (new_pad & PAD_TRIANGLE) {
			yn_alpha = 0;
			return 3;
		}
	}
	
	return -1;
}

int draw_jokerCombo(char *title1, char *title2) {
	int x = 20, y = 20, z = 20;
	
	/* Set Alpha settings */
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0,1,0,1,0), 0);
	gsKit_set_test(gsGlobal, GS_ATEST_OFF);
	
	draw_background(yn_alpha, 1);
	centerString_neuropol(120 * Y_RATIO, yn_alpha, x, 0, title1);
	centerString_neuropol(150 * Y_RATIO, yn_alpha, y, 0, title2);
	
	waitAnyPadReady();
	if(readPad()) {
		if (new_pad & PAD_CROSS) {
			yn_alpha = 0;
			return (jokCombo << 16) >> 16;
		} else if (new_pad & PAD_TRIANGLE) {
			yn_alpha = 0;
			return 0x10000;
		}
	}
	
	if (yn_alpha < 128) 
		yn_alpha += 2;
	if (yn_alpha > 128)
		yn_alpha = 128;
	if (yn_alpha == 128) {
		if (new_pad != old_pad) {
			ParseJokerVal(new_pad & ~old_pad, (char*)valStr);
			jokCombo = new_pad;
		}
		centerString_neuropol(200 * Y_RATIO, 128, z, 0, (char*)valStr);
	}
	
	gsKit_set_test(gsGlobal, GS_ATEST_ON);
    /* Blend Alpha Primitives "Back To Front" */
    gsKit_set_primalpha(gsGlobal, GS_BLEND_BACK2FRONT, 0);
	
	return 0x20000;
}
