/*
String Class
Created by: Gtlcpimp
*/

//============================================================
// UCase() -> Converts to Upper Case
/*
Input:
   a0 = &String
*/
UCase:

addiu sp, sp, $FFE0
sq ra, $0000(sp)
sq s0, $0010(sp)

daddu s0, a0, zero

UCase_Loop:
lb a0, $0000(s0)

dsll32 a0, a0, 24
dsrl32 a0, a0, 24

slti v0, a0, $0061
bne v0, zero, :UCase_SkipChar
nop

slti v0, a0, $007B
beq v0, zero, :UCase_SkipChar
nop

addiu a0, a0, $FFE0
sb a0, $0000(s0)

UCase_SkipChar:
bne a0, zero, :UCase_Loop
addiu s0, s0, 1

lq ra, $0000(sp)
lq s0, $0010(sp)
jr ra
addiu sp, sp, $0020

//============================================================
// LCase() -> Converts to Lower Case
/*
Input:
   a0 = &String
*/
LCase:

addiu sp, sp, $FFE0
sq ra, $0000(sp)
sq s0, $0010(sp)

daddu s0, a0, zero

LCase_Loop:
lb a0, $0000(s0)

dsll32 a0, a0, 24
dsrl32 a0, a0, 24

slti v0, a0, $0041
bne v0, zero, :LCase_SkipChar
nop

slti v0, a0, $005B
beq v0, zero, :LCase_SkipChar
nop

addiu a0, a0, $0020
sb a0, $0000(s0)

LCase_SkipChar:
bne a0, zero, :LCase_Loop
addiu s0, s0, 1

lq ra, $0000(sp)
lq s0, $0010(sp)
jr ra
addiu sp, sp, $0020


//============================================================
// Split()
/*
Input:
   a0 = &String
   a1 = Delimiter
   a2 = Array Destination
Output:
   v0 = Array Size
*/
SplitStringArray:

addiu sp, sp, $FF80
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)
sq s4, $0050(sp)
sq s5, $0060(sp)
sq s6, $0070(sp)

daddu s0, a0, zero
daddu s1, a1, zero
daddu s2, a2, zero

daddu s6, s0, zero

jal :StrLen
daddu a0, s1, zero
daddu s3, v0, zero

jal :StrLen
daddu a0, s0, zero
daddu s4, v0, zero
daddu s5, v0, zero

slt v0, s3, s4
beq v0, zero, :_SplitStringArray_Exit
daddu v0, zero, zero

_SplitStringArray_Loop:

daddu a0, s0, zero
daddu a1, s1, zero
jal :ChunkCompare
daddu a2, s3, zero

beq v0, zero, :_SplitStringArray_SkipSet
nop

daddu a0, s0, zero
daddu v0, s3, zero
_SplitStringArray_NOPLoop:
sb zero, $0000(a0)
addiu a0, a0, 1
addiu v0, v0, -1
bne v0, zero, :_SplitStringArray_NOPLoop
nop

_SplitStringArray_SkipSet:

addiu s4, s4, -1
slt v0, s3, s4
bne v0, zero, :_SplitStringArray_Loop
addiu s0, s0, 1

daddu s0, s6, zero
daddu s1, s5, zero
daddu s4, zero, zero
daddiu s5, zero, 1
daddu s6, zero, zero

_SplitStringArray_NOPReadLoop:
lb v0, $0000(s0)

beq v0, zero, :_SplitStringArray_NOPReadZERO
nop
_SplitStringArray_NOPReadNOTZERO:

daddu s5, zero, zero

bne s4, zero, :_SplitStringArray_NOPReadNEXT
nop

sw s0, $0000(s2)
sw zero, $0004(s2)
addiu s2, s2, 4
addiu s4, zero, 1
addiu s6, s6, 1

beq zero, zero, :_SplitStringArray_NOPReadNEXT
nop

_SplitStringArray_NOPReadZERO:

bne s5, zero, :_SplitStringArray_NOPReadNEXT
nop

daddu s4, zero, zero
addiu s5, zero, 1

_SplitStringArray_NOPReadNEXT:
addiu s1, s1, -1
slt v0, s3, s1
bne v0, zero, :_SplitStringArray_NOPReadLoop
addiu s0, s0, 1

