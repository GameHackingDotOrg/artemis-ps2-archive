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
#include <libconfig.h>
#include "config.h"
#include "textures.h"
#include "timer.h"
#include <sifcmd.h>

/*
	0 = off, 1 = all but textures, 2 = all
	If you are making a release make this a 0
	Otherwise the IOP won't reset upon boot
	Only booting from apps like uLE will work (not CDVD or OSDSYS)
*/
int DEBUG = 0;

//All the arrays that will be stored in the kernel
#include "C Exports/LDFont.c"
#include "C Exports/LDSpace.c"
#include "C Exports/LDEngine.c"
#include "C Exports/LDCheats.c"
#include "C Exports/LDHook.c"
#include "C Exports/LDScanner.c"
#include "C Exports/LDRegD.c"
#include "C Exports/LDExcept.c"

//qwerty IOP mod from CogSwap source (booting backups)
#include "qwerty.c"

//Stuff that have to do with pre-set jokers
extern u32 TextToCode(const char *text);
extern const char *LoadJoker(char *elfname, char *ver);
extern char *replace_str(char *str, char *orig, char *rep);
extern char *LoadJokerArray(char *elf, char *ver);
extern int GetSGJoker(char *elf);

//Stuff to do with sounds
extern int audsvr_start(int vol);
extern int play_sound(void ** ptr, int size);

/* IRX Modules and elf loader */
extern void usbd_irx;
extern void usb_mass_irx;
extern void elf_loader;
extern void freesd_irx;
extern void audsrv_irx;

extern u32 size_usbd_irx;
extern u32 size_usb_mass_irx;
extern u32 size_elf_loader;
extern u32 size_freesd_irx;
extern u32 size_audsrv_irx;

/* Sounds */
/*
extern void menu_trans;
extern void start_game;
extern void stop_disc;

extern u32 size_menu_trans;
extern u32 size_start_game;
extern u32 size_stop_disc;
*/

#define PROGRAM_VERSION		"4.0"

struct about_content {
	char *name;
	char *desc;
};

/* gui.c */
extern void Setup_GS(int gs_vmode);
extern void gfx_set_defaults(void);
extern void vram_free(void);
extern void Clear_Screen(void);
extern int  Draw_INTRO_part1(void);
extern int  Draw_INTRO_part2(void);
extern int  Draw_MainMenu(int selected_button, int highlight_pulse);
extern int  Draw_CheatsMenu(void);
extern int  Draw_OptionsMenu(void);
extern int  Init_AboutMenu(void);
extern int  Draw_AboutMenu(char *version, struct about_content *about_text);
extern void Render_GUI(void);
extern int Draw_PatchesMenu(void);

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int SCREEN_X;
extern int SCREEN_Y;
extern float Y_RATIO;
extern void padaddr;

/* gui.c variables */
extern int patches_image_alpha;
extern int options_text_alpha;
extern char *patchret;
extern int menu_bar_x;
extern int about_menu_header_alpha;
//extern GSGLOBAL *gsGlobal;

/* pad.c */
extern u32  new_pad;
extern int  readPad(void);
extern void waitAnyPadReady(void);
extern int  setupPad(void);

/* joker.c */
extern int J_SIZE;

/* Keyboard */
extern int kb_yoff;
extern int kb_yrate;
extern int kb_yspace;
extern int kb_xoff;
extern int kb_xrate;
extern int kb_xspace;
extern int kb_frame_y;
extern int kb_frame_x;
extern int kb_frameH_y;
extern int kb_frameH_x;
extern int kb_yoffH;
extern int kb_yrateH;
extern int kb_yenterH;
extern int kb_xoffH;
extern int kb_xrateH;
extern int kb_xenterH;
extern int kb_xcancelH;

/* main */
#define MAX_PATH 	1024

int TV_mode;
u8  romver[16];
int selected_button, window = 0;
int highlight_pulse;

char run_path[MAX_PATH];

