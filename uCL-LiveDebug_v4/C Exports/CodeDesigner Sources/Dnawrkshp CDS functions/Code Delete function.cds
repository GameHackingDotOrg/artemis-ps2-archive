//
//Written by Dnawrkshp
//You can use the emulator with this one
//Deletes the code from s0 (passed from s7 as a0 when called in debugger)
//Then moves everything after it up 0x20

setreg s0, $000AF000


addiu t0, s0, $0020
add t1, zero, zero

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
nop
nop

//CodeDesigner emulator test
address $000AF000
//Code 1
print "ABCD Number 1"
hexcode $00123456
nop
nop
nop

//Code 2
print "ABCD Number 2"
hexcode $00123456
nop
nop
nop

//Code 3
print "ABCD Number 3"
hexcode $00123456
nop
nop
nop

//Code 4
print "ABCD Number 4"
hexcode $00123456
nop
nop
nop
