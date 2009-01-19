#include <tamtypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <loadfile.h>
#include <fileio.h>
#include <debug.h>
#include "libpad.h"
#include "malloc.h"
#include "libcdvd.h"
#include "gsKit.h"
#include "dmaKit.h"
#include "disassembler_regs.h"
//#include "defines.h"

//#define VERSION "v0.1a"
//#define _useGS 1


int main(void)
{
	//SifInitRpc(0);
	// init debug screen
	//init_scr();


	//scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);

	u32 address = 0x0010013c;
	//u32 history = address;
	opPacket opp;
	//int count = 1;
	//history = (u32*)malloc(count * sizeof(u32));

	opp = execOp(address);

	//scr_printf("%s",opp.opString);
	while(address < 0x0010023c){
		//memset(&opp.opString, 0, sizeof(opp.opString));

			address +=4;
			opp = execOp(address);
			//scr_clear();
			//scr_printf("\tWelcome to Driver's disassembler %s\n\n",VERSION);
			printf("%s",opp.opString);
	}

    return 0;
}