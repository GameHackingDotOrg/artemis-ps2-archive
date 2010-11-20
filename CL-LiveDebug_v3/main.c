/*
	CL-LiveDebug v3 Launcher
	CheatersLounge Copyright ©
	Written by: Gtlcpimp

	Some functionality borrowed from other sources.

	Source version: 3.0.3
*/

#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <packet.h>
#include <dma.h>
#include <kernel.h>
#include <libpad.h>

// These IRX modules were taken from the PS2SDK downloaded from GSHI.org
extern void mod_usbd;
extern void mod_usb_mass;
extern void cdvdinit_irx;

extern u32 size_mod_usbd;
extern u32 size_mod_usb_mass;
extern u32 size_cdvdinit_irx;

// From CogSwap source
#include "qwerty.c"

// Modded from PS2SDK sample
#include "pad.c"

// My lovely creations
#include "cl-gs.c"
#include "arrow.c"
#include "BACKGROUND.c"
#include "CourierNew.c"
#include "LDv3Engine.c"
#include "LDv3Cheats.c"
#include "LDv3Hook.c"

// Pointless to me, but got tired of having a million warnings from the compiler
// Now I just have a thousand..
void ExecCMD(int sel);
void DrawArrow(int sel);
int LoadModules(void);
void StopDisc(void);
int ExecGame(void);
u32 LoadELF(char *elf);
void WriteConfig(void);
int LoadPatches(int m);
int WritePatches(void);
void WriteLabels(void);
void ReadCNF(char *elfnm);
void ResetIOP(void);
void ResetCNF(void);

// ELF Header structure from something out of the PS2SDK or the CogSwap source
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
	void	*vaddr;
	u32	paddr;
	u32	filesz;
	u32	memsz;
	u32	flags;
	u32	align;
} elf_pheader_t;

elf_header_t elfh;

struct padButtonStatus buttons;
u32 paddata;
u32 old_pad = 0;
u32 new_pad;

int PatchesLoaded = 0;

int main(void)
{
	int sel = 0;

	ResetIOP();
	LoadModules();

	InitGraph();
	graph_set_clearbuffer(0x00, 0x00, 0x00);
	graph_wait_vsync();

	DrawImage(background, 0, 0, 0, 640, 480, -1);
	graph_wait_vsync();

	padInit(0);
	padPortOpen(0, 0, padBuf);
	initializePad(0, 0);

	DrawArrow(sel);
	while (1)
	{
		if (padRead(0, 0, &buttons));
		{
			paddata = 0xffff ^ buttons.btns;
			new_pad = paddata & ~old_pad;
			old_pad = paddata;

			if (new_pad & PAD_UP)
			{
				if (sel > 0)
				{
					sel--;
					DrawArrow(sel);
				}
			}
			if (new_pad & PAD_DOWN)
			{
				if (sel < 4)
				{
					sel++;
					DrawArrow(sel);
				}
			}
			if (new_pad & PAD_CROSS)
			{
				ExecCMD(sel);
			}
		}
	}



	return 0;
}

void ExecCMD(int sel)
{
	int x, y, z, f;

	x = 0;
	y = 0;
	z = 0;

	if (sel < 3) { x = 165; }
	if (sel == 3) { x = 145; }
	if (sel == 4) { x = 60; }

	if (sel == 0) { y = 165; }
	if (sel == 1) { y = 201; }
	if (sel == 2) { y = 237; }
	if (sel == 3) { y = 273; }
	if (sel == 4) { y = 309; }

	if (sel == 0) // Load From Mc0
	{
		f = LoadPatches(0);
		if (f < 0) { DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 3, 1, 25); }
		else { DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 2, 1, 25); }
	}
	else if (sel == 1) // Load From Mc1
	{
		f = LoadPatches(1);
		if (f < 0) { DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 3, 1, 25); }
		else { DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 2, 1, 25); }
	}
	else if (sel == 2) // Load From USB
	{
		f = LoadPatches(2);
		if (f < 0) { DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 3, 1, 25); }
		else { DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 2, 1, 25); }
	}
	else if (sel == 3) // Launch Game Disc
	{
		ExecGame();
		DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 3, 1, 25);
	}
	else if (sel == 4) // Stop Disc (For backup loading)
	{
		DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 3, 1, 25);
		graph_wait_vsync();
		StopDisc();
		DrawPhotoSpot(arrow, 0, 0, 49, 29, x, y, z, 49, 29, 0x00, 0x00, 0x00, -1, 2, 1, 25);
	}

	graph_wait_vsync();
}

void DrawArrow(int sel)
{
	int x, y, z;

	x = 0;
	y = 0;
	z = 0;

	if (sel < 3) { x = 165; }
	if (sel == 3) { x = 145; }
	if (sel == 4) { x = 60; }

	if (sel == 0) { y = 165; }
	if (sel == 1) { y = 201; }
	if (sel == 2) { y = 237; }
	if (sel == 3) { y = 273; }
	if (sel == 4) { y = 309; }

	DrawPhotoSpot(background, 53, 165, 170, 180, 53, 165, 0, 640, 480, 0x00, 0x00, 0x00, -1, 0, 0, 0);
	DrawImage(arrow, x, y, z, 49, 29, -1);
	graph_wait_vsync();
}

int LoadModules(void)
{
	int err;
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
	SifExecModuleBuffer(&cdvdinit_irx, size_cdvdinit_irx, 0, NULL, &ret);
	SifExecModuleBuffer(&mod_usbd, size_mod_usbd, 0, NULL, &ret);
	SifExecModuleBuffer(&mod_usb_mass, size_mod_usb_mass, 0, NULL, &ret);

	// QWERTY Module from CogSwap
	loadmodulemem(iopmod, 1865, 0, 0);

	return ret;
}

void StopDisc(void)
{
	char elf[40];

	fioOpen("qwerty:r",1); // Wait for the disc to be ready
	ReadCNF(elf);

	ResetCNF();
	ResetIOP();

	SifInitRpc(0);
	LoadModules();

	fioOpen("qwerty:c",1); // Set LBA to 0
	fioOpen("qwerty:s",1); // Stop the disc

	padInit(0);
	padPortOpen(0, 0, padBuf);
	initializePad(0, 0);
}

int ExecGame(void)
{
	int rv, i, f;
	char *args[1];
	char elf[40];
	u32 etr = 0;

	u32 HookValue = (0x0007F000 / 4) + 0x0C000000;

	// Install the font
	u32 FontStore = 0x80030000;
	u32 FontRead = (void*)CourierNew;
	for (i = 0; i < sizeof(CourierNew); i++)
	{
		ee_kmode_enter();
		*(u8*)FontStore = *(u8*)FontRead;
		ee_kmode_exit();
		FontStore++;
		FontRead++;
	}

	// Install the hook filter
	u32 HookStore = 0x8007F000;
	u32 HookRead = (void*)LDv3Hook;
	for (i = 0; i < sizeof(LDv3Hook); i += 4)
	{
		ee_kmode_enter();
		*(u32*)HookStore = *(u32*)HookRead;
		ee_kmode_exit();
		HookStore += 4;
		HookRead += 4;
	}

	// Install the LiveDebug v3 Engine
	u32 EngineStore = 0x80078250;
	u32 EngineRead = (void*)LDv3Engine;
	for (i = 0; i < sizeof(LDv3Engine); i += 4)
	{
		ee_kmode_enter();
		*(u32*)EngineStore = *(u32*)EngineRead;
		ee_kmode_exit();
		EngineStore += 4;
		EngineRead += 4;
	}

	// Install the LiveDebug v3 Cheat Engine
	u32 CheatStore = 0x8007F800;
	u32 CheatRead = (void*)LDv3Cheats;
	for (i = 0; i < sizeof(LDv3Cheats); i += 4)
	{
		ee_kmode_enter();
		*(u32*)CheatStore = *(u32*)CheatRead;
		ee_kmode_exit();
		CheatStore += 4;
		CheatRead += 4;
	}

	// Install string table
	WriteLabels();

	// Configure our Engine
	WriteConfig();

	// Install joker scanner
	WriteScanner();

	//init_scr();
	// Install Patches
	WritePatches();

	/*
	for (i = 0x80047810; i < 0x80047870; i += 4)
	{
		ee_kmode_enter();
		CheatRead = *(u32*)i;
		ee_kmode_exit();
		//scr_printf("%08X %08X\n", i, CheatRead);
	}
	while (1);
	*/

	fioOpen("qwerty:r",1);

	ReadCNF(elf);
	if (elf[0] != '!')
	{
		args[0] = elf;

		etr = LoadELF(elf);

		ee_kmode_enter();
		*(u32*)0x800002FC = HookValue; // Write the hook for everything
		ee_kmode_exit();
		//LoadExecPS2(elf, 0, &args);

		ExecPS2(etr, 0, 1, &args);
	}



	return 0;
}

