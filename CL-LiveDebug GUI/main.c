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

//For making the new directory 'LiveDebug'
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//All the arrays that will be stored in the kernel
#include "Exports/LDFont.c"
#include "Exports/LDSpace.c"
#include "Exports/LDEngine.c"
#include "Exports/LDCheats.c"
#include "Exports/LDHook.c"
#include "Exports/LDScanner.c"

//qwerty IOP mod from CogSwap source (booting backups)
#include "qwerty.c"

//Stuff that have to do with pre-set jokers
extern u32 TextToCode(char *text);
extern char *LoadJoker(char *elfname, char *file);
extern char *replace_str(char *str, char *orig, char *rep);
extern char *LoadJokerArray(char *elf);

extern void usbd_irx;
extern void usb_mass_irx;
extern void elf_loader;

extern u32 size_usbd_irx;
extern u32 size_usb_mass_irx;
extern u32 size_elf_loader;

/*
extern void usbd_irx;
extern void usb_mass_irx;

extern u32 size_usbd_irx;
extern u32 size_usb_mass_irx; */

#define PROGRAM_VERSION		"4.0"

struct about_content {
	char *name;
	char *desc;
};

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
extern int  Draw_OptionsMenu(u32 new_pad);
extern int  Init_AboutMenu(void);
extern int  Draw_AboutMenu(char *version, struct about_content *about_text);
extern void Render_GUI(void);
extern int Draw_PatchesMenu(u32 new_pad);
void StopDisc(void);
extern void load_patches_Texture(void);
extern void drawString_neuropol(u32 x, u32 y, int alpha, int fontsize, int fontspacing, const char *string);

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
int selected_button, window = 0;
int highlight_pulse;

char run_path[MAX_PATH];

int PatchesLoaded = 0;
static struct about_content about_text[] = {
		{ "Berion",
		  "GUI graphic designer." },
		{ "Gtlcpimp",
		  "Core developer, wrote 90% of the debugger." },
		{ "Dnawrkshp",
		  "Developer, added to CL-LiveDebug's debugger." },
		{ NULL,
		  NULL }
};
	
/* functions prototypes */
void load_elf(char *elf_path);
void IOP_Reset(void);
void CleanUp(void);
void delay(int count);
void launch_OSDSYS(void);
void Update_MainMenu(void);
void Update_AboutMenu(void);
char *readcnf(void);
int LoadPatches(int m);
int WritePatches(void);
void WriteLabels(void);
void ResetCNF(void);
int LoadModules(void);
int LoadSettings(void);
void NewSett(void);
void StoreSett(void);

//Settings variables
char *SettFile = "mc0:/LiveDebug/settings.txt";
char *SettLayout = "autojoker 0\nulejoker 0\n";
int AutoJoker = 0;
int uLEJoker = 0;

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
 * Loads in all the modules and the qwerty mod
