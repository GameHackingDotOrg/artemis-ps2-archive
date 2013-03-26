/*
	CodeDesigner v2.0
	Created by: Gtlcpimp
	Official CheatersLounge Product Copyright ©
*/
/*
   CL-LiveDebug v3 Debugger Engine v3.2b (Modded by Dnawrkshp)
   Created by: Gtlcpimp
   CheatersLounge Copyright ©
*/
/*
Configuration block
address $00080018
hexcode $00085000
hexcode $80030008
*/

//==========================================================
address $00060000

_main:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)


daddu s4, a0, zero // Z

jal :_initScreen
nop
jal :_drawWatermark
nop

jal :vSync
nop

addiu s0, zero, 300 // Loop timeout

addiu s1, zero, $00FF
addiu s2, zero, $00C0
addu s3, zero, zero

addu s5, zero, zero

_mainLoop:

bne s5, zero, 5
nop
addiu s1, s1, -1
addiu s2, s2, -1
beq zero, zero, 3
nop
addiu s1, s1, 1
addiu s2, s2, 1

addiu v0, zero, $00C0
bne s2, v0, 2
nop
addu s5, zero, zero

bne s2, zero, 2
nop
addiu s5, zero, 1


// Text (*Fail-Safe mode* Press 'START' to enter the menu.)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $454C
addiu t0, zero, 26
addiu t1, zero, 210
daddu t2, s4, zero
daddu t3, s1, zero
daddu t4, s2, zero
daddu t5, s3, zero
addiu t6, zero, $0000
call _drawString

// Fail safe, sometimes the controller shuts off in IOP
jal :_ReadPad
addiu a0, zero, $FFF7
beq v0, zero, 5
nop
jal :_MainMenu
addu a0, zero, zero
goto _mainExit

jal :vSync
nop

bne s0, zero, :_mainLoop
addiu s0, s0, -1

_mainExit:
jal :_ClearPad
nop
lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_MainMenu:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

daddu s4, a0, zero

jal :_initScreen
nop
jal :_drawWatermark
nop
jal :_drawMenuControls
addu a0, zero, zero

//000A0094 = op
//000A009C = j $RETURNADDR
lui t0, $000A
lw t1, $0000(t0)
bne t1, zero, :_DumperExists
nop
jal :_installDumper
nop

_DumperExists:
addu s6, zero, zero // Holding
addu s5, zero, zero // Selected
_MainMenuLoop:

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 10   // x
addiu t2, zero, 20   // y
multu t2, t2, s5
addiu t2, t2, 60
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 620  // w
addiu t9, zero, 20  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

// Text (Memory Browser)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $460C
addiu t0, zero, 236
addiu t1, zero, 60
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString


// Text (Codes)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $462C
addiu t0, zero, 280
addiu t1, zero, 80
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Patches)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4634
addiu t0, zero, 272
addiu t1, zero, 100
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Search)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4794
addiu t0, zero, 278
addiu t1, zero, 120
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Register Dumper)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $489C
addiu t0, zero, 236
addiu t1, zero, 140
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Return To Game)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $461C
addiu t0, zero, 236
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//For spacing testing
/*
// Characters 0x21 - 0x4C
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $48A0
addiu t0, zero, 10
addiu t1, zero, 180
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Characters 0x4D - 0x7E
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $48D0
addiu t0, zero, 10
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString
*/

jal :vSync
nop

bne s6, zero, :_MainMenuWaitNoInput
nop

jal :_ReadPad
addiu a0, zero, $FFEF // Up
beq v0, zero, 6
nop
addiu s6, zero, 1
sltiu v0, s5, $0001
bne v0, zero, 2
nop
addiu s5, s5, -1

jal :_ReadPad
addiu a0, zero, $FFBF // Down
beq v0, zero, 6
nop
addiu s6, zero, 1
sltiu v0, s5, $0005
beq v0, zero, 2
nop
addiu s5, s5, 1

jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_MainMenuExitToLoop
nop
addiu s6, zero, 1
bne s5, zero, 5 // ------- Memory Browser
nop
jal :_MenuBrowser
addu a0, zero, zero
goto _MainMenuExitToLoop

addiu v0, zero, 1 // ----- Codes
bne s5, v0, 5
nop
jal :_CodesMenu
addu a0, zero, zero
goto _MainMenuExitToLoop

addiu v0, zero, 2 // ----- Patches
bne s5, v0, 5
nop
jal :_PatchesMenuBrowser
addu a0, zero, zero
goto _MainMenuExitToLoop

addiu v0, zero, 3 // ----- Search
bne s5, v0, 5
nop
jal :_MemSearchMenu
addu a0, zero, zero
goto _MainMenuExitToLoop

addiu v0, zero, 4 // ----- Register Dumper
bne s5, v0, 5
nop
jal :_RegisterDumpMenu
addu a0, zero, zero
goto _MainMenuExitToLoop

addiu v0, zero, 5 // ----- Return to Game
bne s5, v0, 5
nop
nop
nop
goto _MainMenuExit

_MainMenuWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_MainMenuExitToLoop
nop
addu s6, zero, zero

_MainMenuExitToLoop:


jal :vSync
nop
goto _MainMenuLoop

_MainMenuExit:
lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_installDumper:
//Kernel location: 80044A00
//EE location: 000A0000
//Dumper size: 000A00A4

lui t0, $000B //Store location
lui t1, $8004
ori t1, t1, $5000 //Load location
ori t2, t0, $00B8 //Size

_LoopDumpInstall:
beq t0, t2, :_DoneInstall
nop

lw t3, $0000(t1)
sw t3, $0000(t0)

addiu t1, t1, 4
addiu t0, t0, 4
beq zero, zero, :_LoopDumpInstall
nop

_DoneInstall:
jr ra
nop

//==========================================================
/*
a0 = Install address
Returns v0 as 0 if successful or 1 if not within 0x00080000 and 0x02000000
*/
_PlaceDumper:
addiu sp, sp, $FFE0
sq s0, $0000(sp)
sq ra, $0010(sp)
daddu s0, a0, zero

daddu a0, s0, zero
lui a1, $0008
jal :_IfBetween
lui a2, $0200
bne v0, zero, :_ExitPDumper
nop

lui t0, $000B
lw t1, $FFFC(t0)
beq t1, zero, 3
nop
jal :_RemoveDumper
daddu a0, t1, zero
sw s0, $FFFC(t0)

//j $regdumper, t9
lui t0, $000B
addiu t1, zero, 4
divu t0, t1
mflo t0
lui t1, $0800
addu t9, t1, t0 //Result is a j $REGDUMPERADDR

//Return j $installaddr + 8, t8
addu t0, s0, zero
addiu t1, zero, 4
divu t0, t1
mflo t0
lui t1, $0800
addiu t1, t1, 2
addu t8, t1, t0 //Result is a j $INSTALLEDADDR + 8

//get installedaddr's operation, t7
lw t7, $0000(s0)

//Actual installation
lui t0, $000B
sw t7, $00A8(t0) //Store installaddr's op
sw t8, $00B0(t0) //Store j $install addr + 4
sw t9, $0000(s0) //Store j $regdumper

add v0, zero, zero
_ExitPDumper:
lq s0, $0000(sp)
lq ra, $0010(sp)
jr ra
addiu sp, sp, $0020

//==========================================================
/*
a0 = installed address
*/
_RemoveDumper:
addiu sp, sp, $FFF0
sq ra, $0000(sp)

lui t0, $000B
lw t7, $00A8(t0)

lui a1, $0008
jal :_IfBetween
lui a2, $0200
bne v0, zero, :_RemDMain
nop
sw t7, $0000(a0) //Store installaddr's operation

_RemDMain:
sw zero, $FFFC(t0)
sw zero, $00A8(t0)
sw zero, $00B0(t0)

//Clear register dump
addiu t1, t0, $0100
addiu t2, t0, $0300
_RemDLoop:
beq t1, t2, :_ExitRemD
nop
sq zero, $0000(t1)
addiu t1, t1, $0010
beq zero, zero, :_RemDLoop
nop

_ExitRemD:
lq ra, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
/*
a0 = value
a1 = small between
a2 = big between
v0 returns 0 if it is between, 1 if it is not
NOTE: This is designed to reserve the used temporary registers!
*/
_IfBetween:
addiu sp, sp, $FFE0
sq t0, $0000(sp)
sq t1, $0010(sp)

slt t0, a0, a1
slt t1, a2, a0

or v0, t0, t1 //If t0 or t1 is 1, returns 1. Otherwise returns 0
lq t0, $0000(sp)
lq t1, $0010(sp)
jr ra
addiu sp, sp, $0020

//==========================================================
_RegisterDumpMenu:
addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

addu s1, zero, zero //Blue marker
addiu s6, zero, 1
addiu k0, zero, 0 //Register scroll (0 - 1)

_MainRDLoop:
// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, zero, zero // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160


// Text (^)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $49AE
addiu t0, zero, 540
addiu t1, zero, 120
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (v)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $49AC
addiu t0, zero, 540
addiu t1, zero, 340
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Register Dumper hooked to:)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4978
addiu t0, zero, 100
addiu t1, zero, 60
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui s5, $000A
ori s5, s5, $FFFC
lui s4, $8004
ori s4, s4, $5200

addu a0, s5, zero
addu a1, s4, zero
jal :_lineToHex
addiu a2, zero, 8

// Text of ADDRESS
lui a0, $0008
lw a0, $001C(a0)
daddu a1, s4, zero
addiu t0, zero, 350
addiu t1, zero, 60
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Loop for printing registers (if k0 = 0: at - s0. if k0 = 1: s1 - ra)
//at - s0
lui a1, $8004
ori a1, a1, $48AC //Starting register label address
lui s3, $8004
ori s3, s3, $48EC //End point
beq k0, zero, 5
nop
//s1 - ra
lui a1, $8004
ori a1, a1, $48EC //Starting register label address
lui s3, $8004
ori s3, s3, $4928 //End point

lui a0, $0008
lw a0, $001C(a0)
addiu s2, zero, 80

_RegPrintLoop:
beq a1, s3, :_RegPrintExitLoop
nop

addiu t0, zero, 40
daddu t1, zero, s2
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu a1, a1, 4 //Register label address
addiu s2, s2, 20 //Y value

beq zero, zero, :_RegPrintLoop
nop
_RegPrintExitLoop:

//Loop for printing quad-words of registers
addiu sp, sp, $FFF0
sq s4, $0000(sp)

lui s5, $000B
ori s5, s5, $0100
lui s3, $000B
ori s3, s3, $0200
beq k0, zero, 5
nop
lui s5, $000B
ori s5, s5, $0200
lui s3, $000B
ori s3, s3, $02F0

addiu s2, zero, 80

_RegValPrintLoop:
lui s4, $8004
ori s4, s4, $5200
beq s5, s3, :_RegValPrintExitLoop
nop

//Part 1
addiu a0, s5, 12
addu a1, s4, zero
jal :_lineToHex
addiu a2, zero, 8
addiu s4, s4, 8

//Adds a space
addiu v0, zero, $0020
sb v0, $0000(s4)
addiu s4, s4, 1

//Part 2
addiu a0, s5, 8
addu a1, s4, zero
jal :_lineToHex
addiu a2, zero, 8
addiu s4, s4, 8

//Adds a space
addiu v0, zero, $0020
sb v0, $0000(s4)
addiu s4, s4, 1

//Part 3
addiu a0, s5, 4
addu a1, s4, zero
jal :_lineToHex
addiu a2, zero, 8
addiu s4, s4, 8

//Adds a space
addiu v0, zero, $0020
sb v0, $0000(s4)
addiu s4, s4, 1

//Part 4
addiu a0, s5, 0
addu a1, s4, zero
jal :_lineToHex
addiu a2, zero, 8
addiu s4, s4, 8

addiu s5, s5, $0010

lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
ori a1, a1, $5200
addiu t0, zero, 80
addu t1, s2, zero
lq t2, $0000(sp) //s4 (Z) is stored here
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, 12 //Spacing
call _drawString

addiu s2, s2, 20
beq zero, zero, :_RegValPrintLoop
nop

_RegValPrintExitLoop:
lq s4, $0000(sp)
addiu sp, sp, $0010

//Instead of looping and overwriting the register stuff above, since they never change,
//I have it just redraw the bottom part with the options
//This just speeds it up a bit

_RDMenuLoop:
// Clear lower region space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 400   // y
daddu t3, zero, zero // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 40  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 220
multu t1, t1, s1
addiu t1, t1, 60     // x
addiu t2, zero, 400  // y
daddu t3, zero, zero // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 200  // w
addiu t9, zero, 20   // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

// Text (Go Back)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $47D4
addiu t0, zero, 120
addiu t1, zero, 400
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Remove dumper)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4968
addiu t0, zero, 310
addiu t1, zero, 400
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

bne s6, zero, :_RDMenuWaitNoInput
nop
//------------------------------ Cross
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_RDNoCross
nop
addiu s6, zero, 1
bne zero, s1, 3
nop
beq zero, zero, :_RDumperMenuExit
nop
lui t0, $000A
jal :_RemoveDumper
lw a0, $FFFC(t0)
beq zero, zero, :_MainRDLoop
nop

_RDNoCross:
//------------------------------- Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, :_RDNoUp
nop
addiu s6, zero, 1
beq k0, zero, :_RDNoUp
nop
addiu k0, k0, -1
beq zero, zero, :_MainRDLoop
nop

_RDNoUp:
//------------------------------- Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, :_RDNoDown
nop
addiu v0, zero, 1
beq k0, v0, :_RDNoDown
nop
addiu k0, k0, 1
beq zero, zero, :_MainRDLoop
nop

_RDNoDown:
//------------------------------- Left
jal :_ReadPad
addiu a0, zero, $FF7F
beq v0, zero, :_RDNoLeft
nop
beq s1, zero, :_RDNoLeft
nop
addiu s1, s1, -1

_RDNoLeft:
//------------------------------- Right
jal :_ReadPad
addiu a0, zero, $FFDF
beq v0, zero, :_RDNoRight
nop
addiu v0, zero, 1
beq s1, v0, :_RDNoRight
nop
addiu s1, s1, 1

_RDNoRight:

beq zero, zero, :_RDMenuExitToLoop
nop

_RDMenuWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_RDMenuExitToLoop
nop
addu s6, zero, zero

_RDMenuExitToLoop:
jal :vSync
nop
beq zero, zero, :_RDMenuLoop
nop

_RDumperMenuExit:
lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_InstallRDMenu:
addiu sp, sp, $FFA0
sq ra, $0000(sp)
sq s1, $0010(sp)
sq s0, $0020(sp)
sq s5, $0030(sp)
sq s6, $0040(sp)
sq s7, $0050(sp)

daddu s7, a0, zero

jal :_ClearPad
nop

lui s1, $0008
add s5, zero, zero
daddu s6, zero, zero

_IRDMenuLoop:
// Make a little box
lui t0, $0009        // Packet
addiu t1, zero, 150  // x
addiu t2, zero, 150  // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $040 // b
daddu t7, zero, zero // size
addiu t8, zero, 300  // w
addiu t9, zero, 100  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 160   // x
addiu t2, zero, 20   // y
multu t2, t2, s5
addiu t2, t2, 200
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 280  // w
addiu t9, zero, 20   // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

//Text "Ok"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $44DD // "Ok"
addiu t0, zero, 300
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Text "Cancel"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $44C8 // "Cancel"
addiu t0, zero, 280
addiu t1, zero, 220
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Text "Install Dumper at"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $4928 // "Install Dumper at"
addiu t0, zero, 170
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui s0, $8004
sw s7, $4BFC(s0)
addiu a0, s0, $4BFC
ori a1, s0, $4C00
jal :_lineToHex
addiu a2, zero, 8

//Text Name of Code
lw a0, $001C(s1)
addiu a1, s0, $4C00 // Pointer to address
addiu t0, zero, 320
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

jal :vSync
nop

bne s6, zero, :_IRDMenuWaitNoInput
nop

//------------------------------ Cross
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_IRD_NotCross
nop
addiu s6, zero, 1

bne zero, s5, :_AfterIRDOk //Ok
nop
daddu a0, s7, zero
jal :_PlaceDumper
nop
beq v0, zero, :_IRDMenuExit
nop

jal :_ReadPad
addiu a0, zero, $BFFF
bne a0, zero, :_LoopIRDError
nop

_LoopIRDError:
// Make a little box
lui t0, $0009        // Packet
addiu t1, zero, 50   // x
addiu t2, zero, 150  // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $040 // b
daddu t7, zero, zero // size
addiu t8, zero, 500  // w
addiu t9, zero, 100  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 160  // x
addiu t2, zero, 20   // y
multu t2, t2, s5
addiu t2, t2, 200
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 280  // w
addiu t9, zero, 20   // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

//Text "Address not between 00080000 and 02000000!"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $493C // "Address not between 00080000 and 02000000!"
addiu t0, zero, 100
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

jal :vSync
nop

jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_LoopIRDError
nop

//So I was having issue making it wait for no button to be pressed before looking for X to be pressed..
//I ended up having to use this method. It waits for it to be ready then it prints the press X and waits for X to be pressed
_IRDRP:

//Text (Press (X) To Continue.)
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $4818
addiu t0, zero, 170
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

jal :vSync
nop

jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_IRDRP
nop

beq zero, zero, :_IRDMenuExit
nop

_AfterIRDOk:
// Exit CDMenu and return 0
add v0, zero, zero
beq zero, zero, :_IRDMenuExit
nop

