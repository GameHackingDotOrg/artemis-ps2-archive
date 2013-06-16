// Pixel 2.0 Class

//==========================================================
vSync:
addiu sp, sp, $fff0
sd fp, $0000(sp)

daddu fp, sp, zero
lui v0, $1200
ld v0, $1000(v0)
addiu v1, zero, $0008
or v0, v0, v1
lui at, $1200
sd v0, $1000(at)
lui v0, $1200
ld v0, $1000(v0)
addiu v1, zero, $0008
and v0, v0, v1
nop
beq v0, zero, -6
nop
lui v0, $1200
ld v0, $1000(v0)
lui at, $1200
sd v0, $1000(at)
daddu sp, fp, zero

ld fp, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_GetDisplay:

lui a0, $1200

ld v0, $0080(a0) // Get Display 1
ld v1, $00A0(a0) // Get Display 2

jr ra
nop
//==========================================================
_RestoreDisplay:

lui v0, $1200

sd a0, $0080(v0) // Get Display 1
sd a1, $00A0(v0) // Get Display 2

jr ra
nop

//==========================================================
// GS_SET_DISPLAY(632, 50, 3, 0, 2559,  447)
// 0x12000080 = GS_REG_DISPLAY1
// 0x120000A0 = GS_REG_DISPLAY2
// 01832278 001BF9FF
_SetDisplay:

lui a0, $1200

lui v0, $001B
ori v0, v0, $F9FF
dsll32 v0, v0, 0
lui v1, $0183
ori v1, v1, $2278
daddu v0, v0, v1
sd v0, $0080(a0)   // Set Display 1
sd v0, $00A0(a0)   // Set Display 2

jr ra
nop

//==========================================================
// a0: packet
// a1: bg R
// a2: bg G
// a3: bg B
_initScreen:

addiu sp, sp, $FFE0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)
sw s2, $000C(sp)
sw s3, $0010(sp)
sw s4, $0014(sp)

// Flush screen & Initialize draw space
addiu s0, zero, 50
daddu s1, a0, zero

daddu s2, a1, zero // R
daddu s3, a2, zero // G
daddu s4, a3, zero // B

_initScreenFullFlush:
jal :_SetDisplayBuffer
nop

jal :_SetDrawBuffer
daddu a0, s1, zero

jal :_SetZBuffer
daddu a0, s1, zero


daddu a1, s2, zero
daddu a2, s3, zero
daddu a3, s4, zero
jal :_ClearBuffer
daddu a0, s1, zero

jal :vSync
nop

bne s0, zero, :_initScreenFullFlush
addiu s0, s0, -1

lw ra, $0000(sp)
lw s0, $0004(sp)
lw s1, $0008(sp)
lw s2, $000c(sp)
lw s3, $0010(sp)
lw s4, $0014(sp)
jr ra
addiu sp, sp, $0020

//==========================================================
_SetDisplayBuffer:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

lui s0, $1200

addiu t0, zero, $1400
sd t0, $0070(s0)
sd t0, $0090(s0)

lw s0, $0004(sp)
lw ra, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_SetDrawBuffer:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

daddu s0, a0, zero // Packet

// *(u64*)pckt = GIF_SET_TAG(5, 1, 0, 0, GIF_TAG_PACKED, 1);
lui t0, $1000
ori v0, zero, $8005
dsll32 t0, t0, 0
daddu t0, t0, v0

// *(u64*)pckt = 0xE;
addiu t1, zero, $000E

// *(u64*)pckt = GIF_SET_FRAME(0, 10, 32, 0);
lui t2, $200A

// *(u64*)pckt = GIF_REG_FRAME_1;
addiu t3, zero, $004C

// *(u64*)pckt = GIF_SET_SCISSOR(0, 639, 0, 447);
lui t4, $01BF
lui v0, $027F
dsll32 t4, t4, 0
daddu t4, t4, v0

// *(u64*)pckt = GIF_REG_SCISSOR_1;
addiu t5, zero, $0040