int PatchesLoaded = 0;
struct about_content about_text[] = {
		{ "Berion",
		  "GUI graphic designer." },
		{ "Gtlcpimp",
		  "Core developer, author of the debugger." },
		{ "Dnawrkshp",
		  "Developer, improved the debugger." },
		{ NULL,
		  NULL }
};

config_t config; //For libconfig stuff

/* functions prototypes */
void load_elf(char *elf_path);
void IOP_Reset(void);
void CleanUp(void);
void delay(int count);
void launch_OSDSYS(void);
void Update_MainMenu(void);
void Update_AboutMenu(void);
int LoadPatches(int m);
int WritePatches(void);
void WriteLabels(void);
void ResetCNF(void);
int LoadModules(void);
int LoadSettings(void);
void NewSett(void);
int StoreSett(char *add_joker, char *add_jokerV, char *add_ver);
int ParseSYSTEMCNF(char *system_cnf, char *boot_path);
void Update_MainMenu(void);
void Update_OptionsMenu(void);
void Update_PatchesMenu(void);
void GetValidPath(void);
int ParseVerNumber(char *cnf, char *ret);
int LoadTextures(int mode);
int clearVRAM(void);
void StopDisc(void);

/*
 * Texture modes
 * Each bit represents a texture
 * 1		=	MainMenu	(0x0001)
 * 10		=	Background	(0x0002)
 * 100		=	Patches 	(0x0004)
 * 1000		=	Font		(0x0008)
 * 10000	=	Submenu 	(0x0010)
 * 100000	=	About		(0x0020)
 * 1000000	=	YorN		(0x0040)
 * 10000000	=	Keyboard 	(0x0080)
 */
int lmainM	=	0x0003; //Main, backg
int lPatch	=	0x000E; //Patches, backg, font
int loptM	=	0x001A; //Sub, backg, font
int labout	=	0x002A; //About, backg, font
int lyorn	=	0x0048; //YorN, font
int lkeyb	=	0x0088; //Keyb, font

/* Textures bools */
int mmText = 0; /* load_mainmenu_Textures() */
int bgText = 0; /* load_background_Textures() */
int ptText = 0; /* load_patches_Texture() */
int ftText = 0; /* load_Font() */
int smText = 0; /* load_submenu_Textures() */
int abText = 0; /* load_about_Textures() */
int ynText = 0; /* load_YorN_Textures() */
int kbText = 0; /* load_keyboard_Textures() */

/* Texture sizes */
int mmText_size = 0x3C000;
int bgText_size = 0x1B4000;
int ptText_size = 0x3C000;
int ftText_size = 0x60000;
int smText_size = 0x4000;
int abText_size = 0x8000;
int ynText_size = 0x140000;
int kbText_size = 0xDC000;

int vramMax = 0; /* Later defined in main.c */

//Settings variables
char *sfile = "mc0:/LiveDebug/CL-LiveDebug.conf";
char *SettLayout = "# CL-LiveDebug config file\n\n" \
					"main: {\n\t" \
						"preset_joker = false;\n\t" \
						"preset_ule = false;\n\t" \
						"install_ehandler = false;\n\t" \
						"ule_path = \"mc0:/BOOT/BOOT.ELF\";\n" \
					"};\n\n" \
					"debugger: {\n\t" \
						"pkt_alloc = 0x00090000;\n\t" \
						"eh_alloc = 0x000A0000;\n\t" \
						"regd_alloc = 0x00080000;\n" \
						"jok_combo = 0xFFFFFFF9;\n" \
					"};\n\n" \
					"joker: {\n\t" \
						"joker_array = [\n\t" \
						"];\n" \
					"};\n\n";
int AutoJoker = 0;
int uLEJoker = 0;
int ExcHand = 0;
int pktAlloc = 0;
int ehAlloc = 0;
int regdAlloc = 0;
int jokCombo = 0;
const char *ulePath = NULL;

char *sys_cnf = "cdrom0:\\SYSTEM.CNF;1";

/* Kernel/Syscall hook address and value */
u32 HookValue = (0x00078250 / 4) + 0x08000000;
//u32 HookAddr = 0x800002FC;
u32 HookAddr = 0x800001A0;

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