u32 LoadELF(char *elf)
{
	u32 elfdata = 0x00F00000;
	int i, n, f;

	f = fioOpen(elf, O_RDONLY);
	if (f < 0)
		return -1;

	i = fioLseek(f, 0, SEEK_END);
	fioLseek(f, 0, SEEK_SET);

	if (i <= 0)
	{
		fioClose(f);
		return -2;
	}

	fioRead(f, elfdata, i);
	fioClose(f);
	elfdata = 0x00F00000;

	elf_header_t *eh = &elfdata;
	elf_pheader_t *eph;
	void *pdata;

	eh = (elf_header_t*)elfdata;
	eph = (elf_pheader_t*)(elfdata + eh->phoff);

	for (i = 0; i < eh->phnum; i++)
	{
		if (eph[i].type != 1)
			continue;
	
		pdata = (void *)(elfdata + eph[i].offset);

		for (n = 0; n < eph[i].filesz; n++)
		{
			*(u8*)(eph[i].vaddr + n) = *(u8*)(pdata + n);
		}

	
		if (eph[i].memsz > eph[i].filesz)
		{
			for (n = 0; n < (eph[i].memsz - eph[i].filesz); n++)
			{
				*(u8*)((eph[i].vaddr + eph[i].filesz) + n) = 0x00;
			}
		}
	}

	return eh->entry;
}

void WriteConfig(void)
{
*(u32*)0x00080018 = 0x00085000;
*(u32*)0x0008001C = 0x80030000;
}

int LoadPatches(int m)
{
	int f, i;
	u32 textAddr = 0x00F00000;

	for (i = 0; i < 0x00100000; i += 4)
		*(u32*)(textAddr + i) = 0;

	if (m == 0) { f = fioOpen("mc0:/LDv3Patches.txt", O_RDONLY); }
	if (m == 1) { f = fioOpen("mc1:/LDv3Patches.txt", O_RDONLY); }
	if (m == 2) { f = fioOpen("mass:/LDv3Patches.txt", O_RDONLY); }

	if (f < 0) { return -1; }

	i = fioLseek(f, 0, SEEK_END);
	fioLseek(f, 0, SEEK_SET);

	if (i <= 0) { fioClose(f); return -2; }

	fioRead(f, textAddr, i);
	fioClose(f);

	*(u32*)0x00EFFFFC = i;

	PatchesLoaded = 1;

	return 1;
}

