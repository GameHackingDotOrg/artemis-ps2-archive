setreg a0, $0000006c

_GetSpacing:

addiu sp, sp, $FFF0
sq s0, $0000(sp)

daddu s0, a0, zero //Byte

//Rid possible FF's
sll s0, s0, 24
srl s0, s0, 24

//Load spacing from 8002FE00 + byte
lui t0, $0002
ori t0, t0, $FE00
addu t1, t0, s0
lb v0, $0000(t1) //Return spacing

//Rid possible FF's
sll v0, v0, 24
srl v0, v0, 24

lq s0, $0000(sp)
addiu sp, sp, $0010

setreg t0, $00000001
add t0, t0, v0

nop
nop

address $0002FE00
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0A060C
hexcode $060C0E0C
hexcode $0C060606
hexcode $08060806
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $06060C0C
hexcode $0C0C0C0C
hexcode $0C0C0C10
hexcode $0C0C0C0C
hexcode $0C0C060C
hexcode $0D0C0F0C
hexcode $0C0C0D0C
hexcode $100C0C0C
hexcode $060C0C0C
hexcode $0C0A0606
hexcode $0B0C0C06
hexcode $0C060C0C
hexcode $0C06060B
hexcode $0C0B0E06
hexcode $0B080C0C
hexcode $0D0B0B06
hexcode $090B0B0B
hexcode $0C0C0604
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
hexcode $0C0C0C0C
