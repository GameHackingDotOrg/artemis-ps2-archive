/*
	CodeDesigner v2.0
	Created by: Gtlcpimp
	Official CheatersLounge Product Copyright ©
*/
/*
  CL-LiveDebug v3 Cheat Engine v0.1a
  Created by: Gtlcpimp
  CheatersLounge Copyright ©
*/

address $0007F800


//==========================================================
_CheatEngineMain:

addiu sp, sp, $FF00
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)
sq s4, $0050(sp)
sq s5, $0060(sp)
sq s6, $0070(sp)
sq s7, $0080(sp)

lui s0, $8004
ori s0, s0, $7810
addiu s1, zero, 512

_CheatEngineMainLoop:
beq s1, zero, :_CheatEngineExit
nop

lb v0, $0000(s0)
beq v0, zero, :_CheatEngineExit
nop

lb v0, $001C(s0)
addiu v1, zero, 1
bne v0, v1, 3
nop
jal :_WriteCode
addu a0, s0, zero

lw s2, $0010(s0) // Address

beq s2, zero, :_CheatEngineExit
nop

addiu s0, s0, $0020
addiu s1, s1, -1
beq zero, zero, :_CheatEngineMainLoop
nop

_CheatEngineExit:
lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
lq s4, $0050(sp)
lq s5, $0060(sp)
lq s6, $0070(sp)
lq s7, $0080(sp)
jr ra
addiu sp, sp, $0100

//==========================================================
_WriteCode:
addiu sp, sp, $FF00
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)
sq s4, $0050(sp)
sq s5, $0060(sp)
sq s6, $0070(sp)
sq s7, $0080(sp)

addu s0, a0, zero

lw s1, $0010(s0) // Address
lw s2, $0014(s0) // Data ON

srl a0, s1, 25 // a0 = Command
sll v0, a0, 25
subu s1, s1, v0
srl a0, v0, 24

beq a0, zero, :_WC_0_Bits
nop

addiu v0, zero, $0010
beq a0, v0, :_WC_16_Bits
nop

addiu v0, zero, $0020
beq a0, v0, :_WC_32_Bits
nop

beq zero, zero, :_WC_Exit
nop


//-----------------------------------
_WC_0_Bits:

sb s2, $0000(s1)

beq zero, zero, :_WC_Exit
nop

//-----------------------------------
_WC_16_Bits:

sh s2, $0000(s1)

beq zero, zero, :_WC_Exit
nop
//-----------------------------------
_WC_32_Bits:

sw s2, $0000(s1)

beq zero, zero, :_WC_Exit
nop


_WC_Exit:
lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
lq s4, $0050(sp)
lq s5, $0060(sp)
lq s6, $0070(sp)
lq s7, $0080(sp)
jr ra
addiu sp, sp, $0100
