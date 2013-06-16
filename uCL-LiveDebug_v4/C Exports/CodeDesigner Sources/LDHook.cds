/*
        CodeDesigner v2.0
        Created by: Gtlcpimp
        Official CheatersLounge Product Copyright ©
*/
/*
   CL-LiveDebug v3 Hook Filter v0.2c
   Created by: Gtlcpimp
   CheatersLounge Copyright ©
*/

address $00078250

_init:
addiu sp, sp, $FFF0
sq ra, $0000(sp)
call _start
lq ra, $0000(sp)
addiu sp, sp, $0010
//jalr k0
jr k0
nop

//==========================================================

_start:
addiu sp, sp, $FE00
sq at, $0000(sp)
sq v0, $0010(sp)
sq v1, $0020(sp)
sq a0, $0030(sp)
sq a1, $0040(sp)
sq a2, $0050(sp)
sq a3, $0060(sp)
sq t0, $0070(sp)
sq t1, $0080(sp)
sq t2, $0090(sp)
sq t3, $00a0(sp)
sq t4, $00b0(sp)
sq t5, $00c0(sp)
sq t6, $00d0(sp)
sq t7, $00e0(sp)
sq s0, $00f0(sp)
sq s1, $0100(sp)
sq s2, $0110(sp)
sq s3, $0120(sp)
sq s4, $0130(sp)
sq s5, $0140(sp)
sq s6, $0150(sp)
sq s7, $0160(sp)
sq t8, $0170(sp)
sq t9, $0180(sp)
sq k0, $0190(sp)
sq k1, $01a0(sp)
sq fp, $01b0(sp)
sq gp, $01c0(sp)
sq ra, $01d0(sp)

lui s0, $8005

//Joker scanner on/off switch
lui t1, $8008
lw t1, $E7FC(t1)

bne t1, zero, 3
nop
jal $0007E880 //Joker scanner
nop

jal $0007F800 //Cheat engine
nop

//Check if the debugger is already open
lw t0, $0030(s0)
bne t0, zero, :LDv3HookExit
nop

jal :_ReadPad
lh a0, $002C(s0) //Joker combo
beq v0, zero, :LDv3HookExit
nop

//Update state
addiu v0, zero, 1
sw v0, $0030(s0)

//Finds a memory region for the packet
lui a0, $0008
lui a1, $0200
jal :_FindFreeRegion
ori a2, zero, $A000

sw v0, $0050(s0)
bne v0, zero, 4
nop
lw v0, $0060(s0)
sw v0, $0050(s0)

//Finds a memory region for the register dumper
lw t0, $0058(s0)
bne t0, zero, 7
nop
lui a0, $0008
lui a1, $0200
jal :_FindFreeRegion
ori a2, zero, $0180

sw v0, $0058(s0)
bne v0, zero, 4
nop
lw v0, $0068(s0)
sw v0, $0058(s0)

//Install the EH
jal :_InstallEH
nop

_SkipExcI:
//Gets current screen width and height
lui t1, $8006
jal :_GetDisplay
nop
sd v0, $FFF0(t1)
sd v1, $FFF8(t1)

//Sets screen width and height
jal :_SetDisplay
nop

jal $00060000
daddu a0, zero, zero

//Clear packet, and exception handler memory
//Packet
lui s0, $8005
lw a0, $0050(s0)
beq a0, zero, 4
nop
addu a1, zero, zero
jal :_memset
ori a2, zero, $A000

lw a0, $0054(s0)
beq a0, zero, 4
nop
addu a1, zero, zero
jal :_memset
ori a2, zero, $3000

sd zero, $0050(s0)

//Return screen to normal width and height
lui t1, $8006
ld a0, $FFF0(t1)
ld a1, $FFF8(t1)
jal :_RestoreDisplay
nop

//Update state
sw zero, $0030(s0)

/*
lq t1, $0080(s0)
lq t2, $0090(s0)
lui t0, $1000
sq t1, $0000(t0)
sq t2, $0800(t0)
*/

LDv3HookExit:
lq at, $0000(sp)
lq v0, $0010(sp)
lq v1, $0020(sp)
lq a0, $0030(sp)
lq a1, $0040(sp)
lq a2, $0050(sp)
lq a3, $0060(sp)
lq t0, $0070(sp)
lq t1, $0080(sp)
lq t2, $0090(sp)
lq t3, $00a0(sp)
lq t4, $00b0(sp)
lq t5, $00c0(sp)
lq t6, $00d0(sp)
lq t7, $00e0(sp)
lq s0, $00f0(sp)
lq s1, $0100(sp)
lq s2, $0110(sp)
lq s3, $0120(sp)
lq s4, $0130(sp)
lq s5, $0140(sp)
lq s6, $0150(sp)
lq s7, $0160(sp)
lq t8, $0170(sp)
lq t9, $0180(sp)
lq k0, $0190(sp)
lq k1, $01a0(sp)
lq fp, $01b0(sp)
lq gp, $01c0(sp)
lq ra, $01d0(sp)
jr ra
addiu sp, sp, $0200