// *(u64*)pckt = GIF_SET_TEST(0, 0, 0, 0, 0, 0, 1, 2);
lui t6, $0005

// *(u64*)pckt = GIF_REG_TEST_1;
addiu t7, zero, $0047

// *(u64*)pckt = GIF_SET_XYOFFSET(1728 << 4, 1824 << 4);
addiu t8, zero, $7200
addiu v0, zero, $6C00
dsll32 t8, t8, 0
daddu t8, t8, v0

// *(u64*)pckt = GIF_REG_XYOFFSET_1;
addiu a0, zero, $0018

// *(u64*)pckt = GIF_SET_PRMODECONT(1);
addiu a1, zero, $0001

// *(u64*)pckt = GIF_REG_PRMODECONT;
addiu a2, zero, $001A

sd t0, $0000(s0)
sd t1, $0008(s0)
sd t2, $0010(s0)
sd t3, $0018(s0)
sd t4, $0020(s0)
sd t5, $0028(s0)
sd t6, $0030(s0)
sd t7, $0038(s0)
sd t8, $0040(s0)
sd a0, $0048(s0)
sd a1, $0050(s0)
sd a2, $0058(s0)


daddu a0, s0, zero
jal :_SyncDCache
addiu a1, s0, 96

daddu a0, s0, zero
jal :_DMA_2_Upload
addiu a1, zero, 96

lw s0, $0004(sp)
lw ra, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
_SetZBuffer:
addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)

daddu s0, a0, zero

// *(u64*)pckt = GIF_SET_TAG(1, 1, 0, 0, 0, 1);
lui t0, $1000
dsll32 t0, t0, 0
lui v0, $0000
ori v0, v0, $8001
daddu t0, t0, v0

// *(u64*)pckt = 0xE;
daddiu t1, zero, $000e

// *(u64*)pckt = GIF_SET_ZBUF(0x96, 32, 0);
addiu t2, zero, $0096

// *(u64*)pckt = GIF_REG_ZBUF_1;
addiu t3, zero, $004E

sd t0, $0000(s0)
sd t1, $0008(s0)
sd t2, $0010(s0)
sd t3, $0018(s0)



daddu a0, s0, zero
jal :_SyncDCache
addiu a1, s0, 32

daddu a0, s0, zero
jal :_DMA_2_Upload
addiu a1, zero, 32

lw s0, $0004(sp)
lw ra, $0000(sp)
jr ra
addiu sp, sp, $0010

//==========================================================
// a0 = Packet
// a1 = Background R
// a2 = Background G
// a3 = Background B
_ClearBuffer:

addiu sp, sp, $FFE0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)
sw s2, $000C(sp)
sw s3, $0010(sp)

daddu s0, a0, zero // Packet
daddu s1, a1, zero // R
daddu s2, a2, zero // G
daddu s3, a3, zero // B


// *(u64*)pckt = GIF_SET_TAG(6, 1, 0, 0, 0, 1);
lui t0, $1000
ori v0, zero, $8006
dsll32 t0, t0, 0
daddu t0, t0, v0

// *(u64*)pckt = 0xE;
addiu t1, zero, $000E

// *(u64*)pckt = GIF_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1);
lui t2, $0003

// *(u64*)pckt = GIF_REG_TEST_1;
addiu t3, zero, $0047

// *(u64*)pckt = GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0);
addiu t4, zero, $0006

// *(u64*)pckt = GIF_REG_PRIM;
daddu t5, zero, zero

// *(u64*)pckt = GIF_SET_RGBAQ(0x12, 0x34, 0x56, 0x80, 0x3F800000);
lui t6, $3F80
addiu t6, t6, 1
addiu v0, zero, $0080
sll v0, v0, 8
addu v0, v0, s3
sll v0, v0, 8
addu v0, v0, s2
sll v0, v0, 8
addu v0, v0, s1
dsll32 t6, t6, 0
daddu t6, t6, v0

// *(u64*)pckt = GIF_REG_RGBAQ;
addiu t7, zero, $0001

