
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <gsKit.h>
#include <libpad.h>
#include <loadfile.h>
#include <iopheap.h>
#include <iopcontrol.h>
#include <sbv_patches.h>
#include <png.h>
#include <string.h>
#include <debug.h>

extern void usbd_irx;
extern void usb_mass_irx;
extern void elf_loader;

extern u32 size_usbd_irx;
extern u32 size_usb_mass_irx;
extern u32 size_elf_loader;

//#define PCSX2_DEBUG
#define PROGRAM_VERSION		"0.6"

/* gui.c */
extern void Setup_GS(int gs_vmode);
extern void gfx_set_defaults(void);
extern void vram_free(void);
extern void load_background_Textures(void);
extern void load_mainmenu_Textures(void);
extern void load_submenu_Textures(void);
extern void load_Font(void);
extern void Clear_Screen(void);
extern int  Draw_INTRO_part1(void);
extern int  Draw_INTRO_part2(void);
extern int  Draw_MainMenu(int selected_button, int highlight_pulse);
extern int  Draw_CheatsMenu(void);
extern int  Draw_OptionsMenu(void);
extern int  Init_AboutMenu(void);
extern int  Draw_AboutMenu(char *version);
extern void Render_GUI(void);

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int SCREEN_X;
extern int SCREEN_Y;
extern float Y_RATIO;

/* timer.c */
extern void TimerInit(void);
extern u64  Timer(void);
extern void TimerEnd(void);

/* pad.c */
extern u32  new_pad;
extern int  readPad(void);
extern void waitAnyPadReady(void);
extern int  setupPad(void);

/* main */
#define MAX_PATH 	1024

int TV_mode;
u8  romver[16];
int selected_button;
int highlight_pulse;

char run_path[MAX_PATH];
	
/* functions prototypes */
void load_elf(char *elf_path);
void IOP_Reset(void);
void CleanUp(void);
void delay(int count);
int  load_USB_modules(void);
void launch_OSDSYS(void);
void Update_MainMenu(void);
void Update_AboutMenu(void);

/* ELF-header structures and identifiers */
#define ELF_MAGIC	0x464c457f
#define ELF_PT_LOAD	1

typedef struct {
	u8	ident[16];
	u16	type;
	u16	machine;
	u32	version;
	u32	entry;
	u32	phoff;
	u32	shoff;
	u32	flags;
	u16	ehsize;
	u16	phentsize;
	u16	phnum;
	u16	shentsize;
	u16	shnum;
	u16	shstrndx;
} elf_header_t;

typedef struct {
	u32	type;
	u32	offset;
	void *vaddr;
	u32	paddr;
	u32	filesz;
	u32	memsz;
	u32	flags;
	u32	align;
} elf_pheader_t;

/*
 * load an elf file using embedded elf loader
 * this allow to fix memory overlapping problem
 */
