##
# EE startup file
##

	.set	noat
	.set	noreorder

	.globl	_start
	.globl	_exit

# Support for _init() and _fini().
	.globl	_init
	.globl	_fini
	.type	_init, @function
	.type	_fini, @function

# The .weak keyword ensures there's no error if _init/_fini aren't defined.
	.weak	_init
	.weak	_fini

	.extern	_heap_size
	.extern	_stack
	.extern	_stack_size

	.text

	nop
	nop

	.ent _start
_start:

# Clear bss section (static uninitialized data).
zerobss:
	la	$v0, _fbss
	la	$v1, _end
loop:
	nop
	nop
	nop
	sq	$zero, 0($v0)
	sltu	$at, $v0, $v1
	bne	$at, $zero, loop
	addiu	$v0, $v0, 16

# Some program loaders (such as Pukklink) execute programs as a thread, but
# support passing argc and argv values via a0.  This also indicates that when
# the program terminates, control should be returned to the program loader
# instead of the PS2 browser.
	la	$v0, _args_ptr
    	sw	$a0, 0($v0)

# Setup a thread to use.
	la	$a0, _gp
	la	$a1, _stack
	la	$a2, _stack_size
	la	$a3, _args
	la	$t0, _root
	move	$gp, $a0
	li	$v1, 60
	syscall			# SetupThread(gp, stack, stack_size, args, root_func)
	move	$sp, $v0

# Heap
	li	$v1, 61
	la	$a0, _end
	la	$a1, _heap_size
	syscall			# SetupHeap(heap_start, heap_size)
	nop

# Flush the data cache (no need to preserve regs for this call).
	li	$v1, 0x64
	move	$a0, $zero
	syscall			# FlushCache(0) - Writeback data cache

# Call ps2sdk's libc initialization.
	jal	_ps2sdk_libc_init
	nop

# Call global constructors through _init().
#	la	$t0, _init
#	beqz	$t0, 1f		# Does _init() exist?
#	nop
#	jalr	$t0
#	nop
1:

# Jump to main(), now that environment and args are setup.
	ei

# Check for arguments passed via ExecPS2() or LoadExecPS2().
	la	$v0, _args
	lw	$v1, 0($v0)
	bnez	$v1, 1f
	nop

# Otherwise check for arguments passed by a loader via a0 (_arg_ptr).
	la	$v0, _args_ptr
	lw	$v1, 0($v0)
	beqzl	$v1, 2f
	move	$a0, $zero
	addiu	$v0, $v1, 4
1:
	lw	$a0, 0($v0)
	addiu	$a1, $v0, 4
2:
	jal	main
	nop
	.end	_start

	.ent	_exit
_exit:

# Call global deconstructors through _fini().
#	la	$t0, _fini
#	beqz	$t0, 3f		# Does _fini() exist?
#	nop
#	jalr	$t0
#	nop
3:

# Call ps2sdk's libc deinitialization.
	jal     _ps2sdk_libc_deinit
	nop

# If we received our program arguments in a0, then we were executed by a
# loader, and we don't want to return to the browser.
	la	$a0, _args_ptr
	lw	$a1, 0($a0)
	beqz	$a1, 1f
	move	$a0, $v0	# main()'s return code
	lw	$a2, 0($a1)
	sw	$zero, 0($a2)
	li	$v1, 36
	syscall			# ExitDeleteThread(void)

# Return to the browser via Exit().
1:
	li	$v1, 4
	syscall			# Exit(exit_code)
	.end	_exit

	.ent	_root
_root:
	li	$v1, 35
	syscall			# ExitThread(void)
	.end	_root

	.bss
	.align	6
_args:
	.space	256+16*4+4
_args_ptr:
	.space	4

# EOF