// *(u64*)pckt = GIF_SET_XYZ(0x0000, 0x0000, 0x0000);
daddu t8, zero, zero

// *(u64*)pckt = GIF_REG_XYZ2;
addiu a0, zero, $0005

// *(u64*)pckt = GIF_SET_XYZ(0xFFFF, 0xFFFF, 0x0000);
addiu a1, zero, 1
dsll32 a1, a1, 0
daddiu a1, a1, -1

// *(u64*)pckt = GIF_REG_XYZ2;
addiu a2, zero, $0005

// *(u64*)pckt = GIF_SET_TEST(0, 0, 0, 0, 0, 0, 1, 2);
lui a3, zero, $0005

// *(u64*)pckt = GIF_REG_TEST_1;
addiu v0, zero, $0047

sd t0, $0000(s0)
sd t1, $0008(s0)
sd t2, $0010(s0)
sd t3, $0018(s0)
sd t4, $0020(s0)
sd t5, $0028(s0)
sd t6, $0030(s0)
sd t7, $0038(s0)
sd t8, $0040(s0)
sd a0, $0048(s0)
sd a1, $0050(s0)
sd a2, $0058(s0)
sd a3, $0060(s0)
sd v0, $0068(s0)


daddu a0, s0, zero
jal :_SyncDCache
addiu a1, s0, 112

daddu a0, s0, zero
jal :_DMA_2_Upload
addiu a1, zero, 112

lw ra, $0000(sp)
lw s0, $0004(sp)
lw s1, $0008(sp)
lw s2, $000C(sp)
lw s3, $0010(sp)
jr ra
addiu sp, sp, $0020

//==========================================================
// AddPixel
/*
Input:
   a0 = PacketAddress
   a1 = X
   a2 = Y
   a3 = Z
   t0 = R
   t1 = G
   t2 = B
   t3 = Packet Size
   t4 = W
   t5 = H
Output:
   v0 = Size
*/
_AddPixel:

addiu sp, sp, $FF80
sq s0, $0000(sp)
sq s1, $0010(sp)
sq s2, $0020(sp)
sq s3, $0030(sp)
sq s4, $0040(sp)
sq s5, $0050(sp)
sq s6, $0060(sp)
sq s7, $0070(sp)

daddu s0, a0, t3
daddu s1, t0, zero
daddu s2, t1, zero
daddu s3, t2, zero
daddu s4, t4, zero
daddu s5, t5, zero

daddiu a1, a1, 1732
daddiu a2, a2, 1829

// GIF_SET_TAG(4, 1, 0, 0, 0, 1);
lui t0, $1000
dsll32 t0, t0, 0
ori t0, t0, $8004

// 0x0E
ori t1, zero, $000E

// GIF_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0);
ori t2, zero, $0006

// GIF_REG_PRIM;
daddu t3, zero, zero

// GIF_SET_RGBAQ(r, g, b, 0x80, 0x3F800000);
lui t4, $3f80
dsll32 t4, t4, 0
daddiu v0, zero, $0080
sll v0, v0, 8
daddu v0, v0, s1 // b
sll v0, v0, 8
daddu v0, v0, s2 // g
sll v0, v0, 8
daddu v0, v0, s3 // r
slti v1, v0, 0
beq v1, zero, 4
nop
ori v1, zero, 1
dsll32 v1, v1, 0
daddu v0, v0, v1
daddu t4, t4, v0

// GIF_REG_RGBAQ;
ori t5, zero, $0001

// GIF_SET_XYZ( NewX << 4, NewY << 4, z);
sll v0, a2, 16     // NewY
daddu v0, v0, a1   // NewX
sll v0, v0, 4
daddu t6, a3, zero // z
dsll32 t6, t6, 0
slti v1, v0, 0
beq v1, zero, 4
nop
ori v1, zero, 1
dsll32 v1, v1, 0
daddu v0, v0, v1
daddu t6, t6, v0


// GIF_REG_XYZ2;
ori t7, zero, $0005

