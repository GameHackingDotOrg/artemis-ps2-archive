/*
	CodeDesigner v2.0
	Created by: Gtlcpimp
	Official CheatersLounge Product Copyright ©
*/
/*
    Data Cache Class
*/

//============================================================
// SyncDCache
/*
Input:
   a0 = &Data
   a1 = Size
*/
SyncDCache:


addiu sp, sp, $FFE0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)
sw s2, $000C(sp)
sw s3, $0010(sp)

daddu s0, a0, zero
daddu s1, a0, a1
daddu s3, zero, zero

mfc0 s2, $Status
lui v0, $0001
and s2, s2, v0

//-------------------------------------- DI
beq s2, zero, :_SyncDCache_SkipDI
nop

addiu s3, zero, 1

_SyncDCache_DI_Loop:

hexcode $42000039 // di
sync.p

mfc0 s2, $Status
nop
and s2, s2, v0
nop
bne s2, zero, :_SyncDCache_DI_Loop
nop


_SyncDCache_SkipDI:

//-------------------------------------- call opDCache
daddu a0, s0, zero
daddu a1, s1, zero
addiu v0, zero, $FFC0
and a0, a0, v0
jal :opDCache
and a1, a1, v0


//-------------------------------------- EI
beq s3, zero, :_SyncDCache_SkipEI
nop

hexcode $42000038

_SyncDCache_SkipEI:
//-------------------------------------- Done


lw ra, $0000(sp)
lw s0, $0004(sp)
lw s1, $0008(sp)
lw s2, $000C(sp)
lw s3, $0010(sp)
jr ra
addiu sp, sp, $0020

//============================================================
// opDCache
/*
Input:
   a0 = &Data
   a1 = Size
*/
opDCache:

addiu sp, sp, $FFE0
sq s0, $0000(sp)
sq s1, $0010(sp)

daddu s0, a0, zero
daddu s1, a0, a1

daddu a2, zero, zero
lui a3, $FFFF
ori a3, a3, $F000

//--------------------------------------
_opDCache_Loop:


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

bne v0, zero, :_opDCache_Loop
nop

lq s0, $0000(sp)
lq s1, $0010(sp)
jr ra
addiu sp, sp, $0020
