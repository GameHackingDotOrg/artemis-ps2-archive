/*
	CodeDesigner v2.0
	Created by: Gtlcpimp
	Official CheatersLounge Product Copyright ©
*/
/*
   CL-LiveDebug v3 Hook Filter v0.2c
   Created by: Gtlcpimp
   CheatersLounge Copyright ©
*/

address $0007F000

_init:
addiu sp, sp, $FFF0
sq ra, $0000(sp)
call _start
lq ra, $0000(sp)
addiu sp, sp, $0010
//jalr k0
jr k0
nop

//==========================================================

_start:
addiu sp, sp, $FE00
sq at, $0000(sp)
sq v0, $0010(sp)
sq v1, $0020(sp)
sq a0, $0030(sp)
sq a1, $0040(sp)
sq a2, $0050(sp)
sq a3, $0060(sp)
sq t0, $0070(sp)
sq t1, $0080(sp)
sq t2, $0090(sp)
sq t3, $00a0(sp)
sq t4, $00b0(sp)
sq t5, $00c0(sp)
sq t6, $00d0(sp)
sq t7, $00e0(sp)
sq s0, $00f0(sp)
sq s1, $0100(sp)
sq s2, $0110(sp)
sq s3, $0120(sp)
sq s4, $0130(sp)
sq s5, $0140(sp)
sq s6, $0150(sp)
sq s7, $0160(sp)
sq t8, $0170(sp)
sq t9, $0180(sp)
sq k0, $0190(sp)
sq k1, $01a0(sp)
sq fp, $01b0(sp)
sq gp, $01c0(sp)
sq ra, $01d0(sp)

//This was designed to set the screen w/h, but this has nothing to do with that
//I am leaving it here in case someone wants to force high resolution and use this...?
//addiu t1, zero, $0002
//beq v1, t1, :_SetGsCrtSyscall
//nop

_AfterGsCrt:
//Check if the install handler has already been installed
lui t1, $0009
lw t1, $A000(t1)

bne t1, zero, 3
nop

jal :_InstallEH
nop

//Joker scanner on/off switch
lui t1, $8008
lw t1, $E7FC(t1)

bne t1, zero, 3
nop

jal $0007E880 //Joker scanner
nop

jal $0007F800 //Cheat engine
nop

jal :_ReadPad
addiu a0, zero, $FFF9
beq v0, zero, :LDv3HookExit
nop

//Gets current screen width and height
lui t1, $8005
jal :_GetDisplay
nop
sd v0, $0100(t1)
sd v1, $0108(t1)

//Sets screen width and height
jal :_SetDisplay
nop

jal $00060000
daddu a0, zero, zero

//Return screen to normal width and height
lui t1, $8005
ld a0, $0100(t1)
ld a1, $0108(t1)
jal :_RestoreDisplay
nop

LDv3HookExit:
lq at, $0000(sp)
lq v0, $0010(sp)
lq v1, $0020(sp)
lq a0, $0030(sp)
lq a1, $0040(sp)
lq a2, $0050(sp)
lq a3, $0060(sp)
lq t0, $0070(sp)
lq t1, $0080(sp)
lq t2, $0090(sp)
lq t3, $00a0(sp)
lq t4, $00b0(sp)
lq t5, $00c0(sp)
lq t6, $00d0(sp)
lq t7, $00e0(sp)
lq s0, $00f0(sp)
lq s1, $0100(sp)
lq s2, $0110(sp)
lq s3, $0120(sp)
lq s4, $0130(sp)
lq s5, $0140(sp)
lq s6, $0150(sp)
lq s7, $0160(sp)
lq t8, $0170(sp)
lq t9, $0180(sp)
lq k0, $0190(sp)
lq k1, $01a0(sp)
lq fp, $01b0(sp)
lq gp, $01c0(sp)
lq ra, $01d0(sp)
jr ra
addiu sp, sp, $0200

//==========================================================
_ReadPad:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)

lui s0, $8008
addiu s0, s0, $E810