_IRD_NotCross:
//------------------------------ Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, :_IRD_NotUp
nop
addiu s6, zero, $0001
beq s5, zero, 2
nop
addiu s5, s5, -1

_IRD_NotUp:
//------------------------------ Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, :_IRD_NotDown
nop
addiu t1, zero, $0001
addiu s6, zero, $0001
beq s5, t1, 2
nop
addiu s5, s5, 1

_IRD_NotDown:

beq zero, zero, :_IRDMenuLoop
nop

_IRDMenuWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_IRDMenuExitToLoop
nop
addu s6, zero, zero

_IRDMenuExitToLoop:
beq zero, zero, :_IRDMenuLoop
nop

_IRDMenuExit:

lq ra, $0000(sp)
lq s1, $0010(sp)
lq s0, $0020(sp)
lq s5, $0030(sp)
lq s6, $0040(sp)
lq s7, $0050(sp)
jr ra
addiu sp, sp, $0060

//==========================================================
_CodesMenu:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

lui k0, $0008

lui s0, $8004
ori s0, s0, $7810

daddu s1, zero, zero // Select Index
daddu s2, zero, zero // Top Index
daddu s3, zero, zero // Total Codes
addiu s6, zero, 1
daddu s7, zero, zero


// Count how many codes there are
addiu v0, zero, 512
addu a0, s0, zero

_CML_CountLoop:
beq v0, zero, :_CML_CountLoopExit
nop

lb v1, $0000(a0)
beq v1, zero, :_CML_CountLoopExit
nop

addiu s3, s3, 1
addiu v0, v0, -1
addiu a0, a0, $0020
beq zero, zero, :_CML_CountLoop
nop
_CML_CountLoopExit:

_CodesMenuLoop:

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, zero, zero // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 10   // x
addiu t2, zero, 20   // y
multu t2, t2, s1
addiu t2, t2, 95
daddu t3, zero, zero // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 620  // w
addiu t9, zero, 20   // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

// Text (Codes)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $462C
addiu t0, zero, 290
addiu t1, zero, 70
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addu s4, s0, zero
sll v0, s2, 5
addu s4, s4, v0
addu s5, zero, zero
_CML_DataLoop:

bne s1, s5, 2
nop
addu s7, s4, zero

addu v0, s2, s5
slt v0, v0, s3
beq v0, zero, :_CML_DataLoopExit
nop

addiu v0, zero, 17
slt v0, s5, v0
beq v0, zero, :_CML_DataLoopExit
nop

lb v0, $0000(s4)
beq v0, zero, :_CML_DataLoopExit
nop

addu a1, s4, zero
lw a0, $001C(k0)
addiu t0, zero, 230
addiu t1, zero, 20
multu t1, t1, s5
addiu t1, t1, 95
daddu t2, zero, zero

lb v0, $001C(s4)
bne v0, zero, 4
nop
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc

addiu v1, zero, 1
bne v0, v1, 4
nop
addiu t3, zero, $00
addiu t4, zero, $c0
addiu t5, zero, $00

addiu v1, zero, 2
bne v0, v1, 4
nop
addiu t3, zero, $f0
addiu t4, zero, $c0
addiu t5, zero, $00
addiu t6, zero, $0000
call _drawString

addiu s5, s5, 1
addiu s4, s4, $0020
beq zero, zero, :_CML_DataLoop
nop
_CML_DataLoopExit:

jal :vSync
nop

bne s6, zero, :_CodesMenuWaitNoInput
nop

//------------------------------ Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, :_CML_NotUp
nop
addiu s6, zero, 1
beq s1, zero, 4
nop
addiu s1, s1, -1
beq zero, zero, :_CodesMenuWaitNoInput
nop
beq s2, zero, :_CodesMenuWaitNoInput
nop
addiu s2, s2, -1
beq zero, zero, :_CodesMenuWaitNoInput
nop

_CML_NotUp:
//------------------------------ Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, :_CML_NotDown
nop
addiu s6, zero, 1
addiu v0, zero, 16
slt v0, s1, v0
beq v0, zero, 10
nop
addiu v0, s1, 1
sll v0, v0, 5
addu s4, s0, v0
lb v0, $0000(s4)
beq v0, zero, :_CodesMenuWaitNoInput
nop
addiu s1, s1, 1
beq zero, zero, :_CodesMenuWaitNoInput
nop
addu v0, s1, s2
addiu v0, v0, 1
slt v0, v0, s3
beq v0, zero, :_CodesMenuWaitNoInput
nop
addiu v0, s2, 1
sll v0, v0, 5
addu s4, s0, v0
lb v0, $0000(s4)
beq v0, zero, :_CodesMenuWaitNoInput
nop
addiu s2, s2, 1
beq zero, zero, :_CodesMenuWaitNoInput
nop



_CML_NotDown:
//------------------------------ Left
jal :_ReadPad
addiu a0, zero, $FF7F
beq v0, zero, :_CML_NotLeft
nop
addiu s6, zero, 1
addiu v0, zero, 17
daddu s1, zero, zero
subu v0, s2, v0
slt v0, v0, zero
bne v0, zero, 4
nop
addiu s2, s2, -17
beq zero, zero, :_CodesMenuWaitNoInput
nop
daddu s2, zero, zero
beq zero, zero, :_CodesMenuWaitNoInput
nop

_CML_NotLeft:
//------------------------------ Right
jal :_ReadPad
addiu a0, zero, $FFDF
beq v0, zero, :_CML_NotRight
nop
addiu s6, zero, 1
addiu v0, zero, 17
addiu s1, zero, 16
addu v0, s2, v0
slt v0, v0, s3
beq v0, zero, 4
nop
addiu s2, s2, 17
beq zero, zero, :_CodesMenuWaitNoInput
nop
addiu s2, s3, -17
slt v0, s2, zero
beq v0, zero, 3
nop
daddu s2, zero, zero
daddu s1, zero, zero
beq zero, zero, :_CodesMenuWaitNoInput
nop

_CML_NotRight:
//------------------------------ Cross
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_CML_NotCross
nop
addiu s6, zero, 1
lb v0, $001C(s7)
addiu v1, zero, 2
bne v0, v1, 2
nop
addiu v0, zero, 1
addiu v1, zero, 1
subu v0, v1, v0
sb v0, $001C(s7)
bne v0, zero, :_CodesMenuWaitNoInput
nop

lw a0, $0010(s7)
lw a1, $0018(s7)
srl v0, a0, 25
sll v0, v0, 25
subu a0, a0, v0
srl a2, v0, 24

bne a2, zero, 2
nop
sb a1, $0000(a0)
addiu v0, zero, $0010
bne a2, v0, 2
nop
sh a1, $0000(a0)
addiu v0, zero, $0020
bne a2, v0, 2
nop
sw a1, $0000(a0)

beq zero, zero, :_CodesMenuWaitNoInput
nop

_CML_NotCross:
//------------------------------ Square
jal :_ReadPad
addiu a0, zero, $7FFF
beq v0, zero, :_CML_NotSquare
nop

jal :_CodeDelete
daddu a0, s7, zero

beq zero, zero, :_CodesMenuWaitNoInput
nop

_CML_NotSquare:
//------------------------------ Triangle
jal :_ReadPad
addiu a0, zero, $EFFF
beq v0, zero, :_CodesMenuWaitNoInput
nop
goto _CodesMenuExit


_CodesMenuWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_CodesMenuExitToLoop
nop
addu s6, zero, zero


_CodesMenuExitToLoop:
jal :vSync
nop
beq zero, zero, :_CodesMenuLoop
nop

_CodesMenuExit:

lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_CodeDelete:
addiu sp, sp, $FFD0
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s5, $0020(sp)

daddu s0, a0, zero //Get address of to be deleted

jal :_CDMenu
nop

beq v0, zero, :_CDExit
nop
//Time to delete

sq zero, $0000(s0)
sq zero, $0010(s0)

//Now move everything after it back up
addiu t0, s0, $0020

_CDLoop:
lw t2, $0010(t0) //Load address that is to be moved

beq t2, zero, :_CDExit //If it is zero, then there is no code; exit
nop

lq t3, $0000(t0)
sq t3, $0000(s0)
lq t3, $0010(t0)
sq t3, $0010(s0)
sq zero, $0000(t0)
sq zero, $0010(t0)

addiu t0, t0, $0020
addiu s0, s0, $0020

beq zero, zero, :_CDLoop
nop


_CDExit:

lq ra, $0000(sp)
lq s0, $0010(sp)
lq s5, $0020(sp)
jr ra
addiu sp, sp, $0030

//==========================================================
_CDMenu:
addiu sp, sp, $FFD0
sq ra, $0000(sp)
sq s1, $0010(sp)
sq s0, $0020(sp)

jal :_ClearPad
nop

lui s1, $0008
add s5, zero, zero
daddu s6, zero, zero

_CDMenuLoop:
// Make a little box
lui t0, $0009        // Packet
addiu t1, zero, 150  // x
addiu t2, zero, 150  // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $040 // b
daddu t7, zero, zero // size
addiu t8, zero, 300  // w
addiu t9, zero, 100  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 160   // x
addiu t2, zero, 20   // y
multu t2, t2, s5
addiu t2, t2, 200
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 280  // w
addiu t9, zero, 20   // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

//Text "Ok"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $44DD // "Ok"
addiu t0, zero, 300
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Text "Cancel"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $44C8 // "Cancel"
addiu t0, zero, 280
addiu t1, zero, 220
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Text "Delete"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $44C0 // "Delete"
addiu t0, zero, 170
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Text Name of Code
lw a0, $001C(s1)
daddu a1, s0, zero // Pointer to name of code
addiu t0, zero, 260
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

jal :vSync
nop

// DC Menu Controls

bne s6, zero, :_CDMenuWaitNoInput
nop

//------------------------------ Cross
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_CD_NotCross
nop
addiu s6, zero, 1

bne zero, s5, 4 //Ok
nop
// Exit CDMenu and return 1
addiu v0, zero, $0001
beq zero, zero, :_CDMenuExit
nop

//addiu v0, zero, 1 //Cancel
beq v0, zero, :_CD_NotCross
nop
// Exit CDMenu and return 0
add v0, zero, zero
beq zero, zero, :_CDMenuExit
nop

_CD_NotCross:
//------------------------------ Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, :_CD_NotUp
nop
addiu s6, zero, $0001
beq s5, zero, 2
nop
addiu s5, s5, -1

_CD_NotUp:
//------------------------------ Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, :_CD_NotDown
nop
addiu t1, zero, $0001
addiu s6, zero, $0001
beq s5, t1, 2
nop
addiu s5, s5, 1

_CD_NotDown:

beq zero, zero, :_CDMenuLoop
nop

_CDMenuWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_CDMenuExitToLoop
nop
addu s6, zero, zero

_CDMenuExitToLoop:

beq zero, zero, :_CDMenuLoop
nop

_CDMenuExit:


lq ra, $0000(sp)
lq s1, $0010(sp)
lq s0, $0020(sp)
jr ra
addiu sp, sp, $0030

//==========================================================
_MemSearchMenu:

addiu sp, sp, $FF00
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)


lui v0, $8004
lw v1, $4440(v0)
sw v1, $0090(sp)    // Start
lw v1, $4444(v0)
sw v1, $0094(sp)    // Stop

lw v1, $4420(v0)
sw v1, $0098(sp)    // Value 1
lw v1, $4424(v0)
sw v1, $009C(sp)    // Value 2
lw v1, $4428(v0)
sw v1, $00A0(sp)    // Value 3
lw v1, $442C(v0)
sw v1, $00A4(sp)    // Value 4
/*
sw zero, $009C(sp)  // Value 2
sw zero, $00A0(sp)  // Value 3
sw zero, $00A4(sp)  // Value 4
*/
lui v1, $8004
lb v0, $4410(v1)
sb v0, $00AC(sp)    // Search Operator
lb v0, $4411(v1)
sb v0, $00AD(sp)    // Next Search: Op/Changed/Unchanged
lb v0, $4412(v1)
sb v0, $00A8(sp)    // Bits

/*
Bits
0 - 8 Bit
1 - 16 Bit
2 - 32 Bit
3 - 64 Bit
4 - 128 Bit
*/

/*
Search Operator
0 - =   Equal
1 - !=  Not Equal
2 - <   Less Than
3 - >   Greater Than
4 - <=  Less Than or Equal to
5 - >=  Greater Than or Equal to
*/

daddu s4, a0, zero
daddu k0, zero, zero // Current Operation
daddu k1, zero, zero // Selected Operation Index
_MemSearchMenuCleanOff:

jal :_ClearPad
nop


lui s0, $8004
addiu s0, s0, $6000
lui s1, $0008
daddu s5, zero, zero
addiu s6, zero, 1
_MemSearchMenuLoop:


// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop

// Selection Bar
lui t0, $0009        // Packet

addiu t1, zero, 12
multu t1, t1, k1
addiu t1, t1, 110
addiu v0, zero, 8
slt v0, k1, v0

// Value operator needed

bne k0, zero, 2
nop
addiu t1, zero, 10   // x

addiu t2, zero, 20   // y
multu t2, t2, s5
addiu t2, t2, 95
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size

addiu t8, zero, 12

bne k0, zero, 2
nop
addiu t8, zero, 620  // w


addiu t9, zero, 20  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160



// Text (Search)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4794
addiu t0, zero, 278
addiu t1, zero, 70
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu s2, zero, -1
daddu s3, s0, zero
_MemSearchMenuListingLoop:

//Saving Start, Stop, and Value
lui v0, $8004
lw v1, $0090(sp)
sw v1, $4440(v0)
lw v1, $0094(sp)
sw v1, $4444(v0)

lw v1, $0098(sp)
sw v1, $4420(v0)
lw v1, $009C(sp)
sw v1, $4424(v0)
lw v1, $00A0(sp)
sw v1, $4428(v0)
lw v1, $00A4(sp)
sw v1, $442C(v0)

lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $479C // Start:
addiu t0, zero, 26
addiu t1, zero, 95
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu a0, sp, $0090
lw a1, $0018(s1)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(s1)
lw a0, $001C(s1)
addiu t0, zero, 110
addiu t1, zero, 95
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString






lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $47A4 // Stop:
addiu t0, zero, 26
addiu t1, zero, 115
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu a0, sp, $0094
lw a1, $0018(s1)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(s1)
lw a0, $001C(s1)
addiu t0, zero, 110
addiu t1, zero, 115
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString




lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $47Ac // Value:
addiu t0, zero, 26
addiu t1, zero, 135
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lw s7, $0018(s1)

addiu a0, sp, $00A4
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu s7, s7, 8

addiu a0, sp, $00A0
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu s7, s7, 8

addiu a0, sp, $009C
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu s7, s7, 8

addiu a0, sp, $0098
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

msmValueSkip:
lw a1, $0018(s1)
lw a0, $001C(s1)
addiu t0, zero, 110
addiu t1, zero, 135
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString




lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $47B4 // Bits:
addiu t0, zero, 26
addiu t1, zero, 155
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a1, $8004
lb v0, $00A8(sp)

bne v0, zero, 2
nop
addiu a1, a1, $47BC // 8

addiu v1, zero, 1
bne v0, v1, 2
nop
addiu a1, a1, $47BE // 16

addiu v1, zero, 2
bne v0, v1, 2
nop
addiu a1, a1, $47C1 // 32

addiu v1, zero, 3
bne v0, v1, 2
nop
addiu a1, a1, $47C4 // 64

addiu v1, zero, 4
bne v0, v1, 2
nop
addiu a1, a1, $47C7 // 128

lw a0, $001C(s1)
addiu t0, zero, 110
addiu t1, zero, 155
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString



lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $44E0 // Op:
addiu t0, zero, 62
addiu t1, zero, 175
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a1, $8004
lb v0, $00AC(sp)
bne v0, zero, 2
nop
addiu a1, a1, $44E4 // =

addiu v1, zero, 1
bne v0, v1, 2
nop
addiu a1, a1, $44E6 // !=

addiu v1, zero, 2
bne v0, v1, 2
nop
addiu a1, a1, $44E9 // <

addiu v1, zero, 3
bne v0, v1, 2
nop
addiu a1, a1, $44EB // >

addiu v1, zero, 4
bne v0, v1, 2
nop
addiu a1, a1, $44ED // <=

addiu v1, zero, 5
bne v0, v1, 2
nop
addiu a1, a1, $44F0 // >=

lw a0, $001C(s1)
addiu t0, zero, 110
addiu t1, zero, 175
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Text "Next Search:"
lui a1, $8004
addiu a1, a1, $4840 // Next Search:
lw a0, $001C(s1)
addiu t0, zero, 20
addiu t1, zero, 195
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a1, $8004
lb v0, $00AD(sp)
bne v0, zero, 2
nop
addiu a1, a1, $4850 // Operator

addiu v1, zero, 1
bne v0, v1, 2
nop
addiu a1, a1, $485C //Changed

addiu v1, zero, 2
bne v0, v1, 2
nop
addiu a1, a1, $4864 // Unchanged

lw a0, $001C(s1)
addiu t0, zero, 170
addiu t1, zero, 195
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a1, $8004
addiu a1, a1, $4794 // Search
lw a0, $001C(s1)
addiu t0, zero, 278
addiu t1, zero, 215
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a1, $8004
addiu a1, a1, $44F4 // Next Search
lw a0, $001C(s1)
addiu t0, zero, 278
addiu t1, zero, 235
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a1, $8004
addiu a1, a1, $47CC // Results
lw a0, $001C(s1)
addiu t0, zero, 278
addiu t1, zero, 255
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a1, $8004
addiu a1, a1, $47D4 // Go Back
lw a0, $001C(s1)
addiu t0, zero, 278
addiu t1, zero, 275
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu s2, s2, 1