*/
int LoadModules(void) {
	int ret = 1;

	SifLoadFileInit();
	SifInitRpc(0);

	// Load modules off ROM
	ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	ret = SifLoadModule("rom0:MCMAN", 0, NULL);
	ret = SifLoadModule("rom0:MCSERV", 0, NULL);
	ret = SifLoadModule("rom0:PADMAN", 0, NULL);
	ret = SifLoadModule("rom0:LIBSD", 0, NULL);

	// Modules from PS2SDK
	SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, &ret);
	SifExecModuleBuffer(&usb_mass_irx, size_usb_mass_irx, 0, NULL, &ret);
	
	//SifExecModuleBuffer(&usb_mass_irx, size_usb_mass_irx, 0, NULL, &ret);
	//SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, &ret);
	
	// QWERTY Module from CogSwap
	loadmodulemem(iopmod, 1865, 0, 0);

	SifLoadFileExit();
	
	delay(1);
	
	return ret;
}

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
	char *args[1], *elfp = malloc(8);
	strcpy(elfp, elf_path);
	elfp[7] = '\0';
	
	//If it is booting from the disc, then AutoJoker is the deciding factor
	if (strncmp(elfp, "cdrom0:", 7) == 0) { uLEJoker = 1; }
	free(elfp);
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

	/* Create hook hex value */
	u32 HookValue = (0x0007F000 / 4) + 0x0C000000;

	/* Install the spacing into the kernel */
	u32 SpaceStore = 0x8002FE00;
	u32 SpaceRead = (u32)LDSpace;
	for (i = 0; i < sizeof(LDSpace); i += 1)
	{
		DI();
		ee_kmode_enter();
		*(u8*)SpaceStore = *(u8*)SpaceRead;
		ee_kmode_exit();
		EI();
		SpaceStore += 1;
		SpaceRead += 1;
	}
	
	/* Install the font into the kernel */
	u32 FontStore = 0x80030000;
	u32 FontRead = (u32)LDFont;
	for (i = 0; i < sizeof(LDFont); i += 1)
	{
		DI();
		ee_kmode_enter();
		*(u8*)FontStore = *(u8*)FontRead;
		ee_kmode_exit();
		EI();
		FontStore += 1;
		FontRead += 1;
	}
	
	/* Install the hook filter into the kernel */
	u32 HookStore = 0x8007F000;
	u32 HookRead = (u32)LDHook;
	for (i = 0; i < sizeof(LDHook); i += 4)
	{
		DI();
		ee_kmode_enter();
		*(u32*)HookStore = *(u32*)HookRead;
		ee_kmode_exit();
		EI();
		HookStore += 4;
		HookRead += 4;
	}

	/* Install the LiveDebug v3 Engine into the kernel */
	u32 EngineStore = 0x80078250;
	u32 EngineRead = (u32)LDEngine;
	for (i = 0; i < sizeof(LDEngine); i += 4)
	{
		DI();
		ee_kmode_enter();
		*(u32*)EngineStore = *(u32*)EngineRead;
		ee_kmode_exit();
		EI();
		EngineStore += 4;
		EngineRead += 4;
	}

	/* Install the LiveDebug v3 Cheat Engine into the kernel */
	u32 CheatStore = 0x8007F800;
	u32 CheatRead = (u32)LDCheats;
	for (i = 0; i < sizeof(LDCheats); i += 4)
	{
		DI();
		ee_kmode_enter();
		*(u32*)CheatStore = *(u32*)CheatRead;
		ee_kmode_exit();
		EI();
		CheatStore += 4;
		CheatRead += 4;
	}
	
	/* Install the Joker Scanner into the kernel */
	u32 ScannerStore = 0x8007E880;
	u32 ScannerRead = (u32)LDScanner;
	for (i = 0; i < sizeof(LDScanner); i += 4)
	{
		DI();
		ee_kmode_enter();
		*(u32*)ScannerStore = *(u32*)ScannerRead;
		ee_kmode_exit();
		EI();
		ScannerStore += 4;
		ScannerRead += 4;
	}
	
	u32 joker = 0;
	if (AutoJoker == 1 && uLEJoker == 1) {
	
		char *loc = "mass:/LiveDebug/LDJoker.txt";
		if (fioOpen(loc, O_RDONLY) < 0) { 
			loc = "mc0:/LiveDebug/LDJoker.txt";
			if (fioOpen(loc, O_RDONLY) < 0) {
				loc = "mc1:/LiveDebug/LDJoker.txt";
				if (fioOpen(loc, O_RDONLY) < 0) {
					loc = NULL;
				}
			}
		}
	
		char *temp = readcnf();
		temp = replace_str(temp, ";1", "");
		temp = replace_str(temp, " ", ""); //Don't ask...
		temp = replace_str(temp, "cdrom0:\\", "");
		joker = TextToCode(LoadJoker(temp, loc));
		
		//If that doesn't find a joker, try looking in the array
		if (joker == 0) { joker = TextToCode(LoadJokerArray(temp)); }
	}
	
	/* Install the Scanner Config into the kernel */
	DI();
	ee_kmode_enter();
	*(u32*)0x8007E7FC = (u32)joker;
	*(u32*)0x8007E800 = 0x00100000; //Start Point
	*(u32*)0x8007E804 = 0x00000800; //Scan Size
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
	ee_kmode_exit();
	EI();
	
	/* Install string table into the kernel */
	WriteLabels();

	/* Install Patches into the kernel */
	WritePatches();
	
	/* Configure the engine */
	*(u32*)0x00080018 = 0x00085000;
	*(u32*)0x0008001C = 0x80030000;
	
	DI();
	ee_kmode_enter();
		*(u32*)0x800002FC = HookValue; // Install the kernel hook
	ee_kmode_exit();
	EI();
	
	/* Cleanup befor launching elf loader */
	CleanUp();

	args[0] = elf_path;
	
	/* Execute Elf Loader */
	ExecPS2((void *)boot_header->entry, 0, 1, args);
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
*(u32*)0x80044500 = 0x4C2D4C43;
*(u32*)0x80044504 = 0x44657669; 
*(u32*)0x80044508 = 0x67756265;
*(u32*)0x8004450C = 0x00337620;
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