void load_elf(char *elf_path)
{
	u8 *boot_elf;
	elf_header_t *boot_header;
	elf_pheader_t *boot_pheader;
	int i;
	char *args[1];

	/* The loader is embedded */
	boot_elf = (u8 *)&elf_loader;

	/* Get Elf header */
	boot_header = (elf_header_t *)boot_elf;

	/* Check elf magic */
	if ((*(u32*)boot_header->ident) != ELF_MAGIC)
		return;

	/* Get program headers */
	boot_pheader = (elf_pheader_t *)(boot_elf + boot_header->phoff);

	/* Scan through the ELF's program headers and copy them into apropriate RAM
	 * section, then padd with zeros if needed.
	 */
	for (i = 0; i < boot_header->phnum; i++) {
		if (boot_pheader[i].type != ELF_PT_LOAD)
			continue;

		memcpy(boot_pheader[i].vaddr, boot_elf + boot_pheader[i].offset, boot_pheader[i].filesz);

		if (boot_pheader[i].memsz > boot_pheader[i].filesz)
			memset(boot_pheader[i].vaddr + boot_pheader[i].filesz, 0, boot_pheader[i].memsz - boot_pheader[i].filesz);
	}
	
	/* Cleanup befor launching elf loader */
	CleanUp();

	args[0] = elf_path;

	/* Execute Elf Loader */
	ExecPS2((void *)boot_header->entry, 0, 1, args);
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
 * load USB device driver & usb mass driver
 */
int load_USB_modules(void)
{
	int ret, id;

	if ((id = SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, &ret)) < 0) {
		scr_printf("\t ERROR: cannot load usbd.\n");
		return 0;
	}

	if ((id = SifExecModuleBuffer(&usb_mass_irx, size_usb_mass_irx, 0, NULL, &ret)) < 0) {
		scr_printf("\t ERROR: cannot load usbhdfsd.\n");
		return 0;
	}

	/* delay to allow usb mass driver to finish to load */
	delay(1);

	return 1;
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
 * Draw & render Cheats Menu
 */
void Update_CheatsMenu(void)
{
	/* Update GUI and render */
	Draw_CheatsMenu();
	Render_GUI();
}

/*
 * Draw & render Options Menu
 */
void Update_OptionsMenu(void)
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
	Draw_AboutMenu(PROGRAM_VERSION);
	Render_GUI();
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

   	init_scr();

	SifInitRpc(0);

	/* Reset IOP */
	IOP_Reset();

	/* Init loadfile service & apply sbv patches */
	SifLoadFileInit();
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();

	/* load needed modules */
	if (!load_USB_modules()) goto err_out;
    SifLoadModule("rom0:SIO2MAN", 0, 0);
    SifLoadModule("rom0:MCMAN", 0, 0);
    SifLoadModule("rom0:MCSERV", 0, 0);
    SifLoadModule("rom0:PADMAN", 0, 0);  

	SifLoadFileExit();

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
	} else { /* NTSC values can be adjusted, altought it fit fine too on my TV */
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

	/* Initializes pads */
	setupPad();

	/* Load Textures into VRAM */
	vram_free();
	load_background_Textures();	
	load_mainmenu_Textures();
	load_Font();
		
	/* Prior to call any of the gfx functions */
	gfx_set_defaults();
		
	/* Playing INTRO part 1 */
	while (!Draw_INTRO_part1())
		Render_GUI();
	Render_GUI();

	/* delay */
	#ifndef PCSX2_DEBUG
	WaitTime = Timer();
	while (Timer() < (WaitTime + 1000))
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

				/* load submenu textures to vram */
				vram_free();
				load_background_Textures();	
				load_submenu_Textures();
				load_Font();
				
				/* Taking action for button press */
				switch (selected_button) {
					
					case 1: /* start game */
						launch_OSDSYS();
						break;
						
					case 2: /* cheats menu */
						while (1) {
							waitAnyPadReady();
							if (readPad()) {
								if (new_pad)
									break;
							}
							Update_CheatsMenu();	
						}					
						break;
						
					case 3: /* options menu */
						while (1) {
							waitAnyPadReady();
							if (readPad()) {
								if (new_pad)
									break;
							}
							Update_OptionsMenu();	
						}					
						break;
						
					case 4: /* about menu */
						Init_AboutMenu();					
						while (1) {
							waitAnyPadReady();
							if (readPad()) {
								if (new_pad)
									break;
							}
							Update_AboutMenu();	
						}
						break;
						
					default:
						break;
				}
				
				/* reload main menu textures to vram */
				vram_free();
				load_background_Textures();	
				load_mainmenu_Textures();
				load_Font();
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
					if (selected_button < 1) selected_button = 4;
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
					if (selected_button > 4) selected_button = 1;				
				}
			}
			else if ((new_pad & PAD_START) && (new_pad & PAD_SELECT) && (new_pad & PAD_L1) &&
					(new_pad & PAD_L2) && (new_pad & PAD_R1) && (new_pad & PAD_R2)) {
				launch_OSDSYS();
			}			
		}
		
		/* Update GUI and render */
		Update_MainMenu();
	}

	/* We won't get here */
err_out:
	scr_printf("\t Artemis GUI Init failed...\n");

	SleepThread();

	return 0;
}