/*
addiu a0, s3, $0000
lw a1, $0018(s1)
jal :_lineToHex
addiu a2, zero, 8

addiu v0, zero, $0020
lw a1, $0018(s1)
sb v0, $0008(a1)
addiu a1, a1, $0009

addiu a0, s3, $0004
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(s1)
lw a0, $001C(s1)
addiu t0, zero, 218
addiu t1, zero, 20
multu t1, t1, s2
addiu t1, t1, 95
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
call _drawString
*/

addiu s3, s3, 12

slti v0, s2, 9
bne v0, zero, :_MemSearchMenuListingLoop
nop

_MemSearchMenuListingLoopExit:

bltz s2, :_MemSearchMenuExit
nop

jal :vSync
nop

// Browser Controls

bne s6, zero, :_MemSearchMenuWaitNoInput
nop
//------------------------------ Exit
//jal :_ReadPad
//addiu a0, zero, $DFFF
//bne v0, zero, :_MemSearchMenuExit
//nop

//------------------------------ Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, :msmbrowsenotUp
nop
addiu s6, zero, 1
bne k0, zero, 14
nop
sltiu v0, s5, $0001
bne v0, zero, 4
nop
addiu s5, s5, -1
beq zero, zero, :msmbrowsenotUp
nop
lui v0, $8004
addiu v0, v0, $6000
beq s0, v0, 2
nop
addiu s0, s0, -12
beq zero, zero, :_MemSearchMenuWaitNoInput
nop

addiu v0, zero, 3
beq v0, k0, :msmbrowseValUp
nop
addiu v0, zero, 7
subu v0, v0, k1

addiu v1, zero, 1

bltz v0, 6
nop

beq v0, zero, 4
nop
addiu v0, v0, -1
bne v0, zero, -2
sll v1, v1, 4

addu v0, v1, zero

addiu v1, k0, -1
sll v1, v1, 2
addiu v1, v1, $0090
addu v1, v1, sp
lw a0, $0000(v1)
addu a0, a0, v0
sw a0, $0000(v1)
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
msmbrowseValUp:

addiu a1, zero, $0090
addiu v0, zero, 31
subu v0, v0, k1

addiu a2, zero, 8
slt v1, v0, a2
bne v1, zero, 5
nop
addiu a1, a1, 4
addiu v0, v0, -8
beq v1, zero, -6
nop

addiu v1, zero, 1

bltz v0, 6
nop

beq v0, zero, 4
nop
addiu v0, v0, -1
bne v0, zero, -2
sll v1, v1, 4

addu v0, v1, zero

addiu v1, k0, -1
sll v1, v1, 2
addu v1, v1, a1
addu v1, v1, sp
lw a0, $0000(v1)
addu a0, a0, v0
sw a0, $0000(v1)


msmbrowsenotUp:


//------------------------------ Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, :msmbrowsenotDown
nop
addiu s6, zero, 1
bne k0, zero, 18
nop
slt v0, s5, s2
beq v0, zero, 4
nop
addiu s5, s5, 1
beq zero, zero, :msmbrowsenotDown
nop
addu s3, s0, zero
addiu v0, zero, 12
multu v0, v0, s5
addu s3, s3, v0
addiu s3, s3, 12
lw v0, $0000(s3)
beq v0, zero, 2
nop
addiu s0, s0, 12
beq zero, zero, :_MemSearchMenuWaitNoInput
nop

addiu v0, zero, 3
beq v0, k0, :msmbrowseValDown
nop
addiu v0, zero, 7
subu v0, v0, k1

lui v1, $8004
sq v0, $4420(v1)

addiu v1, zero, 1

bltz v0, 6
nop

beq v0, zero, 4
nop
addiu v0, v0, -1
bne v0, zero, -2
sll v1, v1, 4

addu v0, v1, zero

addiu v1, k0, -1
sll v1, v1, 2
addiu v1, v1, $0090
addu v1, v1, sp
lw a0, $0000(v1)
subu a0, a0, v0
sw a0, $0000(v1)
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
msmbrowseValDown:

addiu a1, zero, $0090
addiu v0, zero, 31
subu v0, v0, k1

addiu a2, zero, 8
slt v1, v0, a2
bne v1, zero, 5
nop
addiu a1, a1, 4
addiu v0, v0, -8
beq v1, zero, -6
nop

addiu v1, zero, 1

bltz v0, 6
nop

beq v0, zero, 4
nop
addiu v0, v0, -1
bne v0, zero, -2
sll v1, v1, 4

addu v0, v1, zero

addiu v1, k0, -1
sll v1, v1, 2
addu v1, v1, a1
addu v1, v1, sp
lw a0, $0000(v1)
subu a0, a0, v0
sw a0, $0000(v1)

msmbrowsenotDown:

//------------------------------ Left
jal :_ReadPad
addiu a0, zero, $FF7F
beq v0, zero, :msmbrowsenotLeft
nop
addiu s6, zero, 1
bne k0, zero, :_SkipMWNILeft
nop
//Bit value
addiu v0, zero, 3
bne v0, s5, :_SearchOpLeft
nop
lw v0, $00A8(sp)
beq v0, zero, 5
nop
addiu v0, v0, -1
sw v0, $00A8(sp)
lui v1, $8004
sb v0, $4412(v1) //Store for bit storage
beq zero, zero, :_MemSearchMWNILeft
nop

_SearchOpLeft:
//Operator
addiu v0, zero, 4
bne v0, s5, 8
nop
lb v0, $00AC(sp)
beq v0, zero, 3
nop
addiu v0, v0, -1
sb v0, $00AC(sp)
lui v1, $8004
sb v0, $4410(v1) //For search to read when searching

//Next Search
addiu v0, zero, 5
bne v0, s5, :_MemSearchMWNILeft
nop
lb v0, $00AD(sp)
beq v0, zero, 3
nop
addiu v0, v0, -1
sb v0, $00AD(sp)
lui v1, $8004
sb v0, $4411(v1) //For search to read when searching
_MemSearchMWNILeft:
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
_SkipMWNILeft:
beq k1, zero, 2
nop
addiu k1, k1, -1

msmbrowsenotLeft:

//------------------------------ Right
jal :_ReadPad
addiu a0, zero, $FFDF
beq v0, zero, :msmbrowsenotRight
nop
addiu s6, zero, 1
bne k0, zero, :_SkipMWNIRight
nop
//Bit value
addiu v0, zero, 3
bne v0, s5, :_SearchOpRight
nop
lw v0, $00A8(sp)
addiu v1, zero, 4
beq v0, v1, 5
nop
addiu v0, v0, 1
sw v0, $00A8(sp)
lui v1, $8004
sb v0, $4412(v1) //Store for bit storage
beq zero, zero, :_MemSearchMWNIRight
nop

_SearchOpRight:
//Operator
addiu v0, zero, 4
bne v0, s5, 9
nop
lb v0, $00AC(sp)
addiu v1, zero, 5
beq v0, v1, 3
nop
addiu v0, v0, 1
sb v0, $00AC(sp)
lui v1, $8004
sb v0, $4410(v1) //For search to read when searching

//Next Search:
addiu v0, zero, 5
bne v0, s5, :_MemSearchMWNIRight
nop
lb v0, $00AD(sp)
addiu v1, zero, 2
beq v0, v1, 3
nop
addiu v0, v0, 1
sb v0, $00AD(sp)
lui v1, $8004
sb v0, $4411(v1) //For search to read when searching

_MemSearchMWNIRight:
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
_SkipMWNIRight:
addiu v0, zero, 3
bne k0, v0, 8
nop
addiu v0, zero, 31
slt v0, k1, v0
beq v0, zero, 2
nop
addiu k1, k1, 1
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
addiu v0, zero, 7
slt v0, k1, v0
beq v0, zero, 2
nop
addiu k1, k1, 1


msmbrowsenotRight:

//------------------------------ Triangle
jal :_ReadPad
addiu a0, zero, $EFFF
beq v0, zero, :msmbrowsenotTri
nop
daddu k0, zero, zero

msmbrowsenotTri:

//------------------------------ Start

// Reset Start, Stop, Value, and Op
jal :_ReadPad
addiu a0, zero, $FFF7
beq v0, zero, :msmbrowsenotStr
nop
addiu v0, zero, $0002
sw v0, $00A8(sp)
sw zero, $00AC(sp)
sd zero, $0098(sp)
sd zero, $00A0(sp)
lui v0, $0010
sw v0, $0090(sp)
sw v0, $0094(sp)

msmbrowsenotStr:

//------------------------------ Select
//Sets horizontal marker to its max
jal :_ReadPad
addiu a0, zero, $FFFE
beq v0, zero, :msmbrowsenotSel
nop

addiu v0, zero, $0001
bne k0, v0, 2
nop
addiu k1, zero, $0007

addiu v0, zero, $0003
bne k0, v0, 2
nop
addiu k1, zero, $001F

addiu v0, zero, $0002
bne k0, v0, 2
nop
addiu k1, zero, $0007

msmbrowsenotSel:

//------------------------------ Cross
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_MemSearchMenuWaitNoInput
nop
addiu s6, zero, 1
addiu v0, zero, 3
slt v0, s5, v0
beq v0, zero, 5
nop
addiu k0, s5, 1
addu k1, zero, zero
beq zero, zero, :_MemSearchMenuWaitNoInput
nop

addiu v0, zero, 6
bne v0, s5, 5
nop
jal :_MemSearchProcess // jal -> Search
daddiu a0, sp, $0090
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
addiu v0, zero, 7
bne v0, s5, :_AfterNextSearch
nop

lui t0, $8004
lb t0, $4411(t0)

bne zero, t0, 5
nop

jal :_MemReSearchProcess // jal -> Next Search
daddiu a0, sp, $0090

beq zero, zero, 3
nop

jal :_MemChangedSearch // jal -> Next Search -> Changed or Unchanged
daddiu a0, sp, $0090

beq zero, zero, :_MemSearchMenuWaitNoInput
nop
_AfterNextSearch:
addiu v0, zero, 8
bne v0, s5, 5
nop
jal :_MemSearchResults // jal -> Results
nop
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
addiu v0, zero, 9
bne v0, s5, 3
nop
beq zero, zero, :_MemSearchMenuExit
nop


_MemSearchMenuWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_MemSearchMenuExitToLoop
nop
addu s6, zero, zero

_MemSearchMenuExitToLoop:

jal :vSync
nop
goto _MemSearchMenuLoop

_MemSearchMenuExit:

lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $0100

//==========================================================
_MemSearchProcess:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)
sq zero, $0080(sp)

lui v0, $000A
lui v1, $0008
sw v0, $43FC(v1)

addu s0, a0, zero // Search Query
lw s1, $0000(s0) // Start
lw s2, $0004(s0) // Stop

lw v0, $0008(s0)
sw v0, $0080(sp)
lw v0, $000C(s0)
sw v0, $0084(sp)
lw v0, $0010(s0)
sw v0, $0088(sp)
lw v0, $0014(s0)
sw v0, $008C(sp)

lw s3, $0018(s0)

bne s3, zero, 2
nop
lb s4, $0080(sp)

addiu v0, zero, 1
bne s3, v0, 2
nop
lh s4, $0080(sp)

addiu v0, zero, 2
bne s3, v0, 2
nop
lw s4, $0080(sp)

addiu v0, zero, 3
bne s3, v0, 2
nop
ld s4, $0080(sp)

addiu v0, zero, 4
bne s3, v0, 2
nop
lq s4, $0080(sp)

lui s5, $8004        // Results Storage
ori s5, s5, $B020
daddu s6, zero, zero // Results Count

sw s1, $0000(s5)
sw s2, $0004(s5)
sw s3, $0008(s5)
sw zero, $000C(s5)
sq s4, $0010(s5)
// 8004B040

_MemSearchProcessLoop:

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, zero, zero   // z
addiu t4, zero, $000 // r
addiu t5, zero, $000 // g
addiu t6, zero, $000 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 80

// Text (Searching...)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $47DC
addiu t0, zero, 248
addiu t1, zero, 70
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui k0, $0008
sw s1, $0090(sp)
sw s2, $0094(sp)
sw s6, $0098(sp)

// %08X / %08X -----------

lw s7, $0018(k0)

addiu a0, sp, $0090
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu t0, zero, $0020
addiu t1, zero, $002F
sb t0, $0009(s7)
sb t1, $000A(s7)
sb t0, $000B(s7)

addiu s7, s7, 10

addiu a0, sp, $0094
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 206
addiu t1, zero, 135
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

//------------------------

addiu a0, sp, $0098
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 316
addiu t1, zero, 165
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

addiu s7, zero, $2000
_MemSearchMiniLoop:

//-------------------- Compare
bne s3, zero, 4
nop
addu v0, s1, zero
lb v1, $0000(s1)
addiu s1, s1, 1

addiu t0, zero, 1
bne t0, s3, 4
nop
addu v0, s1, zero
lh v1, $0000(s1)
addiu s1, s1, 2

addiu t0, zero, 2
bne t0, s3, 4
nop
addu v0, s1, zero
lw v1, $0000(s1)
addiu s1, s1, 4

addiu t0, zero, 3
bne t0, s3, 4
nop
addu v0, s1, zero
ld v1, $0000(s1)
addiu s1, s1, 8

addiu t0, zero, 4
bne t0, s3, 4
nop
addu v0, s1, zero
lq v1, $0000(s1)
addiu s1, s1, 16

//-------------------- Mark Result

lui t2, $8004

//Determine what operation to perform (=, !=, <, >, <=, >=)
lb t0, $4410(t2) //Load op #

bne t0, zero, 3 // =
nop
beq s4, v1, :_PerformMarking
nop

addiu t1, zero, $0001 // !=
bne t0, t1, 3
nop
bne s4, v1, :_PerformMarking
nop

addiu t1, zero, $0002 // <
bne t0, t1, 4
nop
slt t1, v1, s4
bne t1, zero, :_PerformMarking
nop

addiu t1, zero, $0003 // >
bne t0, t1, 4
nop
slt t1, s4, v1
bne t1, zero, :_PerformMarking
nop

addiu t1, zero, $0004 // <=
bne t0, t1, 4
nop
slt t1, s4, v1
beq t1, zero, :_PerformMarking
nop

addiu t1, zero, $0005 // >=
bne t0, t1, :_AfterMarking
nop
slt t1, v1, s4
beq t1, zero, :_PerformMarking
nop

beq zero, zero, :_AfterMarking //If nothing works skip
nop

_PerformMarking:
sw v0, $0020(s5)
lui t0, $0008
lw at, $43FC(t0)
sw v0, $0000(at) //Store address for changed/unchanged
sw v1, $0004(at) //Store value for changed/unchanged

addiu at, at, 8
sw at, $43FC(t0)
addiu s6, s6, 1
addiu s5, s5, 4


_AfterMarking:

slt v0, s1, s2
beq v0, zero, :_MemSearchProcessComplete
nop

addiu v0, zero, 5100
slt v0, s6, v0
beq v0, zero, :_MemSearchProcessComplete
nop

bne s7, zero, :_MemSearchMiniLoop
addiu s7, s7, -1

jal :vSync
nop

//------------------------------ Circle (Force Stop Scan)
jal :_ReadPad
addiu a0, zero, $DFFF
bne v0, zero, :_MemSearchProcessLeave
nop

beq zero, zero, :_MemSearchProcessLoop
nop
_MemSearchProcessComplete:

sw zero, $0020(s5)
lui s5, $8005
sw s6, $B02C(s5)

//-------------------------------------

addiu s6, zero, 1
_MemSearchProcessWaitQuitLoop:

lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, zero, zero   // z
addiu t4, zero, $000 // r
addiu t5, zero, $000 // g
addiu t6, zero, $000 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 80

// Text (Search process completed.)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $47EC
addiu t0, zero, 170
addiu t1, zero, 70
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Total Results:)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4808
addiu t0, zero, 182
addiu t1, zero, 135
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu a0, s5, $B02C
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 362
addiu t1, zero, 135
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

// Text (Search Query:)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4830
addiu t0, zero, 242
addiu t1, zero, 165
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Start
addu a0, s0, zero
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 110
addiu t1, zero, 185
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

// Stop
addiu a0, s0, $0004
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 110
addiu t1, zero, 205
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

// Value
lw s7, $0018(k0)

addiu a0, s0, $0014
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu s7, s7, 8

addiu a0, s0, $0010
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu s7, s7, 8

addiu a0, s0, $000C
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu s7, s7, 8

addiu a0, s0, $0008
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8


lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 110
addiu t1, zero, 225
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString


// Text (Press (X) To Continue.)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4818
addiu t0, zero, 188
addiu t1, zero, 250
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

jal :vSync
nop

bne s6, zero, :_MemSearchProcessWaitNoInput
nop

//------------------------------ Cross
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_MemSearchProcessWaitNoInput
nop
goto _MemSearchProcessLeave

_MemSearchProcessWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_MemSearchProcessExitToLoop
nop
addu s6, zero, zero


_MemSearchProcessExitToLoop:
jal :vSync
nop
beq zero, zero, :_MemSearchProcessWaitQuitLoop
nop

_MemSearchProcessLeave:
lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
//Next Search searching function by Dnawrkshp
//I copied the MemSearchProcess function that Gtlcpimp made
//and modified it to read from the results list at 8004B040
//Then it just searches through those addresses
_MemReSearchProcess:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)
sq zero, $0080(sp)

