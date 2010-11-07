/*
  Automatic Joker Scanner v1.0.2
  Created by: Gtlcpimp
  CodeDesigner v2.0 Source Code
*/

/*
{ 0000005a 00000000 00000000 0001ffff } -0x1C
{ 0000005a 0000ffff ffff0000 } -0x1C
{ 01161204 01000000 } +0x1C
{ 010a0604 01000000 } +0x1C
*/


//==============================================
// ScanJoker();
address $0007E880

_init:
addiu sp, sp, $FFE0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)
sw s2, $000C(sp)
sw s3, $0010(sp)
sw s4, $0014(sp)

// s0 = Scanner config table
// s1 = Scan start point
// s2 = Scan length
// s3 = Scanned index
// s4 = Scan stop point

lui s0, $8008
addiu s0, s0, $E800

lw s1, $0000(s0)
lw s2, $0004(s0)
lw s3, $0008(s0)
addu s4, s1, s2

addiu v0, zero, 4
beq s3, v0, :_ScanJokerQuit // Reached limit
nop

_MainScanLoop:

daddu a0, s1, zero
jal :_Compare
lw a1, $000C(s0)

beq v0, zero, :_ScanNotFound
nop

// Make sure it's not an existing one
lw t0, $0010(s0)
lw t1, $0014(s0)
lw t2, $0018(s0)
lw t3, $001C(s0)
beq t0, v0, :_ScanNotFound
nop
beq t1, v0, :_ScanNotFound
nop
beq t2, v0, :_ScanNotFound
nop
beq t3, v0, :_ScanNotFound
nop

// We found it, add to found list
sll a0, s3, 2
addiu a0, a0, $0010
addu a0, a0, s0
sw v0, $0000(a0)
addiu s3, s3, 1
sw s3, $0008(s0)

_ScanNotFound:
bne s1, s4, :_MainScanLoop
addiu s1, s1, 4

lui v0, $0100
slt v0, s1, v0
bne v0, zero, 2
nop
lui s1, $0010

sw s1, $0000(s0)


_ScanJokerQuit:
lw ra, $0000(sp)
lw s0, $0004(sp)
lw s1, $0008(sp)
lw s2, $000C(sp)
lw s3, $0010(sp)
lw s4, $0014(sp)
jr ra
addiu sp, sp, $0020

//==============================================
// Compare
_Compare:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)

daddu s0, a0, zero // Memory address
daddu s1, a1, zero // Pattern config address

// Type A
lw t0, $0000(s1)
lw t1, $0004(s1)

// Type B
lw t2, $0008(s1)
lw t3, $000c(s1)

// Type C
lw t4, $0010(s1)
lw t5, $0014(s1)
lw t6, $0018(s1)

// Type D
lw t7, $001c(s1)
lw t8, $0020(s1)
lw t9, $0024(s1)
lw v1, $0028(s1)

_CompareA:

lw v0, $0000(s0)
bne t0, v0, :_CompareB
nop
lw v0, $0004(s0)
bne t1, v0, :_CompareB
nop

addiu v0, s0, $001C


goto _CompareExit
_CompareB:

lw v0, $0000(s0)
bne t2, v0, :_CompareC
nop
lw v0, $0004(s0)
bne t3, v0, :_CompareC
nop

addiu v0, s0, $001C


goto _CompareExit
_CompareC:

lw v0, $0000(s0)
bne t4, v0, :_CompareD
nop
lw v0, $0004(s0)
bne t5, v0, :_CompareD
nop
lw v0, $0008(s0)
bne t6, v0, :_CompareD
nop

addiu v0, s0, $FFE6


goto _CompareExit
_CompareD:


lw v0, $0000(s0)
bne t7, v0, :_CompareE
nop
lw v0, $0004(s0)
bne t8, v0, :_CompareE
nop
lw v0, $0008(s0)
bne t9, v0, :_CompareE
nop
lw v0, $000C(s0)
bne v1, v0, :_CompareE
nop

addiu v0, s0, $FFE6


goto _CompareExit
_CompareE:

addu v0, zero, zero

_CompareExit:
lw ra, $0000(sp)
lw s0, $0004(sp)
lw s1, $0008(sp)
jr ra
addiu sp, sp, $0010

//==============================================
// Scanner Configuration
address $0007E800

hexcode $00100000 // Start point
hexcode $00001000 // Scan size
hexcode $00000000 // Index
hexcode $8007E820 // Defined patterns address
hexcode $00000000 // Store slot 1
hexcode $00000000 // Store slot 2
hexcode $00000000 // Store slot 3
hexcode $00000000 // Store slot 4

//==============================================
// Defined Patterns
address $0007E820

// Type A - Half 0 { + 0x1c }
hexcode $04121601
hexcode $00000001

// Type B - Half 0 { + 0x1c }
hexcode $04060a01
hexcode $00000001

// Type C - Half 2 { - 0x1c }
hexcode $5a000000
hexcode $ffff0000
hexcode $0000ffff

// Type D - Half 2 { - 0x1c }
hexcode $5a000000
hexcode $00000000
hexcode $00000000
hexcode $ffff0100