daddu v0, s6, zero

_SplitStringArray_Exit:
lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
lq s4, $0050(sp)
lq s5, $0060(sp)
lq s6, $0070(sp)
jr ra
addiu sp, sp, $0080

//============================================================
// Chunk Compare
/*
Input:
   a0 = &Chunk1
   a1 = &Chunk2
   a2 = Size
Output:
   v0 = 0 (not equal) 1 (equal)
*/
ChunkCompare:

addiu sp, sp, $FFE0
sq ra, $0000(sp)
sq s0, $0010(sp)

daddiu s0, a2, -1

_StrChunkCompareLoop:

lb v0, $0000(a0)
lb v1, $0000(a1)

dsll32 v0, v0, 24
dsll32 v1, v1, 24
dsrl32 v0, v0, 24
dsrl32 v1, v1, 24

bne v0, v1, :_StrChunkCompare_Exit
daddu v0, zero, zero

addiu a0, a0, 1
addiu a1, a1, 1

bne s0, zero, :_StrChunkCompareLoop
addiu s0, s0, -1

addiu v0, zero, 1

_StrChunkCompare_Exit:
lq ra, $0000(sp)
lq s0, $0010(sp)
jr ra
addiu sp, sp, $0020

//============================================================
// Data Columns
/*
Input:
   a0 = # of Columns
   a1 = # of Rows
   a2 = &Data
   a3 = Column Store
*/
_BuildColumnData:

addiu sp, sp, $FF80
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)
sq s4, $0050(sp)

daddu s4, a1, zero

daddiu s0, a0, -1
sll s1, s4, 2
daddu s2, a2, zero
daddu s3, a3, zero
addiu s4, s4, -1

_BColRowLoop:

daddu v1, s0, zero
addu a0, s2, zero
_BColD_Loop:

lw v0, $0000(a0)
sw v0, $0000(s3)

addiu s3, s3, 4
addu a0, a0, s1
bne v1, zero, :_BColD_Loop
addiu v1, v1, -1

addiu s2, s2, 4
bne s4, zero, :_BColRowLoop
addiu s4, s4, -1

lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
lq s4, $0050(sp)
jr ra
addiu sp, sp, $0080

//============================================================
// String Print Format
/*
Input:
   a0 = Source String
   a1 = Formatted Destination String
   a2 = Argument Stack 32 bit points and vals
Output:
   v0 = Success or Error (1 = good; 0 = bad)
*/
sprintf:

addiu sp, sp, $FF80
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)

daddu s0, a0, zero
daddu s1, a1, zero
daddu s2, a2, zero

// s0 = Source
// s1 = Destination
// s2 = Arguments

beq s0, zero, :sprintf_quit
daddu v0, zero, zero

beq s1, zero, :sprintf_quit
daddu v0, zero, zero

beq s2, zero, :sprintf_quit
daddu v0, zero, zero

//------------------------------------------------- Copy Loop
sprintf_mainCopyLoop:

lb a0, $0000(s0)
bne a0, zero, 2
nop
sb zero, $0000(s1)

beq a0, zero, :sprintf_quit
addiu v0, zero, 1

//----------------------------------------- Format %
addiu v0, zero, $25
bne a0, v0, :sprintf_skip_%
nop

addiu s0, s0, 1
lb a0, $0000(s0)

//---------------------------------- %c
addiu v0, zero, $63
bne v0, a0, :sprintf_%_d
nop

lb v0, $0000(s2)
sb v0, $0000(s1)
addiu s2, s2, 4

beq zero, zero, :sprintf_nextChar
nop

//---------------------------------- %d
sprintf_%_d:
addiu v0, zero, $64
bne v0, a0, :sprintf_%_s
nop

lw a0, $0000(s2)
jal :HexToStrDec
addiu a1, sp, $0040

addiu s2, s2, 4

addiu a0, sp, $0040
jal :StrConcat
daddu a1, s1, zero

jal :StrLen
daddu a0, s1, zero
daddiu s1, v1, -1


beq zero, zero, :sprintf_nextChar
nop

