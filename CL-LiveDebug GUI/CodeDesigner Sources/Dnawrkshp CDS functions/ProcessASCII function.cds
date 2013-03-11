
//Written by Dnawrkshp
//Function to put 0x00's between each byte for ASCII tables
//Note: It outputs the text in reverse


address $00000000

setreg a0, $000AF000
setreg a1, $00084200
setreg a2, $00000010

daddu t1, a1, zero    //Location to store result
daddu t2, a2, zero    //Size
addiu t3, zero, $0020 //Space
addiu t5, zero, $0001 //Used for values < 0 (0x80 - 0xFF)
daddu t6, a0, zero    //Address for value to be loaded byte by byte

sq zero, $0000(t1) //reset

add t1, t1, t2
daddiu t6, a0, zero //Byte read address
//addiu t6, t6, -1
_ASCIILoop:
lb t0, $0000(t6) //Value
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
nop

address $000AF000
hexcode $31323334
hexcode $11223344