/*
 * Reads the system.cnf and returns the elf path
*/
char *readcnf(void) {
	int fd, size;
	char *result;
	int x = 0;

	result = malloc(50);

	fd = fioOpen("cdrom0:\\SYSTEM.CNF;1", O_RDONLY); //Open the SYSTEM.CNF
	if (fd < 0) { fioClose(fd); return NULL; } //If it doesn't exist, return null

	size = fioLseek(fd, 0, SEEK_END); //Gets the size of the system.cnf
	fioLseek(fd, 0, SEEK_SET);
	
	if (size < 0) { fioClose(fd); return NULL; }
	
	fioRead(fd, result, size); //Reads the system.cnf and stores it into result
	fioClose(fd); //Closes system.cnf
	if (result == NULL) { return NULL; } //If it can't read, return null

	result += 5; //Skip "BOOT2"
	
	//Since there are multiple CNF styles we must skip some characters.
	while (*result == ' ' || *result == '=' || *result == '\t') { result++; }
	
	while (result[x] != '\n' && result[x] != '\r') //Read each character until it hits a newline or return
	{
		x++;
	}

	if (result[x] == ' ') { result[x - 1] = '\0'; } else { result[x] = '\0'; } //End string with a NULL 0x00
	

	return result;
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
void Update_OptionsMenu(u32 pad)
{
	/* Update GUI and render */
	Draw_OptionsMenu(pad);
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
void Update_PatchesMenu(u32 pad)
{
	Draw_PatchesMenu(pad);
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

	/* Load modules & apply sbv patches */
	LoadModules();
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();
	
	/* Load settings into global variables */
	LoadSettings();
	
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

	/* Initializes pads */
	setupPad();

	/* Load Textures into VRAM */
	vram_free();
	load_background_Textures();	
	load_mainmenu_Textures();
		
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

				/* load submenu textures to vram */
				vram_free();
				load_background_Textures();	
				load_submenu_Textures();
				load_Font();
				
				/* Taking action for button press */
				switch (selected_button) {
					
					case 1: /* start game */
						//launch_OSDSYS();
						load_elf(readcnf());
						break;
						
					case 2: /* stop disc */
						StopDisc();				
						break;
					
					case 3: /* patches menu */
						load_patches_Texture();
						patches_image_alpha = 0;
						menu_bar_x = 0;
						about_menu_header_alpha = 0;
						while (1) {
							//waitAnyPadReady();
							if (readPad()) {
								if (new_pad & PAD_TRIANGLE)
									break;
							}
							Update_PatchesMenu(new_pad);
						}
						patchret = "";
						break;
						
					case 4: /* options menu */
						options_text_alpha = 0;
						menu_bar_x = 0;
						about_menu_header_alpha = 0;
						LoadSettings();
						while (1) {
							//waitAnyPadReady();
							if (readPad()) {
								if (new_pad & PAD_TRIANGLE) {
									StoreSett();
									break;
								}
							}
							Update_OptionsMenu(new_pad);	
						}					
						break;
					
					case 5: /* about menu */
						Init_AboutMenu();
						menu_bar_x = 0;
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
				char *uLE = "mc0:/BOOT/BOOT.ELF";
				int fd = fioOpen(uLE, O_RDONLY);
				
				if (fd < 0) {
					fioClose(fd);
					uLE = "mc1:/BOOT/BOOT.ELF";
					fd = fioOpen(uLE, O_RDONLY);
					if (fd < 0) {
						fioClose(fd);
						uLE = "mass:/BOOT/BOOT.ELF";
						fd = fioOpen(uLE, O_RDONLY);
						if (fd < 0) {
							fioClose(fd);
							uLE = NULL;
							fd = 0;
						}
					}
				}
				if (fd != 0) { fioClose(fd); }
				if (uLE != NULL) {
					load_elf(uLE);
				}
			}
		}
		
		/* Update GUI and render */
		Update_MainMenu();
	}

	/* We won't get here */
//err_out:
	scr_printf("\t Artemis GUI Init failed...\n");

	SleepThread();

	return 0;
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
	readcnf();
	
	ResetCNF();
	IOP_Reset();

	SifInitRpc(0);
	LoadModules();

	fioOpen("qwerty:c",1); // Set LBA to 0
	fioOpen("qwerty:s",1); // Stop the disc

	setupPad();
}

int LoadSettings(void) {
	int fp, size = 0;
	char *result = malloc (200);
	int x = 0, y = 0, z = 0;
	
	fp = fioOpen(SettFile, O_RDONLY);
	if (fp < 0) {
		fioClose(fp);
		NewSett();
		goto exit;
	}
	else {
		size = fioLseek(fp, 0, SEEK_END);
		fioLseek(fp, 0, SEEK_SET);
		if (size <= 0) { fioClose(fp); NewSett(); goto exit; }
		
		fioRead(fp, result, size);
		if (result == NULL) { fioClose(fp); NewSett(); goto exit; }
		fioClose(fp);
		
		while (z < (size - 1)) {
			while (result[x] != '\n' && ((x + z + 1) < size)) { x++; }
			x--;
			if (y == 0) { AutoJoker = (result[x] - 0x30); }
			if (y == 1) { uLEJoker = (result[x] - 0x30); }
			x++;
			while (result[x] == '\n' && ((x + z + 1) < size)) { x++; }
			result += x;
			z += x;
			x = 0;
			y++;
		}
		
	}
	
	exit:
	free(result);
	return 0;
}

void NewSett(void) {
	FILE *fd;
	
	if (stat("mc0:/LiveDebug", 0) < 0) {
		mkdir("mc0:/LiveDebg");
	}
	
	fd = fopen(SettFile, "w");
	fwrite(SettLayout, 1, strlen(SettLayout), fd);
	AutoJoker = 0;
	fclose(fd);
}

void StoreSett(void) {
	int fd = 0;
	char *settings = malloc(100);
	sprintf(settings, "autojoker %d\nulejoker %d\n", AutoJoker, uLEJoker);
	
	fd = fioOpen(SettFile, O_WRONLY);
	fioWrite(fd, settings, strlen(settings));
	fioClose(fd);
	free(settings);
}