//---------------------------------- %s
sprintf_%_s:
addiu v0, zero, $73
bne v0, a0, :sprintf_%_x
nop

lw a0, $0000(s2)
addiu s2, s2, 4

// Check for NULL pointer
beq a0, zero, :sprintf_quit
daddu v0, zero, zero

jal :StrConcat
daddu a1, s1, zero

jal :StrLen
daddu a0, s1, zero
daddiu s1, v1, -1


beq zero, zero, :sprintf_nextChar
nop

//---------------------------------- %x
sprintf_%_x:
addiu v0, zero, $78
bne v0, a0, :sprintf_skip_%
nop

jal :StrLen
daddu a0, s1, zero


lw a0, $0000(s2)
addiu s2, s2, 4

jal :Hex32ValToStr
daddu a1, v1, zero

jal :StrLen
daddu a0, s1, zero
daddiu s1, v1, -1

beq zero, zero, :sprintf_nextChar
nop
sprintf_skip_%:

addiu v0, zero, $5C
bne a0, v0, :sprintf_skip_\
nop

//----------------------------------------- Format \

addiu s0, s0, 1
lb a0, $0000(s0)

//---------------------------------- \n
addiu v0, zero, $6e
bne v0, a0, :sprintf_skip_\
nop

addiu v0, zero, $0A
sb v0, $0000(s1)

beq zero, zero, :sprintf_nextChar
nop


beq zero, zero, :sprintf_nextChar
nop
sprintf_skip_\:

//----------------------------------------- Copy Normal Byte
sb a0, $0000(s1)



//----------------------------------------- Next Char
sprintf_nextChar:

addiu s0, s0, 1
beq zero, zero, :sprintf_mainCopyLoop
addiu s1, s1, 1

//-------------------------------------------------


addiu v0, zero, 1


sprintf_quit:
lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
jr ra
addiu sp, sp, $0080


//============================================================
/*
Input:
   a0 - 32 bit Hex Value
   a1 - Destination String
*/
Hex32ValToStr:

addiu sp, sp, $FFC0
sq s0, $0000(sp)
sq ra, $0010(sp)

daddu s0, a1, zero

addiu v0, sp, $0020
sw a0, $0000(v0)

addiu a0, sp, $0020
addiu a1, sp, $0028
jal :StrToHex
addiu a2, zero, 4

addiu a0, sp, $0028
jal :ReverseStr
daddu a1, s0, zero

lq s0, $0000(sp)
lq ra, $0010(sp)
jr ra
addiu sp, sp, $0040

//============================================================
// Reverse String
/*
Input:
   a0 = Source String
   a1 = Destination String
Ouput:
   v0 = 1 - Success
        0 - Failed
*/

ReverseStr:

addiu sp, sp, $FFC0
sq s0, $0000(sp)
sq s1, $0010(sp)
sq s2, $0020(sp)

daddu s0, a0, zero
daddu s1, a1, zero
daddu s2, zero, zero

//---------------------------- Copy Temp + Count Size
revStr_Copy:

lb v0, $0000(a0)

addiu a0, a0, 1
bne v0, zero, :revStr_Copy
addiu s2, s2, 1

addiu s2, s2, -2

blez s2, :revStr_Exit
daddu v0, zero, zero

daddu s0, s0, s2

//---------------------------- Reverse the String
revStr_Reverse:

lb v0, $0000(s0)
sb v0, $0000(s1)

addiu s0, s0, -1
addiu s1, s1, 1
bgtz s2, :revStr_Reverse
addiu s2, s2, -1

addiu v0, zero, 1

revStr_Exit:
lq s0, $0000(sp)
lq s1, $0010(sp)
lq s2, $0020(sp)
jr ra
addiu sp, sp, $0040

//============================================================
// String To Hex-String
/*
Input:
   a0 = Source String
   a1 = Destination String
   a2 = Source Length
*/

StrToHex:

addiu sp, sp, $FFC0
sq s0, $0000(sp)
sq s1, $0010(sp)
sq s2, $0020(sp)

daddu s0, a0, zero
daddu s1, a1, zero
daddu s2, a2, zero

hts_Loop:

lb a0, $0000(s0)