lui v0, $000A
lui v1, $0008
sw v0, $43FC(v1)

addu s0, a0, zero // Search Query
lw s1, $0000(s0) // Start
lw s2, $0004(s0) // Stop

lw v0, $0008(s0)
sw v0, $0080(sp)
lw v0, $000C(s0)
sw v0, $0084(sp)
lw v0, $0010(s0)
sw v0, $0088(sp)
lw v0, $0014(s0)
sw v0, $008C(sp)

lw s3, $0018(s0)

bne s3, zero, 2
nop
lb s4, $0080(sp)

addiu v0, zero, 1
bne s3, v0, 2
nop
lh s4, $0080(sp)

addiu v0, zero, 2
bne s3, v0, 2
nop
lw s4, $0080(sp)

addiu v0, zero, 3
bne s3, v0, 2
nop
ld s4, $0080(sp)

addiu v0, zero, 4
bne s3, v0, 2
nop
lq s4, $0080(sp)

lui s5, $8004        // Results Storage
ori s5, s5, $B020
daddu s6, zero, zero // Results Count

sw s1, $0000(s5)
sw s2, $0004(s5)
sw s3, $0008(s5)
sw zero, $000C(s5)
sq s4, $0010(s5)
// 8004B040

//Results addressing starts at 8004B040
//What to search for is at 8004B030
lui s1, $8005
addiu s1, s1, $B040 //8004B040

_MemReSearchProcessLoop:

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, zero, zero   // z
addiu t4, zero, $000 // r
addiu t5, zero, $000 // g
addiu t6, zero, $000 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 80

// Text (Searching...)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $47DC
addiu t0, zero, 248
addiu t1, zero, 70
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui k0, $0008
sw s1, $0090(sp)
sw s2, $0094(sp)
sw s6, $0098(sp)

// %08X / %08X -----------

lw s7, $0018(k0)

addiu a0, sp, $0090
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu t0, zero, $0020
addiu t1, zero, $002F
sb t0, $0009(s7)
sb t1, $000A(s7)
sb t0, $000B(s7)

addiu s7, s7, 10

addiu a0, sp, $0094
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 206
addiu t1, zero, 135
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

//------------------------

addiu a0, sp, $0098
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 316
addiu t1, zero, 165
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

addiu s7, zero, $2000

_MemReSearchMiniLoop:

//-------------------- Compare
lw v1, $0000(s1) //An address of a result
beq v1, zero, :_MemReSearchProcessComplete //If address is 00000000 then exit
nop

bne s3, zero, 4
nop
addu v0, s1, zero
lb v1, $0000(v1)
addiu s1, s1, 4

addiu t0, zero, 1
bne t0, s3, 4
nop
addu v0, s1, zero
lh v1, $0000(v1)
addiu s1, s1, 4

addiu t0, zero, 2
bne t0, s3, 4
nop
addu v0, s1, zero
lw v1, $0000(v1)
addiu s1, s1, 4

addiu t0, zero, 3
bne t0, s3, 4
nop
addu v0, s1, zero
ld v1, $0000(v1)
addiu s1, s1, 4

addiu t0, zero, 4
bne t0, s3, 4
nop
addu v0, s1, zero
lq v1, $0000(v1)
addiu s1, s1, 4


//-------------------- Mark Result
// - Operator
//Determine what operation to perform (=, !=, <, >, <=, >=)
lui t2, $8004
lb t0, $4410(t2) //Load op #

bne t0, zero, 3 // =
nop
beq s4, v1, :_PerformMarking2
nop

addiu t1, zero, $0001 // !=
bne t0, t1, 3
nop
bne s4, v1, :_PerformMarking2
nop

addiu t1, zero, $0002 // <
bne t0, t1, 4
nop
slt t1, v1, s4
bne t1, zero, :_PerformMarking2
nop

addiu t1, zero, $0003 // >
bne t0, t1, 4
nop
slt t1, s4, v1
bne t1, zero, :_PerformMarking2
nop

addiu t1, zero, $0004 // <=
bne t0, t1, 4
nop
slt t1, s4, v1
beq t1, zero, :_PerformMarking2
nop

addiu t1, zero, $0005 // >=
bne t0, t1, :_AfterMarking2
nop
slt t1, v1, s4
beq t1, zero, :_PerformMarking2
nop

beq zero, zero, :_AfterMarking2 //If nothing works skip
nop

_PerformMarking2:
lw v0, $0000(v0)
sw v0, $0020(s5)
lui t0, $0008
lw at, $43FC(t0)
sw v0, $0000(at) //Store address for changed/unchanged
sw v1, $0004(at) //Store value for changed/unchanged

addiu s6, s6, 1
addiu s5, s5, 4
addiu at, at, 8
sw at, $43FC(t0)

_AfterMarking2:
addiu v0, zero, 5100
slt v0, s6, v0
beq v0, zero, :_MemReSearchProcessComplete
nop

bne s7, zero, :_MemReSearchMiniLoop
addiu s7, s7, -1

jal :vSync
nop

//------------------------------ Circle (Force Stop Scan)
jal :_ReadPad
addiu a0, zero, $DFFF
bne v0, zero, :_MemReSearchProcessLeave
nop

beq zero, zero, :_MemReSearchProcessLoop
nop
_MemReSearchProcessComplete:

sw zero, $0020(s5)
lui s5, $8005
sw s6, $B02C(s5)

//-------------------------------------

addiu s6, zero, 1
_MemReSearchProcessWaitQuitLoop:

lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, zero, zero   // z
addiu t4, zero, $000 // r
addiu t5, zero, $000 // g
addiu t6, zero, $000 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 80

// Text (Search process completed.)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $47EC
addiu t0, zero, 170
addiu t1, zero, 70
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Total Results:)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4808
addiu t0, zero, 182
addiu t1, zero, 135
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu a0, s5, $B02C
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 362
addiu t1, zero, 135
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

// Text (Search Query:)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4830
addiu t0, zero, 242
addiu t1, zero, 165
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Start
addu a0, s0, zero
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 110
addiu t1, zero, 185
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

// Stop
addiu a0, s0, $0004
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 110
addiu t1, zero, 205
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

// Value
lw s7, $0018(k0)

addiu a0, s0, $0014
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu s7, s7, 8

addiu a0, s0, $0010
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu s7, s7, 8

addiu a0, s0, $000C
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8

addiu s7, s7, 8

addiu a0, s0, $0008
addu a1, s7, zero
jal :_lineToHex
addiu a2, zero, 8


lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 110
addiu t1, zero, 225
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString


// Text (Press (X) To Continue.)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4818
addiu t0, zero, 188
addiu t1, zero, 250
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

jal :vSync
nop

bne s6, zero, :_MemReSearchProcessWaitNoInput
nop

//------------------------------ Cross
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_MemReSearchProcessWaitNoInput
nop
goto _MemReSearchProcessLeave

_MemReSearchProcessWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_MemReSearchProcessExitToLoop
nop
addu s6, zero, zero


_MemReSearchProcessExitToLoop:
jal :vSync
nop
beq zero, zero, :_MemReSearchProcessWaitQuitLoop
nop

_MemReSearchProcessLeave:
lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_MemChangedSearch:
//The idea is loop through the address + values stored (by the search functions) at 00084400
//Load the value from the address and compare it with the value after the address (+4)
//If equal the no change, if not equal then change
// - Dnawrkshp

// - Changed and Unchanged
addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)
sq zero, $0080(sp)

addu s0, a0, zero

lui s5, $8004        // Results Storage
ori s5, s5, $B020
daddu s6, zero, zero // Results Count

lui at, $000A
lui t0, $0008
sw at, $43FC(t0) //Location of pointer to current stored result
sw at, $43F8(t0) //Location of pointer to current place to store result to
//sw zero, $43F8(v0) //Offset

//v1 = Value
//t1 = previous search value
_BigLoop:
lui v0, $8004
lb s1, $4411(v0) //Setting, 1 = Changed, 2 = Unchanged, 0 = Operator (don't handle)
lb s3, $4412(v0) //Bit number, 0 = 8 bit, 1 = 16 bit, 2 = 32 bit, 3 = 64 bit (not supported), 4 = 128 bit (not supported)

addiu t4, zero, $0003
slt t4, s3, t4

beq t4, zero, :_ReSearchChangedProcessComplete //Bit size not supported; exit
nop

addiu t0, zero, $0001
beq t0, s1, :_FindChange
nop

addiu t0, zero, $0002
beq t0, s1, :_FindUnChange
nop

beq zero, zero, :_ReSearchChangedProcessComplete //Else
nop

_FindChange:

_ChangeLoop:
lui t0, $0008
lw at, $43FC(t0)
lw v0, $0000(at) //Address

beq v0, zero, :_ReSearchChangedProcessComplete //If address is 00000000 then exit
nop

//Load new and old value according to bit size
bne s3, zero, 3
nop

lb t1, $0004(at) //Value - old
lb v1, $0000(v0) //Value - current

addiu t4, zero, $0001
bne s3, t4, 3
nop

lh t1, $0004(at) //Value - old
lh v1, $0000(v0) //Value - current

addiu t4, zero, $0002
bne s3, t4, 3
nop

lw t1, $0004(at) //Value - old
lw v1, $0000(v0) //Value - current

beq v1, t1, :_AfterMarking3
nop

beq zero, zero, :_PerformMarking3
//beq zero, zero, :_AfterMarking3
nop

// - Unchanged
_FindUnChange:

_UnChangeLoop:
lui t0, $0008
lw at, $43FC(t0)
lw v0, $0000(at) //Address

beq v0, zero, :_ReSearchChangedProcessComplete //If address is 00000000 then exit
nop

//Load new and old value according to bit size
bne s3, zero, 3
nop

lb t1, $0004(at) //Value - old
lb v1, $0000(v0) //Value - current

addiu t4, zero, $0001
bne s3, t4, 3
nop

lh t1, $0004(at) //Value - old
lh v1, $0000(v0) //Value - current

addiu t4, zero, $0002
bne s3, t4, 3
nop

lw t1, $0004(at) //Value - old
lw v1, $0000(v0) //Value - current

bne v1, t1, :_AfterMarking3
nop

beq zero, zero, :_PerformMarking3
nop

_PerformMarking3:
lui t0, $0008
lw at, $43FC(t0)
lw t1, $0004(at) //Value - old
lw v0, $0000(at) //Address

//Load according to bit size
bne s3, zero, 2
nop
lb v1, $0000(v0) //Value - current

addiu t4, zero, $0001
bne s3, t4, 2
nop
lh v1, $0000(v0) //Value - current

addiu t4, zero, $0002
bne s3, t4, 2
nop
lw v1, $0000(v0) //Value - current

sw v0, $0020(s5)

lw at, $43F8(t0) //Load storing result pointer
sw v0, $0000(at) //Store address for changed/unchanged
sw v1, $0004(at) //Store value for changed/unchanged

addiu at, at, 8
sw at, $43F8(t0)

addiu s6, s6, 1
addiu s5, s5, 4

_AfterMarking3:
lui t0, $0008
lw at, $43FC(t0)
addiu at, at, $0008
sw at, $43FC(t0)

lui t0, $0009 //Max address
addiu v0, t0, $E364
slt v0, t9, v0
beq v0, zero, :_ReSearchChangedProcessComplete
nop

beq zero, zero, :_BigLoop
nop

_ReSearchChangedProcessComplete:
//sq zero, $0020(s5)
lui s5, $8005
sw s6, $B02C(s5)
lui t0, $0008
lw at, $43F8(t0)
sd zero, $0000(at) //Nop after the result so that when you do another search for change/unchange it doesn't read old stuff
addiu s5, s5, $B040
addiu t0, zero, 4
multu s6, s6, t0
add s5, s5, s6
sw zero, $0000(s5)

jal :_ClearPad
nop

lui s1, $0008
add s5, zero, zero
daddiu s6, zero, 1

_ChangeMenuLoop:
lui k0, $0008
// Make a little box
lui t0, $0009        // Packet
addiu t1, zero, 150  // x
addiu t2, zero, 150  // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $040 // b
daddu t7, zero, zero // size
addiu t8, zero, 300  // w
addiu t9, zero, 100  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

// Text (Total Results:)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4808
addiu t0, zero, 160
addiu t1, zero, 160
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui s5, $8005
addiu a0, s5, $B02C
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 320
addiu t1, zero, 160
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

// Text (Press (X) To Continue.)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4818
addiu t0, zero, 170
addiu t1, zero, 220
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

jal :vSync
nop

bne s6, zero, :_ReSearchChangeProcessWaitNoInput
nop

//------------------------------ Cross
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_ReSearchChangeProcessWaitNoInput
nop
bne s6, zero, :_ReSearchChangeProcessWaitNoInput
nop
beq zero, zero, :_ReSearchChangedProcessLeave
nop

_ReSearchChangeProcessWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_ReSearchChangeProcessExitToLoop
nop
addu s6, zero, zero


_ReSearchChangeProcessExitToLoop:

beq zero, zero, :_ChangeMenuLoop
nop

_ReSearchChangedProcessLeave:
lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_MemSearchResults:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

lui k0, $0008

lui s0, $8004
ori s0, s0, $B020

daddu s1, zero, zero // Select Index
daddu s2, zero, zero // Top Index
lw s3, $000C(s0)     // Total Results
addiu s6, zero, 1
daddu s7, zero, zero
_MemSearchResultsLoop:

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, zero, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 10   // x
addiu t2, zero, 20   // y
multu t2, t2, s1
addiu t2, t2, 95
daddu t3, zero, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 620  // w
addiu t9, zero, 20  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

// Text (Total Results:)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4808
addiu t0, zero, 182
addiu t1, zero, 70
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a0, $8004
ori a0, a0, $B02C
lw a1, $0018(k0)
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 362
addiu t1, zero, 70
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

addiu s4, s0, $0020
sll v0, s2, 2
addu s4, s4, v0
addu s5, zero, zero
_MSR_DataLoop:

bne s1, s5, 2
nop
lw s7, $0000(s4)

addu v0, s2, s5
slt v0, v0, s3
beq v0, zero, :_MSR_DataLoopExit
nop

addiu v0, zero, 17
slt v0, s5, v0
beq v0, zero, :_MSR_DataLoopExit
nop

lw a1, $0018(k0)
addu a0, s4, zero
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(k0)
lw a0, $001C(k0)
addiu t0, zero, 272
addiu t1, zero, 20
multu t1, t1, s5
addiu t1, t1, 95
daddu t2, zero, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

addiu s5, s5, 1
addiu s4, s4, 4
beq zero, zero, :_MSR_DataLoop
nop
_MSR_DataLoopExit:

jal :vSync
nop

bne s6, zero, :_MemSearchResultsWaitNoInput
nop

//------------------------------ Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, :_MSR_NotUp
nop
addiu s6, zero, 1
beq s1, zero, 4
nop
addiu s1, s1, -1
beq zero, zero, :_MemSearchResultsWaitNoInput
nop
beq s2, zero, :_MemSearchResultsWaitNoInput
nop
addiu s2, s2, -1
beq zero, zero, :_MemSearchResultsWaitNoInput
nop

_MSR_NotUp:
//------------------------------ Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, :_MSR_NotDown
nop
addiu s6, zero, 1
addiu v0, zero, 16
slt v0, s1, v0
beq v0, zero, 4
nop
addiu s1, s1, 1
beq zero, zero, :_MemSearchResultsWaitNoInput
nop
addu v0, s1, s2
slt v0, v0, s3
beq v0, zero, :_MemSearchResultsWaitNoInput
nop
addiu s2, s2, 1
beq zero, zero, :_MemSearchResultsWaitNoInput
nop



_MSR_NotDown:
//------------------------------ Left
jal :_ReadPad
addiu a0, zero, $FF7F
beq v0, zero, :_MSR_NotLeft
nop
addiu s6, zero, 1
addiu v0, zero, 17
daddu s1, zero, zero
subu v0, s2, v0
slt v0, v0, zero
bne v0, zero, 4
nop
addiu s2, s2, -17
beq zero, zero, :_MemSearchResultsWaitNoInput
nop
daddu s2, zero, zero
beq zero, zero, :_MemSearchResultsWaitNoInput
nop

_MSR_NotLeft:
//------------------------------ Right
jal :_ReadPad
addiu a0, zero, $FFDF
beq v0, zero, :_MSR_NotRight
nop
addiu s6, zero, 1
addiu v0, zero, 17
addiu s1, zero, 16
addu v0, s2, v0
slt v0, v0, s3
beq v0, zero, 4
nop
addiu s2, s2, 17
beq zero, zero, :_MemSearchResultsWaitNoInput
nop
addiu s2, s3, -17
slt v0, s2, zero
beq v0, zero, 3
nop
daddu s2, zero, zero
daddu s1, zero, zero
beq zero, zero, :_MemSearchResultsWaitNoInput
nop

_MSR_NotRight:
//------------------------------ R2
jal :_ReadPad
addiu a0, zero, $FDFF
beq v0, zero, :_MSR_NotR2
nop
addiu s6, zero, 1
beq s7, zero, :_MemSearchResultsWaitNoInput
nop

addiu sp, sp, $FFF0
sq ra, $0000(sp)
jal :_ProcessAddress
daddu a0, s7, zero
daddu s7, v0, zero
lq ra, $0000(sp)
addiu sp, sp, $0010

jal :_MenuBrowser
sw s7, $0020(k0)
beq zero, zero, :_MemSearchResultsWaitNoInput
nop

