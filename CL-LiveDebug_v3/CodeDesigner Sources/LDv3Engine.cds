/*
	CodeDesigner v2.0
	Created by: Gtlcpimp
	Official CheatersLounge Product Copyright ©
*/
/*
   CL-LiveDebug v3 Debugger Engine v3.2b
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
address $00078250

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
call _drawString


// Text (Codes)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $462C
addiu t0, zero, 290
addiu t1, zero, 80
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
call _drawString

// Text (Patches)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $4634
addiu t0, zero, 278
addiu t1, zero, 100
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
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
call _drawString

// Text (Return To Game)
lui a0, $0008
lw a0, $001C(a0)
lui a1, $8004
addiu a1, a1, $461C
addiu t0, zero, 236
addiu t1, zero, 140
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
call _drawString


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
sltiu v0, s5, $0004
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

addiu v0, zero, 4 // ----- Return to Game
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


lui v0, $0010
sw v0, $0090(sp)    // Start
sw v0, $0094(sp)    // Stop
sw zero, $0098(sp)  // Value 1
sw zero, $009C(sp)  // Value 2
sw zero, $00A0(sp)  // Value 3
sw zero, $00A4(sp)  // Value 4
addiu v0, zero, 2
sw v0, $00A8(sp)    // Bits

/*
0 - 8 Bit
1 - 16 Bit
2 - 32 Bit
3 - 64 Bit
4 - 128 Bit
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
call _drawString

addiu s2, zero, -1
daddu s3, s0, zero
_MemSearchMenuListingLoop:

lw a0, $001C(s1)
lui a1, $8004
addiu a1, a1, $479C // Start:
addiu t0, zero, 26
addiu t1, zero, 95
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
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
call _drawString

lui a1, $8004
lw v0, $00A8(sp)

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
call _drawString




lui a1, $8004
addiu a1, a1, $4794 // Search
lw a0, $001C(s1)
addiu t0, zero, 284
addiu t1, zero, 175
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
call _drawString

lui a1, $8004
addiu a1, a1, $47CC // Results
lw a0, $001C(s1)
addiu t0, zero, 278
addiu t1, zero, 195
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
call _drawString

lui a1, $8004
addiu a1, a1, $47D4 // Go Back
lw a0, $001C(s1)
addiu t0, zero, 278
addiu t1, zero, 215
daddu t2, s4, zero
addiu t3, zero, $dc
addiu t4, zero, $dc
addiu t5, zero, $dc
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

slti v0, s2, 6
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
bne k0, zero, 11
nop
addiu v0, zero, 3
bne v0, s5, :msmbrowsenotLeft
nop
lw v0, $00A8(sp)
beq v0, zero, 3
nop
addiu v0, v0, -1
sw v0, $00A8(sp)
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
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
bne k0, zero, 12
nop
addiu v0, zero, 3
bne v0, s5, :msmbrowsenotRight
nop
lw v0, $00A8(sp)
addiu v1, zero, 4
beq v0, v1, 3
nop
addiu v0, v0, 1
sw v0, $00A8(sp)
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
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

addiu v0, zero, 4
bne v0, s5, 5
nop
jal :_MemSearchProcess // jal -> Search
daddiu a0, sp, $0090
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
addiu v0, zero, 5
bne v0, s5, 5
nop
jal :_MemSearchResults // jal -> Results
nop
beq zero, zero, :_MemSearchMenuWaitNoInput
nop
addiu v0, zero, 6
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
bne v1, s4, 4
nop
sw v0, $0020(s5)
addiu s6, s6, 1
addiu s5, s5, 4

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
slti v0, s5, 2
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
addiu t9, zero, 30  // h
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
addiu t8, zero, 12  // w
addiu t9, zero, 20  // h
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
call _drawString

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

lui s0, $0071
addiu s0, s0, $575c

addiu v1, zero, -1
sh v1, $0000(s0)

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
bne v0, v1, 16 // if ty == 17
nop
sw zero, $0000(s7)
sw zero, $0004(s7)
lw s2, $000C(s7)
addiu s2, s2, 12
addiu s6, s6, 1
lb t1, $0000(s6)
addiu t2, zero, 324
multu t0, t1, t2
lw s5, $001C(s7)
addu s5, s5, t0
sw s2, $000c(s7)
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
