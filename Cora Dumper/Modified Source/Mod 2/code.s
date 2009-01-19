# *********************************************************************
# Copyright (C) 2006 Jordan Hamill
# This file is subject to the terms and conditions of the GNU License.
# See the file LICENSE in the main directory of this distribution for more
# details. 
# 
# PAL SOCOM 2 CHEATS
# 

# ASM
.include "r5900_regs.h"

.set noreorder


# main function variables
.extern jal_cheats
.extern masterCodeAddr
.global j_defhandler

.extern jal_scr_init



#.section .resident, "a"

	.global cheat_hook
	.ent	cheat_hook
cheat_hook:
	addiu $sp, $sp,	-240
	sq $ra,		0($sp)
	sq $s0,		16($sp)
	sq $s1,		32($sp)
	sq $s2,		48($sp)
	sq $s3,		64($sp)
	sq $s4,		80($sp)
	sq $s5,		96($sp)
	sq $s6,		112($sp)
	sq $s7,		128($sp)
	sq $a0,		144($sp)
	sq $a1,		160($sp)
	sq $a2,		176($sp)
	sq $v0,		192($sp)
	sq $v1,		208($sp)
	
	# PAD READ MEMCOPY
	
	li $v0,		0x001bfa68
	#lw $v0,		masterCodeAddr
	
	# END PAD READ MEMCOPY
	# copy orional jal code
	li $v1,		0x0c0655f4
	sw $v1,		_orig_jal
	
	lw $v1,		jal_cheats 	# write jal on padread
	sw $v1,		0($v0)
	


	# END SOCOM 2 MEMORY VIEWER HOOK
	
	lq $v1,		208($sp)
	lq $v0,		192($sp)
	lq $a2,		176($sp)
	lq $a1,		160($sp)
	lq $a0,		144($sp)
	lq $s7,		128($sp)
	lq $s6,		112($sp)
	lq $s5,		96($sp)
	lq $s4,		80($sp)
	lq $s3,		64($sp)
	lq $s2,		48($sp)
	lq $s1,		32($sp)
	lq $s0,		16($sp)
	lq $ra,		0($sp)
	addiu $sp, $sp,	240
	#addiu $sp, $sp, -192
	j_defhandler:
	nop
	nop
	jr $ra
	nop
	.end cheat_hook
	
	
	.global write_cheats
	.ent	write_cheats
write_cheats:
	addiu $sp, $sp, -240
	sq $ra,		0($sp)
	sq $s0,		16($sp)
	sq $s1,		32($sp)
	sq $s2,		48($sp)
	sq $s3,		64($sp)
	sq $s4,		80($sp)
	sq $s5,		96($sp)
	sq $s6,		112($sp)
	sq $s7,		128($sp)
	sq $a0,		144($sp)
	sq $a1,		160($sp)
	sq $a2,		176($sp)
	sq $v0,		192($sp)
	sq $v1,		208($sp)
	
	# CODES
	
	# SOCOM 2 MEMORY VIEWER HOOK ( i think i just chose this position so that it would initalise when i went in scope in game)
	li $v0,		0x00210660
	lw $v1,		jal_scr_init
	sw $v1,		0($v0)
	
	li $v0,		0x005b9ad0
	li $v1,		0x3c010008
	sw $v1,		0x0($v0)
	li $v0,		0x005b9adc
	li $v1,		0xac350030
	sw $v1,		0x0($v0)
	# END SOCOM 2 MEMORY VIEWER HOOK

	# END CODES
	lq $v1,		208($sp)
	lq $v0,		192($sp)
	lq $a2,		176($sp)
	lq $a1,		160($sp)
	lq $a0,		144($sp)
	lq $s7,		128($sp)
	lq $s6,		112($sp)
	lq $s5,		96($sp)
	lq $s4,		80($sp)
	lq $s3,		64($sp)
	lq $s2,		48($sp)
	lq $s1,		32($sp)
	lq $s0,		16($sp)
	
_orig_jal:
	nop
	nop
	lq $ra,		0($sp)
	jr $ra
	addiu $sp, $sp, 240
	nop
	.end write_cheats

	nop