_MSR_NotR2:
//------------------------------ Triangle
jal :_ReadPad
addiu a0, zero, $EFFF
beq v0, zero, :_MemSearchResultsWaitNoInput
nop
goto _MemSearchResultsExit


_MemSearchResultsWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_MemSearchResultsExitToLoop
nop
addu s6, zero, zero


_MemSearchResultsExitToLoop:
jal :vSync
nop
beq zero, zero, :_MemSearchResultsLoop
nop

_MemSearchResultsExit:

lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0


//==========================================================
//ProcessAddress will take an address like 01334321 and return 01334320
//This will make you be able to press R2 on an address that does not end with 0, 4, 8, or C and not freeze
//Returns result in v0
_ProcessAddress:
addiu sp, sp, $FFE0
sq t0, $0000(sp)
sq t1, $0010(sp)

sll t1, a0, 28
srl t1, t1, 28 //Gets last 4 bits
addiu t0, zero, 4
divu t1, t0
mfhi t1

subu v0, a0, t1

lq t0, $0000(sp)
lq t1, $0010(sp)
jr ra
addiu sp, sp, $0020

//==========================================================
_PatchesMenuBrowser:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

daddu s4, a0, zero

_PatchesMenuBrowserCleanOff:

jal :_ClearPad
nop


lui s0, $8004
addiu s0, s0, $6000
lui s1, $0008
daddu s5, zero, zero
daddiu s6, zero, 1
_PatchesMenuBrowserLoop:


// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 10   // x
addiu t2, zero, 20   // y
multu t2, t2, s5
addiu t2, t2, 95
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 620  // w
addiu t9, zero, 20  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $4634
addiu t0, zero, 278
addiu t1, zero, 70
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString


addiu s2, zero, -1
daddu s3, s0, zero
_PatchesMenuBrowserListingLoop:


lw v0, $0000(s3)
beq v0, zero, :_PatchesMenuBrowserListingLoopExit
nop

addiu s2, s2, 1

addiu a0, s3, $0000
lw a1, $0018(s1)
jal :_lineToHex
addiu a2, zero, 8

addiu v0, zero, $0020
lw a1, $0018(s1)
sb v0, $0008(a1)
addiu a1, a1, $0009

addiu a0, s3, $0004
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(s1)
lw a0, $001C(s1)
addiu t0, zero, 218
addiu t1, zero, 20
multu t1, t1, s2
addiu t1, t1, 95
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

addiu s3, s3, 12

slti v0, s2, 16
bne v0, zero, :_PatchesMenuBrowserListingLoop
nop

_PatchesMenuBrowserListingLoopExit:

bltz s2, :_PatchesMenuBrowserExit
nop

jal :vSync
nop

// Browser Controls

bne s6, zero, :_PatchesMenuBrowserWaitNoInput
nop
//------------------------------ Exit
jal :_ReadPad
addiu a0, zero, $DFFF
bne v0, zero, :_PatchesMenuBrowserExit
nop

//------------------------------ Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, :ptbrowsenotUp
nop
addiu s6, zero, 1
sltiu v0, s5, $0001
bne v0, zero, 4
nop
addiu s5, s5, -1
beq zero, zero, :ptbrowsenotUp
nop
lui v0, $8004
addiu v0, v0, $6000
beq s0, v0, 2
nop
addiu s0, s0, -12
ptbrowsenotUp:


//------------------------------ Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, :ptbrowsenotDown
nop
addiu s6, zero, 1
slt v0, s5, s2
beq v0, zero, 4
nop
addiu s5, s5, 1
beq zero, zero, :ptbrowsenotDown
nop
addu s3, s0, zero
addiu v0, zero, 12
multu v0, v0, s5
addu s3, s3, v0
addiu s3, s3, 12
lw v0, $0000(s3)
beq v0, zero, 2
nop
addiu s0, s0, 12

ptbrowsenotDown:

//------------------------------ Select
jal :_ReadPad
addiu a0, zero, $7FFF
beq v0, zero, 6
nop
addiu v0, zero, 12
multu a1, v0, s5
addu a1, a1, s0
jal :_TurnPatchONOFF
daddu a0, zero, zero



_PatchesMenuBrowserWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_PatchesMenuBrowserExitToLoop
nop
addu s6, zero, zero

_PatchesMenuBrowserExitToLoop:

jal :vSync
nop
goto _PatchesMenuBrowserLoop

_PatchesMenuBrowserExit:


lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0
//==========================================================
_TurnPatchONOFF:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)


daddu s4, a0, zero
daddu s2, a1, zero

jal :_ClearPad
nop


lui s1, $0008
daddu s5, zero, zero
daddiu s6, zero, 1
_TurnPatchONOFFLoop:

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 10   // x
addiu t2, zero, 20   // y
multu t2, t2, s5
addiu t2, t2, 200
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 620  // w
addiu t9, zero, 20  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160


// Draw String (Patch) y: 80
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $4780
addiu t0, zero, 290
addiu t1, zero, 80
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Draw ON code
addiu a0, s2, $0000
lw a1, $0018(s1)
jal :_lineToHex
addiu a2, zero, 8

addiu v0, zero, $0020
lw a1, $0018(s1)
sb v0, $0008(a1)
addiu a1, a1, $0009

addiu a0, s2, $0004
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(s1)
lw a0, $001C(s1)
addiu t0, zero, 218
addiu t1, zero, 105
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

// Draw String (Original) y: 130
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $4788
addiu t0, zero, 272
addiu t1, zero, 130
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Draw OFF code
addiu a0, s2, $0000
lw a1, $0018(s1)
jal :_lineToHex
addiu a2, zero, 8

addiu v0, zero, $0020
lw a1, $0018(s1)
sb v0, $0008(a1)
addiu a1, a1, $0009

addiu a0, s2, $0008
jal :_lineToHex
addiu a2, zero, 8

lw a1, $0018(s1)
lw a0, $001C(s1)
addiu t0, zero, 218
addiu t1, zero, 155
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString



// Draw "Turn ON" y: 200
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $4764
addiu t0, zero, 278
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Draw "Turn OFF" y: 220
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $476C
addiu t0, zero, 272
addiu t1, zero, 220
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Draw "Go Back" y: 240
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $4778
addiu t0, zero, 278
addiu t1, zero, 240
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Draw "Add to codes" y: 260
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $44D0
addiu t0, zero, 270
addiu t1, zero, 260
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString


jal :vSync
nop

// Browser Controls

bne s6, zero, :_TurnPatchONOFFWaitNoInput
nop

//------------------------------ Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, 6
nop
addiu s6, zero, 1
slti v0, s5, 1
bne v0, zero, 2
nop
addiu s5, s5, -1


//------------------------------ Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, 6
nop
addiu s6, zero, 1
slti v0, s5, 3
beq v0, zero, 2
nop
addiu s5, s5, 1


//------------------------------ Select
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_TurnPatchONOFFnotX
nop
addiu v0, zero, 0
bne s5, v0, 4
nop
lw v0, $0000(s2)
lw v1, $0004(s2)
sw v1, $0000(v0)

addiu v0, zero, 1
bne s5, v0, 4
nop
lw v0, $0000(s2)
lw v1, $0008(s2)
sw v1, $0000(v0)

addiu v0, zero, 3
bne s5, v0, 4
nop

jal :_AddToCodes
daddu a0, s2, zero //Address of selected patch

goto _TurnPatchONOFFExit

addiu v0, zero, 2
bne s5, v0, 3
nop
goto _TurnPatchONOFFExit
_TurnPatchONOFFnotX:



_TurnPatchONOFFWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_TurnPatchONOFFExitToLoop
nop
addu s6, zero, zero

_TurnPatchONOFFExitToLoop:

jal :vSync
nop
goto _TurnPatchONOFFLoop

_TurnPatchONOFFExit:
lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_AddToCodes:
addiu sp, sp, $FF90
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s5, $0040(sp)
sq k0, $0050(sp)
sq k1, $0060(sp)

daddu s0, a0, zero

lui t0, $0008
addiu t1, t0, $4260
sw t1, $0040(t0) //Pointer to quad word containing all 4 values

lui s1, $8004
ori s1, s1, $7810 //Place where the address of Codes are stored (+ 0x10)
//I am going to make a loop to find the next 00000000 address or same address (no duplicates) and store the code there

beq zero, zero, 2 //Skip the add 20 for the first time
nop

_ATCloop:
addiu s1, s1, $0020

lw s2, $0010(s1)

beq s2, zero, :_FoundMemRegion
nop

lw t0, $0000(s0)
sll s2, s2, 4
srl s2, s2, 4 //Get rid of command
bne s2, t0, :_ATCloop //If there is an address equal to the one being added, replace it (go to _FoundMemRegion)
nop

_FoundMemRegion:

//Store address at pointer
lui t0, $0008
lw t1, $0040(t0)

//lw t0, $0000(s0)
sw s0, $0000(t1)

/*
_lineToHex arguments setup:
a0 = Value to turn to string
a1 = Address to output to
a2 = length (bytes) of value * 2
*/

addiu a0, s0, $0000
lui t0, $0008
lw a1, $0040(t0)
jal :_lineToHex
addiu a2, zero, 8

jal :_ATCMenu
nop

beq v0, zero, :_ExitATCloop //Cancel was selected
nop

lui t0, $0008
lw t1, $0040(t0)
lq t0, $0000(t1) //Grab string of Code name
sq t0, $0000(s1) //Store string of Code name

//Calculate bit offset of value
lui v1, $0008
lb v0, $4301(v1) //Offset
addiu v1, zero, 8
multu v0, v0, v1

lw t0, $0004(s0) //Load Value (ON)
srlv t0, t0, v0
sw t0, $0014(s1) //Store value
lw t0, $0008(s0) //Load Value (OFF)
srlv t0, t0, v0
sw t0, $0018(s1) //Store value

lui v1, $0008
lb v0, $4300(v1) //Loads bit (8 = 0, 16 = 1, or 32 = 2)
lw t0, $0000(s0) //Load address
sll v0, v0, 28 //32 bit would return 2, then sll 28 would make it 20000000
add t0, v0, t0 //Adds bit command
lb v0, $4301(v1) //Loads offset
add t0, t0, v0 //Adds offset

//A check to make sure that they entered a valid offset
//8 bit (bit = 0) accepts anything
//If the bit is 1 (16), offset must be 0 or 2
//If the bit is 2 (32), offset must be 0
lb t1, $4300(v1) //Bit
lb t2, $4301(v1) //Offset
addiu v0, zero, 1 //16 bit
bne t1, v0, :_BitOff32
nop
beq t2, zero, :_BitOffOK //0
nop
addiu v0, zero, 2
beq t2, v0, :_BitOffOK //2
nop
//Not ok
subu t0, t0, t2 //Just make it go back to the original address
beq zero, zero, :_BitOffOK
nop
_BitOff32:
addiu v0, zero, 2 //32 bit
bne t1, v0, :_BitOffOK
nop
beq t2, zero, :_BitOffOK
nop
subu t0, t0, t2 //Just make it go back to the original address

_BitOffOK:
sw t0, $0010(s1) //Store address

_ExitATCloop:

lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s5, $0040(sp)
lq k0, $0050(sp)
lq k1, $0060(sp)
jr ra
addiu sp, sp, $0070

//==========================================================
_ATCMenu:
addiu sp, sp, $FFD0
sq ra, $0000(sp)
sq s1, $0010(sp)
sq s0, $0020(sp)

jal :_ClearPad
nop

lui t1, $0008
addiu t0, zero, $0002
sw t0, $4300(t1)

lui s1, $0008
add s5, zero, zero
daddiu s6, zero, $0001

daddu k0, zero, zero
daddu k1, zero, zero

_ACTMenuLoop:
// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet

//Credit is due to Gtlcpimp for the brilliant setup to highlight certain values in blue
//Taken from his Memory Search Menu

//k1 being the marker placement
//k0 being whether you are in edit value mode or scroll mode
//0 = scroll, 1 = edit

addiu t1, zero, 18   // x (Edit mode)
multu t1, t1, k1
addiu t1, t1, 240

bne k0, zero, 2
nop
addiu t1, zero, 10   // x

addiu t2, zero, 20   // y
multu t2, t2, s5
addiu t2, t2, 180
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size

addiu t8, zero, 12   // w (Edit mode)

bne k0, zero, 2
nop
addiu t8, zero, 620  // w

addiu t9, zero, 20   // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

//Draw editable string
lw s3, $0040(s1)
sb zero, $000F(s3) //End the string a with a null byte

addiu a1, s3, $0000
lw a0, $001C(s1)
addiu t0, zero, 240
addiu t1, zero, 180
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0012
call _drawString

//Draw "Code Name:"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $4870 // Code Name:
addiu t0, zero, 120
addiu t1, zero, 180
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Draw "R1 to store 00"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $487C // R1 to store 00
addiu t0, zero, 140
addiu t1, zero, 100
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Draw "L1 to store 20"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $488C // L1 to store 20
addiu t0, zero, 140
addiu t1, zero, 120
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Draw bits: and bit options
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $47B4 // Bits:
addiu t0, zero, 120
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a1, $8004
lui v0, $0008
lb v0, $4300(v0)

bne v0, zero, 2
nop
addiu a1, a1, $47BC // 8

addiu v1, zero, 1
bne v0, v1, 2
nop
addiu a1, a1, $47BE // 16

addiu v1, zero, 2
bne v0, v1, 2
nop
addiu a1, a1, $47C1 // 32

//Text "8", or "16", or "32"
lw a0, $001C(s1)
addiu t0, zero, 230
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Draw Offset: and Offset options
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $49B0 // Offset:
addiu t0, zero, 120
addiu t1, zero, 220
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lui a1, $8004
lui v0, $0008
lb v0, $4301(v0)

bne v0, zero, 2
nop
addiu a1, a1, $49B8 // 0

addiu v1, zero, 1
bne v0, v1, 2
nop
addiu a1, a1, $49BA // 1

addiu v1, zero, 2
bne v0, v1, 2
nop
addiu a1, a1, $49BC // 2

addiu v1, zero, 3
bne v0, v1, 2
nop
addiu a1, a1, $49BE // 3

//Text "0", or "1", or "2" or "3"
lw a0, $001C(s1)
addiu t0, zero, 230
addiu t1, zero, 220
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Text "Ok"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $44DD // "Ok"
addiu t0, zero, 230
addiu t1, zero, 240
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//Text "Cancel"
lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $44C8 // "Cancel"
addiu t0, zero, 210
addiu t1, zero, 260
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

jal :vSync
nop

// ACT Menu Controls

bne s6, zero, :_ACTMenuWaitNoInput
nop

//------------------------------ Cross
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, :_ACTNotCross
nop
addiu s6, zero, 1

// Exit ACTMenu and return 1
addiu v0, zero, 3 //Ok
bne v0, s5, 4
nop
addiu v0, zero, $0001
beq zero, zero, :_ACTMenuExit
nop

// Exit ACTMenu and return 0
addiu v0, zero, 4 //Cancel
bne v0, s5, 4
nop
add v0, zero, zero
beq zero, zero, :_ACTMenuExit
nop

//Edit mode for code name - ON
bne s5, zero, 2
nop
addiu k0, zero, $0001

_ACTNotCross:
//------------------------------ Triangle
jal :_ReadPad
addiu a0, zero, $EFFF
beq v0, zero, :_ACTNotTri
nop
addiu s6, zero, 1

//Edit mode for code name - OFF
beq k0, zero, 2
nop
addiu k0, zero, $0000

_ACTNotTri:
//------------------------------ R1
jal :_ReadPad
addiu a0, zero, $F7FF
beq v0, zero, :_ACTNotR1
nop
addiu s6, zero, 1

//Edit mode for code name - OFF
beq k0, zero, :_ACTNotR1
nop

lui t0, $0008
lw t0, $0040(t0)
add t0, k1, t0
sb zero, $0000(t0)

_ACTNotR1:
//------------------------------ L1
jal :_ReadPad
addiu a0, zero, $FBFF
beq v0, zero, :_ACTNotL1
nop
addiu s6, zero, 1

beq k0, zero, :_ACTNotL1
nop

//Store space at marked byte
addiu t1, zero, $0020 //Space character
lui t0, $0008

lui t0, $0008
lw t0, $0040(t0)
add t0, k1, t0
sb t1, $0000(t0)

_ACTNotL1:
//------------------------------ R2
jal :_ReadPad
addiu a0, zero, $FDFF
beq v0, zero, :_ACTNotR2
nop
addiu s6, zero, 1

beq k0, zero, :_ACTNotR2
nop

lui t0, $0008
daddu a0, k1, zero
lw a1, $0040(t0)
jal :_HandleATCIncDec
daddiu a2, zero, $0020

_ACTNotR2:
//------------------------------ L2
jal :_ReadPad
addiu a0, zero, $FEFF
beq v0, zero, :_ACTNotL2
nop
addiu s6, zero, 1

beq k0, zero, :_ACTNotL2
nop

lui t0, $0008
daddu a0, k1, zero
lw a1, $0040(t0)
jal :_HandleATCIncDec
daddiu a2, zero, $FFE0

_ACTNotL2:
//------------------------------ Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, :_ACTNotUp
nop
addiu s6, zero, $0001

bne k0, zero, 4
nop

beq s5, zero, 2
nop
addiu s5, s5, -1

beq k0, zero, :_ACTNotUp
nop

lui t0, $0008
daddu a0, k1, zero
lw a1, $0040(t0)
jal :_HandleATCIncDec
daddiu a2, zero, $0001

_ACTNotUp:
//------------------------------ Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, :_ACTNotDown
nop
addiu s6, zero, $0001

bne k0, zero, 5
nop

addiu t1, zero, $0004
beq s5, t1, 2
nop
addiu s5, s5, 1

beq k0, zero, :_ACTNotDown
nop

lui t0, $0008
daddu a0, k1, zero
lw a1, $0040(t0)
jal :_HandleATCIncDec
addiu a2, zero, $FFFF

_ACTNotDown:
//------------------------------ Left
jal :_ReadPad
addiu a0, zero, $FF7F
beq v0, zero, :_ACTNotLeft
nop
addiu s6, zero, $0001
addiu v0, zero, $0001
bne s5, v0, 7
nop
lui v1, $0008
lb v0, $4300(v1)
beq v0, zero, 3
nop
addiu v0, v0, -1
sb v0, $4300(v1)

addiu v0, zero, 2
bne s5, v0, 7
nop
lui v1, $0008
lb v0, $4301(v1)
beq v0, zero, 3
nop
addiu v0, v0, -1
sb v0, $4301(v1)

//Code name marker decrementor
beq k0, zero, 4
nop
//addiu v0, zero, $0014
beq k1, zero, 2
nop
addiu k1, k1, -1


_ACTNotLeft:
//------------------------------ Right
jal :_ReadPad
addiu a0, zero, $FFDF
beq v0, zero, :_ACTNotRight
nop
addiu s6, zero, $0001
addiu v0, zero, $0001
bne s5, v0, 8
nop
addiu t1, zero, $0002
lui v1, $0008
lb v0, $4300(v1)
beq v0, t1, 3
nop
addiu v0, v0, 1
sb v0, $4300(v1)

addiu v0, zero, $0002
bne s5, v0, 8
nop
addiu t1, zero, $0003
lui v1, $0008
lb v0, $4301(v1)
beq v0, t1, 3
nop
addiu v0, v0, 1
sb v0, $4301(v1)

beq k0, zero, 5
nop
addiu v0, zero, 14
beq k1, v0, 2
nop
addiu k1, k1, 1

_ACTNotRight:

beq zero, zero, :_ACTMenuLoop
nop

_ACTMenuWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_ACTMenuExitToLoop
nop
addu s6, zero, zero

_ACTMenuExitToLoop:

beq zero, zero, :_ACTMenuLoop
nop

_ACTMenuExit:

lq ra, $0000(sp)
lq s1, $0010(sp)
lq s0, $0020(sp)
jr ra
addiu sp, sp, $0030

//==========================================================
_HandleATCIncDec:
//By Dnawrkshp
//Takes k1 (marker, a0) and a string pointer (a1) and applies the inc or dec (a2) to the appropriate byte
addiu sp, sp, $FF80
sq t0, $0000(sp)
sq t1, $0010(sp)
sq t2, $0020(sp)
sq t3, $0030(sp)
sq t4, $0040(sp)
sq s0, $0050(sp)
sq s1, $0060(sp)
sq s2, $0070(sp)

daddu s0, a0, zero  // Marker placement
daddu s1, a1, zero  // String (pointer)
daddu s2, a2, zero  // Increment/decrement value

addu t1, s1, s0
lb t3, $0000(t1)

sll t3, t3, 24
srl t3, t3, 24

add t3, t3, s2
sb t3, $0000(t1)

beq zero, zero, :_CheckForLoop
nop

_CheckForLoop:
//Checks if ((value > 0 && value < 20 ) || value > 7E)

lb t3, $0000(t1)

sll t3, t3, 24
srl t3, t3, 24

addiu t2, zero, $0020
slt t2, t3, zero
bne t2, zero, 5
nop
addiu t2, zero, $0020
slt t2, t3, t2
bne t2, zero, :_LoopingChar
nop



addiu t2, zero, $007E
slt t2, t2, t3

beq t2, zero, 4
nop

beq zero, zero, :_LoopingChar
nop

slt t2, t3, zero
beq t2, zero, :_ExitATCIncDec
nop

beq zero, zero, :_ExitATCIncDec
nop

_LoopingChar:

slt t4, s2, zero
bne t4, zero, :_DecChar
nop

_IncChar:

addiu t3, zero, $0020
sb t3, $0000(t1)

beq zero, zero, :_ExitATCIncDec
nop

_DecChar:

addiu t3, zero, $007E
sb t3, $0000(t1)

beq zero, zero, :_ExitATCIncDec
nop

_ExitATCIncDec:
lq t0, $0000(sp)
lq t1, $0010(sp)
lq t2, $0020(sp)
lq t3, $0030(sp)
lq t4, $0040(sp)
lq s0, $0050(sp)
lq s1, $0060(sp)
lq s2, $0070(sp)
jr ra
addiu sp, sp, $0080


