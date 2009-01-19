# *********************************************************************
# Cheat engine V1.4
# 

# ASM
.include "r5900_regs.h"

.set noreorder
#.set noat


.extern jal_cheats
.global j_defhandler
.extern scr_printf

.section .resident, "a"

	.global write_hook
	.ent	write_hook
write_hook:
	addiu $sp, $sp,	-40
	sq $ra,		0($sp)
	sq $v0,		16($sp)
	sq $v1,		32($sp)

	# PAD READ MEMCOPY

	li $v0,		0x001bfa68

	# END PAD READ MEMCOPY
	# copy orional jal code
	li $v1,		0x0c0655f4
	sw $v1,		_orig_jal

	lw $v1,		jal_cheats 	# write jal on padread
	sw $v1,		0($v0)

	# END SOCOM 2 MEMORY VIEWER HOOK

	lq $v1,		32($sp)
	lq $v0,		16($sp)
	lq $ra,		0($sp)
	addiu $sp, $sp,	40
	j_defhandler:
	nop
	nop
	jr $ra
	nop
	.end write_hook
	
	.global write_cheats
	.ent	write_cheats
write_cheats:
	addiu $sp, $sp, -26
	sq $ra,		0($sp)

	nop
	# CODES
	
	#read joker
	lui		$t7,	0x004
	lh		$t8,	0xD81C($t7)
	# Left
	addiu		$t7,	$zero, 	0xFF7F
	bne		$t7,	$t8, 	end
	nop
	#Do Graphics

__repeateLoop:
	addiu $t2, $zero, 0xFF # B
	addiu $a0, $zero, 0x1E # X1
	addiu $a1, $zero, 0x23 # Y1
	addiu $a2, $zero, 0x64 # X2
	addiu $a3, $zero, 0x65 # Y2
	addiu $t0, $zero, 0xFF # R
	addiu $t1, $zero, 0xFF # G
	jal DrawRect
	nop

	beq $zero, $zero, __repeateLoop
	nop
	#End Graphics
	end:
	# END CODES
	
_orig_jal:
	nop
	nop
	lq $ra,		0($sp)
	jr $ra
	addiu $sp, $sp, 26
	nop
	.end write_cheats

	.global DrawRect
	.ent DrawRect
DrawRect:
	andi		$t1, $t1, 0x00ff	# 00100ff0:312900ff	
	sll		$a3, $a3, 16		# 00100ff4:00073c00	
	dsll		$t1, $t1, 8		# 00100ff8:00094$a38	
	andi		$t0, $t0, 0x00ff	# 00100ffc:310800ff	
	andi		$t2, $t2, 0x00ff	# 00101000:314a00ff	
	or		$t0, $t0, $t1		# 00101004:01094025	
	dsll		$t2, $t2, 16		# 00101008:000a5438	
	sll		$a1, $a1, 16		# 0010100c:00052c00	
	sll		$a2, $a2, 16		# 00101010:00063400	
	sra		$a3, $a3, 12		# 00101014:00073b03	
	or		$t0, $t0, $t2		# 00101018:010a4025	:Arguments
	ori		$v0, $zero, 0x8000	# 0010101c:34028000	$v0=0x00008000
	dsll		$v0, $v0, 16		# 00101020:00021438	
	sra		$a1, $a1, 12		# 00101024:00052b03	
	sra		$a2, $a2, 12		# 00101028:00063303	
	addiu		$a3, $a3, 0x0010	# 0010102c:24e70010	
