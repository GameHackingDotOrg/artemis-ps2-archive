
//============================================================
/*
Input:
   a0 = Result Address
Output:
   v0 = # of Results
*/
_StackTrace:

addiu sp, sp, $FF70
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)
sq s4, $0050(sp)
sq s5, $0060(sp)
sq s6, $0070(sp)
sq s7, $0080(sp)

addiu s0, sp, $0090
daddu s1, a0, zero
daddu s2, zero, zero

setreg s3, $27BD0000 // s3 = addiu sp, sp, $0000
setreg s4, $7FBF0000 // s4 = sq ra, $0000(sp)
setreg s5, $FFBF0000 // s5 = sd ra, $0000(sp)
setreg s6, $AFBF0000 // s6 = sw ra, $0000(sp)

daddu a0, ra, zero
daddu a1, zero, zero



_StackTraceLoop:

lui v0, $0200
slt v0, s0, v0
beq v0, zero, :_StackTraceLoopExit
nop

/*
Store Rules:
   0000 = Parent Function
   0004 = Hook (ra - 8)
*/

addiu v0, a0, -8
sw v0, $0004(s1)

_STLTopLoop:

lw v0, $0000(a0)
lui v1, $FFFF
and v0, v0, v1
beq v0, s3, :_STL_Found_SP
nop
beq v0, s4, :_STL_Found_RA
nop
beq v0, s5, :_STL_Found_RA
nop
beq v0, s6, :_STL_Found_RA
nop
beq zero, zero, :_STL_NoFind
nop
_STL_Found_SP:

lh v0, $0000(a0)

slti v1, v0, 0
beq v1, zero, :_STL_NoFind
nop


lui v1, $0001
subu v0, v1, v0
dsll32 v0, v0, 16
dsrl32 v0, v0, 16
addu s0, s0, v0
sw a0, $0000(s1)

lui v0, $0008
slt v0, a1, v0
bne v0, zero, :_STL_ERROR
nop
lui v0, $0200
slt v0, a1, v0
beq v0, zero, :_STL_ERROR
nop

addiu v0, a1, -8
sw v0, $000C(s1)
daddu a0, a1, zero
addiu s1, s1, 8
addiu s2, s2, 1

beq zero, zero, :_STL_Found_TOP
nop

_STL_ERROR:
sw a1, $0010(s1)
addiu v1, zero, -1
sw v1, $0014(s1)
beq zero, zero, :_StackTraceLoopExit
nop


_STL_Found_RA:

lh v0, $0000(a0)
addu v0, s0, v0
lw a1, $0000(v0)
//addiu v0, a1, -8
//sw v0, $0004(s1)

_STL_NoFind:
beq zero, zero, :_STLTopLoop
addiu a0, a0, -4

_STL_Found_TOP:
beq zero, zero, :_StackTraceLoop
nop
_StackTraceLoopExit:

daddu v0, s2, zero

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
addiu sp, sp, $0090

