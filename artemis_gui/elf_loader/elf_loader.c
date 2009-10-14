
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <string.h>
#include <stdio.h>

/* GS macro */
#define GS_BGCOLOUR	*((vu32*)0x120000e0)

char boot_path[1024];

/*
 * launch OSDSYS in case elf load failed (if fmcb installed, reloads it)
 */
void launch_OSDSYS(void)
{
	__asm__ __volatile__(
		"	li $3, 0x04;"
		"	syscall;"
		"	nop;"
	);
}

/*
 * Execute an elf with SifLoadElf + ExecPS2
 */
void execute_elf(char *path)
{
	t_ExecData elf;
	int r;
	char *args[1];

	memset(&elf, 0, sizeof(t_ExecData));

    SifLoadFileInit();
	r = SifLoadElf(path, &elf);
	if ((!r) && (elf.epc)) {

  	    SifLoadFileExit();
  	  	fioExit();
  	  	SifExitRpc(); /* some programs need it to be here */

  	  	FlushCache(0);
  	  	FlushCache(2);

    	args[0] = path;
    	ExecPS2((void*)elf.epc, (void*)elf.gp, 1, args);

    	GS_BGCOLOUR = 0x0000ff; /* Error, Red screen */
    	while (1){;}
  	}
  	else {
  	    SifLoadFileExit();
  	  	fioExit();
  	  	SifExitRpc();

  		launch_OSDSYS();
	}

 	GS_BGCOLOUR = 0xffffff; /* Error, White screen */
  	while (1){;}
}

/*
 * main function
 */
int main (int argc, char *argv[1])
{
	int i;

	/* preserves elf path before clearing stack !!! */
	strcpy(boot_path, argv[0]);

	/* clearing mem, so better not to have anything valuable on stack */
	for (i = 0x100000; i < 0x2000000 ; i += 64) {
		asm (
			"\tsq $0, 0(%0) \n"
			"\tsq $0, 16(%0) \n"
			"\tsq $0, 32(%0) \n"
			"\tsq $0, 48(%0) \n"
			:: "r" (i) );
	}

	SifInitRpc(0);

	execute_elf(argv[0]);

	return 0;
}