lw t0, $0000(s0)
lw t1, $0004(s0)
lw t2, $0008(s0)
lw t3, $000C(s0)
addiu t4, zero, -1
addiu t5, zero, -1
addiu t6, zero, -1
addiu t7, zero, -1
addiu t8, zero, -1

beq t0, zero, :_ReadPadQuit
addu v0, zero, zero
lh t4, $0000(t0)

beq t1, zero, :_ReadPadCompare
addu v0, zero, zero
lh t5, $0000(t1)

beq t2, zero, :_ReadPadCompare
addu v0, zero, zero
lh t6, $0000(t2)

beq t3, zero, :_ReadPadCompare
addu v0, zero, zero
lh t7, $0000(t3)


_ReadPadCompare:
beq t8, a0, :_ReadPadCompareNOINPUT
nop

beq t4, a0, :_ReadPadQuit
addiu v0, zero, 1
beq t5, a0, :_ReadPadQuit
addiu v0, zero, 1
beq t6, a0, :_ReadPadQuit
addiu v0, zero, 1
beq t7, a0, :_ReadPadQuit
addiu v0, zero, 1

addu v0, zero, zero

goto _ReadPadQuit
_ReadPadCompareNOINPUT:

bne t4, t8, :_ReadPadQuit
addu v0, zero, zero
bne t5, t8, :_ReadPadQuit
addu v0, zero, zero
bne t6, t8, :_ReadPadQuit
addu v0, zero, zero
bne t7, t8, :_ReadPadQuit
addu v0, zero, zero

addiu v0, zero, 1

_ReadPadQuit:
lw ra, $0000(sp)
lw s0, $0004(sp)
lw s1, $0008(sp)
jr ra
addiu sp, sp, $0010

/*
GRAPH_MODE graph_mode[22] =
{

	{   0,   0,    0,    0,    0 }, // AUTO
	{ 652,  26, 2560,  256, 0x02 }, // NTSC-NI
	{ 680,  37, 2560,  256, 0x03 }, // PAL-NI
	{ 232,  35, 1440,  480, 0x50 }, // 480P
	{ 320,  64, 1312,  576, 0x53 }, // 576P only in bios>=220
	{ 420,  40, 1280,  720, 0x52 }, // 720P
	{ 300, 120, 1920,  540, 0x51 }, // 1080I
	{ 280,  18, 1280,  480, 0x1A }, // VGA   640x480@60
	{ 330,  18, 1280,  480, 0x1B }, // VGA   640x480@72
	{ 360,  18, 1280,  480, 0x1C }, // VGA   640x480@75
	{ 260,  18, 1280,  480, 0x1D }, // VGA   640x480@85
	{ 450,  25, 1600,  600, 0x2A }, // VGA   800x600@56
	{ 465,  25, 1600,  600, 0x2B }, // VGA   800x600@60
	{ 465,  25, 1600,  600, 0x2C }, // VGA   800x600@72
	{ 510,  25, 1600,  600, 0x2D }, // VGA   800x600@75
	{ 500,  25, 1600,  600, 0x2E }, // VGA   800x600@85
	{ 580,  30, 2048,  768, 0x3B }, // VGA  1024x768@60
	{ 266,  30, 1024,  768, 0x3C }, // VGA  1024x768@70
	{ 260,  30, 1024,  768, 0x3D }, // VGA  1024x768@75
	{ 290,  30, 1024,  768, 0x3E }, // VGA  1024x768@85
	{ 350,  40, 1280, 1024, 0x4A }, // VGA 1280x1024@60
	{ 350,  40, 1280, 1024, 0x4B }, // VGA 1280x1024@75

};
*/

_SetGsCrtSyscall:
lui t1, $8005
sq a0, $0030(t1)
sq a1, $0040(t1)
sq a2, $0050(t1)
sq a3, $0060(t1)

//I have no method to shift a register over 96 bits...
//So I have to be elaborate
lui a2, $0500
sw a2, $000C(t1)
lq a2, $0000(t1)
sq a2, $0010(t1)

