/*
	CodeDesigner v2.0
	Created by: Gtlcpimp
	Official CheatersLounge Product Copyright ©
*/

//============================================================
address $000c0000
debug:

lui sp, $0200


addiu sp, sp, $FFF0
sq ra, $0000(sp)
jal :debug_1
nop
lq ra, $0000(sp)
jr ra
addiu sp, sp, $0010

debug_1:
addiu sp, sp, $FFF0
sq ra, $0000(sp)
jal :debug_2
nop
lq ra, $0000(sp)
jr ra
addiu sp, sp, $0010

debug_2:
addiu sp, sp, $FFF0
sq ra, $0000(sp)
jal :debug_3
nop
lq ra, $0000(sp)
jr ra
addiu sp, sp, $0010

debug_3:
addiu sp, sp, $FFF0
sq ra, $0000(sp)


jal :Stack.TraceJAL
lui a0, $000d

beq zero, zero, -1
nop

lq ra, $0000(sp)
jr ra
addiu sp, sp, $0010


//============================================================
/*
Input:
   a0 = Result Address (64 bit, fnc;;jal)
Output:
   v0 = # of Results
*/
Stack.TraceJAL:

addiu sp, sp, $FF80
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)

daddu s0, a0, zero
daddu s1, ra, zero
addiu s2, sp, $0080
daddu s3, zero, zero

_s.tJAL_loop:
lw v0, $FFF8(s1)
dsll32 v0, v0, 8
dsrl32 v0, v0, 8
sll v0, v0, 2
sw v0, $0000(s0)
sw s2, $0004(s0)
addiu s3, s3, 1

beq v0, zero, :_s.tJAL_loopExit
nop

lui v0, $0200
slt v0, s2, v0
beq v0, zero, :_s.tJAL_loopExit
daddu v0, s3, zero

daddu a0, s1, zero
jal :Stack.Trace.FindSxRA_wSP
daddu a1, s2, zero

daddu s1, v0, zero
daddu s2, v1, zero
beq zero, zero, :_s.tJAL_loop
addiu s0, s0, 8
_s.tJAL_loopExit:


_s.tJAL_Exit:
lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
jr ra
addiu sp, sp, $0080

//============================================================
/*
Input:
   a0 = Start Address
   a1 = Stack Pointer
Output:
   v0 = Return Address
   v1 = New Stack Pointer
*/
Stack.Trace.FindSxRA_wSP:

addiu sp, sp, $FF80
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)

daddu s0, a0, zero
daddu s1, a1, zero
daddu s2, zero, zero

_s.t.fsRAwSP_Loop:
lh v0, $0002(s0)
dsll32 v0, v0, 16
dsrl32 v0, v0, 16

ori v1, zero, $7FBF
beq v0, v1, :_s.t.fsRAwSP_FoundRA // sq ra, $xxxx(sp)
ori v1, zero, $FFBF
beq v0, v1, :_s.t.fsRAwSP_FoundRA // sd ra, $xxxx(sp)
ori v1, zero, $AFBF
beq v0, v1, :_s.t.fsRAwSP_FoundRA // sw ra, $xxxx(sp)
ori v1, zero, $27BD
beq v0, v1, :_s.t.fsRAwSP_FoundSP // addiu sp, sp, $xxxx
nop
beq zero, zero, :_s.t.fsRAwSP_LoopNext
nop

_s.t.fsRAwSP_FoundRA:
lh v0, $0000(s0)
daddu v0, s1, v0
beq zero, zero, :_s.t.fsRAwSP_LoopNext
lw s2, $0000(v0)
_s.t.fsRAwSP_FoundSP:

lh v0, $0000(s0)
dsll32 v0, v0, 16
dsrl32 v0, v0, 16
lui v1, $0001
subu v1, v1, v0
daddu v1, s1, v1
beq zero, zero, :_s.t.fsRAwSP_LoopExit
daddu v0, s2, zero

_s.t.fsRAwSP_LoopNext:
beq zero, zero, :_s.t.fsRAwSP_Loop
addiu s0, s0, -4
_s.t.fsRAwSP_LoopExit:


_s.t.fsRAwSP_Exit:
lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
jr ra
addiu sp, sp, $0080

//============================================================
/*
Input:
   a0 = Result Address (64 bit, sp->ra)
Output:
   v0 = # of Results
*/
Stack.TraceRA:

