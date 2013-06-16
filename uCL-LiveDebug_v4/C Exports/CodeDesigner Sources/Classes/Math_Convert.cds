/*
Math Conversion Class
Created by: Gtlcpimp
*/

//============================================================
// Byte to BIN
/*
Input:
   a0 = Byte
Output:
   v0 = Binary
*/
ByteToBIN:

daddu v0, zero, zero
addiu v1, zero, 7

dsll32 a0, a0, 24
dsrl32 a0, a0, 24
addiu a1, zero, 2
daddu a2, zero, zero

ByteToBIN_Loop:

div a0, a1

mflo a0

mfhi a3
sllv a3, a3, a2
addu v0, v0, a3

addiu a2, a2, 4
bne v1, zero, :ByteToBIN_Loop
addiu v1, v1, -1

jr ra
nop

//============================================================
// BIN To Byte
/*
Input:
   a0 = Binary
Output:
   v0 = Byte
*/
BINToByte:

addiu sp, sp, $FFF0
sq s0, $0000(sp)

daddu v0, zero, zero
addiu v1, zero, 7
addiu a1, zero, 2
addiu a2, zero, 28
dsll32 a0, a0, 0
dsrl32 a0, a0, 0

addiu a3, zero, 1

BINToByte_Loop:

dsll32 s0, a0, 28
dsrl32 s0, s0, 28

beq s0, zero, 2
nop
addu v0, v0, a3

srl a0, a0, 4
sll a3, a3, 1
bne v1, zero, :BINToByte_Loop
addiu v1, v1, -1

lq s0, $0000(sp)
jr ra
addiu sp, sp, $0010