//==========================================================
_ReadPad:

addiu sp, sp, $FFF0
sw ra, $0000(sp)
sw s0, $0004(sp)
sw s1, $0008(sp)

lui s0, $8008
addiu s0, s0, $E810

lw t0, $0000(s0)
lw t1, $0004(s0)
lw t2, $0008(s0)
lw t3, $000C(s0)
addiu t4, zero, -1
addiu t5, zero, -1
addiu t6, zero, -1
addiu t7, zero, -1
addiu t8, zero, -1

beq t0, zero, :_ReadPadQuit
addu v0, zero, zero
lh t4, $0000(t0)

beq t1, zero, :_ReadPadCompare
addu v0, zero, zero
lh t5, $0000(t1)

beq t2, zero, :_ReadPadCompare
addu v0, zero, zero
lh t6, $0000(t2)

beq t3, zero, :_ReadPadCompare
addu v0, zero, zero
lh t7, $0000(t3)


_ReadPadCompare:
beq t8, a0, :_ReadPadCompareNOINPUT
nop

beq t4, a0, :_ReadPadQuit
addiu v0, zero, 1
beq t5, a0, :_ReadPadQuit
addiu v0, zero, 1
beq t6, a0, :_ReadPadQuit
addiu v0, zero, 1
beq t7, a0, :_ReadPadQuit
addiu v0, zero, 1

addu v0, zero, zero

goto _ReadPadQuit
_ReadPadCompareNOINPUT:

bne t4, t8, :_ReadPadQuit
addu v0, zero, zero
bne t5, t8, :_ReadPadQuit
addu v0, zero, zero
bne t6, t8, :_ReadPadQuit
addu v0, zero, zero
bne t7, t8, :_ReadPadQuit
addu v0, zero, zero

addiu v0, zero, 1

_ReadPadQuit:
lw ra, $0000(sp)
lw s0, $0004(sp)
lw s1, $0008(sp)
jr ra
addiu sp, sp, $0010

//By Gtlcpimp
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

//Moves exception handler from 80050000 to 000E0000 and finds a region for it
//The exception handler has been removed from LDv4, but I left this here in case someone wants to bring it back
//Just keep in mind you will have to find a place to store it in the kernel and then change all the jal's to branches or find some way to make those dynamic
//==============================================================
_InstallEH:

//Check if it should install
lui a0, $8005
lw a1, $0028(a0)
beq a1, zero, 3
nop

jr ra
nop

lw a1, $0050(a0) //Check if the debugger has been opened and the packet has been allocated
beq a1, zero, -3
nop

addiu sp, sp, $FFB0
sq ra, $0000(sp)
sq a0, $0010(sp)
sq a1, $0020(sp)
sq v0, $0030(sp)
sq v1, $0040(sp)

lui a0, $0008
lui a1, $0200
jal :_FindFreeRegion
ori a2, zero, $3000
lui t0, $8005

sw v0, $0054(s0)
bne v0, zero, 4
nop
lw v0, $0064s0)
sw v0, $0054(s0)

lui s0, $8005 //Start
ori s0, s0, $1000
addiu s1, s0, $27EC //End
lw s2, $0054(t0)

_LoopIEH:
lw t0, $0000(s0)
sw t0, $0000(s2)

addiu s2, s2, 4
addiu s0, s0, 4

bne s1, s0, :_LoopIEH
nop

//Then installs a pointer to the handler
//==============================================================
_installHandler:

lui a0, $8000
addiu a1, zero, $8f5a

_IH_ScanLoop:

lh v0, $0000(a0)
lh v1, $0002(a0)
bne a1, v1, :_IH_ScanLoop
addiu a0, a0, 4

lui a0, $8001
daddu a0, a0, v0

lui t0, $8005
lw v0, $0054(t0)
//lui v1, $8000
//addu v0, v0, v1

sw v0, $0004(a0) // TLB Modification
sw v0, $0008(a0) // TLB Load/Inst Fetch
sw v0, $000c(a0) // TLB Store
sw v0, $0010(a0) // Address Load/Inst Fetch
sw v0, $0014(a0) // Address Store
sw v0, $0018(a0) // Bus Error (instr)
sw v0, $001c(a0) // Bus Error (data)
//sw v0, $0020(a0) // Syscall()
//sw v0, $0024(a0) // Breakpoint
sw v0, $0028(a0) // Reserved Instruction
sw v0, $002c(a0) // Coprocessor Unsuable
sw v0, $0030(a0) // Overflow
sw v0, $0034(a0) // Something

