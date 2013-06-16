/*
	CodeDesigner v2.0
	Created by: Gtlcpimp
	Official CheatersLounge Product Copyright ©
*/
/*
    Kernel Class
*/

//============================================================
// Disable Interrupt

DI:
mfc0 v0, $Status
lui v1, $0001
and v0, v0, v1

beq v0, zero, :DI_Exit
nop

DI_Loop:

hexcode $42000039 // di
sync.p

mfc0 v0, $Status
and v0, v0, v1
bne v0, zero, :DI_Loop
nop

DI_Exit:
jr ra
nop

//============================================================
// Enable Interrupt

EI:
hexcode $42000038 // ei
jr ra
nop

//============================================================
// Enter User Kernel Mode

ee_kmode_enter:
mfc0 v0, $Status
addiu v1, zero, -25
and v0, v0, v1
mtc0 v0, $Status
sync.p
jr ra
nop

//============================================================
// Exit User Kernel Mode

ee_kmode_exit:
mfc0 v0, $Status
ori v0, v0, $0010
mtc0 v0, $Status
sync.p
jr ra
nop