dsll32 v0, a0, 28
dsrl32 v0, v0, 28
srl v1, a0, 4
dsll32 v1, v1, 28
dsrl32 v1, v1, 28

addiu a0, v0, $0030
addiu a1, v1, $0030

slti v0, a0, $003a
slti v1, a1, $003a

bne v0, zero, 2
nop
addiu a0, a0, $0007

bne v1, zero, 2
nop
addiu a1, a1, $0007

sb a0, $0000(s1)
sb a1, $0001(s1)
sb zero, $0002(s1)

addiu s0, s0, 1
addiu s1, s1, 2
addiu s2, s2, -1
bne s2, zero, :hts_Loop
nop


lq s0, $0000(sp)
lq s1, $0010(sp)
lq s2, $0020(sp)
jr ra
addiu sp, sp, $0040


//============================================================
// Concat Strings
/*
   a0 = Source String
   a1 = Destination String
*/
StrConcat:

addiu sp, sp, $FFC0
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)

daddu s0, a0, zero
daddu s1, a1, zero

jal :StrLen
daddu a0, s1, zero

daddu a1, v1, zero
daddu a2, zero, zero
jal :StrCopy
daddu a0, s0, zero


lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
jr ra
addiu sp, sp, $0040

//============================================================
// String Compare
/*
Input:
   a0 = Source 1
   a1 = String 2

Output:
   v0 = 0 (not equal) 1 (equal)
*/

StrCompare:

addiu sp, sp, $FFC0
sq s0, $0000(sp)
sq s1, $0010(sp)
sq s2, $0020(sp)

daddu s0, a0, zero
daddu s1, a1, zero

_str_cmp_loop:

lb v0, $0000(s0)
lb v1, $0000(s1)
bne v0, v1, :_str_cmp_exit_0
nop

beq v0, zero, :_str_cmp_exit_1
nop

addiu s0, s0, 1
addiu s1, s1, 1
beq zero, zero, :_str_cmp_loop
nop


_str_cmp_exit_0:
daddu v0, zero, zero
beq zero, zero, 2
nop

_str_cmp_exit_1:
addiu v0, zero, 1

lq s0, $0000(sp)
lq s1, $0010(sp)
lq s2, $0020(sp)
jr ra
addiu sp, sp, $0040

//============================================================
// String Copy
/*
Input:
   a0 = Source String
   a1 = Destination String
   a2 = Source Start Position (index)

Output:
   v0 = End of Source String (NULL + 1)
*/
StrCopy:

addiu sp, sp, $FFC0
sq s0, $0000(sp)
sq s1, $0010(sp)
sq s2, $0020(sp)

daddu s0, a0, a2   // Source + Index
daddu s1, a1, zero // Destination

_StrCpyLoop:

lb v0, $0000(s0)
sb v0, $0000(s1)

beq v0, zero, :_StrCpyExit
addiu v0, s0, 1

addiu s0, s0, 1
addiu s1, s1, 1
beq zero, zero, :_StrCpyLoop
nop

_StrCpyExit:
lq s0, $0000(sp)
lq s1, $0010(sp)
lq s2, $0020(sp)
jr ra
addiu sp, sp, $0040

//============================================================
// String Clear
/*
Input:
   a0 = String
*/
StrClear:

addiu sp, sp, $FFC0
sq s0, $0000(sp)
sq s1, $0010(sp)
sq s2, $0020(sp)

daddu s0, a0, zero // String

_StrClrLoop:

lb v0, $0000(s0)
sb zero, $0000(s0)

beq v0, zero, :_StrClrExit
nop

addiu s0, s0, 1
beq zero, zero, :_StrClrLoop
nop

_StrClrExit:
lq s0, $0000(sp)
lq s1, $0010(sp)
lq s2, $0020(sp)
jr ra
addiu sp, sp, $0040

//============================================================
// Strlen()
/*
Input:
   a0 = String
Output:
   v0 = String Length
   v1 = End of String
*/

StrLen:

addiu sp, sp, $FFE0
sq ra, $0000(sp)
sq s0, $0010(sp)

daddu s0, a0, zero
daddu v0, zero, zero