#	lui		$v1, 0x0011		# 00101030	:        fix
	la		$v1, gs_dma_buf
	or		$t0, $t0, $v0		# 00101034:01024025	
	dsll		$a3, $a3, 16		# 00101038:00073c38	
	lui		$v0, 0x1000		# 0010103c:3c021000	$v0=0x10000000
	dsll32		$v0, $v0, 0		# 00101040:0002103c	
	ori		$v0, $v0, 0x8004	# 00101044:34428004	$v0=0x10008004
	sll		$a0, $a0, 16		# 00101048:00042400	
	addiu		$a2, $a2, 0x0010	# 0010104c:24c60010	
	sd		$v0, 0x0000($v1)	# 00101050	:      (store at gs_dma_buf)
	or		$a2, $a2, $a3		# 00101054:00c73025	
	sra		$a0, $a0, 12		# 00101058:00042303	
	la		$v0, gs_dma_buf_cur		# 0010105c	: 
	addiu		$a3, $zero, 0x000a	# 00101060:2407000a	$a3=0x0000000a
	dsll		$a1, $a1, 16		# 00101064:00052c38	
	or		$a1, $a0, $a1		# 00101068:00852825	
	addiu		$t2, $zero, 0x0005	# 0010106c:240a0005	$t2=0x00000005
	la		$a0, gs_dma_buf		# 00101070	: 
	sw		$a3, 0x0($v0)		# 00101074	:  (store at gs_dma_buf_cur)
	la		$v1, gs_dma_buf_dma_size# 00101078	:  
	addiu		$v0, $zero, 0x000e	# 0010107c:2402000e	$v0=0x0000000e
	addiu		$t1, $zero, 0x0005	# 00101080:24090005	$t1=0x00000005
	sw		$t2, 0x0000($v1)	# 00101084	:       (store at gs_dma_buf_dma_size)
	sd		$v0, 0x0008($a0)	# 00101088:fc820008	2
	addiu		$v1, $zero, 0x0006	# 0010108c:24030006	$v1=0x00000006
	addiu		$v0, $zero, 0x0001	# 00101090:24020001	$v0=0x00000001
	sd		$v1, 0x0010($a0)	# 00101094:fc830010	3
	sd		$t0, 0x0020($a0)	# 00101098:fc880020	5
	sd		$v0, 0x0028($a0)	# 0010109c:fc820028	6
	sd		$a1, 0x0030($a0)	# 001010a0:fc850030	7
	sd		$a2, 0x0040($a0)	# 001010a4:fc860040	9
	sd		$t1, 0x0048($a0)	# 001010a8:fc890048	10
	sd		$zero, 0x0018($a0)	# 001010ac:fc800018	4
	sd		$t1, 0x0038($a0)	# 001010b0:fc890038	8					# 
	addiu		$v1, $zero, 0x0064	# 001010b4:24030064	$v1=0x00000064
	syscall		(00000)			# 001010b8:0000000c	
	lui		$v0, 0x1000		# 001010bc:3c021000	$v0=0x10000000
	lui		$v1, 0x1000		# 001010c0:3c031000	$v1=0x10000000
	ori		$v0, $v0, 0xa020	# 001010c4:3442a020	$v0=0x1000a020
	lui		$a1, 0x1000		# 001010c8:3c051000	$a1=0x10000000
	sw		$t2, 0x0000($v0)	# 001010cc:ac4a0000	[1000a020]
	ori		$v1, $v1, 0xa010	# 001010d0:3463a010	$v1=0x1000a010
	ori		$a1, $a1, 0xa000	# 001010d4:34a5a000	$a1=0x1000a000
	addiu		$v0, $zero, 0x0101	# 001010d8:24020101	$v0=0x00000101
	sw		$a0, 0x0000($v1)	# 001010dc:ac640000	[1000a010]
	daddu		$v1, $a1, $zero		# 001010e0:00a0182d	$v1=0x1000a000
	sw		$v0, 0x0000($a1)	# 001010e4:aca20000	[1000a000]
__loopBack1:					# 
	lw		$v0, 0x0000($v1)	# 001010e8:8c620000	$v0=0x1000a000
	andi		$v0, $v0, 0x0100	# 001010ec:30420100	
	nop					# 001010f0:00000000	
	bne		$v0, $zero, __loopBack1	# 001010f4:1440fffc	^ __001010e8
	nop					# 001010f8:00000000	
	jr		$ra			# 001010fc:03e00008	
	nop					# 00101100:00000000	

	nop
	.end DrawRect

gs_dma_buf_dma_size:
	.word 0x0
gs_dma_buf_cur:
	.word 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
gs_dma_buf:
	.word 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
	.word 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0