// GIF_SET_XYZ( (NewX + w) << 4, (NewY + h) << 4, z);
daddu v0, a2, s5  // NewY += h
sll v0, v0, 16
daddu v0, v0, a1
daddu v0, v0, s4  // NewX += w
sll v0, v0, 4
daddu t8, a3, zero // z
dsll32 t8, t8, 0
slti v1, v0, 0
beq v1, zero, 4
nop
ori v1, zero, 1
dsll32 v1, v1, 0
daddu v0, v0, v1
daddu t8, t8, v0

// GIF_REG_XYZ2;
ori t9, zero, $0005

sd t0, $0000(s0)
sd t1, $0008(s0)
sd t2, $0010(s0)
sd t3, $0018(s0)
sd t4, $0020(s0)
sd t5, $0028(s0)
sd t6, $0030(s0)
sd t7, $0038(s0)
sd t8, $0040(s0)
sd t9, $0048(s0)

//--------------------- Accelerate Store
dsll32 s0, s0, 4
dsrl32 s0, s0, 4
lui v0, $2000
addu s0, s0, v0
sd t0, $0000(s0)
sd t1, $0008(s0)
sd t2, $0010(s0)
sd t3, $0018(s0)
sd t4, $0020(s0)
sd t5, $0028(s0)
sd t6, $0030(s0)
sd t7, $0038(s0)
sd t8, $0040(s0)
sd t9, $0048(s0)



addiu v0, zero, 80

lq s0, $0000(sp)
lq s1, $0010(sp)
lq s2, $0020(sp)
lq s3, $0030(sp)
lq s4, $0040(sp)
lq s5, $0050(sp)
lq s6, $0060(sp)
lq s7, $0070(sp)
jr ra
addiu sp, sp, $0080


//==========================================================
// DMA Channel 2 Upload (Send GIF Packet)
/*
Input:
   a0 = Packet Address
   a1 = Packet Size
*/
_DMA_2_Upload:

addiu sp, sp, $FFD0
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)


daddu s0, a0, zero
daddu s1, a1, zero

lui a0, $1000
ori a0, a0, $A000

//---------------------------- Wait for channel to be open
jal :_WaitDMAChannelReady
nop

// 0000 = Initialize Transfer (0x0181)
// 0010 = Memory Address
// 0020 = Quad Size

//---------------------------- Set Data Address
sw s0, $0010(a0)

//---------------------------- Set Quad Size
addiu v0, s1, 15
srl v0, v0, 4
ori v1, zero, -1
and v0, v0, v1
sw v0, $0020(a0)

//---------------------------- Start Data Transfer
addiu v0, zero, $0181
sw v0, $0000(a0)


lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
jr ra
addiu sp, sp, $0030

//==========================================================
// Wait Channel Ready
/*
Input:
   a0 = Channel Address
*/
_WaitDMAChannelReady:

lw v0, $0000(a0)
addiu v1, zero, $0100
and v0, v0, v1

bne v0, zero, :_WaitDMAChannelReady
nop

jr ra
nop


//==========================================================
// void SyncDCache(void *start, void *end)
_SyncDCache:

addiu sp, sp, $FFE0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)
sw s2, $000C(sp)
sw s3, $0010(sp)

daddu s0, a0, zero
daddu s1, a1, zero
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
jal :_opDCache
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
//==========================================================
// void _opDCache(void *start, void *end)

_opDCache:
addiu sp, sp, $FFE0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)
sw s2, $000C(sp)
sw s3, $0010(sp)
sw s4, $0014(sp)
sw s5, $0018(sp)
sw s6, $001C(sp)

daddu s0, a0, zero
daddu s1, a1, zero


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
//--------------------------------------

lw ra, $0000(sp)
lw s0, $0004(sp)
lw s1, $0008(sp)
lw s2, $000C(sp)
lw s3, $0010(sp)
lw s4, $0014(sp)
lw s5, $0018(sp)
lw s6, $001C(sp)
jr ra
addiu sp, sp, $0020