StrLenLoop:
lb v1, $0000(s0)
beq v1, zero, :StrLenExit
nop
addiu v0, v0, 1
beq zero, zero, :StrLenLoop
addiu s0, s0, 1

StrLenExit:
daddu v1, s0, zero
lq ra, $0000(sp)
lq s0, $0010(sp)
jr ra
addiu sp, sp, $0020

//============================================================
// Val();
/*
Input:
   a0 - Dec String
Output:
   v0 - Value
*/
StrDecVal:

addiu sp, sp, $FF80
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq s2, $0030(sp)
sq s3, $0040(sp)

daddu s0, a0, zero
daddu s1, zero, zero
daddu s2, zero, zero
daddu s3, zero, zero
daddu s4, a0, zero

_StrDecVal_FlipLoop:
lb v0, $0000(s0)

addiu v1, zero, $20
beql v0, v1, :_StrDecVal_Loop
addiu s0, s0, -1

beql v0, zero, :_StrDecVal_Loop
addiu s0, s0, -1

beq zero, zero, :_StrDecVal_FlipLoop
addiu s0, s0, 1


_StrDecVal_Loop:
lb v0, $0000(s0)
beq v0, zero, :_StrDecVal_Exit
nop

slti v1, v0, $0030
bnel v1, zero, :_StrDecVal_Exit
daddu s2, zero, zero

slti v1, v0, $003A
beql v1, zero, :_StrDecVal_Exit
daddu s2, zero, zero

addiu v1, zero, $30
subu v0, v0, v1

daddu s3, v0, zero

addiu a0, zero, 10
jal :PowerOf
daddu a1, s1, zero

multu v0, s3, v0

daddu s2, s2, v0

_StrDecVal_Next:
beq s0, s4, :_StrDecVal_Exit
nop
addiu s1, s1, 1
beq zero, zero, :_StrDecVal_Loop
addiu s0, s0, -1


_StrDecVal_Exit:
daddu v0, s2, zero
lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq s2, $0030(sp)
lq s3, $0040(sp)
jr ra
addiu sp, sp, $0080

//============================================================
/*
Input:
   a0 - Hex Value
   a1 - Destination String
*/
HexToStrDec:

addiu sp, sp, $FF80
sq ra, $0000(sp)
sq s0, $0010(sp)
sq s1, $0020(sp)
sq at, $0030(sp)

daddu s0, a1, zero
daddu s1, zero, zero

daddu t0, zero, zero
daddu t1, zero, zero
daddu t2, zero, zero
daddu t3, zero, zero
daddu t4, zero, zero
daddu t5, zero, zero
daddu t6, zero, zero
daddu t7, zero, zero
daddu t8, zero, zero
daddu t9, zero, zero


superLoop:

slt v1, a0, zero
beq v1, zero, 4
nop
addiu v1, zero, 1
dsll32 v1, v1, 0
daddu a0, a0, v1

lui v0, $0098
ori v0, v0, $9680
slt v1, a0, v0
beq v1, zero, :ADD_10000000
nop

lui v0, $000F
ori v0, v0, $4240
slt v1, a0, v0
beq v1, zero, :ADD_1000000
nop

lui v0, $0001
ori v0, v0, $86A0
slt v1, a0, v0
beq v1, zero, :ADD_100000
nop

addiu v0, zero, 10000
slt v1, a0, v0
beq v1, zero, :ADD_10000
nop

addiu v0, zero, 1000
slt v1, a0, v0
beq v1, zero, :ADD_1000
nop

addiu v0, zero, 100
slt v1, a0, v0
beq v1, zero, :ADD_100
nop

addiu v0, zero, 10
slt v1, a0, v0
beq v1, zero, :ADD_10
nop

daddu t7, a0, zero

beq zero, zero, :superLoopExit
nop

ADD_10000000:

addiu t0, t0, 1

beq zero, zero, :superLoopNext
nop
ADD_1000000:

addiu t1, t1, 1

beq zero, zero, :superLoopNext
nop
ADD_100000:

addiu t2, t2, 1

beq zero, zero, :superLoopNext
nop
ADD_10000:

addiu t3, t3, 1


beq zero, zero, :superLoopNext
nop
ADD_1000:

addiu t4, t4, 1