//==========================================================
_MenuBrowser:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)
sq s4, $0050(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

daddu s4, a0, zero

_MenuBrowserCleanOff:

jal :_ClearPad
nop

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $000 // r
addiu t5, zero, $000 // g
addiu t6, zero, $000 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 80

jal :_drawMenuControls
addiu a0, zero, 1

jal :vSync
nop

_MenuBrowserLoop:

// Clear memory view space
//Memory box
lui t0, $0009        // Packet
addiu t1, zero, 5    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 214  // w
addiu t9, zero, 350  // h
jal :_AddPixel
nop
//Blue highlight (whole box)
lui t0, $0009        // Packet
addiu t1, zero, 10   // x
addiu t2, zero, 65   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 204  // w
addiu t9, zero, 340  // h
jal :_AddPixel
nop

lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

jal :_MemBrowser
addu a0, zero, zero

jal :_MemBrowserASCII
addu a0, zero, zero

jal :vSync
nop

// Browser Controls

//------------------------------ Exit
jal :_ReadPad
addiu a0, zero, $EFFF
bne v0, zero, :_MenuBrowserExit
nop

//------------------------------ Line Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, 5
nop
lui v0, $0008
lw v1, $0020(v0)
addiu v1, v1, -4
sw v1, $0020(v0)

//------------------------------ Line Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, 5
nop
lui v0, $0008
lw v1, $0020(v0)
addiu v1, v1, 4
sw v1, $0020(v0)

//------------------------------ Page Up
jal :_ReadPad
addiu a0, zero, $FF7F
beq v0, zero, 5
nop
lui v0, $0008
lw v1, $0020(v0)
addiu v1, v1, -80
sw v1, $0020(v0)

//------------------------------ Page Down
jal :_ReadPad
addiu a0, zero, $FFDF
beq v0, zero, 5
nop
lui v0, $0008
lw v1, $0020(v0)
addiu v1, v1, 80
sw v1, $0020(v0)

//------------------------------ Line Selector
jal :_ReadPad
addiu a0, zero, $7FFF
beq v0, zero, 5
nop
jal :_LineBrowser
addu a0, zero, zero
beq zero, zero, :_MenuBrowserCleanOff
nop

//------------------------------ Go To
jal :_ReadPad
addiu a0, zero, $F7FF
beq v0, zero, 5
nop
jal :_GotoAddress
addu a0, zero, zero
beq zero, zero, :_MenuBrowserCleanOff
nop

jal :vSync
nop
goto _MenuBrowserLoop

_MenuBrowserExit:
lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
lq s4, $0050(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_LineBrowser:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

lui s0, $0008
daddu s4, a0, zero

_LineBrowserCleanOff:

jal :_ClearPad
nop

_LineBrowserCleanOff2:

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $000 // r
addiu t5, zero, $000 // g
addiu t6, zero, $000 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 80

jal :_drawMenuControls
addiu a0, zero, 4

jal :vSync
nop

daddu s5, zero, zero
daddu s6, zero, zero
_LineBrowserLoop:

// Clear memory view space
lui t0, $0009        // Packet
addiu t1, zero, 5    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 214  // w
addiu t9, zero, 350  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 10   // x
addiu t2, zero, 20   // y
multu t2, t2, s5
addiu t2, t2, 65
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 204  // w
addiu t9, zero, 20  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

jal :_MemBrowser
addu a0, zero, zero

jal :_MemBrowserASCII
addu a0, zero, zero

jal :vSync
nop

// Browser Controls

bne s6, zero, :_LineBrowserWaitNoInput
nop
//------------------------------ Exit
jal :_ReadPad
addiu a0, zero, $DFFF
bne v0, zero, :_LineBrowserExit
nop

//------------------------------ Line Up
jal :_ReadPad
addiu a0, zero, $FFEF
beq v0, zero, 6
nop
addiu s6, zero, 1
sltiu v0, s5, $0001
bne v0, zero, 2
nop
addiu s5, s5, -1

//------------------------------ Line Down
jal :_ReadPad
addiu a0, zero, $FFBF
beq v0, zero, 6
nop
addiu s6, zero, 1
sltiu v0, s5, 16
beq v0, zero, 2
nop
addiu s5, s5, 1

//------------------------------ Edit
jal :_ReadPad
addiu a0, zero, $BFFF
beq v0, zero, 8
nop
lw v0, $0020(s0)
sll a1, s5, 2
addu a1, a1, v0
jal :_EditAddress
addu a0, zero, zero
beq zero, zero, :_LineBrowserCleanOff
nop

//------------------------------ Install Dumper - L1
jal :_ReadPad
addiu a0, zero, $FBFF
beq v0, zero, :_NotLBL1
nop
lw v0, $0020(s0)
sll a0, s5, 2
jal :_InstallRDMenu
addu a0, a0, v0
addiu s6, zero, 1
beq zero, zero, :_LineBrowserCleanOff
nop
_NotLBL1:

_LineBrowserWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_LineBrowserExitToLoop
nop
addu s6, zero, zero

_LineBrowserExitToLoop:

jal :vSync
nop
goto _LineBrowserLoop

_LineBrowserExit:
lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_MemBrowserASCII:

addiu sp, sp, $FFF0
sq ra, $0000(sp)

//ASCII table box
lui t0, $0009        // Packet
addiu t1, zero, 225  // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 90   // w
addiu t9, zero, 350  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

lui s0, $0008
lw s3, $0020(s0)
daddu s1, zero, zero
daddu s4, a0, zero
daddiu s2, zero, 65

_MemBASCIIloop:

sll t0, s1, 2
addu t0, s3, t0
sw t0, $0020(s0)

addiu sp, sp, $FFF0
sq ra, $0000(sp)
//Calls function that changes the data to have a null byte at the end of each character
lui t0, $0008
lw a0, $0020(t0)
addiu a1, t0, $4200
jal :_ProcessASCII
addiu a2, zero, $0008
lq ra, $0000(sp)
addiu sp, sp, $0010

//ASCII
addiu a1, s0, $4201
lw a0, $001C(s0)
addiu t0, zero, 230
daddu t1, s2, zero
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString


//ASCII
addiu a1, s0, $4203
lw a0, $001C(s0)
addiu t0, zero, 250
daddu t1, s2, zero
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//ASCII
addiu a1, s0, $4205
lw a0, $001C(s0)
addiu t0, zero, 270
daddu t1, s2, zero
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

//ASCII
addiu a1, s0, $4207
lw a0, $001C(s0)
addiu t0, zero, 290
daddu t1, s2, zero
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString


addiu s1, s1, 1
addiu s2, s2, 20
addiu v0, zero, 17
beq s1, v0, 3
nop

beq zero, zero, :_MemBASCIIloop
nop

_ExitMemBASCIIloop:

sw s3, $0020(s0)

lq ra, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_EditAddress:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

daddu s4, a0, zero
daddu s7, a1, zero

jal :_ClearPad
nop

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $000 // r
addiu t5, zero, $000 // g
addiu t6, zero, $000 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 80

jal :_drawMenuControls
addiu a0, zero, 3

jal :vSync
nop


lui s0, $0008

lw v0, $0000(s7)
sw s7, $0030(s0)
sw v0, $0034(s0)

daddu s5, zero, zero
daddu s6, zero, zero
_EditAddressLoop:

// Clear memory view space
lui t0, $0009        // Packet
addiu t1, zero, 5    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 214  // w
addiu t9, zero, 30   // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 12   // x
multu t1, t1, s5
addiu t1, t1, 118
addiu t2, zero, 65   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 12   // w
addiu t9, zero, 20   // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

lui s0, $0008
addiu a0, s0, $0030
lw a1, $0018(s0)
addiu a2, zero, 8
call _lineToHex

addiu a0, s0, $0034
addiu v0, zero, $0020
lw a1, $0018(s0)
sb v0, $0008(a1)
addiu a1, a1, 9
addiu a2, zero, 8
call _lineToHex

lw a1, $0018(s0)
lw a0, $001C(s0)
addiu t0, zero, 10
addiu t1, zero, 65
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

lui a0, $0008
addiu a0, a0, $0034
jal :_EditValASCII
nop

jal :vSync
nop


bne s6, zero, :_EditAddressWaitNoInput
nop

jal :_ReadPad
addiu a0, zero, $FFEF // Up
beq v0, zero, 13
nop
addiu s6, zero, 1
lw a0, $0034(s0)
addiu v0, zero, 1
addiu v1, zero, 7
subu v1, v1, s5
beq v1, zero, 4
nop
addiu v1, v1, -1
bne v1, zero, -2
sll v0, v0, 4
addu a0, a0, v0
sw a0, $0034(s0)

jal :_ReadPad
addiu a0, zero, $FFBF // Down
beq v0, zero, 13
nop
addiu s6, zero, 1
lw a0, $0034(s0)
addiu v0, zero, 1
addiu v1, zero, 7
subu v1, v1, s5
beq v1, zero, 4
nop
addiu v1, v1, -1
bne v1, zero, -2
sll v0, v0, 4
subu a0, a0, v0
sw a0, $0034(s0)

jal :_ReadPad
addiu a0, zero, $FF7F // Left
beq v0, zero, 6
nop
addiu s6, zero, 1
sltiu v0, s5, $0001
bne v0, zero, 2
nop
addiu s5, s5, -1

jal :_ReadPad
addiu a0, zero, $FFDF // Right
beq v0, zero, 6
nop
addiu s6, zero, 1
sltiu v0, s5, $0007
beq v0, zero, 2
nop
addiu s5, s5, 1

jal :_ReadPad
addiu a0, zero, $EFFF // Triangle - Cancel Save
beq v0, zero, 4
nop
sw zero, $0034(s0)
goto _EditAddressExit


jal :_ReadPad
addiu a0, zero, $F7FF // R1 - Save Data
beq v0, zero, 11
nop
lui s0, $0008
lw a0, $0030(s0)
lw a1, $0034(s0)
jal :_AddPatchData
lw a2, $0000(a0)
lw v1, $0030(s0)
lw v0, $0034(s0)
sw v0, $0000(v1)
beq zero, zero, :_EditAddressExit
nop


_EditAddressWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_EditAddressExitToLoop
nop
addu s6, zero, zero


_EditAddressExitToLoop:

jal :vSync
nop
goto _EditAddressLoop

_EditAddressExit:



lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_EditValASCII:

addiu sp, sp, $FFF0
sq ra, $0000(sp)

//a0 defined when called

lui a1, $0008
addiu a1, a1, $4200
jal :_ProcessASCII
addiu a2, zero, $0008

lui t0, $0009        // Packet
addiu t1, zero, 220  // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 90   // w
addiu t9, zero, 30   // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

addiu a1, s0, $4201
lw a0, $001C(s0)
addiu t0, zero, 230
addiu t1, zero, 65
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu a1, s0, $4203
lw a0, $001C(s0)
addiu t0, zero, 250
addiu t1, zero, 65
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu a1, s0, $4205
lw a0, $001C(s0)
addiu t0, zero, 270
addiu t1, zero, 65
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

addiu a1, s0, $4207
lw a0, $001C(s0)
addiu t0, zero, 290
addiu t1, zero, 65
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lq ra, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_AddPatchData:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

daddu s1, a0, zero // Address
daddu s2, a1, zero // New Data
daddu s3, a2, zero // Old Data

lui s0, $8004
addiu s0, s0, $6000
addiu v1, zero, 512 // Patch Limit

// Scan for existing, add if non existent
_AddPatchDataLoop:
lw v0, $0000(s0)
beq v0, s1, :_AddPatchDataOverwrite
nop
beq v0, zero, :_AddPatchDataNewWrite
nop
addiu s0, s0, 12
bne v1, zero, :_AddPatchDataLoop
addiu v1, v1, -1
goto _AddPatchDataExit // Limit reached

_AddPatchDataNewWrite:
sw s1, $0000(s0)
sw s2, $0004(s0)
sw s3, $0008(s0)

goto _AddPatchDataExit
_AddPatchDataOverwrite:
sw s1, $0000(s0)
sw s2, $0004(s0)

_AddPatchDataExit:
lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_GotoAddress:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sw s0, $0010(sp)
sw s1, $0014(sp)
sw s2, $0018(sp)
sw s3, $001C(sp)
sw s4, $0020(sp)
sw s5, $0024(sp)
sw s6, $0028(sp)
sw s7, $002C(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

daddu s4, a0, zero

jal :_ClearPad
nop

// Clear menu space
lui t0, $0009        // Packet
addiu t1, zero, 0    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $000 // r
addiu t5, zero, $000 // g
addiu t6, zero, $000 // b
daddu t7, zero, zero // size
addiu t8, zero, 640  // w
addiu t9, zero, 380  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 80

jal :_drawMenuControls
addiu a0, zero, 2

jal :vSync
nop


lui s0, $0008

lw s7, $0020(s0)

daddu s5, zero, zero
daddu s6, zero, zero
_GotoAddressLoop:

// Clear memory view space
lui t0, $0009        // Packet
addiu t1, zero, 5    // x
addiu t2, zero, 60   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $030 // b
daddu t7, zero, zero // size
addiu t8, zero, 107  // w
addiu t9, zero, 30  // h
jal :_AddPixel
nop
lui t0, $0009        // Packet
addiu t1, zero, 12   // x
multu t1, t1, s5
addiu t1, t1, 10
addiu t2, zero, 65   // y
daddu t3, s4, zero   // z
addiu t4, zero, $030 // r
addiu t5, zero, $030 // g
addiu t6, zero, $0A0 // b
addiu t7, zero, 80   // size
addiu t8, zero, 12  // w
addiu t9, zero, 20  // h
jal :_AddPixel
nop
lui a0, $0009
jal :_SendPacket
addiu a1, zero, 160

addiu a0, s0, $0020
lw a1, $0018(s0)
addiu a2, zero, 8
call _lineToHex

lw a1, $0018(s0)
lw a0, $001C(s0)
addiu t0, zero, 10
addiu t1, zero, 65
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

jal :vSync
nop


bne s6, zero, :_GotoAddressWaitNoInput
nop

jal :_ReadPad
addiu a0, zero, $FFEF // Up
beq v0, zero, 13
nop
addiu s6, zero, 1
lw a0, $0020(s0)
addiu v0, zero, 1
addiu v1, zero, 7
subu v1, v1, s5
beq v1, zero, 4
nop
addiu v1, v1, -1
bne v1, zero, -2
sll v0, v0, 4
addu a0, a0, v0
sw a0, $0020(s0)

jal :_ReadPad
addiu a0, zero, $FFBF // Down
beq v0, zero, 13
nop
addiu s6, zero, 1
lw a0, $0020(s0)
addiu v0, zero, 1
addiu v1, zero, 7
subu v1, v1, s5
beq v1, zero, 4
nop
addiu v1, v1, -1
bne v1, zero, -2
sll v0, v0, 4
subu a0, a0, v0
sw a0, $0020(s0)

jal :_ReadPad
addiu a0, zero, $FF7F // Left
beq v0, zero, 6
nop
addiu s6, zero, 1
sltiu v0, s5, $0001
bne v0, zero, 2
nop
addiu s5, s5, -1

jal :_ReadPad
addiu a0, zero, $FFDF // Right
beq v0, zero, 6
nop
addiu s6, zero, 1
sltiu v0, s5, $0007
beq v0, zero, 2
nop
addiu s5, s5, 1

jal :_ReadPad
addiu a0, zero, $DFFF // Circle - Cancel Goto
beq v0, zero, 4
nop
sw s7, $0020(s0)
goto _GotoAddressExit


jal :_ReadPad
addiu a0, zero, $BFFF // Cross - Keep Address
bne v0, zero, :_GotoAddressExit
nop

_GotoAddressWaitNoInput:
jal :_ReadPad
addiu a0, zero, -1
beq v0, zero, :_GotoAddressExitToLoop
nop
addu s6, zero, zero


_GotoAddressExitToLoop:

jal :vSync
nop
goto _GotoAddressLoop

_GotoAddressExit:
jal :_ProcessAddress
lw a0, $0020(s0)
sw v0, $0020(s0)

lq ra, $0000(sp)
lw s0, $0010(sp)
lw s1, $0014(sp)
lw s2, $0018(sp)
lw s3, $001C(sp)
lw s4, $0020(sp)
lw s5, $0024(sp)
lw s6, $0028(sp)
lw s7, $002C(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_MemBrowser:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)
sq s4, $0050(sp)
sq k0, $0060(sp)
sq k1, $0070(sp)

daddu s4, a0, zero // Z

lui s0, $0008

lw v0, $0020(s0)

bne v0, zero, 3
nop
lui v0, $0010
sw v0, $0020(s0)

lw s1, $0018(s0)

/*
addu s0, t0, t7   // s0 = (Packet + Size)
daddu s1, t1, zero // s1 = x
daddu s2, t2, zero // s2 = y
daddu s3, t3, zero // s3 = z
daddu s4, t4, zero // s4 = r
daddu s5, t5, zero // s5 = g
daddu s6, t6, zero // s6 = b
daddu s7, t7, zero // s7 = Size
daddu k0, t8, zero // k0 = w
daddu k1, t9, zero // k1 = h
*/
daddu s1, zero, zero
daddiu s2, zero, 65
lw a0, $0020(s0)
daddu s3, a0, zero

_drawLinesLoop:

sll v0, s1, 2
addu a0, s3, v0
sw a0, $0020(s0)

addiu a0, s0, $0020
lw a1, $0018(s0)
addiu a2, zero, 8
call _lineToHex

lw a0, $0020(s0)
lw a1, $0018(s0)
addiu v0, zero, $0020
sb v0, $0008(a1)
addiu a1, a1, 9
addiu a2, zero, 8
call _lineToHex

lw a1, $0018(s0)
lw a0, $001C(s0)
addiu t0, zero, 10
daddu t1, s2, zero
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $000C
call _drawString

addiu s1, s1, 1
addiu s2, s2, 20
addiu v0, zero, 17
beq s1, v0, 3
nop
goto _drawLinesLoop

sw s3, $0020(s0)

lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
lq s4, $0050(sp)
lq k0, $0060(sp)
lq k1, $0070(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
_ProcessASCII:
addiu sp, sp, $FFD0
sq t0, $0000(sp)
sq t1, $0010(sp)
sq t2, $0020(sp)

//Setting up
daddu t1, a1, zero    //Location to store result
daddu t2, a2, zero    //Size
addiu t3, zero, $0020 //Space
addiu t5, zero, $0001 //Used for values < 0 (0x80 - 0xFF)
daddu t6, a0, zero    //Address for value to be loaded byte by byte

sq zero, $0000(t1) //reset

add t1, t1, t2
daddiu t6, a0, zero //Byte read address
_ASCIILoop:
lb t0, $0000(t6) //Value
//Remove possible 0xFF's (negative)
sll t0, t0, 24
srl t0, t0, 24

beq t2, zero, :_ExitASCIILoop
nop

sll t4, t0, 24

slt t4, t4, zero

beq t4, zero, 4
nop

sb t5, $FFFF(t1)

beq zero, zero, 2
nop

sb t0, $FFFF(t1)

sb zero, $FFFE(t1)
addiu t1, t1, $FFFE
addiu t2, t2, $FFFE
addiu t6, t6, 1

beq zero, zero, :_ASCIILoop
nop

_ExitASCIILoop:

lq t0, $0000(sp)
lq t1, $0010(sp)
lq t2, $0020(sp)
jr ra
addiu sp, sp, $0030

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

//==========================================================
_ClearPad:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

lui s0, $8008
addiu s0, s0, $E810

lw t0, $0000(s0)
lw t1, $0004(s0)
lw t2, $0008(s0)
lw t3, $000C(s0)

addiu v1, zero, -1

beq t0, zero, 2
nop
sh v1, $0000(t0)

beq t1, zero, 2
nop
sh v1, $0000(t1)

beq t2, zero, 2
nop
sh v1, $0000(t2)

beq t3, zero, 2
nop
sh v1, $0000(t3)


addiu v0, zero, 1
lw ra, $0000(sp)
lw s0, $0004(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_drawString:

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

lui s0, $0009          // Packet
daddu s1, zero, zero   // Size
daddu s2, t0, zero     // X
daddu s3, t1, zero     // Y
daddu s4, t2, zero     // Z
daddu s5, a0, zero     // Font Address
daddu s6, a1, zero     // String Address
daddu at, t6, zero     // Constant spacing (if not 0)
lui s7, $0008          // Vars

sb t3, $0008(s7) // Preserve R
sb t4, $0009(s7) // Preserve G
sb t5, $000A(s7) // Preserve B
sw s2, $000C(s7) // Preserve X
sw s3, $0010(s7) // Preserve Y
sw s5, $001C(s7) // Preserve Font Address

lb t1, $0000(s6)
addiu t2, zero, 324

multu t0, t1, t2
addu s5, s5, t0
//addiu s5, s5, 1

__drawStringLoop:

lb t1, $0000(s6)
beq t1, zero, :overLoad
nop

lb a0, $0000(s5)
addiu s5, s5, 1

addiu v1, zero, 1
beq a0, v1, 3
nop
goto isTransparent

lb a0, $0008(s7) // Grab R
lb a1, $0009(s7) // Grab G
lb a2, $000A(s7) // Grab B

daddu t0, s0, zero
daddu t1, s2, zero
daddu t2, s3, zero
daddu t3, s4, zero
daddu t4, a0, zero
daddu t5, a1, zero
daddu t6, a2, zero
daddu t7, s1, zero
addiu t8, zero, 2
addiu t9, zero, 2
call _AddPixel
daddu s1, s1, v0

isTransparent:

lw v1, $0000(s7)
addiu v1, v1, 1 // tx++
sw v1, $0000(s7)
addiu s2, s2, 1 // nx++

//slti v0, v1, 18
addiu v0, zero, 18
bne v0, v1, 7 // if tx == 17
nop
lw s2, $000C(s7) // nx = x
lw v1, $0004(s7)
addiu v1, v1, 1 // ty++
sw v1, $0004(s7)
addiu s3, s3, 1 // ny++
sw zero, $0000(s7) // tx = 0

lw v1, $0004(s7)
//slti v0, v1, 18
addiu v0, zero, 18
bne v0, v1, :skipChar // if ty == 17
nop
sw zero, $0000(s7)
sw zero, $0004(s7)
lw s2, $000C(s7)

lb a0, $0000(s6) //Pass it the byte
call _GetSpacing

beq at, zero, 2
nop
add v0, at, zero

addu s2, s2, v0
addiu s6, s6, 1
lb t1, $0000(s6)
addiu t2, zero, 324
multu t0, t1, t2
lw s5, $001C(s7)
addu s5, s5, t0
sw s2, $000C(s7)
lw s3, $0010(s7)
lw v1, $0014(s7)
addiu v1, v1, 1
sw v1, $0014(s7)

//daddu a0, s0, zero
//daddu a1, s1, zero
//call _SendPacket   // Draw char by char
//daddu s1, zero, zero


skipChar:

// Wraplen
lw v1, $000C(s7)
slti v0, v1, 600
bne v0, zero 5
nop
daddu s2, v1, zero
lw v1, $0010(s7)
daddiu s3, v1, 20
sw s3, $0010(s7)

lw v1, $0014(s7)
addiu v0, zero, 17 // String length limit (replaces pixel counter)
bne v0, v1, :strLimitNotReached
nop
lb v1, $0000(s6)
beq v1, zero, :overLoad
nop
daddu a0, s0, zero
daddu a1, s1, zero
call _SendPacket
sw zero, $0014(s7)
daddu s1, zero, zero


strLimitNotReached:
lb v1, $0000(s6)
bne v1, zero, 3
nop
goto overLoad


goto __drawStringLoop


overLoad:

beq s1, zero, :emptyStr
nop

sw zero, $0000(s7)
sw zero, $0004(s7)
sw zero, $0008(s7)
sw zero, $000C(s7)
sw zero, $0010(s7)
sw zero, $0014(s7)
//sw zero, $0018(s7)
//sw zero, $001C(s7)

daddu a0, s0, zero
daddu a1, s1, zero
call _SendPacket // Send the string

emptyStr:
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
_GetSpacing:

addiu sp, sp, $FFF0
sq s0, $0000(sp)

daddu s0, a0, zero //Byte

//Rid possible FF's
sll s0, s0, 24
srl s0, s0, 24

//Load spacing from 8002FE00 + byte
lui t0, $8002
ori t0, t0, $FE00
addu t1, t0, s0
lb v0, $0000(t1) //Return spacing

//Rid possible FF's
sll v0, v0, 24
srl v0, v0, 24

lq s0, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_drawMenuControls:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

daddu s0, a0, zero

// Text (Menu Controls)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4580
addiu t0, zero, 382
addiu t1, zero, 100
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

beq s0, zero, :_drawMenuControls_Main
nop
addiu a0, zero, 1
beq s0, a0, :_drawMenuControls_Browser
nop
addiu a0, zero, 2
beq s0, a0, :_drawMenuControls_Goto
nop
addiu a0, zero, 3
beq s0, a0, :_drawMenuControls_Edit
nop
addiu a0, zero, 4
beq s0, a0, :_drawMenuControls_LineSelect
nop
addiu a0, zero, 5
beq s0, a0, :_drawMenuControls_Codes
nop

goto _drawMenuControls_Leave

_drawMenuControls_Main:



goto _drawMenuControls_Leave
_drawMenuControls_Browser:

// Text (Up Arrow)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4590
addiu t0, zero, 334
addiu t1, zero, 120
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Down Arrow)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $45A8
addiu t0, zero, 322
addiu t1, zero, 140
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Left Arrow)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $45C0
addiu t0, zero, 346
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Right Arrow)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $45D4
addiu t0, zero, 328
addiu t1, zero, 180
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (R1)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $45EC
addiu t0, zero, 406
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Square)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $46D8
addiu t0, zero, 334
addiu t1, zero, 220
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Triangle)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $45F8
addiu t0, zero, 346
addiu t1, zero, 240
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

goto _drawMenuControls_Leave
_drawMenuControls_Goto:

// Text (Left Arrow - Left 1 Digit)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $463C
addiu t0, zero, 290
addiu t1, zero, 120
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Right Arrow - Right 1 Digit)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4658
addiu t0, zero, 272
addiu t1, zero, 140
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Up Arrow - Increment 1 Bit)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4674
addiu t0, zero, 284
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Down Arrow - Decrement 1 Bit)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4690
addiu t0, zero, 272
addiu t1, zero, 180
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Cross - Go To Address)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $46B0
addiu t0, zero, 314
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Circle - Cancel)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $46C8
addiu t0, zero, 350
addiu t1, zero, 220
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString


goto _drawMenuControls_Leave
_drawMenuControls_Edit:

// Text (Left Arrow - Left 1 Digit)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $463C
addiu t0, zero, 290
addiu t1, zero, 120
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Right Arrow - Right 1 Digit)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4658
addiu t0, zero, 272
addiu t1, zero, 140
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Up Arrow - Increment 1 Bit)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4674
addiu t0, zero, 284
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Down Arrow - Decrement 1 Bit)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4690
addiu t0, zero, 272
addiu t1, zero, 180
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString



// Text (R1 - Save Data)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $46F0
addiu t0, zero, 356
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Triangle - Cancel)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4700
addiu t0, zero, 338
addiu t1, zero, 220
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString


goto _drawMenuControls_Leave
_drawMenuControls_LineSelect:

// Text (Up Arrow: Line Up 1)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4714
addiu t0, zero, 326
addiu t1, zero, 120
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Down Arrow: Line Down 1)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4728
addiu t0, zero, 302
addiu t1, zero, 140
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Cross: Edit Line)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4740
addiu t0, zero, 344
addiu t1, zero, 160
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (Circle: Go Back)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4754
addiu t0, zero, 350
addiu t1, zero, 180
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Text (L1: Install Reg Dumper)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4994
addiu t0, zero, 320
addiu t1, zero, 200
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

goto _drawMenuControls_Leave
_drawMenuControls_Codes:

goto _drawMenuControls_Leave