int WritePatches(void)
{
/*
// Testing
ee_kmode_enter();
// 123456789ABCDEF
*(u32*)0x80047810 = 0x34333231;
*(u32*)0x80047814 = 0x38373635;
*(u32*)0x80047818 = 0x43424139;
*(u32*)0x8004781C = 0x00464544;
*(u32*)0x80047820 = 0x200FFFF0;
*(u32*)0x80047824 = 0x12345678;
*(u32*)0x80047828 = 0x00000000;
*(u32*)0x8004782C = 0x00000000;

// Test Code 2
*(u32*)0x80047830 = 0x74736554;
*(u32*)0x80047834 = 0x646F4320;
*(u32*)0x80047838 = 0x00322065;
*(u32*)0x8004783C = 0x00000000;
*(u32*)0x80047840 = 0x100FFFF4;
*(u32*)0x80047844 = 0x12345678;
*(u32*)0x80047848 = 0x00000000;
*(u32*)0x8004784C = 0x00000000;

// Test Code 3
*(u32*)0x80047850 = 0x74736554;
*(u32*)0x80047854 = 0x646F4320;
*(u32*)0x80047858 = 0x00332065;
*(u32*)0x8004785C = 0x00000000;
*(u32*)0x80047860 = 0x000FFFF8;
*(u32*)0x80047864 = 0x12345678;
*(u32*)0x80047868 = 0x00000000;
*(u32*)0x8004786C = 0x00000000;
ee_kmode_exit();
*/
	int i, n, added;
	char c;
	u32 textAddr = 0x00F00000;
	u32 WriteAddr = 0x80047810;
	u8 b1, b2, b3, b4, b5, b6, b7, b8;
	u32 val, szLimit;

	szLimit = textAddr + *(u32*)0x00EFFFFC;

	for (i = 0x80047810; i < 0x8004B810; i += 4)
	{
		ee_kmode_enter();
		*(u32*)i = 0;
		ee_kmode_exit();
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

			ee_kmode_enter();
			*(u8*)(WriteAddr + n) = c;
			*(u8*)(WriteAddr + (n + 1)) = 0x00;
			ee_kmode_exit();
			
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

			ee_kmode_enter();
			*(u32*)WriteAddr = val;
			ee_kmode_exit();
			WriteAddr += 4;
		}
		ee_kmode_enter();
		*(u32*)WriteAddr = 0;
		ee_kmode_exit();
		WriteAddr += 4;

		added++;

		while (*(u8*)textAddr != 0x0A)
			textAddr++;

		textAddr += 3;

		}
		else
		{
			textAddr += 2;
		}
	}
	WC_Done:

	return 1;
}

void WriteLabels(void)
{
ee_kmode_enter();
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



ee_kmode_exit();
}

