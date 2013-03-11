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
jalr k0
nop
call _start
lq ra, $0000(sp)
addiu sp, sp, $0010

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

lui t1, $8008
lw t1, $E7FC(t1)

bne t1, zero, 3
nop

jal $0007E880
nop

jal $0007F800
nop

jal :_ReadPad
addiu a0, zero, $FFF9
beq v0, zero, :LDv3HookExit
nop

jal $00078250
daddu a0, zero, zero



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