beq zero, zero, :superLoopNext
nop
ADD_100:

addiu t5, t5, 1


beq zero, zero, :superLoopNext
nop
ADD_10:

addiu t6, t6, 1

superLoopNext:

subu a0, a0, v0

beq zero, zero, :superLoop
addiu s1, s1, 1

superLoopExit:

daddu a0, t7, zero
bgezal zero, :RegFix
daddu a1, t6, zero

daddu t7, v0, zero
daddu t6, v1, zero

daddu a0, t6, zero
bgezal zero, :RegFix
daddu a1, t5, zero

daddu t6, v0, zero
daddu t5, v1, zero

daddu a0, t5, zero
bgezal zero, :RegFix
daddu a1, t4, zero

daddu t5, v0, zero
daddu t4, v1, zero

daddu a0, t4, zero
bgezal zero, :RegFix
daddu a1, t3, zero

daddu t4, v0, zero
daddu t3, v1, zero

daddu a0, t3, zero
bgezal zero, :RegFix
daddu a1, t2, zero

daddu t3, v0, zero
daddu t2, v1, zero

daddu a0, t2, zero
bgezal zero, :RegFix
daddu a1, t1, zero

daddu t2, v0, zero
daddu t1, v1, zero

daddu a0, t1, zero
bgezal zero, :RegFix
daddu a1, t0, zero

daddu t1, v0, zero
daddu t0, v1, zero

daddu a0, t0, zero
bgezal zero, :RegFix
daddu a1, t9, zero

daddu t0, v0, zero
daddu t9, v1, zero

daddu a0, t9, zero
bgezal zero, :RegFix
daddu a1, t8, zero

daddu t9, v0, zero
daddu t8, v1, zero

bgezal zero, :WriteStr
daddu a0, s0, zero

bgezal zero, :DelInSigs
daddu a0, s0, zero

daddu v1, s1, zero

lq ra, $0000(sp)
lq s0, $0010(sp)
lq s1, $0020(sp)
lq at, $0030(sp)
jr ra
addiu sp, sp, $0080


RegFix:

daddu v0, a0, zero
daddu v1, a1, zero

RFixLoop:

slti v0, a0, 10
bne v0, zero, :RFixLeave
daddu v0, a0, zero

addiu a0, a0, -10
addiu v1, v1, 1

beq zero, zero, :RFixLoop
nop

RFixLeave:
daddu v0, a0, zero

jr ra
nop


WriteStr:

addiu t0, t0, $0030
addiu t1, t1, $0030
addiu t2, t2, $0030
addiu t3, t3, $0030
addiu t4, t4, $0030
addiu t5, t5, $0030
addiu t6, t6, $0030
addiu t7, t7, $0030
addiu t8, t8, $0030
addiu t9, t9, $0030

sb t8, $0000(a0)
sb t9, $0001(a0)
sb t0, $0002(a0)
sb t1, $0003(a0)
sb t2, $0004(a0)
sb t3, $0005(a0)
sb t4, $0006(a0)
sb t5, $0007(a0)
sb t6, $0008(a0)
sb t7, $0009(a0)
sb zero, $000a(a0)

jr ra
nop

DelInSigs:

daddu a1, a0, zero

DelInSigsLoop1:
lb v0, $0000(a1)
addiu v1, zero, $0030
beq v0, v1, :DelInSigsLoop1
addiu a1, a1, 1
beq v0, zero, :DelInSigsZero
nop


addiu a1, a1, -1

DelInSigsLoop2:
lb v0, $0000(a1)
sb v0, $0000(a0)
addiu a0, a0, 1
bne v0, zero, :DelInSigsLoop2
addiu a1, a1, 1
beq zero, zero, :DelInSigsExit
nop

DelInSigsZero:
sb zero, $0001(a0)

DelInSigsExit:
jr ra
nop

//============================================================
// Non-String Imported Functions


//---------------------------------------------- Power Of (a0^a1)
PowerOf:

daddiu v0, zero, 1

_PowerOfLoop:
beq a1, zero, :_PowerOfExit
nop

multu v0, v0, a0

beq zero, zero, :_PowerOfLoop
addiu a1, a1, -1

_PowerOfExit:
jr ra
nop
