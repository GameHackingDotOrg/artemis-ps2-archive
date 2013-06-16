address $00000000

setreg a0, $00000000
setreg a1, $00080080
setreg a2, $00000020

_HandleATCIncDec:
//By Dnawrkshp
//Takes k1 (marker, a0) and a string pointer (a1) and applies the inc or dec (a2) to the appropriate byte
addiu sp,sp, $FF80
sq t0, $0000(sp)
sq t1, $0010(sp)
sq t2, $0020(sp)
sq t3, $0030(sp)
sq t4, $0040(sp)
sq s0, $0050(sp)
sq s1, $0060(sp)
sq s2, $0070(sp)


daddu s0, a0, zero  // Marker placement
daddu s1, a1, zero  // String (pointer)
daddu s2, a2, zero  // Increment/decrement value

addu t1, s1, s0
lb t3, $0000(t1)

sll t3, t3, 24
srl t3, t3, 24

add t3, t3, s2
sb t3, $0000(t1)

beq zero, zero, :_CheckForLoop
nop

_CheckForLoop:
//Checks if ((value > 0 && value < 20 ) || value > 7E)

lb t3, $0000(t1)

sll t3, t3, 24
srl t3, t3, 24

addiu t2, zero, $0020
slt t2, t3, zero
bne t2, zero, 5
nop
addiu t2, zero, $0020
slt t2, t3, t2
bne t2, zero, :_LoopingChar
nop



addiu t2, zero, $007E
slt t2, t2, t3

beq t2, zero, 4
nop

beq zero, zero, :_LoopingChar
nop

slt t2, t3, zero
beq t2, zero, :_ExitATCIncDec
nop

beq zero, zero, :_ExitATCIncDec
nop

_LoopingChar:

slt t4, s2, zero
bne t4, zero, :_DecChar
nop

_IncChar:

addiu t3, zero, $0020
sb t3, $0000(t1)

beq zero, zero, :_ExitATCIncDec
nop

_DecChar:

addiu t3, zero, $007E
sb t3, $0000(t1)

beq zero, zero, :_ExitATCIncDec
nop

_ExitATCIncDec:
lq t0, $0000(sp)
lq t1, $0010(sp)
lq t2, $0020(sp)
lq t3, $0030(sp)
lq t4, $0040(sp)
lq s0, $0050(sp)
lq s1, $0060(sp)
lq s2, $0070(sp)
addiu sp, sp, $0080

address $0008040
hexcode $0008080

address $00080080
hexcode $34333260
hexcode $38373635
hexcode $44434241
hexcode $48474645