lq ra, $0000(sp)
lq a0, $0010(sp)
lq a1, $0020(sp)
lq v0, $0030(sp)
lq v1, $0040(sp)
jr ra
addiu sp, sp, $0050

//===============================================================
/*
a0 = Start address
a1 = Address limit
a2 = Size of block
returns v0 as pointer to beginning of memory
*/
_FindFreeRegion:
addiu sp, sp, $FFB0
sq s0, $0000(sp)
sq s1, $0010(sp)
sq s2, $0020(sp)
sq s3, $0030(sp)
sq s4, $0040(sp)

daddu s0, a0, zero //Start addr
daddu s1, a1, zero //Addr limit
daddu s2, a2, zero //Size of block
daddu s3, zero, zero, //Counter
daddu v0, zero, zero

_FFRReLoop:
daddu s4, s0, zero //Place holder for return

_FFRLoop:
slt t0, s0, s1
beq t0, zero, :_FFREnd
nop

lui t1, $0200 //Just for safety
slt t0, s0, t1
beq t0, zero, :_FFREnd
nop

lq t0, $0000(s0)

bne t0, zero, :_FFRNotZ
nop

slt t0, s3, s2
beq t0, zero, :_FFRFoundR
nop

addiu s3, s3, $0010
beq zero, zero, :_FFRLoop
addiu s0, s0, $0010

_FFRNotZ:
addiu s0, s4, $0800 //Skip 0x800 addresses (for sake of speed)
addu s0, s0, s3
daddu s3, zero, zero

beq zero, zero, :_FFRReLoop
nop

_FFRFoundR:
lui t0, $8005
lw t1, $0050(t0) //Packet
lw t2, $0054(t0) //Exception Handler
lw t3, $0058(t0) //Reg Dumper

//This checks if the addr is equal to one of the listed above
//If it is, it adds the size of the one it's equal to to the addr and loops back
bne s4, t1, 6
nop

ori t0, zero, $A000 //Size of packet is 0xA000
addu s0, s4, t0
addu s0, s0, s3
beq zero, zero, :_FFRReLoop
daddu s3, zero, zero

bne s4, t2, 5
nop
addiu s0, s4, $3000 //Size of exc handler is 0x3000
addu s0, s0, s3
beq zero, zero, :_FFRReLoop
daddu s3, zero, zero

bne s4, t3, 5
nop

addiu s0, s4, $0320 //Size of reg dumper is 0x0320
addu s0, s0, s3
beq zero, zero, :_FFRReLoop
daddu s3, zero, zero

daddu v0, s4, zero

_FFREnd:
lq s0, $0000(sp)
lq s1, $0010(sp)
lq s2, $0020(sp)
lq s3, $0030(sp)
lq s4, $0040(sp)
jr ra
addiu sp, sp, $0050

//===============================================================
/*
a0 = &addr
a1 = (u8)value to set
a2 = (int)size
*/
_memset:
addiu sp, sp, $FFA0
sq s0, $0000(sp)
sq s1, $0010(sp)
sq s2, $0020(sp)
sq s3, $0030(sp)
sq s4, $0040(sp)
sq ra, $0050(sp)

daddu s0, a0, zero
daddu s1, a1, zero
daddu s2, a2, s0 //Size + packet

//If value is not within the EE, exit
lui a1, $0008
lui a2, $0200
jal :_IfBetween
daddu a0, s0, zero

bne v0, zero, :_msDone
nop

//Makes value = (value << 24) + (value << 16) + (value << 8) + value
sll s1, s1, 24 //Just to make it one byte long
srl s1, s1, 24

daddu s3, s1, zero
sll s3, s3, 8
addu s1, s1, s3
sll s3, s3, 8
addu s1, s1, s3
sll s3, s3, 8
addu s1, s1, s3

_msLoop:
slt t0, s0, s2
beq t0, zero, :_msDone

sw s1, $0000(s0)

beq zero, zero, :_msLoop
addiu s0, s0, 4 //I am incrementing in words for speed

_msDone:
lq s0, $0000(sp)
lq s1, $0010(sp)
lq s2, $0020(sp)
lq s3, $0030(sp)
lq s4, $0040(sp)
lq ra, $0050(sp)
jr ra
addiu sp, sp, $0060

//==========================================================
/*
a0 = value
a1 = small between
a2 = big between
v0 returns 0 if it is between, 1 if it is not
NOTE: This is designed to reserve the used temporary registers!
*/
_IfBetween:
addiu sp, sp, $FFE0
sq t0, $0000(sp)
sq t1, $0010(sp)

slt t0, a0, a1
slt t1, a2, a0

or v0, t0, t1 //If t0 or t1 is 1, returns 1. Otherwise returns 0
lq t0, $0000(sp)
lq t1, $0010(sp)
jr ra
addiu sp, sp, $0020