void WriteScanner(void)
{
	ee_kmode_enter();

*(u32*)0x8007E880 = 0x27BDFFE0;
*(u32*)0x8007E884 = 0xAFBF0000;
*(u32*)0x8007E888 = 0xAFB00004;
*(u32*)0x8007E88C = 0xAFB10008;
*(u32*)0x8007E890 = 0xAFB2000C;
*(u32*)0x8007E894 = 0xAFB30010;
*(u32*)0x8007E898 = 0xAFB40014;
*(u32*)0x8007E89C = 0x3C108008;
*(u32*)0x8007E8A0 = 0x2610E800;
*(u32*)0x8007E8A4 = 0x8E110000;
*(u32*)0x8007E8A8 = 0x8E120004;
*(u32*)0x8007E8AC = 0x8E130008;
*(u32*)0x8007E8B0 = 0x0232A021;
*(u32*)0x8007E8B4 = 0x24020004;
*(u32*)0x8007E8B8 = 0x12620020;
*(u32*)0x8007E8BC = 0x00000000;
*(u32*)0x8007E8C0 = 0x0220202D;
*(u32*)0x8007E8C4 = 0x0C01FA57;
*(u32*)0x8007E8C8 = 0x8E05000C;
*(u32*)0x8007E8CC = 0x10400013;
*(u32*)0x8007E8D0 = 0x00000000;
*(u32*)0x8007E8D4 = 0x8E080010;
*(u32*)0x8007E8D8 = 0x8E090014;
*(u32*)0x8007E8DC = 0x8E0A0018;
*(u32*)0x8007E8E0 = 0x8E0B001C;
*(u32*)0x8007E8E4 = 0x1102000D;
*(u32*)0x8007E8E8 = 0x00000000;
*(u32*)0x8007E8EC = 0x1122000B;
*(u32*)0x8007E8F0 = 0x00000000;
*(u32*)0x8007E8F4 = 0x11420009;
*(u32*)0x8007E8F8 = 0x00000000;
*(u32*)0x8007E8FC = 0x11620007;
*(u32*)0x8007E900 = 0x00000000;
*(u32*)0x8007E904 = 0x00132080;
*(u32*)0x8007E908 = 0x24840010;
*(u32*)0x8007E90C = 0x00902021;
*(u32*)0x8007E910 = 0xAC820000;
*(u32*)0x8007E914 = 0x26730001;
*(u32*)0x8007E918 = 0xAE130008;
*(u32*)0x8007E91C = 0x1634FFE8;
*(u32*)0x8007E920 = 0x26310004;
*(u32*)0x8007E924 = 0x3C020100;
*(u32*)0x8007E928 = 0x0222102A;
*(u32*)0x8007E92C = 0x14400002;
*(u32*)0x8007E930 = 0x00000000;
*(u32*)0x8007E934 = 0x3C110010;
*(u32*)0x8007E938 = 0xAE110000;
*(u32*)0x8007E93C = 0x8FBF0000;
*(u32*)0x8007E940 = 0x8FB00004;
*(u32*)0x8007E944 = 0x8FB10008;
*(u32*)0x8007E948 = 0x8FB2000C;
*(u32*)0x8007E94C = 0x8FB30010;
*(u32*)0x8007E950 = 0x8FB40014;
*(u32*)0x8007E954 = 0x03E00008;
*(u32*)0x8007E958 = 0x27BD0020;
*(u32*)0x8007E95C = 0x27BDFFF0;
*(u32*)0x8007E960 = 0xAFBF0000;
*(u32*)0x8007E964 = 0xAFB00004;
*(u32*)0x8007E968 = 0xAFB10008;
*(u32*)0x8007E96C = 0x0080802D;
*(u32*)0x8007E970 = 0x00A0882D;
*(u32*)0x8007E974 = 0x8E280000;
*(u32*)0x8007E978 = 0x8E290004;
*(u32*)0x8007E97C = 0x8E2A0008;
*(u32*)0x8007E980 = 0x8E2B000C;
*(u32*)0x8007E984 = 0x8E2C0010;
*(u32*)0x8007E988 = 0x8E2D0014;
*(u32*)0x8007E98C = 0x8E2E0018;
*(u32*)0x8007E990 = 0x8E2F001C;
*(u32*)0x8007E994 = 0x8E380020;
*(u32*)0x8007E998 = 0x8E390024;
*(u32*)0x8007E99C = 0x8E230028;
*(u32*)0x8007E9A0 = 0x8E020000;
*(u32*)0x8007E9A4 = 0x15020007;
*(u32*)0x8007E9A8 = 0x00000000;
*(u32*)0x8007E9AC = 0x8E020004;
*(u32*)0x8007E9B0 = 0x15220004;
*(u32*)0x8007E9B4 = 0x00000000;
*(u32*)0x8007E9B8 = 0x2602001C;
*(u32*)0x8007E9BC = 0x10000026;
*(u32*)0x8007E9C0 = 0x00000000;
*(u32*)0x8007E9C4 = 0x8E020000;
*(u32*)0x8007E9C8 = 0x15420007;
*(u32*)0x8007E9CC = 0x00000000;
*(u32*)0x8007E9D0 = 0x8E020004;
*(u32*)0x8007E9D4 = 0x15620004;
*(u32*)0x8007E9D8 = 0x00000000;
*(u32*)0x8007E9DC = 0x2602001C;
*(u32*)0x8007E9E0 = 0x1000001D;
*(u32*)0x8007E9E4 = 0x00000000;
*(u32*)0x8007E9E8 = 0x8E020000;
*(u32*)0x8007E9EC = 0x1582000A;
*(u32*)0x8007E9F0 = 0x00000000;
*(u32*)0x8007E9F4 = 0x8E020004;
*(u32*)0x8007E9F8 = 0x15A20007;
*(u32*)0x8007E9FC = 0x00000000;
*(u32*)0x8007EA00 = 0x8E020008;
*(u32*)0x8007EA04 = 0x15C20004;
*(u32*)0x8007EA08 = 0x00000000;
*(u32*)0x8007EA0C = 0x2602FFE6;
*(u32*)0x8007EA10 = 0x10000011;
*(u32*)0x8007EA14 = 0x00000000;
*(u32*)0x8007EA18 = 0x8E020000;
*(u32*)0x8007EA1C = 0x15E2000D;
*(u32*)0x8007EA20 = 0x00000000;
*(u32*)0x8007EA24 = 0x8E020004;
*(u32*)0x8007EA28 = 0x1702000A;
*(u32*)0x8007EA2C = 0x00000000;
*(u32*)0x8007EA30 = 0x8E020008;
*(u32*)0x8007EA34 = 0x17220007;
*(u32*)0x8007EA38 = 0x00000000;
*(u32*)0x8007EA3C = 0x8E02000C;
*(u32*)0x8007EA40 = 0x14620004;
*(u32*)0x8007EA44 = 0x00000000;
*(u32*)0x8007EA48 = 0x2602FFE6;
*(u32*)0x8007EA4C = 0x10000002;
*(u32*)0x8007EA50 = 0x00000000;
*(u32*)0x8007EA54 = 0x00001021;
*(u32*)0x8007EA58 = 0x8FBF0000;
*(u32*)0x8007EA5C = 0x8FB00004;
*(u32*)0x8007EA60 = 0x8FB10008;
*(u32*)0x8007EA64 = 0x03E00008;
*(u32*)0x8007EA68 = 0x27BD0010;

*(u32*)0x8007E800 = 0x00100000;
*(u32*)0x8007E804 = 0x00001000;
*(u32*)0x8007E808 = 0x00000000;
*(u32*)0x8007E80C = 0x8007e820;
*(u32*)0x8007E810 = 0x00000000;
*(u32*)0x8007E814 = 0x00000000;
*(u32*)0x8007E818 = 0x00000000;
*(u32*)0x8007E81C = 0x00000000;

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
}