addiu a0, zero, $0001
addiu a1, zero, $0002
addiu a3, zero, zero

GsCrtHookExit:
lq at, $0000(sp)
lq v0, $0010(sp)
lq v1, $0020(sp)
lq t0, $0070(sp)
lq t1, $0080(sp)
lq t2, $0090(sp)
lq t3, $00a0(sp)
lq t4, $00b0(sp)
lq t5, $00c0(sp)
lq t6, $00d0(sp)
lq t7, $00e0(sp)
lq s0, $00f0(sp)
lq s1, $0100(sp)
lq s2, $0110(sp)
lq s3, $0120(sp)
lq s4, $0130(sp)
lq s5, $0140(sp)
lq s6, $0150(sp)
lq s7, $0160(sp)
lq t8, $0170(sp)
lq t9, $0180(sp)
lq k0, $0190(sp)
lq k1, $01a0(sp)
lq fp, $01b0(sp)
lq gp, $01c0(sp)
lq ra, $01d0(sp)
jr ra
addiu sp, sp, $0200

//By Gtlcpimp
//==========================================================
_GetDisplay:

lui a0, $1200

ld v0, $0080(a0) // Get Display 1
ld v1, $00A0(a0) // Get Display 2

jr ra
nop
//==========================================================
_RestoreDisplay:

lui v0, $1200

sd a0, $0080(v0) // Get Display 1
sd a1, $00A0(v0) // Get Display 2

jr ra
nop

//==========================================================
// GS_SET_DISPLAY(632, 50, 3, 0, 2559,  447)
// 0x12000080 = GS_REG_DISPLAY1
// 0x120000A0 = GS_REG_DISPLAY2
// 01832278 001BF9FF
_SetDisplay:

lui a0, $1200

lui v0, $001B
ori v0, v0, $F9FF
dsll32 v0, v0, 0
lui v1, $0183
ori v1, v1, $2278
daddu v0, v0, v1
sd v0, $0080(a0)   // Set Display 1
sd v0, $00A0(a0)   // Set Display 2

jr ra
nop

//Moves exception handler from 80050000 to 000E0000
//==============================================================
_InstallEH:
addiu sp, sp, $FFB0
sq ra, $0000(sp)
sq a0, $0010(sp)
sq a1, $0020(sp)
sq v0, $0030(sp)
sq v1, $0040(sp)

lui s0, $8005 //Start
addiu s1, s0, $27EC //End
setreg s2, $0008A000

_LoopIEH:
lw t0, $0000(s0)
sw t0, $0000(s2)

addiu s2, s2, 4
addiu s0, s0, 4

bne s1, s0, :_LoopIEH
nop

//Then installs a pointer to the handler
//==============================================================
_installHandler:

lui a0, $8000
addiu a1, zero, $8f5a

_IH_ScanLoop:

lh v0, $0000(a0)
lh v1, $0002(a0)
bne a1, v1, :_IH_ScanLoop
addiu a0, a0, 4

lui a0, $8001
daddu a0, a0, v0

setreg v0, $0008A000
//lui v1, $8000
//addu v0, v0, v1

sw v0, $0004(a0) // TLB Modification
sw v0, $0008(a0) // TLB Load/Inst Fetch
sw v0, $000c(a0) // TLB Store
sw v0, $0010(a0) // Address Load/Inst Fetch
sw v0, $0014(a0) // Address Store
sw v0, $0018(a0) // Bus Error (instr)
sw v0, $001c(a0) // Bus Error (data)
//sw v0, $0020(a0) // Syscall()
//sw v0, $0024(a0) // Breakpoint
sw v0, $0028(a0) // Reserved Instruction
sw v0, $002c(a0) // Coprocessor Unsuable
sw v0, $0030(a0) // Overflow
sw v0, $0034(a0) // Something

lq ra, $0000(sp)
lq a0, $0010(sp)
lq a1, $0020(sp)
lq v0, $0030(sp)
lq v1, $0040(sp)
jr ra
addiu sp, sp, $0050