_drawMenuControls_Leave:
lw ra, $0000(sp)
lw s0, $0004(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_drawWatermark:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

// Water mark (CL-LiveDebug v3)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4500
addiu t0, zero, 224
addiu t1, zero, 0
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Water mark (Created by: Gtlcpimp)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4510
addiu t0, zero, 200
addiu t1, zero, 20
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

// Water mark (Visit us at www.Cheaters-Lounge.net)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4528
addiu t0, zero, 110
addiu t1, zero, 40
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
addiu t6, zero, $0000
call _drawString

lw ra, $0000(sp)
lw s0, $0004(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_initScreen:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

// Flush screen & Initialize draw space
addiu s0, zero, 50
_initScreenFullFlush:
jal :_SetDisplayBuffer
nop
jal :_SetDrawBuffer
lui a0, $0009
jal :_SetZBuffer
lui a0, $0009
addu a1, zero, zero
addu a2, zero, zero
addu a3, zero, zero
jal :_ClearBuffer
lui a0, $0009

//This is a work in progress and may not be possible
//jal :_SetScreenWH
//nop

jal :vSync
nop
bne s0, zero, :_initScreenFullFlush
addiu s0, s0, -1

lw ra, $0000(sp)
lw s0, $0004(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
vSync:
addiu sp, sp, $fff0
sd fp, $0000(sp)

daddu fp, sp, zero
lui v0, $1200
ld v0, $1000(v0)
addiu v1, zero, $0008
or v0, v0, v1
lui at, $1200
sd v0, $1000(at)
lui v0, $1200
ld v0, $1000(v0)
addiu v1, zero, $0008
and v0, v0, v1
nop
beq v0, zero, -6
nop
lui v0, $1200
ld v0, $1000(v0)
lui at, $1200
sd v0, $1000(at)
daddu sp, fp, zero

ld fp, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_ClearBuffer:

addiu sp, sp, $FFE0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)
sw s2, $000C(sp)
sw s3, $0010(sp)

daddu s0, a0, zero // Packet
daddu s1, a1, zero // R
daddu s2, a2, zero // G
daddu s3, a3, zero // B

// *(u64*)pckt = GIF_SET_TAG(6, 1, 0, 0, 0, 1);
lui t0, $1000
ori v0, zero, $8006
dsll32 t0, t0, 0
daddu t0, t0, v0

// *(u64*)pckt = 0xE;
addiu t1, zero, $000E

// *(u64*)pckt = GIF_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1);
lui t2, $0003

// *(u64*)pckt = GIF_REG_TEST_1;
addiu t3, zero, $0047

// *(u64*)pckt = GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0);
addiu t4, zero, $0006

// *(u64*)pckt = GIF_REG_PRIM;
daddu t5, zero, zero

// *(u64*)pckt = GIF_SET_RGBAQ(0x12, 0x34, 0x56, 0x80, 0x3F800000);
lui t6, $3F80
addiu t6, t6, 1
addiu v0, zero, $0080
sll v0, v0, 8
addu v0, v0, s3
sll v0, v0, 8
addu v0, v0, s2
sll v0, v0, 8
addu v0, v0, s1
dsll32 t6, t6, 0
daddu t6, t6, v0

// *(u64*)pckt = GIF_REG_RGBAQ;
addiu t7, zero, $0001

// *(u64*)pckt = GIF_SET_XYZ(0x0000, 0x0000, 0x0000);
daddu t8, zero, zero

// *(u64*)pckt = GIF_REG_XYZ2;
addiu a0, zero, $0005

// *(u64*)pckt = GIF_SET_XYZ(0xFFFF, 0xFFFF, 0x0000);
addiu a1, zero, 1
dsll32 a1, a1, 0
daddiu a1, a1, -1

// *(u64*)pckt = GIF_REG_XYZ2;
addiu a2, zero, $0005

// *(u64*)pckt = GIF_SET_TEST(0, 0, 0, 0, 0, 0, 1, 2);
lui a3, zero, $0005

// *(u64*)pckt = GIF_REG_TEST_1;
addiu v0, zero, $0047

sd t0, $0000(s0)
sd t1, $0008(s0)
sd t2, $0010(s0)
sd t3, $0018(s0)
sd t4, $0020(s0)
sd t5, $0028(s0)
sd t6, $0030(s0)
sd t7, $0038(s0)
sd t8, $0040(s0)
sd a0, $0048(s0)
sd a1, $0050(s0)
sd a2, $0058(s0)
sd a3, $0060(s0)
sd v0, $0068(s0)

daddu a0, s0, zero
jal :_SendPacket
addiu a1, zero, 112

lw ra, $0000(sp)
lw s0, $0004(sp)
lw s1, $0008(sp)
lw s2, $000C(sp)
lw s3, $0010(sp)
jr ra
addiu sp, sp, $0020

//==========================================================
_SetDisplayBuffer:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

lui s0, $1200

addiu t0, zero, $1400
sd t0, $0070(s0)
sd t0, $0090(s0)

lw s0, $0004(sp)
lw ra, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_SetScreenWH:
addiu sp, sp, $FFF0
sw a0, $0000(sp)
sw v0, $0004(sp)

lui v1, $1200
lui a0, $1200
lui a1, $1200
lui a2, $001b
ori a2, a2, $f9ff
dsll a2, a2, 16
ori a2, a2, $0183
dsll a2, a2, 16
ori a2, a2, $2278
ori v1, v1, $1000
ori a0, a0, $0080
ori a1, a1, $00a0
addiu v0, zero, $0300
sd v0, $0000(v1)
sd a2, $0000(a0)
sd a2, $0000(a1)

lw v0, $0004(sp)
lw a0, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_SetDrawBuffer:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

daddu s0, a0, zero // Packet

// *(u64*)pckt = GIF_SET_TAG(5, 1, 0, 0, GIF_TAG_PACKED, 1);
lui t0, $1000
ori v0, zero, $8005
dsll32 t0, t0, 0
daddu t0, t0, v0

// *(u64*)pckt = 0xE;
addiu t1, zero, $000E

// *(u64*)pckt = GIF_SET_FRAME(0, 10, 32, 0);
lui t2, $200A

// *(u64*)pckt = GIF_REG_FRAME_1;
addiu t3, zero, $004C

// *(u64*)pckt = GIF_SET_SCISSOR(0, 639, 0, 447);
lui t4, $01BF
lui v0, $027F
dsll32 t4, t4, 0
daddu t4, t4, v0

// *(u64*)pckt = GIF_REG_SCISSOR_1;
addiu t5, zero, $0040

// *(u64*)pckt = GIF_SET_TEST(0, 0, 0, 0, 0, 0, 1, 2);
lui t6, $0005

// *(u64*)pckt = GIF_REG_TEST_1;
addiu t7, zero, $0047

// *(u64*)pckt = GIF_SET_XYOFFSET(1728 << 4, 1824 << 4);
addiu t8, zero, $7200
addiu v0, zero, $6C00
dsll32 t8, t8, 0
daddu t8, t8, v0

// *(u64*)pckt = GIF_REG_XYOFFSET_1;
addiu a0, zero, $0018

// *(u64*)pckt = GIF_SET_PRMODECONT(1);
addiu a1, zero, $0001

// *(u64*)pckt = GIF_REG_PRMODECONT;
addiu a2, zero, $001A

sd t0, $0000(s0)
sd t1, $0008(s0)
sd t2, $0010(s0)
sd t3, $0018(s0)
sd t4, $0020(s0)
sd t5, $0028(s0)
sd t6, $0030(s0)
sd t7, $0038(s0)
sd t8, $0040(s0)
sd a0, $0048(s0)
sd a1, $0050(s0)
sd a2, $0058(s0)

daddu a0, s0, zero
jal :_SendPacket
addiu a1, zero, 96

lw s0, $0004(sp)
lw ra, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_SetZBuffer:
addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

daddu s0, a0, zero

// *(u64*)pckt = GIF_SET_TAG(1, 1, 0, 0, 0, 1);
lui t0, $1000
dsll32 t0, t0, 0
lui v0, $0000
ori v0, v0, $8001
daddu t0, t0, v0

// *(u64*)pckt = 0xE;
daddiu t1, zero, $000e

// *(u64*)pckt = GIF_SET_ZBUF(0x96, 32, 0);
addiu t2, zero, $0096

// *(u64*)pckt = GIF_REG_ZBUF_1;
addiu t3, zero, $004E

sd t0, $0000(s0)
sd t1, $0008(s0)
sd t2, $0010(s0)
sd t3, $0018(s0)

daddu a0, s0, zero
jal :_SendPacket
addiu a1, zero, 32

lw s0, $0004(sp)
lw ra, $0000(sp)
jr ra
addiu sp, sp, $0010


//==========================================================
_AddPixel:

addiu sp, sp, $FF50
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)
sq s4, $0050(sp)
sq s5, $0060(sp)
sq s6, $0070(sp)
sq s7, $0080(sp)
sq k0, $0090(sp)
sq k1, $00A0(sp)

addu s0, t0, t7   // s0 = (Packet + Size)
daddu s1, t1, zero // s1 = x
daddu s2, t2, zero // s2 = y
daddu s3, t3, zero // s3 = z
daddu s4, t4, zero // s4 = r
daddu s5, t5, zero // s5 = g
daddu s6, t6, zero // s6 = b
daddu s7, t7, zero // s7 = Size
daddu k0, t8, zero // k0 = w
daddu k1, t9, zero // k1 = h

daddu t0, zero, zero
daddu t1, zero, zero
daddu t2, zero, zero
daddu t3, zero, zero
daddu t4, zero, zero
daddu t5, zero, zero
daddu t6, zero, zero
daddu t7, zero, zero
daddu t8, zero, zero
daddu t9, zero, zero

daddiu s1, s1, 1732 // x += Left
daddiu s2, s2, 1829 // y += Top


// GIF_SET_TAG(4, 1, 0, 0, 0, 1);
lui t0, $1000
dsll32 t0, t0, 0
ori t0, t0, $8004

// 0x0E
ori t1, zero, $000E

// GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0);
ori t2, zero, $0006

// GIF_REG_PRIM;
daddu t3, zero, zero

// GIF_SET_RGBAQ(r, g, b, 0x80, 0x3F800000);
daddiu v0, zero, $0080
sll v0, v0, 8
daddu v0, v0, s6 // b
sll v0, v0, 8
daddu v0, v0, s5 // g
sll v0, v0, 8
daddu v0, v0, s4 // r
lui t4, $3f80
dsll32 t4, t4, 0
slti v1, v0, 0
beq v1, zero, 4
nop
ori v1, zero, 1
dsll32 v1, v1, 0
daddu v0, v0, v1
daddu t4, t4, v0

// GIF_REG_RGBAQ;
ori t5, zero, $0001

// GIF_SET_XYZ( NewX << 4, NewY << 4, z);
sll v0, s2, 16     // NewY
daddu v0, v0, s1   // NewX
sll v0, v0, 4
daddu t6, s3, zero // z
dsll32 t6, t6, 0
slti v1, v0, 0
beq v1, zero, 4
nop
ori v1, zero, 1
dsll32 v1, v1, 0
daddu v0, v0, v1
daddu t6, t6, v0

// GIF_REG_XYZ2;
ori t7, zero, $0005

// GIF_SET_XYZ( (NewX + w) << 4, (NewY + h) << 4, z);
daddu v0, s2, k1  // NewY += h
sll v0, v0, 16
daddu v0, v0, s1
daddu v0, v0, k0  // NewX += w
sll v0, v0, 4
daddu t8, s3, zero // z
dsll32 t8, t8, 0
slti v1, v0, 0
beq v1, zero, 4
nop
ori v1, zero, 1
dsll32 v1, v1, 0
daddu v0, v0, v1
daddu t8, t8, v0

// GIF_REG_XYZ2;
ori t9, zero, $0005


sd t0, $0000(s0)
sd t1, $0008(s0)
sd t2, $0010(s0)
sd t3, $0018(s0)
sd t4, $0020(s0)
sd t5, $0028(s0)
sd t6, $0030(s0)
sd t7, $0038(s0)
sd t8, $0040(s0)
sd t9, $0048(s0)


addiu v0, zero, 80

lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
lq s4, $0050(sp)
lq s5, $0060(sp)
lq s6, $0070(sp)
lq s7, $0080(sp)
lq k0, $0090(sp)
lq k1, $00A0(sp)
jr ra
addiu sp, sp, $00B0

//==========================================================
// Modification from PS2SDK

_SendPacket:
// a0 = &packet; a1 = size;

addiu sp, sp, $FFD0
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)

daddu s0, a0, zero // s0 = &Packet
daddu s1, a1, zero // s1 = size

lui a0, $1001
addiu a0, a0, $A000

_wait_channel_ready:

lw v0, $0000(a0)
addiu v1, zero, $0100
and v1, v1, v0

nop
nop
nop
nop
nop
nop
nop
nop

beq v1, zero, 3
nop
goto _wait_channel_ready

// Set transfer intitialize data
addiu t0, zero, $0181

// Set memory address data
addu t1, s0, zero

// Set size in quadwords data
addiu t2, s1, 15
srl t2, t2, 4
ori v0, zero, $FFFF
and t2, t2, v0


// Operate SyncDCashe
addiu sp, sp, $FFB0
sq a0, $0000(sp)
sq t0, $0010(sp)
sq t1, $0020(sp)
sq t2, $0030(sp)

mfc0 v0, $Status
lui v1, $1000
and v0, v0, v1
sw v0, $0040(sp)
bne v0, zero, 3
nop
goto _SyncDCache

_DI:

mfc0 v1, $Status
lui v0, $0001
and v1, v1, v0
daddu a0, zero, zero
beq v1, zero, 12
sltu a1, zero, v1
lui v0, $0001
nop
hexcode $42000039 // di
sync.p
mfc0 v1, $Status
nop
and v1, v1, v0
nop
bne v1, zero, -7
nop
daddu a0, a1, zero
nop
daddu v0, a0, zero

_SyncDCache:
// Operate _SyncDCache
//lui a0, $0009 // Data Start
daddu a0, s0, zero
daddu a1, a0, s1 // Data Stop
addiu v0, zero, $FFC0
and a0, a0, v0
and a1, a1, v0

lui a3, $ffff
daddu a2, zero, zero
ori a3, a3, $F000
nop

_SyncDCacheLoop:
sync
hexcode $bcd00000
sync
mfc0 v0, $TagLo
and v0, v0, a3
addu v0, v0, a2
sltu v1, a1, v0
sltu v0, v0, a0
bne v0, zero, 6
nop
bne v1, zero, 4
nop
sync
hexcode $bcd40000
sync
sync
hexcode $bcd00001
sync
mfc0 v0, $TagLo
and v0, v0, a3
addu v0, v0, a2
sltu v1, a1, v0
sltu v0, v0, a0
bne v0, zero, 6
nop
bne v1, zero, 4
nop
sync
hexcode $bcd40001
sync
sync
addiu a2, a2, $0040
slti v0, a2, $1000
beq v0, zero, 3
nop
goto _SyncDCacheLoop


// EI(); (if DI()'ed)

lw v0, $0040(sp)
bne v0, zero, 3
nop
goto _SyncDCache_Exit

_EI:

mfc0 v0, $Status
lui v1, $0001
and v0, v0, v1
hexcode $42000038
sltu v0, zero, v0

_SyncDCache_Exit:
lq a0, $0000(sp)
lq t0, $0010(sp)
lq t1, $0020(sp)
lq t2, $0030(sp)
addiu sp, sp, $0050

sw t1, $0010(a0) // d2_maddr
sw t2, $0020(a0) // d2_qwc

sw t0, $0000(a0) // d2_chcr


lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
jr ra
addiu sp, sp, $0030

//==========================================================
// Loops caused it to slow
// Tagging the if statements increased speed and accuracy
// a0 = read addr; a1 = store addr; a2 = len;
_lineToHex:

addiu sp, sp, $FFF0
sq ra, $0000(sp)

lw a2, $0000(a0)
srl a0, a2, 28
call _chrToHex

sb v0, $0000(a1)

sll a0, a0, 28
subu a2, a2, a0
srl a0, a2, 24
call _chrToHex

sb v0, $0001(a1)

sll a0, a0, 24
subu a2, a2, a0
srl a0, a2, 20
call _chrToHex

sb v0, $0002(a1)

sll a0, a0, 20
subu a2, a2, a0
srl a0, a2, 16
call _chrToHex

sb v0, $0003(a1)

sll a0, a0, 16
subu a2, a2, a0
srl a0, a2, 12
call _chrToHex

sb v0, $0004(a1)

sll a0, a0, 12
subu a2, a2, a0
srl a0, a2, 8
call _chrToHex

sb v0, $0005(a1)

sll a0, a0, 8
subu a2, a2, a0
srl a0, a2, 4
call _chrToHex

sb v0, $0006(a1)

sll a0, a0, 4
subu a2, a2, a0
addu a0, a2, zero
call _chrToHex

sb v0, $0007(a1)

sb zero, $0008(a1)

/*
addu a1, a1, a2
addiu a1, a1, -2
sb zero, $0002(a1)
_strtohexloop:
lb v0, $0000(a0)
srl v1, v0, 4
sll v1, v1, 4
subu v0, v0, v1
srl v1, v1, 4


slti t0, v0, $000a
bne t0, zero, 2
nop
addiu v0, v0, $0007

slti t0, v1, $000a
bne t0, zero, 2
nop
addiu v1, v1, $0007

addiu v0, v0, $0030
addiu v1, v1, $0030

sb v1, $0000(a1)
sb v0, $0001(a1)

addiu a1, a1, -2
addiu a0, a0, 1

addiu a2, a2, -2
beq a2, zero, 3
nop
goto _strtohexloop
*/

lq ra, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_chrToHex:

addiu v1, zero, $0000
bne v1, a0, 4
nop
addiu v0, zero, $0030
jr ra
nop

addiu v1, zero, $0001
bne v1, a0, 4
nop
addiu v0, zero, $0031
jr ra
nop

addiu v1, zero, $0002
bne v1, a0, 4
nop
addiu v0, zero, $0032
jr ra
nop

addiu v1, zero, $0003
bne v1, a0, 4
nop
addiu v0, zero, $0033
jr ra
nop

addiu v1, zero, $0004
bne v1, a0, 4
nop
addiu v0, zero, $0034
jr ra
nop

addiu v1, zero, $0005
bne v1, a0, 4
nop
addiu v0, zero, $0035
jr ra
nop

addiu v1, zero, $0006
bne v1, a0, 4
nop
addiu v0, zero, $0036
jr ra
nop

addiu v1, zero, $0007
bne v1, a0, 4
nop
addiu v0, zero, $0037
jr ra
nop

addiu v1, zero, $0008
bne v1, a0, 4
nop
addiu v0, zero, $0038
jr ra
nop

addiu v1, zero, $0009
bne v1, a0, 4
nop
addiu v0, zero, $0039
jr ra
nop

addiu v1, zero, $000a
bne v1, a0, 4
nop
addiu v0, zero, $0041
jr ra
nop

addiu v1, zero, $000b
bne v1, a0, 4
nop
addiu v0, zero, $0042
jr ra
nop

addiu v1, zero, $000c
bne v1, a0, 4
nop
addiu v0, zero, $0043
jr ra
nop

addiu v1, zero, $000d
bne v1, a0, 4
nop
addiu v0, zero, $0044
jr ra
nop

addiu v1, zero, $000e
bne v1, a0, 4
nop
addiu v0, zero, $0045
jr ra
nop

addiu v1, zero, $000f
bne v1, a0, 4
nop
addiu v0, zero, $0046
jr ra
nop


addiu v0, zero, $0030
jr ra
nop