void ReadCNF(char *elfnm)
{
	FILE *fd;
	char *systemcnf;
	int fsize;
	int OnFirstLine = 1;
	int IsElfName, ElfIndex;

	fd = fopen("cdrom0:\\SYSTEM.CNF;1", "r");
	if (fd < 0)
	{
		fd = fopen("cdrom0:\\system.cnf;1", "r");
	}

	if (fd > 0)
	{
		fseek(fd, 0, SEEK_END);
		fsize = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		systemcnf = memalign(128, fsize);
		fread(systemcnf, fsize, 1, fd);
		fclose(fd);

		//printf("SYSTEM.CNF: Contents ->\n %s ", systemcnf);
		//printf("%i Bytes ", fsize);

		int i;

		IsElfName = 0;
		OnFirstLine = 1;

		for (i = 0; i < fsize; i++)
		{
			if (OnFirstLine == 1)
			{
				if (IsElfName == 0)
				{
					if (systemcnf[i] == 'c')
					{
						if (systemcnf[i + 1] == 'd')
						{
							if (systemcnf[i + 2] == 'r')
							{
								if (systemcnf[i + 3] == 'o')
								{
									if (systemcnf[i + 4] == 'm')
									{
										if (systemcnf[i + 5] == '0')
										{
											ElfIndex = 0;
											IsElfName = 1;
										}
									}
								}
							}
						}
					}
				}
				if (IsElfName == 1)
				{
					elfnm[ElfIndex] = systemcnf[i];
					ElfIndex += 1;
					if (systemcnf[i] == ';')
					{
						if (systemcnf[i + 1] == '1')
						{
							elfnm[ElfIndex] = systemcnf[i + 1];
							elfnm[ElfIndex + 1] = 0x00;
							OnFirstLine = 0;
						}
					}
				}
			}
		}

		//elfnme = elfname;

		//printf("SYSTEM.CNF: ELF = %s ", elfnm);

	}
	else
	{
		elfnm[0] = '!';
		elfnm[1] = 0x00;
	}
}

void ResetIOP(void)
{
	SifIopReset("rom0:UDNL rom0:EELOADCNF", 0);
	while ( SifIopSync()) ;
	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();
	EI;
	FlushCache(0);
	FlushCache(2);

	SifIopReset("rom0:UDNL rom0:EELOADCNF", 0);
	while ( SifIopSync()) ;
	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();
	SifExitRpc();
	SifExitCmd();
	EI;
	FlushCache(0);
	FlushCache(2);

	SifInitRpc(0);
	SifInitCmd();
	SifLoadFileInit();
	fioInit();
	
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
	EI;

	SifInitRpc(0);

	FlushCache(0);
	FlushCache(2);
}
