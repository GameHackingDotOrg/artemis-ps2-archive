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
#include "disassembler_regs.h"
//create register set
cpuRegisters cpuRegs;
opPacket oppPacket;

char *printOp(void){
	char *final = "";
	final = (char*)malloc(sizeof(char)*500);
	sprintf(final,"%8.8x : %8.8x -> %s  %s%s%s%s  Result: %8.8x\n",
		oppPacket.address,
		oppPacket.data,
		oppPacket.name,
		oppPacket.regString,
		oppPacket.immidiateString,
		oppPacket.loadoffsetString,
		oppPacket.targetString,
		oppPacket.u32Result);
	printf("%s",final);
	return final;
}

void UnknownOpcode() {
	printf("Unknown Opp %0x\n",cpuRegs.code);
}

char *getGPReg(int reg)
{
	if(reg == 0){return "zero";}
	if(reg == 1){return "at";}
	if(reg == 2){return "v0";}
	if(reg == 3){return "v1";}
	if(reg == 4){return "a0";}
	if(reg == 5){return "a1";}
	if(reg == 6){return "a2";}
	if(reg == 7){return "a3";}
	if(reg == 8){return "t0";}
	if(reg == 9){return "t1";}
	if(reg == 10){return "t2";}
	if(reg == 11){return "t3";}
	if(reg == 12){return "t4";}
	if(reg == 13){return "t5";}
	if(reg == 14){return "t6";}
	if(reg == 15){return "t7";}
	if(reg == 16){return "s0";}
	if(reg == 17){return "s1";}
	if(reg == 18){return "s2";}
	if(reg == 19){return "s3";}
	if(reg == 20){return "s4";}
	if(reg == 21){return "s5";}
	if(reg == 22){return "s6";}
	if(reg == 23){return "s7";}
	if(reg == 24){return "t8";}
	if(reg == 25){return "t9";}
	if(reg == 26){return "k0";}
	if(reg == 27){return "k1";}
	if(reg == 28){return "gp";}
	if(reg == 29){return "sp";}
	if(reg == 30){return "fp";}
	if(reg == 31){return "ra";}
}

char *getFPReg(int reg) 
{
	char *buffer;
	buffer = (char*)malloc(sizeof(int)*3);
	sprintf(buffer,"f%i",reg);
	return buffer;
}
__inline void doBranch(u32 tar) {

}

void intDoBranch(u32 target) {doBranch(target);}

void intSetBranch() {}

//#pragma region operations
/*********************************************************
* Arithmetic with immediate operand                      *
* Format:  OP rt, rs, immediate                          *
*********************************************************/
void ADDI() 	{ 
	if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].SL[0] + _Imm_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "addi";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	sprintf(oppPacket.immidiateString," $%0x", oppPacket.imm);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));//oppPacket.loadoffsetString = "";
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));//oppPacket.targetString = "";
	
	//printf("1: %s  %s%s%s%s\n",oppPacket.name,oppPacket.regString,oppPacket.immidiateString,oppPacket.loadoffsetString,oppPacket.targetString);
	//printf("%s  %s, %s, $%0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}// Rt = Rs + Im signed!!!!
void ADDIU()    { if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].SL[0] + _Imm_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "addiu";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

	sprintf(oppPacket.immidiateString," $%0x", oppPacket.imm);
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));//oppPacket.loadoffsetString = "";
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));//oppPacket.targetString = "";

	//printf("%s  %s, %s, $%0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}// Rt = Rs + Im signed !!!
void DADDI()    { if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].SD[0] + _Imm_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "daddi";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	sprintf(oppPacket.immidiateString," $%0x", oppPacket.imm);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, $%0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}// Rt = Rs + Im 
void DADDIU()   { if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].SD[0] + _Imm_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "daddiu";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
	
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	sprintf(oppPacket.immidiateString," $%0x", oppPacket.imm);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, $%0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}// Rt = Rs + Im 
void ANDI() 	{ if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] & (s64)_ImmU_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "andi";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
	
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	sprintf(oppPacket.immidiateString," $%0x", oppPacket.imm);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, $%0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}	// Rt = Rs And Im
void ORI() 	    { if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] | (s64)_ImmU_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "ori";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
	
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	sprintf(oppPacket.immidiateString," $%0x", oppPacket.imm);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, $%0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}	// Rt = Rs Or  Im
void XORI() 	{ if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] ^ (s64)_ImmU_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "xori";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
	
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	sprintf(oppPacket.immidiateString," $%0x", oppPacket.imm);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, $%0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}	// Rt = Rs Xor Im
void SLTI()     { if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].SD[0] < (s64)(_Imm_); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "slti";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
	
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	sprintf(oppPacket.immidiateString," $%0x", oppPacket.imm);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, $%0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
} // Rt = Rs < Im (signed)
void SLTIU()    { if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] < (u64)(_Imm_); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "sltiu";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
	
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	sprintf(oppPacket.immidiateString," $%0x", oppPacket.imm);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, $%0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
} // Rt = Rs < Im (unsigned)

/*********************************************************
* Register arithmetic                                    *
* Format:  OP rd, rs, rt                                 *
*********************************************************/
void ADD()	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].SL[0]  + cpuRegs.GPR.r[_Rt_].SL[0];
	oppPacket.isTargetUsed = 0; oppPacket.name = "add";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	// Rd = Rs + Rt		(Exception on Integer Overflow)
void ADDU() 	{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].SL[0]  + cpuRegs.GPR.r[_Rt_].SL[0];
	oppPacket.isTargetUsed = 0; oppPacket.name = "addu";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	// Rd = Rs + Rt
void DADD()     { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].SD[0]  + cpuRegs.GPR.r[_Rt_].SD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "dadd";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}
void DADDU()    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].SD[0]  + cpuRegs.GPR.r[_Rt_].SD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "daddu";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}
void SUB() 	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].SL[0]  - cpuRegs.GPR.r[_Rt_].SL[0];
	oppPacket.isTargetUsed = 0; oppPacket.name = "sub";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	// Rd = Rs - Rt		(Exception on Integer Overflow)
void SUBU() 	{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].SL[0]  - cpuRegs.GPR.r[_Rt_].SL[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "subu";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	// Rd = Rs - Rt
void DSUB() 	{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].SD[0]  - cpuRegs.GPR.r[_Rt_].SD[0];
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsub";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	
void DSUBU() 	{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].SD[0]  - cpuRegs.GPR.r[_Rt_].SD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsubu";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);

}
void AND() 	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]  & cpuRegs.GPR.r[_Rt_].UD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "and";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	// Rd = Rs And Rt
void OR() 	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]  | cpuRegs.GPR.r[_Rt_].UD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "or";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	// Rd = Rs Or  Rt
void XOR() 	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]  ^ cpuRegs.GPR.r[_Rt_].UD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "xor";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	// Rd = Rs Xor Rt
void NOR() 	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] =~(cpuRegs.GPR.r[_Rs_].UD[0] | cpuRegs.GPR.r[_Rt_].UD[0]); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "nor";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}// Rd = Rs Nor Rt
void SLT()		{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].SD[0] < cpuRegs.GPR.r[_Rt_].SD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "slt";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	// Rd = Rs < Rt (signed)
void SLTU()		{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] < cpuRegs.GPR.r[_Rt_].UD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "sltu";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.imm = _Imm_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}	// Rd = Rs < Rt (unsigned)

/*********************************************************
* Jump to target                                         *
* Format:  OP target                                     *
*********************************************************/

void J()   {
	oppPacket.isTargetUsed = 0; oppPacket.name = "j";
	oppPacket.target = _JumpTarget_; oppPacket.isTargetUsed = 1;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString,0,sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s \t %0x\n",oppPacket.name,oppPacket.target);
	doBranch(_JumpTarget_);
}

void JAL() {
	oppPacket.isTargetUsed = 0; oppPacket.isTargetUsed = 0; oppPacket.name = "jal";
	oppPacket.target = _JumpTarget_; oppPacket.isTargetUsed = 1;
	_SetLink(31);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString,0,sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s \t %0x\n",oppPacket.name,oppPacket.target);
	_SetLink(31); doBranch(oppPacket.target);

}

/*********************************************************
* Register jump                                          *
* Format:  OP rs, rd                                     *
*********************************************************/
void JR()   { 
	doBranch(cpuRegs.GPR.r[_Rs_].UL[0]); 
	oppPacket.name = "jr";
	oppPacket.reg[0] = getGPReg(_Rs_);


	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s \t %0x\n",oppPacket.name,oppPacket.reg[0], 0x0);//FIX AND PLACE REGISTER RESULT
}

void JALR() { 
	u32 temp = cpuRegs.GPR.r[_Rs_].UL[0];

	if (_Rd_) { _SetLink(_Rd_); }
	doBranch(temp);
	oppPacket.name = "jalr";
	oppPacket.reg[0] = getGPReg(_Rs_);
	_SetLink(31);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s \t %0x\n",oppPacket.name,oppPacket.reg[0],0x0);//FIX AND PLACE REGISTER RESULT
}


/*********************************************************
* Register mult/div & Register trap logic                *
* Format:  OP rs, rt                                     *
*********************************************************/
void DIV() {
    if (cpuRegs.GPR.r[_Rt_].SL[0] != 0) {
        cpuRegs.LO.SD[0] = cpuRegs.GPR.r[_Rs_].SL[0] / cpuRegs.GPR.r[_Rt_].SL[0];
        cpuRegs.HI.SD[0] = cpuRegs.GPR.r[_Rs_].SL[0] % cpuRegs.GPR.r[_Rt_].SL[0];
    }
	oppPacket.isTargetUsed = 0;oppPacket.isTargetUsed = 0; oppPacket.name = "div";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

void DIVU() {
	if (cpuRegs.GPR.r[_Rt_].UL[0] != 0) {
		cpuRegs.LO.SD[0] = cpuRegs.GPR.r[_Rs_].UL[0] / cpuRegs.GPR.r[_Rt_].UL[0];
		cpuRegs.HI.SD[0] = cpuRegs.GPR.r[_Rs_].UL[0] % cpuRegs.GPR.r[_Rt_].UL[0];
	}
	oppPacket.isTargetUsed = 0;oppPacket.isTargetUsed = 0; oppPacket.name = "divu";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

void MULT() { //different in ps2...
	s64 res = (s64)cpuRegs.GPR.r[_Rs_].SL[0] * (s64)cpuRegs.GPR.r[_Rt_].SL[0];

	cpuRegs.LO.UD[0] = (s32)(res & 0xffffffff);
	cpuRegs.HI.UD[0] = (s32)(res >> 32);

	if (!_Rd_) return;
	cpuRegs.GPR.r[_Rd_].UD[0]= cpuRegs.LO.UD[0]; //that is the difference
	oppPacket.isTargetUsed = 0; oppPacket.isTargetUsed = 0; oppPacket.name = "mult";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));


	//printf("%s  %s, %s\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

void MULTU() { //different in ps2..
	u64 res = (u64)cpuRegs.GPR.r[_Rs_].UL[0] * (u64)cpuRegs.GPR.r[_Rt_].UL[0];

	cpuRegs.LO.UD[0] = (s32)(res & 0xffffffff);
	cpuRegs.HI.UD[0] = (s32)(res >> 32);

	if (!_Rd_) return;
	cpuRegs.GPR.r[_Rd_].UD[0]= cpuRegs.LO.UD[0]; //that is the difference
	oppPacket.isTargetUsed = 0; oppPacket.isTargetUsed = 0; oppPacket.name = "multu";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

/*********************************************************
* Load higher 16 bits of the first word in GPR with imm  *
* Format:  OP rt, immediate                              *
*********************************************************/
void LUI()  { 
	if (!_Rt_) return; 
	cpuRegs.GPR.r[_Rt_].UD[0] = (s32)(cpuRegs.code << 16);
	oppPacket.isTargetUsed = 0; oppPacket.isTargetUsed = 0; oppPacket.name = "lui";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.imm = (u16)(cpuRegs.code << 16);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s\t %0x %s\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result);
}

/*********************************************************
* Move from HI/LO to GPR                                 *
* Format:  OP rd                                         *
*********************************************************/
void MFHI() { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.HI.UD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.isTargetUsed = 0; oppPacket.name = "mfhi";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result);
} // Rd = Hi
void MFLO() { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.isTargetUsed = 0; oppPacket.name = "mflo";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result);
} // Rd = Lo

/*********************************************************
* Move to GPR to HI/LO & Register jump                   *
* Format:  OP rs                                         *
*********************************************************/
void MTHI() { cpuRegs.HI.UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.isTargetUsed = 0; oppPacket.name = "mthi";
	oppPacket.reg[0] = getGPReg(_Rs_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\n",oppPacket.name,oppPacket.reg[0]);
} // Hi = Rs
void MTLO() { cpuRegs.LO.UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "mthi";
	oppPacket.reg[0] = getGPReg(_Rs_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\n",oppPacket.name,oppPacket.reg[0]);
} // Lo = Rs


/*********************************************************
* Shift arithmetic with constant shift                   *
* Format:  OP rd, rt, sa                                 *
*********************************************************/
void SLL()   { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].UL[0] << _Sa_); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "sll";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.imm = _Sa_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, "$%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
} // Rd = Rt << sa
void DSLL()  { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] << _Sa_); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsll";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.imm = _Sa_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, "$%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}
void DSLL32(){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] << (_Sa_+32));
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsll32";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.imm = _Sa_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, "$%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}
void SRA()   { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].SL[0] >> _Sa_); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "sra";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.imm = _Sa_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, "$%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
} // Rd = Rt >> sa (arithmetic)
void DSRA()  { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (u64)(cpuRegs.GPR.r[_Rt_].SD[0] >> _Sa_); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsra";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.imm = _Sa_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, "$%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}
void DSRA32(){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (u64)(cpuRegs.GPR.r[_Rt_].SD[0] >> (_Sa_+32));
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsra32";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.imm = _Sa_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, "$%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}
void SRL()   { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].UL[0] >> _Sa_); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "srl";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.imm = _Sa_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, "$%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
} // Rd = Rt >> sa (logical)
void DSRL() {  if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] >> _Sa_); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsrl";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.imm = _Sa_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, "$%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}
void DSRL32(){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] >> (_Sa_+32));
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsrl32";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.imm = _Sa_;
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	sprintf(oppPacket.immidiateString, " $%.4x",oppPacket.imm);
	sprintf(oppPacket.regString, "$%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %0x \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.imm,oppPacket.u32Result);
}

/*********************************************************
* Shift arithmetic with variant register shift           *
* Format:  OP rd, rt, rs                                 *
*********************************************************/
void SLLV() { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].UL[0] << (cpuRegs.GPR.r[_Rs_].UL[0] &0x1f));
	oppPacket.isTargetUsed = 0; oppPacket.name = "sllv";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.reg[2] = getGPReg(_Rs_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, "$%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
} // Rd = Rt << rs
void SRAV() { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].SL[0] >> (cpuRegs.GPR.r[_Rs_].UL[0] &0x1f));
	oppPacket.isTargetUsed = 0; oppPacket.name = "srav";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.reg[2] = getGPReg(_Rs_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, "$%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
} // Rd = Rt >> rs (arithmetic)
void SRLV() { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].UL[0] >> (cpuRegs.GPR.r[_Rs_].UL[0] &0x1f));
	oppPacket.isTargetUsed = 0; oppPacket.name = "srlv";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.reg[2] = getGPReg(_Rs_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, "$%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
} // Rd = Rt >> rs (logical)
void DSLLV(){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] << (cpuRegs.GPR.r[_Rs_].UL[0] &0x3f));
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsllv";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.reg[2] = getGPReg(_Rs_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, "$%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}  
void DSRAV(){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s64)(cpuRegs.GPR.r[_Rt_].SD[0] >> (cpuRegs.GPR.r[_Rs_].UL[0] &0x3f));
	oppPacket.isTargetUsed = 0; oppPacket.name = "dsrav";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.reg[2] = getGPReg(_Rs_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, "$%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}
void DSRLV(){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] >> (cpuRegs.GPR.r[_Rs_].UL[0] &0x3f));
	oppPacket.isTargetUsed = 0; oppPacket.name = "drslv";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.reg[2] = getGPReg(_Rs_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, "$%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}


/*********************************************************
* Register branch logic                                  *
* Format:  OP rs, rt, offset                             *
*********************************************************/
#define RepBranchi32(op) \
	if (cpuRegs.GPR.r[_Rs_].SD[0] op cpuRegs.GPR.r[_Rt_].SD[0]) doBranch(_BranchTarget_); \
	//else IntcpuBranchTest();


void BEQ() {	RepBranchi32(==) 
	oppPacket.isTargetUsed = 0; oppPacket.name = "beq";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s, %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs == Rt
void BNE() {	RepBranchi32(!=) 
	oppPacket.isTargetUsed = 0; oppPacket.name = "beq";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s, %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs != Rt

/*********************************************************
* Register branch logic                                  *
* Format:  OP rs, offset                                 *
*********************************************************/
#define RepZBranchi32(op) \
	if(cpuRegs.GPR.r[_Rs_].SD[0] op 0) { \
		doBranch(_BranchTarget_); \
	} //else IntcpuBranchTest();

#define RepZBranchLinki32(op) \
	_SetLink(31); \
	if(cpuRegs.GPR.r[_Rs_].SD[0] op 0) { \
		doBranch(_BranchTarget_); \
	} //else IntcpuBranchTest();

void BGEZ()   { RepZBranchi32(>=) 
	oppPacket.isTargetUsed = 0; oppPacket.name = "bgez";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}      // Branch if Rs >= 0
void BGEZAL() { RepZBranchLinki32(>=) 
	oppPacket.isTargetUsed = 0; oppPacket.name = "bgezal";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs >= 0 and link
void BGTZ()   { RepZBranchi32(>) 
	oppPacket.isTargetUsed = 0; oppPacket.name = "bgtz";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}       // Branch if Rs >  0
void BLEZ()   { RepZBranchi32(<=) 
	oppPacket.isTargetUsed = 0; oppPacket.name = "blez";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}      // Branch if Rs <= 0
void BLTZ()   { RepZBranchi32(<) 
	oppPacket.isTargetUsed = 0; oppPacket.name = "bltz";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}       // Branch if Rs <  0
void BLTZAL() { RepZBranchLinki32(<) 
	oppPacket.isTargetUsed = 0; oppPacket.name = "bltzal";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}   // Branch if Rs <  0 and link


/*********************************************************
* Register branch logic  Likely                          *
* Format:  OP rs, offset                                 *
*********************************************************/
#define RepZBranchi32Likely(op) \
	if(cpuRegs.GPR.r[_Rs_].SD[0] op 0) { \
		doBranch(_BranchTarget_); \
	} else { cpuRegs.pc +=4;} //IntcpuBranchTest(); 

#define RepZBranchLinki32Likely(op) \
	_SetLink(31); \
	if(cpuRegs.GPR.r[_Rs_].SD[0] op 0) { \
		doBranch(_BranchTarget_); \
	} else { cpuRegs.pc +=4; }//IntcpuBranchTest(); 


#define RepBranchi32Likely(op) \
	if(cpuRegs.GPR.r[_Rs_].SD[0] op cpuRegs.GPR.r[_Rt_].SD[0]) { \
		doBranch(_BranchTarget_); \
	} else { cpuRegs.pc +=4;} //IntcpuBranchTest(); 



void BEQL()    {  RepBranchi32Likely(==)      
	oppPacket.isTargetUsed = 0; oppPacket.name = "beql";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs == Rt
void BNEL()    {  RepBranchi32Likely(!=)      
	oppPacket.isTargetUsed = 0; oppPacket.name = "bnel";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs != Rt
void BLEZL()   {  RepZBranchi32Likely(<=)     
	oppPacket.isTargetUsed = 0; oppPacket.name = "blezl";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs <= 0
void BGTZL()   {  RepZBranchi32Likely(>)      
	oppPacket.isTargetUsed = 0; oppPacket.name = "bgtzl";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs >  0
void BLTZL()   {  RepZBranchi32Likely(<)      
	oppPacket.isTargetUsed = 0; oppPacket.name = "bltzl";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs <  0
void BGEZL()   {  RepZBranchi32Likely(>=)     
	oppPacket.isTargetUsed = 0; oppPacket.name = "bgezl";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs >= 0
void BLTZALL() {  RepZBranchLinki32Likely(<)  
	oppPacket.isTargetUsed = 0; oppPacket.name = "bltzall";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs <  0 and link
void BGEZALL() {  RepZBranchLinki32Likely(>=) 
	oppPacket.isTargetUsed = 0; oppPacket.name = "bgezall";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.target = _BranchTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}  // Branch if Rs >= 0 and link

/*********************************************************
* Load and store for GPR                                 *
* Format:  OP rt, offset(base)                           *
*********************************************************/

void LB() {
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	if (_Rt_) {
		//memRead8RS(addr, &cpuRegs.GPR.r[_Rt_].UD[0]);
	} else {
		u64 dummy;
		//memRead8RS(addr, &dummy);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "lb";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void LBU() { 
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	if (_Rt_) {
		//memRead8RU(addr, &cpuRegs.GPR.r[_Rt_].UD[0]);
	} else {
		u64 dummy;
		//memRead8RU(addr, &dummy);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "lbu";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	////printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void LH() { 
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	if (_Rt_) {
		//memRead16RS(addr, &cpuRegs.GPR.r[_Rt_].UD[0]);
	} else {
		u64 dummy;
		//memRead16RS(addr, &dummy);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "lh";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	////printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void LHU() { 
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	if (_Rt_) {
		//memRead16RU(addr, &cpuRegs.GPR.r[_Rt_].UD[0]);
	} else {
		u64 dummy;
		//memRead16RU(addr, &dummy);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "lhu";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	////printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void LW() {
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	if (_Rt_) {
		//memRead32RS(addr, &cpuRegs.GPR.r[_Rt_].UD[0]);
	} else {
		u64 dummy;
		//memRead32RS(addr, &dummy);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "lw";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	////printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void LWU() { 
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	if (_Rt_) {
		//memRead32RU(addr, &cpuRegs.GPR.r[_Rt_].UD[0]);
	} else {
		u64 dummy;
		//memRead32RU(addr, &dummy);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "lwu";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

u32 LWL_MASK[4] = { 0xffffff, 0xffff, 0xff, 0 };
u32 LWL_SHIFT[4] = { 24, 16, 8, 0 };

void LWL() {
	s32 addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	u32 shift = addr & 3;
	u32 mem;

	if (!_Rt_) return;
	//if (//memRead32(addr & ~3, &mem) == -1) return;
	cpuRegs.GPR.r[_Rt_].UD[0] =	(cpuRegs.GPR.r[_Rt_].UL[0] & LWL_MASK[shift]) | 
								(mem << LWL_SHIFT[shift]);

	/*
	Mem = 1234.  Reg = abcd

	0   4bcd   (mem << 24) | (reg & 0x00ffffff)
	1   34cd   (mem << 16) | (reg & 0x0000ffff)
	2   234d   (mem <<  8) | (reg & 0x000000ff)
	3   1234   (mem      ) | (reg & 0x00000000)
	*/
	oppPacket.isTargetUsed = 0; oppPacket.name = "lwl";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

u32 LWR_MASK[4] = { 0, 0xff000000, 0xffff0000, 0xffffff00 };
u32 LWR_SHIFT[4] = { 0, 8, 16, 24 };

void LWR() {
	s32 addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	u32 shift = addr & 3;
	u32 mem;

	if (!_Rt_) return;
	//if (//memRead32(addr & ~3, &mem) == -1) return;
	cpuRegs.GPR.r[_Rt_].UD[0] =	(cpuRegs.GPR.r[_Rt_].UL[0] & LWR_MASK[shift]) | 
								(mem >> LWR_SHIFT[shift]);

	/*
	Mem = 1234.  Reg = abcd

	0   1234   (mem      ) | (reg & 0x00000000)
	1   a123   (mem >>  8) | (reg & 0xff000000)
	2   ab12   (mem >> 16) | (reg & 0xffff0000)
	3   abc1   (mem >> 24) | (reg & 0xffffff00)
	*/
	oppPacket.isTargetUsed = 0; oppPacket.name = "lwr";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void LD() {
    s32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	if (_Rt_) {
		////memRead64(addr, &cpuRegs.GPR.r[_Rt_].UD[0]);
	} else {
		u64 dummy;
		////memRead64(addr, &dummy);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "ld";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

u64 LDL_MASK[8] = { 0x00ffffffffffffffLL, 0x0000ffffffffffffLL, 0x000000ffffffffffLL, 0x00000000ffffffffLL, 
					0x0000000000ffffffLL, 0x000000000000ffffLL, 0x00000000000000ffLL, 0x0000000000000000LL };
u32 LDL_SHIFT[8] = { 56, 48, 40, 32, 24, 16, 8, 0 };

void LDL() {
	u32 addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	u32 shift = addr & 7;
	u64 mem;

	if (!_Rt_) return;
	//if (////memRead64(addr & ~7, &mem) == -1) return;
	cpuRegs.GPR.r[_Rt_].UD[0] =	(cpuRegs.GPR.r[_Rt_].UD[0] & LDL_MASK[shift]) | 
								(mem << LDL_SHIFT[shift]);
	oppPacket.isTargetUsed = 0; oppPacket.name = "ld";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

u64 LDR_MASK[8] = { 0x0000000000000000LL, 0xff00000000000000LL, 0xffff000000000000LL, 0xffffff0000000000LL,
					0xffffffff00000000LL, 0xffffffffff000000LL, 0xffffffffffff0000LL, 0xffffffffffffff00LL };
u32 LDR_SHIFT[8] = { 0, 8, 16, 24, 32, 40, 48, 56 };

void LDR() {  
	u32 addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	u32 shift = addr & 7;
	u64 mem;

	if (!_Rt_) return;
	//if (////memRead64(addr & ~7, &mem) == -1) return;
	cpuRegs.GPR.r[_Rt_].UD[0] =	(cpuRegs.GPR.r[_Rt_].UD[0] & LDR_MASK[shift]) | 
								(mem >> LDR_SHIFT[shift]);
	oppPacket.isTargetUsed = 0; oppPacket.name = "ldr";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void LQ() {
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	addr&=~0xf;

	if (_Rt_) {
		////memRead128(addr, &cpuRegs.GPR.r[_Rt_].UD[0]);
	} else {
		u64 val[2];
		////memRead128(addr, val);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "lq";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void SB() { 
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
    //memWrite8(addr, cpuRegs.GPR.r[_Rt_].UC[0]); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "sb";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void SH() { 
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	//memWrite16(addr, cpuRegs.GPR.r[_Rt_].US[0]); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "sh";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void SW(){  
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
    //memWrite32(addr, cpuRegs.GPR.r[_Rt_].UL[0]); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "sw";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

u32 SWL_MASK[4] = { 0xffffff00, 0xffff0000, 0xff000000, 0x00000000 };
u32 SWL_SHIFT[4] = { 24, 16, 8, 0 };

void SWL() {
	u32 addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	u32 shift = addr & 3;
	u32 mem;

	if (//memRead32(addr & ~3, &mem) == -1) return;

	//memWrite32(addr & ~3,  (cpuRegs.GPR.r[_Rt_].UL[0] >> SWL_SHIFT[shift]) |
		      (  mem & SWL_MASK[shift]) );
	/*
	Mem = 1234.  Reg = abcd

	0   123a   (reg >> 24) | (mem & 0xffffff00)
	1   12ab   (reg >> 16) | (mem & 0xffff0000)
	2   1abc   (reg >>  8) | (mem & 0xff000000)
	3   abcd   (reg      ) | (mem & 0x00000000)
	*/
	oppPacket.isTargetUsed = 0; oppPacket.name = "swl";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

u32 SWR_MASK[4] = { 0x00000000, 0x000000ff, 0x0000ffff, 0x00ffffff };
u32 SWR_SHIFT[4] = { 0, 8, 16, 24 };

void SWR() {
	u32 addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	u32 shift = addr & 3;
	u32 mem;

	if (//memRead32(addr & ~3, &mem) == -1) return;

	//memWrite32(addr & ~3,  (cpuRegs.GPR.r[_Rt_].UL[0] << SWR_SHIFT[shift]) |
		      ( mem & SWR_MASK[shift]) );

	/*
	Mem = 1234.  Reg = abcd

	0   abcd   (reg      ) | (mem & 0x00000000)
	1   bcd4   (reg <<  8) | (mem & 0x000000ff)
	2   cd34   (reg << 16) | (mem & 0x0000ffff)
	3   d234   (reg << 24) | (mem & 0x00ffffff)
	*/
	oppPacket.isTargetUsed = 0; oppPacket.name = "swr";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void SD() {
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
    ////memWrite64(addr,cpuRegs.GPR.r[_Rt_].UD[0]); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "sd";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

u64 SDL_MASK[8] = { 0xffffffffffffff00LL, 0xffffffffffff0000LL, 0xffffffffff000000LL, 0xffffffff00000000LL, 
					0xffffff0000000000LL, 0xffff000000000000LL, 0xff00000000000000LL, 0x0000000000000000LL };
u32 SDL_SHIFT[8] = { 56, 48, 40, 32, 24, 16, 8, 0 };

void SDL() {
	u32 addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	u32 shift = addr & 7;
	u64 mem;

	if (////memRead64(addr & ~7, &mem) == -1) return;

	////memWrite64(addr & ~7,  (cpuRegs.GPR.r[_Rt_].UD[0] >> SDL_SHIFT[shift]) |
		      ( mem & SDL_MASK[shift]) );
	oppPacket.isTargetUsed = 0; oppPacket.name = "sdl";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

u64 SDR_MASK[8] = { 0x0000000000000000LL, 0x00000000000000ffLL, 0x000000000000ffffLL, 0x0000000000ffffffLL,
					0x00000000ffffffffLL, 0x000000ffffffffffLL, 0x0000ffffffffffffLL, 0x00ffffffffffffffLL };
u32 SDR_SHIFT[8] = { 0, 8, 16, 24, 32, 40, 48, 56 };

void SDR() {
	u32 addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	u32 shift = addr & 7;
	u64 mem;

	if (////memRead64(addr & ~7, &mem) == -1) return;

	////memWrite64(addr & ~7,  (cpuRegs.GPR.r[_Rt_].UD[0] << SDR_SHIFT[shift]) |
		      ( mem & SDR_MASK[shift]) );
	oppPacket.isTargetUsed = 0; oppPacket.name = "sdr";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

void SQ() {
	u32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + _Imm_;
	addr&=~0xf;
	////memWrite128(addr, &cpuRegs.GPR.r[_Rt_].UD[0]);
	oppPacket.isTargetUsed = 0; oppPacket.name = "sq";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.loadOffset = _Imm_;
	oppPacket.u32Result = addr;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[1]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result,oppPacket.reg[1],oppPacket.u32Result);
}

/*********************************************************
* Conditional Move                                       *
* Format:  OP rd, rs, rt                                 *
*********************************************************/

void MOVZ() {
	if (!_Rd_) return;
	if (cpuRegs.GPR.r[_Rt_].UD[0] == 0) {
		cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0];
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "movz";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);

}
void MOVN() {
	if (!_Rd_) return;
	if (cpuRegs.GPR.r[_Rt_].UD[0] != 0) {
		cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0];
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "movz";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.reg[2] = getGPReg(_Rt_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0],oppPacket.reg[1], oppPacket.reg[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x(%s)\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],oppPacket.u32Result);
}

/*********************************************************
* Special purpose instructions                           *
* Format:  OP                                            *
*********************************************************/

//#include "Sifcmd.h"
/*
int __Deci2Call(int call, u32 *addr);
*/

/*
 *	int Deci2Call(int, u_int *);
 */

int __Deci2Call(int call, u32 *addr) {

	return 0;
}


void SYSCALL() {

	oppPacket.name = "syscall";
	//u8 call;
	//call = _lb(cpuRegs.pc-4);
	//sprintf(oppPacket.name, "syscall %.2x", call);
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));
	//printf("%s\n",oppPacket.name);
}

void BREAK(void) {
	cpuRegs.pc -= 4;
	//cpuException(0x24, cpuRegs.branch);
	sprintf(oppPacket.name, "break (%.4x)", (cpuRegs.code << 6)>>12);
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));
	//printf("%s\n",oppPacket.name);
}

void MFSA( void ) {
	if (!_Rd_) return;
	cpuRegs.GPR.r[_Rd_].SD[0] = (s64)cpuRegs.sa;
	oppPacket.isTargetUsed = 0; oppPacket.name = "mfsa";
	oppPacket.reg[0] = getGPReg(_Rd_);

	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result);
}

void MTSA( void ) {
	cpuRegs.sa = (s32)cpuRegs.GPR.r[_Rs_].SD[0];
	oppPacket.isTargetUsed = 0; oppPacket.name = "mtsa";
	oppPacket.reg[0] = getGPReg(_Rd_);
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s\t\n",oppPacket.name,oppPacket.reg[0]);
}

void SYNC( void )
{
	oppPacket.isTargetUsed = 0; oppPacket.name = "sync";
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString,0, sizeof(oppPacket.regString) );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  \t\n",oppPacket.name);
}

void PREF( void ) 
{
	oppPacket.isTargetUsed = 0; oppPacket.name = "pref";
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString,0, sizeof(oppPacket.regString) );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  \n",oppPacket.name);
}



/*********************************************************
* Register trap                                          *
* Format:  OP rs, rt                                     *
*********************************************************/

void TGE() {
    if (cpuRegs.GPR.r[_Rs_].SD[0]>= cpuRegs.GPR.r[_Rt_].SD[0]) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tge";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

void TGEU() {
	if (cpuRegs.GPR.r[_Rs_].UD[0]>= cpuRegs.GPR.r[_Rt_].UD[0]) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tgeu";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

void TLT() {
	if (cpuRegs.GPR.r[_Rs_].SD[0] < cpuRegs.GPR.r[_Rt_].SD[0]) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tlt";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

void TLTU() {
	if (cpuRegs.GPR.r[_Rs_].UD[0] < cpuRegs.GPR.r[_Rt_].UD[0]) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tltu";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

void TEQ() {
	if (cpuRegs.GPR.r[_Rs_].SD[0] == cpuRegs.GPR.r[_Rt_].SD[0]) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "teq";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

void TNE() {
	if (cpuRegs.GPR.r[_Rs_].SD[0] != cpuRegs.GPR.r[_Rt_].SD[0]) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tne";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);
	
	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0],oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

/*********************************************************
* Trap with immediate operand                            *
* Format:  OP rs, Imm                                    *
*********************************************************/

void TGEI() {

	if (cpuRegs.GPR.r[_Rs_].SD[0] >= _Imm_) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tgei";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	sprintf(oppPacket.immidiateString," (%.4x)",oppPacket.imm);
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}

void TGEIU() {
	if (cpuRegs.GPR.r[_Rs_].UD[0] >= _ImmU_) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tgeiu";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	sprintf(oppPacket.immidiateString," (%.4x)",oppPacket.imm);
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}

void TLTI() {
	if(cpuRegs.GPR.r[_Rs_].SD[0] < _Imm_) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tlti";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	sprintf(oppPacket.immidiateString," (%.4x)",oppPacket.imm);
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}

void TLTIU() {
	if (cpuRegs.GPR.r[_Rs_].UD[0] < _ImmU_) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tltiu";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	sprintf(oppPacket.immidiateString," (%.4x)",oppPacket.imm);
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}

void TEQI() {
	if (cpuRegs.GPR.r[_Rs_].SD[0] == _Imm_) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "teqi";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	sprintf(oppPacket.immidiateString," (%.4x)",oppPacket.imm);
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}

void TNEI() {
	if (cpuRegs.GPR.r[_Rs_].SD[0] != _Imm_) {
		//cpuException(EXC_CODE_Tr, cpuRegs.branch);
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "tnei";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	sprintf(oppPacket.immidiateString," (%.4x)",oppPacket.imm);
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}

/*********************************************************
* Sa intructions                                         *
* Format:  OP rs, Imm                                    *
*********************************************************/

void MTSAB() {
 	cpuRegs.sa = ((cpuRegs.GPR.r[_Rs_].UL[0] & 0xF) ^ (_Imm_ & 0xF)) << 3;
	oppPacket.isTargetUsed = 0; oppPacket.name = "mtsab";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	sprintf(oppPacket.immidiateString," %0x",oppPacket.imm);
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}

void MTSAH() {
    cpuRegs.sa = ((cpuRegs.GPR.r[_Rs_].UL[0] & 0x7) ^ (_Imm_ & 0x7)) << 4;
	oppPacket.isTargetUsed = 0; oppPacket.name = "mtsah";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	sprintf(oppPacket.immidiateString," %0x",oppPacket.imm);
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0]);
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}
//#pragma endregion
void COP2(){}


//#pragma region opTables
void (*Int_OpcodePrintTable[64])() = 
{
    SPECIAL,       REGIMM,       J,             JAL,           BEQ,          BNE,           BLEZ,  BGTZ,
    ADDI,          ADDIU,        SLTI,          SLTIU,         ANDI,         ORI,           XORI,  LUI,
    COP0,          COP1,         COP2,          UnknownOpcode, BEQL,         BNEL,          BLEZL, BGTZL,
    DADDI,         DADDIU,       LDL,           LDR,           MMI,          UnknownOpcode, LQ,    SQ,
    LB,            LH,           LWL,           LW,            LBU,          LHU,           LWR,   LWU,
    SB,            SH,           SWL,           SW,            SDL,          SDR,           SWR,   CACHE,
    UnknownOpcode, LWC1,         UnknownOpcode, PREF,          UnknownOpcode,UnknownOpcode, LQC2,  LD,
    UnknownOpcode, SWC1,         UnknownOpcode, UnknownOpcode, UnknownOpcode,UnknownOpcode, SQC2,  SD
};


void (*Int_SpecialPrintTable[64])() = 
{
    SLL,           UnknownOpcode, SRL,           SRA,           SLLV,    UnknownOpcode, SRLV,          SRAV,
    JR,            JALR,          MOVZ,          MOVN,          SYSCALL, BREAK,         UnknownOpcode, SYNC,
    MFHI,          MTHI,          MFLO,          MTLO,          DSLLV,   UnknownOpcode, DSRLV,         DSRAV,
    MULT,          MULTU,         DIV,           DIVU,          UnknownOpcode,UnknownOpcode,UnknownOpcode,UnknownOpcode,
    ADD,           ADDU,          SUB,           SUBU,          AND,     OR,            XOR,           NOR,
    MFSA ,         MTSA ,         SLT,           SLTU,          DADD,    DADDU,         DSUB,          DSUBU,
    TGE,           TGEU,          TLT,           TLTU,          TEQ,     UnknownOpcode, TNE,           UnknownOpcode,
    DSLL,          UnknownOpcode, DSRL,          DSRA,          DSLL32,  UnknownOpcode, DSRL32,        DSRA32
};
void SPECIAL() {Int_SpecialPrintTable[_Funct_]();}
void (*Int_REGIMMPrintTable[32])() = {
    BLTZ,   BGEZ,   BLTZL,            BGEZL,         UnknownOpcode, UnknownOpcode, UnknownOpcode, UnknownOpcode,
    TGEI,   TGEIU,  TLTI,             TLTIU,         TEQI,          UnknownOpcode, TNEI,          UnknownOpcode,
    BLTZAL, BGEZAL, BLTZALL,          BGEZALL,       UnknownOpcode, UnknownOpcode, UnknownOpcode, UnknownOpcode,
    MTSAB,  MTSAH , UnknownOpcode,    UnknownOpcode, UnknownOpcode, UnknownOpcode, UnknownOpcode, UnknownOpcode,
};
void REGIMM()  {Int_REGIMMPrintTable[_Rt_]();    }
void (*Int_MMIPrintTable[64])() = 
{
    MADD,                    MADDU,                  MMI_Unknown,          MMI_Unknown,          PLZCW,            MMI_Unknown,       MMI_Unknown,          MMI_Unknown,
    MMI0,                    MMI2,                   MMI_Unknown,          MMI_Unknown,          MMI_Unknown,      MMI_Unknown,       MMI_Unknown,          MMI_Unknown,
    MFHI1,                   MTHI1,                  MFLO1,                MTLO1,                MMI_Unknown,      MMI_Unknown,       MMI_Unknown,          MMI_Unknown,
    MULT1,                   MULTU1,                 DIV1,                 DIVU1,                MMI_Unknown,      MMI_Unknown,       MMI_Unknown,          MMI_Unknown,
    MADD1,                   MADDU1,                 MMI_Unknown,          MMI_Unknown,          MMI_Unknown,      MMI_Unknown,       MMI_Unknown,          MMI_Unknown,
    MMI1 ,                   MMI3,                   MMI_Unknown,          MMI_Unknown,          MMI_Unknown,      MMI_Unknown,       MMI_Unknown,          MMI_Unknown,
    PMFHL,                   PMTHL,                  MMI_Unknown,          MMI_Unknown,          PSLLH,            MMI_Unknown,       PSRLH,                PSRAH,
    MMI_Unknown,             MMI_Unknown,            MMI_Unknown,          MMI_Unknown,          PSLLW,            MMI_Unknown,       PSRLW,                PSRAW,
};

void (*Int_MMI0PrintTable[32])() = 
{ 
	 PADDW,         PSUBW,         PCGTW,          PMAXW,       
	 PADDH,         PSUBH,         PCGTH,          PMAXH,        
	 PADDB,         PSUBB,         PCGTB,          MMI_Unknown,
	 MMI_Unknown,   MMI_Unknown,   MMI_Unknown,    MMI_Unknown,
	 PADDSW,        PSUBSW,        PEXTLW,         PPACW,        
	 PADDSH,        PSUBSH,        PEXTLH,         PPACH,        
	 PADDSB,        PSUBSB,        PEXTLB,         PPACB,        
	 MMI_Unknown,   MMI_Unknown,   PEXT5,          PPAC5,        
};

void (*Int_MMI1PrintTable[32])() =
{ 
	 MMI_Unknown,   PABSW,         PCEQW,         PMINW, 
	 PADSBH,        PABSH,         PCEQH,         PMINH, 
	 MMI_Unknown,   MMI_Unknown,   PCEQB,         MMI_Unknown, 
	 MMI_Unknown,   MMI_Unknown,   MMI_Unknown,   MMI_Unknown, 
	 PADDUW,        PSUBUW,        PEXTUW,        MMI_Unknown,  
	 PADDUH,        PSUBUH,        PEXTUH,        MMI_Unknown, 
	 PADDUB,        PSUBUB,        PEXTUB,        QFSRV, 
	 MMI_Unknown,   MMI_Unknown,   MMI_Unknown,   MMI_Unknown, 
};


void (*Int_MMI2PrintTable[32])() = 
{ 
	 PMADDW,        MMI_Unknown,   PSLLVW,        PSRLVW, 
	 PMSUBW,        MMI_Unknown,   MMI_Unknown,   MMI_Unknown,
	 PMFHI,         PMFLO,         PINTH,         MMI_Unknown,
	 PMULTW,        PDIVW,         PCPYLD,        MMI_Unknown,
	 PMADDH,        PHMADH,        PAND,          PXOR, 
	 PMSUBH,        PHMSBH,        MMI_Unknown,   MMI_Unknown, 
	 MMI_Unknown,   MMI_Unknown,   PEXEH,         PREVH, 
	 PMULTH,        PDIVBW,        PEXEW,         PROT3W, 
};

void (*Int_MMI3PrintTable[32])() = 
{ 
	 PMADDUW,       MMI_Unknown,   MMI_Unknown,   PSRAVW, 
	 MMI_Unknown,   MMI_Unknown,   MMI_Unknown,   MMI_Unknown,
	 PMTHI,         PMTLO,         PINTEH,        MMI_Unknown,
	 PMULTUW,       PDIVUW,        PCPYUD,        MMI_Unknown,
	 MMI_Unknown,   MMI_Unknown,   POR,           PNOR,  
	 MMI_Unknown,   MMI_Unknown,   MMI_Unknown,   MMI_Unknown,
	 MMI_Unknown,   MMI_Unknown,   PEXCH,         PCPYH, 
	 MMI_Unknown,   MMI_Unknown,   PEXCW,         MMI_Unknown,
};

void (*Int_COP0PrintTable[32])() = 
{
    MFC0,         COP0_Unknown, COP0_Unknown, COP0_Unknown, MTC0,         COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_BC0,     COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_Func,    COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
};

void (*Int_COP0BC0PrintTable[32])() = 
{
    BC0F,         BC0T,         BC0FL,        BC0TL,        COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
};
void TLBWI(){}
void TLBWR(){}
void TLBP(){}
void ERET(){}
void _EI(){}
void _DI(){}

void (*Int_COP0C0PrintTable[64])() = {
    COP0_Unknown, TLBR,         TLBWI,        COP0_Unknown, COP0_Unknown, COP0_Unknown, TLBWR,        COP0_Unknown,
    TLBP,         COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    ERET,         COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown,
    EI,           DI,           COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown, COP0_Unknown
};

void (*Int_COP1PrintTable[32])() = {
    MFC1,         COP1_Unknown, CFC1,         COP1_Unknown, MTC1,         COP1_Unknown, CTC1,         COP1_Unknown,
    COP1_BC1,     COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown,
    COP1_S,       COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_W,       COP1_Unknown, COP1_Unknown, COP1_Unknown,
    COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown,
};

void (*Int_COP1BC1PrintTable[32])() = {
    BC1F,         BC1T,         BC1FL,        BC1TL,        COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown,
    COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown,
    COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown,
    COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown, COP1_Unknown,
};

void (*Int_COP1SPrintTable[64])() = {
	ADD_S,       SUB_S,       MUL_S,       DIV_S,       SQRT_S,      ABS_S,       MOV_S,       NEG_S, 
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,   
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,RSQRT_S,     COP1_Unknown,  
	ADDA_S,      SUBA_S,      MULA_S,      COP1_Unknown,MADD_S,      MSUB_S,      MADDA_S,     MSUBA_S,
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,CVT_W,       COP1_Unknown,COP1_Unknown,COP1_Unknown, 
	MAX_S,       MIN_S,       COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown, 
	C_F,         COP1_Unknown,C_EQ,        COP1_Unknown,C_LT,        COP1_Unknown,C_LE,        COP1_Unknown, 
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown, 
};
 
void (*Int_COP1WPrintTable[64])() = { 
/*	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,   	
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,   
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,   
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,   
	CVT_S,       COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,   
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,   
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,   
	COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,COP1_Unknown,  */ 
};


void (*Int_COP2PrintTable[32])() = {
    /*COP2_Unknown, QMFC2,        CFC2,         COP2_Unknown, COP2_Unknown, QMTC2,        CTC2,         COP2_Unknown,
    COP2_BC2,     COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown,
    COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL,
	COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL, COP2_SPECIAL,*/
};

void (*Int_COP2BC2PrintTable[32])() = {
    /*BC2F,         BC2T,         BC2FL,        BC2TL,        COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown,
    COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown,
    COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown,
    COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown, COP2_Unknown,*/
}; 

void (*Int_COP2SPECIAL1PrintTable[64])() = 
{ 
 /*VADDx,       VADDy,       VADDz,       VADDw,       VSUBx,        VSUBy,        VSUBz,        VSUBw,  
 VMADDx,      VMADDy,      VMADDz,      VMADDw,      VMSUBx,       VMSUBy,       VMSUBz,       VMSUBw, 
 VMAXx,       VMAXy,       VMAXz,       VMAXw,       VMINIx,       VMINIy,       VMINIz,       VMINIw, 
 VMULx,       VMULy,       VMULz,       VMULw,       VMULq,        VMAXi,        VMULi,        VMINIi,
 VADDq,       VMADDq,      VADDi,       VMADDi,      VSUBq,        VMSUBq,       VSUBi,        VMSUBi, 
 VADD,        VMADD,       VMUL,        VMAX,        VSUB,         VMSUB,        VOPMSUB,      VMINI,  
 VIADD,       VISUB,       VIADDI,      COP2_Unknown,VIAND,        VIOR,         COP2_Unknown, COP2_Unknown,
 VCALLMS,     VCALLMSR,    COP2_Unknown,COP2_Unknown,COP2_SPECIAL2,COP2_SPECIAL2,COP2_SPECIAL2,COP2_SPECIAL2,  */
};

void (*Int_COP2SPECIAL2PrintTable[128])() = 
{
 /*VADDAx      ,VADDAy      ,VADDAz      ,VADDAw      ,VSUBAx      ,VSUBAy      ,VSUBAz      ,VSUBAw,
 VMADDAx     ,VMADDAy     ,VMADDAz     ,VMADDAw     ,VMSUBAx     ,VMSUBAy     ,VMSUBAz     ,VMSUBAw,
 VITOF0      ,VITOF4      ,VITOF12     ,VITOF15     ,VFTOI0      ,VFTOI4      ,VFTOI12     ,VFTOI15,
 VMULAx      ,VMULAy      ,VMULAz      ,VMULAw      ,VMULAq      ,VABS        ,VMULAi      ,VCLIPw,
 VADDAq      ,VMADDAq     ,VADDAi      ,VMADDAi     ,VSUBAq      ,VMSUBAq     ,VSUBAi      ,VMSUBAi,
 VADDA       ,VMADDA      ,VMULA       ,COP2_Unknown,VSUBA       ,VMSUBA      ,VOPMULA     ,VNOP,   
 VMOVE       ,VMR32       ,COP2_Unknown,COP2_Unknown,VLQI        ,VSQI        ,VLQD        ,VSQD,   
 VDIV        ,VSQRT       ,VRSQRT      ,VWAITQ      ,VMTIR       ,VMFIR       ,VILWR       ,VISWR,  
 VRNEXT      ,VRGET       ,VRINIT      ,VRXOR       ,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown, 
 COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,
 COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,
 COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,
 COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,
 COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,
 COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,
 COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,COP2_Unknown,*/
};
//#pragma endregion

void MMI() {
	Int_MMIPrintTable[_Funct_]();
}

void MMI0() {
	Int_MMI0PrintTable[_Sa_]();
}

void MMI1() {
	Int_MMI1PrintTable[_Sa_]();
}

void MMI2() {
	Int_MMI2PrintTable[_Sa_]();
}

void MMI3() {
	Int_MMI3PrintTable[_Sa_]();
}

void MMI_Unknown() {
	printf ("Unknown MMI opcode called\n");
}
//#pragma region MMI OPCODES
//*****************MMI OPCODES*********************************

void MADD() {
	s64 temp = (s64)((u64)cpuRegs.LO.UL[0] | ((u64)cpuRegs.HI.UL[0] << 32)) +
			  ((s64)cpuRegs.GPR.r[_Rs_].SL[0] * (s64)cpuRegs.GPR.r[_Rt_].SL[0]);

	cpuRegs.LO.SD[0] = (s32)(temp & 0xffffffff);
	cpuRegs.HI.SD[0] = (s32)(temp >> 32);

	if (_Rd_){ cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.LO.SD[0];
		oppPacket.isTargetUsed = 0; oppPacket.name = "madd";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.reg[2] = getGPReg(_Rd_);

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1],  oppPacket.reg[2] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],0x0);
	} else {
		oppPacket.isTargetUsed = 0; oppPacket.name = "madd";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//printf("%s  %s, %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],0x0);
	}
}

void MADDU() {
	u64 tempu =	(u64)((u64)cpuRegs.LO.UL[0] | ((u64)cpuRegs.HI.UL[0] << 32)) +
			   ((u64)cpuRegs.GPR.r[_Rs_].UL[0] * (u64)cpuRegs.GPR.r[_Rt_].UL[0]);

	cpuRegs.LO.SD[0] = (s32)(tempu & 0xffffffff);
	cpuRegs.HI.SD[0] = (s32)(tempu >> 32);

	if (_Rd_) {cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.LO.SD[0];
		oppPacket.isTargetUsed = 0; oppPacket.name = "maddu";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.reg[2] = getGPReg(_Rd_);

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1], oppPacket.reg[2] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],0x0);
	} else {
		oppPacket.isTargetUsed = 0; oppPacket.name = "maddu";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//printf("%s  %s, %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],0x0);
	}
}

#define _PLZCW(n) { \
	sign = cpuRegs.GPR.r[_Rs_].UL[n] >> 31; \
    for (i=30; i>=0; i--) { \
        if (((cpuRegs.GPR.r[_Rs_].UL[n] >> i) & 0x1) != sign) { \
            break; \
        } \
    } \
    cpuRegs.GPR.r[_Rd_].UL[n] = 30 - i; \
}

void PLZCW() {
    int i;
	u32 sign;

    if (!_Rd_) return;

	_PLZCW (0);
	_PLZCW (1);
	oppPacket.isTargetUsed = 0; oppPacket.name = "plzcw";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.u32Result);
}

void MADD1() {
	s64 temp = (s64)((u64)cpuRegs.LO.UL[2] | ((u64)cpuRegs.HI.UL[2] << 32)) +
			  ((s64)cpuRegs.GPR.r[_Rs_].SL[0] * (s64)cpuRegs.GPR.r[_Rt_].SL[0]);

	cpuRegs.LO.SD[1] = (s32)(temp & 0xffffffff);
	cpuRegs.HI.SD[1] = (s32)(temp >> 32);

	if (_Rd_) {cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.LO.SD[1];
		oppPacket.isTargetUsed = 0; oppPacket.name = "madd1";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.reg[2] = getGPReg(_Rd_);

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1],  oppPacket.reg[2] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[2],oppPacket.reg[1],oppPacket.reg[0],0x0);
	} else {
		oppPacket.isTargetUsed = 0; oppPacket.name = "madd1";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//printf("%s  %s, %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],0x0);
	}
}

void MADDU1() {
	u64 tempu = (u64)((u64)cpuRegs.LO.UL[2] | ((u64)cpuRegs.HI.UL[2] << 32)) +
			   ((u64)cpuRegs.GPR.r[_Rs_].UL[0] * (u64)cpuRegs.GPR.r[_Rt_].UL[0]);

	cpuRegs.LO.SD[1] = (s32)(tempu & 0xffffffff);
	cpuRegs.HI.SD[1] = (s32)(tempu >> 32);

	if (_Rd_) {cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.LO.SD[1];
		oppPacket.isTargetUsed = 0; oppPacket.name = "maddu1";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.reg[2] = getGPReg(_Rd_);

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1],  oppPacket.reg[2] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[2],oppPacket.reg[1],oppPacket.reg[0],0x0);
	} else {
		oppPacket.isTargetUsed = 0; oppPacket.name = "maddu1";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//printf("%s  %s, %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],0x0);
	}
}

void MFHI1() {
	if (!_Rd_) return;
	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.HI.UD[1];
	oppPacket.isTargetUsed = 0; oppPacket.name = "mfhi1";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result);
}

void MFLO1() {
	if (!_Rd_) return;
	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[1];
	oppPacket.isTargetUsed = 0; oppPacket.name = "mflo1";
	oppPacket.reg[0] = getGPReg(_Rd_);
	oppPacket.u32Result = cpuRegs.GPR.r[_Rd_].UD[0];

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.u32Result);
}

void MTHI1() {
	cpuRegs.HI.UD[1] = cpuRegs.GPR.r[_Rs_].UD[0];
	oppPacket.isTargetUsed = 0; oppPacket.name = "mthi1";
	oppPacket.reg[0] = getGPReg(_Rd_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s\t\n",oppPacket.name,oppPacket.reg[0]);
}

void MTLO1() {
	cpuRegs.LO.UD[1] = cpuRegs.GPR.r[_Rs_].UD[0];
	oppPacket.isTargetUsed = 0; oppPacket.name = "mtlo1";
	oppPacket.reg[0] = getGPReg(_Rd_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.reg[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s\t\n",oppPacket.name,oppPacket.reg[0]);
}

void MULT1() {
	s64 temp = (s64)cpuRegs.GPR.r[_Rs_].SL[0] * (s64)cpuRegs.GPR.r[_Rt_].SL[0];

	cpuRegs.LO.UD[1] = (s64)(s32)(temp & 0xffffffff);
	cpuRegs.HI.UD[1] = (s64)(s32)(temp >> 32);

	/* Modified a bit . asadr */
	if (_Rd_){ cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[1];
		oppPacket.isTargetUsed = 0; oppPacket.name = "mult1";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.reg[2] = getGPReg(_Rd_);

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1],  oppPacket.reg[2] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],0x0);
	} else {
		oppPacket.isTargetUsed = 0; oppPacket.name = "mult1";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//printf("%s  %s, %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],0x0);
	}
}

void MULTU1() {
	u64 tempu = (u64)cpuRegs.GPR.r[_Rs_].UL[0] * (u64)cpuRegs.GPR.r[_Rt_].UL[0];

	cpuRegs.LO.UD[1] = (s32)(tempu & 0xffffffff);
	cpuRegs.HI.UD[1] = (s32)(tempu >> 32);

	if (_Rd_){ cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[1];
		oppPacket.isTargetUsed = 0; oppPacket.name = "multu1";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.reg[2] = getGPReg(_Rd_);

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.reg[0], oppPacket.reg[1],  oppPacket.reg[2] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
		//printf("%s  %s, %s, %s \t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],oppPacket.reg[2],0x0);
	} else {
		oppPacket.isTargetUsed = 0; oppPacket.name = "multu1";
		oppPacket.reg[0] = getGPReg(_Rs_);
		oppPacket.reg[1] = getGPReg(_Rt_);
		oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];

		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

		//printf("%s  %s, %s\t %0x\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1],0x0);
	}
}

void DIV1() {
	if (cpuRegs.GPR.r[_Rt_].SL[0] != 0) {
		cpuRegs.LO.SD[1] = cpuRegs.GPR.r[_Rs_].SL[0] / cpuRegs.GPR.r[_Rt_].SL[0];
		cpuRegs.HI.SD[1] = cpuRegs.GPR.r[_Rs_].SL[0] % cpuRegs.GPR.r[_Rt_].SL[0];
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "div1";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);

}

void DIVU1() {
	if (cpuRegs.GPR.r[_Rt_].UL[0] != 0) {
		cpuRegs.LO.UD[1] = cpuRegs.GPR.r[_Rs_].UL[0] / cpuRegs.GPR.r[_Rt_].UL[0];
		cpuRegs.HI.UD[1] = cpuRegs.GPR.r[_Rs_].UL[0] % cpuRegs.GPR.r[_Rt_].UL[0];
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "divu1";
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.reg[1] = getGPReg(_Rt_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.reg[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//oppPacket.u32Result = cpuRegs.GPR.r[_Rt_].UD[0];
	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.reg[1]);
}

#define PMFHL_CLAMP(dst, src) \
    if ((int)src > (int)0x00007fff) dst = 0x7fff; \
    else \
    if ((int)src < (int)0xffff8000) dst = 0x8000; \
    else dst = (u16)src;

void PMFHL() {
	if (!_Rd_) return;

	switch (_Sa_) {
		case 0x00: // LW
			cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
			cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
			cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
			cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
			break;

		case 0x01: // UW
			cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[1];
			cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[1];
			cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[3];
			cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[3];
			break;

		case 0x02: // SLW
			{
				u64 TempU64 = ((u64)cpuRegs.HI.UL[0] << 32) | (u64)cpuRegs.LO.UL[0];
				if (TempU64 >= 0x000000007fffffffLL) {
					cpuRegs.GPR.r[_Rd_].UD[0] = 0x000000007fffffffLL;
				} else if (TempU64 <= 0xffffffff80000000LL) {
					cpuRegs.GPR.r[_Rd_].UD[0] = 0xffffffff80000000LL;
				} else {
					cpuRegs.GPR.r[_Rd_].UD[0] = (s64)cpuRegs.LO.SL[0];
				}

				TempU64 = ((u64)cpuRegs.HI.UL[2] << 32) | (u64)cpuRegs.LO.UL[2];
				if (TempU64 >= 0x000000007fffffffLL) {
					cpuRegs.GPR.r[_Rd_].UD[1] = 0x000000007fffffffLL;
				} else if (TempU64 <= 0xffffffff80000000LL) {
					cpuRegs.GPR.r[_Rd_].UD[1] = 0xffffffff80000000LL;
				} else {
					cpuRegs.GPR.r[_Rd_].UD[1] = (s64)cpuRegs.LO.SL[2];
				}
			}
			break;

		case 0x03: // LH
			cpuRegs.GPR.r[_Rd_].US[0] = cpuRegs.LO.US[0];
			cpuRegs.GPR.r[_Rd_].US[1] = cpuRegs.LO.US[2];
			cpuRegs.GPR.r[_Rd_].US[2] = cpuRegs.HI.US[0];
			cpuRegs.GPR.r[_Rd_].US[3] = cpuRegs.HI.US[2];
			cpuRegs.GPR.r[_Rd_].US[4] = cpuRegs.LO.US[4];
			cpuRegs.GPR.r[_Rd_].US[5] = cpuRegs.LO.US[6];
			cpuRegs.GPR.r[_Rd_].US[6] = cpuRegs.HI.US[4];
			cpuRegs.GPR.r[_Rd_].US[7] = cpuRegs.HI.US[6];
			break;

		case 0x04: // SH
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[0], cpuRegs.LO.UL[0]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[1], cpuRegs.LO.UL[1]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[2], cpuRegs.HI.UL[0]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[3], cpuRegs.HI.UL[1]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[4], cpuRegs.LO.UL[2]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[5], cpuRegs.LO.UL[3]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[6], cpuRegs.HI.UL[2]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[7], cpuRegs.HI.UL[3]);
			break;
	}

}

void PMTHL() {
	if (_Sa_ != 0) return;

	cpuRegs.LO.UL[0] = cpuRegs.GPR.r[_Rs_].UL[0];
	cpuRegs.HI.UL[0] = cpuRegs.GPR.r[_Rs_].UL[1];
	cpuRegs.LO.UL[2] = cpuRegs.GPR.r[_Rs_].UL[2];
	cpuRegs.HI.UL[2] = cpuRegs.GPR.r[_Rs_].UL[3];
}

#define _PSLLH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rt_].US[n] << ( _Sa_ & 0xf );

void PSLLH() {
	if (!_Rd_) return;

	_PSLLH(0); _PSLLH(1); _PSLLH(2); _PSLLH(3);
	_PSLLH(4); _PSLLH(5); _PSLLH(6); _PSLLH(7);
}

#define _PSRLH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rt_].US[n] >> ( _Sa_ & 0xf );

void PSRLH () {
	if (!_Rd_) return;

	_PSRLH(0); _PSRLH(1); _PSRLH(2); _PSRLH(3);
	_PSRLH(4); _PSRLH(5); _PSRLH(6); _PSRLH(7);
}

#define _PSRAH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rt_].SS[n] >> ( _Sa_ & 0xf );

void PSRAH() {
	if (!_Rd_) return;

	_PSRAH(0); _PSRAH(1); _PSRAH(2); _PSRAH(3);
	_PSRAH(4); _PSRAH(5); _PSRAH(6); _PSRAH(7);
}

#define _PSLLW(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rt_].UL[n] << _Sa_;

void PSLLW() {
	if (!_Rd_) return;

	_PSLLW(0); _PSLLW(1); _PSLLW(2); _PSLLW(3);
}

#define _PSRLW(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rt_].UL[n] >> _Sa_;

void PSRLW() {
	if (!_Rd_) return;

	_PSRLW(0); _PSRLW(1); _PSRLW(2); _PSRLW(3);
}

#define _PSRAW(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rt_].SL[n] >> _Sa_;

void PSRAW() {
	if (!_Rd_) return;

	_PSRAW(0); _PSRAW(1); _PSRAW(2); _PSRAW(3);
}

//#pragma endregion

//*****************END OF MMI OPCODES**************************
//*************************MMI0 OPCODES************************

//#pragma region MMI0 Opcodes

#define _PADDW(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rs_].UL[n] + cpuRegs.GPR.r[_Rt_].UL[n];

void PADDW() {
	if (!_Rd_) return;

	_PADDW(0); _PADDW(1); _PADDW(2); _PADDW(3);
}

#define _PSUBW(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rs_].UL[n] - cpuRegs.GPR.r[_Rt_].UL[n];

void PSUBW() {
	if (!_Rd_) return;

	_PSUBW(0); _PSUBW(1); _PSUBW(2); _PSUBW(3);
}

#define _PCGTW(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = \
		(cpuRegs.GPR.r[_Rs_].SL[n] > cpuRegs.GPR.r[_Rt_].SL[n]) ? \
		 0xFFFFFFFF : 0x00000000;

void PCGTW() {
	if (!_Rd_) return;

	_PCGTW(0);  _PCGTW(1);  _PCGTW(2);  _PCGTW(3);
}

#define _PMAXW(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = \
		(cpuRegs.GPR.r[_Rs_].SL[n] > cpuRegs.GPR.r[_Rt_].SL[n]) ? \
		 cpuRegs.GPR.r[_Rs_].UL[n] : cpuRegs.GPR.r[_Rt_].UL[n];

void PMAXW() {
	if (!_Rd_) return;

	_PMAXW(0);  _PMAXW(1);  _PMAXW(2);  _PMAXW(3);
}

#define _PADDH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rs_].US[n] + cpuRegs.GPR.r[_Rt_].US[n];

void PADDH() {
	if (!_Rd_) return;

	_PADDH(0);  _PADDH(1);  _PADDH(2);  _PADDH(3);
	_PADDH(4);  _PADDH(5);  _PADDH(6);  _PADDH(7);
}

#define _PSUBH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rs_].US[n] - cpuRegs.GPR.r[_Rt_].US[n];

void PSUBH() {
	if (!_Rd_) return;

	_PSUBH(0);  _PSUBH(1);  _PSUBH(2);  _PSUBH(3);
	_PSUBH(4);  _PSUBH(5);  _PSUBH(6);  _PSUBH(7);
}

#define _PCGTH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = \
		(cpuRegs.GPR.r[_Rs_].SS[n] > cpuRegs.GPR.r[_Rt_].SS[n]) ? \
		 0xFFFF : 0x0000;

void PCGTH() {
	if (!_Rd_) return;

	_PCGTH(0);  _PCGTH(1);  _PCGTH(2);  _PCGTH(3);
	_PCGTH(4);  _PCGTH(5);  _PCGTH(6);  _PCGTH(7);
}

#define _PMAXH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = \
		(cpuRegs.GPR.r[_Rs_].SS[n] > cpuRegs.GPR.r[_Rt_].SS[n]) ? \
		 cpuRegs.GPR.r[_Rs_].US[n] : cpuRegs.GPR.r[_Rt_].US[n];

void PMAXH() {
	if (!_Rd_) return;

	_PMAXH(0);  _PMAXH(1);  _PMAXH(2);  _PMAXH(3);
	_PMAXH(4);  _PMAXH(5);  _PMAXH(6);  _PMAXH(7);
}

#define _PADDB(n) \
	cpuRegs.GPR.r[_Rd_].SC[n] = cpuRegs.GPR.r[_Rs_].SC[n] + cpuRegs.GPR.r[_Rt_].SC[n];

void PADDB() {
	if (!_Rd_) return;

	_PADDB(0);  _PADDB(1);  _PADDB(2);  _PADDB(3);
	_PADDB(4);  _PADDB(5);  _PADDB(6);  _PADDB(7);
	_PADDB(8);  _PADDB(9);  _PADDB(10); _PADDB(11);
	_PADDB(12); _PADDB(13); _PADDB(14); _PADDB(15);
}

#define _PSUBB(n) \
	cpuRegs.GPR.r[_Rd_].SC[n] = cpuRegs.GPR.r[_Rs_].SC[n] - cpuRegs.GPR.r[_Rt_].SC[n];

void PSUBB() {
	if (!_Rd_) return;

	_PSUBB(0);  _PSUBB(1);  _PSUBB(2);  _PSUBB(3);
	_PSUBB(4);  _PSUBB(5);  _PSUBB(6);  _PSUBB(7);
	_PSUBB(8);  _PSUBB(9);  _PSUBB(10); _PSUBB(11);
	_PSUBB(12); _PSUBB(13); _PSUBB(14); _PSUBB(15);
}

#define _PCGTB(n) \
    cpuRegs.GPR.r[_Rd_].UC[n] = (cpuRegs.GPR.r[_Rs_].SC[n] > cpuRegs.GPR.r[_Rt_].SC[n]) ? \
                                 0xFF : 0x00;

void PCGTB() {
	if (!_Rd_) return;

	_PCGTB(0);  _PCGTB(1);  _PCGTB(2);  _PCGTB(3);
	_PCGTB(4);  _PCGTB(5);  _PCGTB(6);  _PCGTB(7);
	_PCGTB(8);  _PCGTB(9);  _PCGTB(10); _PCGTB(11);
	_PCGTB(12); _PCGTB(13); _PCGTB(14); _PCGTB(15);
}

#define _PADDSW(n) \
    sTemp64 = (s64)cpuRegs.GPR.r[_Rs_].SL[n] + (s64)cpuRegs.GPR.r[_Rt_].SL[n]; \
    if (sTemp64 > 0x7FFFFFFF) { \
        cpuRegs.GPR.r[_Rd_].UL[n] = 0x7FFFFFFF; \
    } else \
    if ((sTemp64 < 0x180000000LL) && (sTemp64 >= 0x100000000LL)) { \
        cpuRegs.GPR.r[_Rd_].UL[n] = 0x80000000LL; \
    } else { \
        cpuRegs.GPR.r[_Rd_].UL[n] = (s32)sTemp64; \
    }

void PADDSW() {
	s64 sTemp64;

	if (!_Rd_) return;

	_PADDSW(0); _PADDSW(1); _PADDSW(2); _PADDSW(3);
}

#define _PSUBSW(n) \
    sTemp64 = (s64)cpuRegs.GPR.r[_Rs_].UL[n] - (s64)cpuRegs.GPR.r[_Rt_].UL[n]; \
    if (sTemp64 >= 0x7FFFFFFF) { \
        cpuRegs.GPR.r[_Rd_].UL[n] = 0x7FFFFFFF; \
    } else \
    if ((sTemp64 < 0x180000000LL) && (sTemp64 >= 0x100000000LL)) { \
        cpuRegs.GPR.r[_Rd_].UL[n] = 0x80000000LL; \
    } else { \
        cpuRegs.GPR.r[_Rd_].UL[n] = (s32)sTemp64; \
    }

void PSUBSW() {
	s64 sTemp64;

	if (!_Rd_) return;

	_PSUBSW(0); _PSUBSW(1); _PSUBSW(2); _PSUBSW(3);
}

void PEXTLW() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rs.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[1];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rs.UL[1];
}

void PPACW() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rs.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rs.UL[2];
}

#define _PADDSH(n) \
    sTemp32 = (s32)cpuRegs.GPR.r[_Rs_].SS[n] + (s32)cpuRegs.GPR.r[_Rt_].SS[n]; \
    if (sTemp32 > 0x7FFF) { \
        cpuRegs.GPR.r[_Rd_].US[n] = 0x7FFF; \
    } else \
    if ((sTemp32 < 0x18000) && (sTemp32 >= 0x10000)) { \
        cpuRegs.GPR.r[_Rd_ ].US[n] = 0x8000; \
    } else { \
        cpuRegs.GPR.r[_Rd_ ].US[n] = (s16)sTemp32; \
    }

void PADDSH() {
	s32 sTemp32;

	if (!_Rd_) return;

	_PADDSH(0); _PADDSH(1); _PADDSH(2); _PADDSH(3);
	_PADDSH(4); _PADDSH(5); _PADDSH(6); _PADDSH(7);
}

#define _PSUBSH(n) \
    sTemp32 = (s32)cpuRegs.GPR.r[_Rs_].SS[n] - (s32)cpuRegs.GPR.r[_Rt_].SS[n]; \
    if (sTemp32 >= 0x7FFF) { \
        cpuRegs.GPR.r[_Rd_].US[n] = 0x7FFF; \
    } else \
    if ((sTemp32 < 0x18000) && (sTemp32 >= 0x10000)) { \
        cpuRegs.GPR.r[_Rd_].US[n] = 0x8000; \
    } else { \
        cpuRegs.GPR.r[_Rd_].US[n] = (s16)sTemp32; \
    }

void PSUBSH() {
	s32 sTemp32;

	if (!_Rd_) return;

	_PSUBSH(0); _PSUBSH(1); _PSUBSH(2); _PSUBSH(3);
	_PSUBSH(4); _PSUBSH(5); _PSUBSH(6); _PSUBSH(7);
}

void PEXTLH() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rs.US[0];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[3] = Rs.US[1];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[2];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[3];
}

void PPACH() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[4] = Rs.US[0];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[2];
	cpuRegs.GPR.r[_Rd_].US[6] = Rs.US[4];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[6];
}

#define _PADDSB(n) \
    sTemp16 = (s16)cpuRegs.GPR.r[_Rs_].SC[n] + (s16)cpuRegs.GPR.r[_Rt_].SC[n]; \
    if (sTemp16 > 0x7F) { \
        cpuRegs.GPR.r[_Rd_].UC[n] = 0x7F; \
    } else \
    if ((sTemp16 < 0x180) && (sTemp16 >= 0x100)) { \
        cpuRegs.GPR.r[_Rd_].UC[n] = 0x80; \
    } else { \
        cpuRegs.GPR.r[_Rd_].UC[n] = (s8)sTemp16; \
    }

void PADDSB() {
	s16 sTemp16;

	if (!_Rd_) return;

	_PADDSB(0);  _PADDSB(1);  _PADDSB(2);  _PADDSB(3);
	_PADDSB(4);  _PADDSB(5);  _PADDSB(6);  _PADDSB(7);
	_PADDSB(8);  _PADDSB(9);  _PADDSB(10); _PADDSB(11);
	_PADDSB(12); _PADDSB(13); _PADDSB(14); _PADDSB(15);
}

#define _PSUBSB(n) \
    sTemp16 = (s16)cpuRegs.GPR.r[_Rs_].SC[n] - (s16)cpuRegs.GPR.r[_Rt_].SC[n]; \
    if (sTemp16 >= 0x7F) { \
        cpuRegs.GPR.r[_Rd_].UC[n] = 0x7F; \
    } else \
    if ((sTemp16 < 0x180) && (sTemp16 >= 0x100)) { \
        cpuRegs.GPR.r[_Rd_].UC[n] = 0x80; \
    } else { \
        cpuRegs.GPR.r[_Rd_].UC[n] = (s8)sTemp16; \
    }

void PSUBSB() {
	s16 sTemp16;

	if (!_Rd_) return;

	_PSUBSB(0);  _PSUBSB(1);  _PSUBSB(2);  _PSUBSB(3);
	_PSUBSB(4);  _PSUBSB(5);  _PSUBSB(6);  _PSUBSB(7);
	_PSUBSB(8);  _PSUBSB(9);  _PSUBSB(10); _PSUBSB(11);
	_PSUBSB(12); _PSUBSB(13); _PSUBSB(14); _PSUBSB(15);
}

void PEXTLB() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UC[0]  = Rt.UC[0];
	cpuRegs.GPR.r[_Rd_].UC[1]  = Rs.UC[0];
	cpuRegs.GPR.r[_Rd_].UC[2]  = Rt.UC[1];
	cpuRegs.GPR.r[_Rd_].UC[3]  = Rs.UC[1];

	cpuRegs.GPR.r[_Rd_].UC[4]  = Rt.UC[2];
	cpuRegs.GPR.r[_Rd_].UC[5]  = Rs.UC[2];
	cpuRegs.GPR.r[_Rd_].UC[6]  = Rt.UC[3];
	cpuRegs.GPR.r[_Rd_].UC[7]  = Rs.UC[3];

	cpuRegs.GPR.r[_Rd_].UC[8]  = Rt.UC[4];
	cpuRegs.GPR.r[_Rd_].UC[9]  = Rs.UC[4];
	cpuRegs.GPR.r[_Rd_].UC[10] = Rt.UC[5];
	cpuRegs.GPR.r[_Rd_].UC[11] = Rs.UC[5];

	cpuRegs.GPR.r[_Rd_].UC[12] = Rt.UC[6];
	cpuRegs.GPR.r[_Rd_].UC[13] = Rs.UC[6];
	cpuRegs.GPR.r[_Rd_].UC[14] = Rt.UC[7];
	cpuRegs.GPR.r[_Rd_].UC[15] = Rs.UC[7];
}

void PPACB() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UC[0]  = Rt.UC[0];
	cpuRegs.GPR.r[_Rd_].UC[1]  = Rt.UC[2];
	cpuRegs.GPR.r[_Rd_].UC[2]  = Rt.UC[4];
	cpuRegs.GPR.r[_Rd_].UC[3]  = Rt.UC[6];

	cpuRegs.GPR.r[_Rd_].UC[4]  = Rt.UC[8];
	cpuRegs.GPR.r[_Rd_].UC[5]  = Rt.UC[10];
	cpuRegs.GPR.r[_Rd_].UC[6]  = Rt.UC[12];
	cpuRegs.GPR.r[_Rd_].UC[7]  = Rt.UC[14];

	cpuRegs.GPR.r[_Rd_].UC[8]  = Rs.UC[0];
	cpuRegs.GPR.r[_Rd_].UC[9]  = Rs.UC[2];
	cpuRegs.GPR.r[_Rd_].UC[10] = Rs.UC[4];
	cpuRegs.GPR.r[_Rd_].UC[11] = Rs.UC[6];

	cpuRegs.GPR.r[_Rd_].UC[12] = Rs.UC[8];
	cpuRegs.GPR.r[_Rd_].UC[13] = Rs.UC[10];
	cpuRegs.GPR.r[_Rd_].UC[14] = Rs.UC[12];
	cpuRegs.GPR.r[_Rd_].UC[15] = Rs.UC[14];
}

#define _PEXT5(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = \
		((cpuRegs.GPR.r[_Rt_].UL[n] & 0x0000001F) <<  3) | \
		((cpuRegs.GPR.r[_Rt_].UL[n] & 0x000003E0) <<  6) | \
		((cpuRegs.GPR.r[_Rt_].UL[n] & 0x00007C00) <<  9) | \
		((cpuRegs.GPR.r[_Rt_].UL[n] & 0x00008000) << 16);

void PEXT5() {
	if (!_Rd_) return;

	_PEXT5(0); _PEXT5(1); _PEXT5(2); _PEXT5(3);
}

#define _PPAC5(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = \
		((cpuRegs.GPR.r[_Rt_].UL[n] >>  3) & 0x0000001F) | \
		((cpuRegs.GPR.r[_Rt_].UL[n] >>  6) & 0x000003E0) | \
		((cpuRegs.GPR.r[_Rt_].UL[n] >>  9) & 0x00007C00) | \
		((cpuRegs.GPR.r[_Rt_].UL[n] >> 16) & 0x00008000);

void PPAC5() {
	if (!_Rd_) return;

	_PPAC5(0); _PPAC5(1); _PPAC5(2); _PPAC5(3);
}
//#pragma endregion
//***END OF MMI0 OPCODES******************************************
//**********MMI1 OPCODES**************************************
//#pragma region MMI1 Opcodes
#define _PABSW(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = abs(cpuRegs.GPR.r[_Rt_].SL[n]);

void PABSW() {
	if (!_Rd_) return;

	_PABSW(0);  _PABSW(1);  _PABSW(2);  _PABSW(3);
}

#define _PCEQW(n) \
	cpuRegs.GPR.r[_Rd_].UL[n] = \
		(cpuRegs.GPR.r[_Rs_].UL[n] == cpuRegs.GPR.r[_Rt_].UL[n]) ? \
		 0xFFFFFFFF : 0x00000000;

void PCEQW() {
	if (!_Rd_) return;

	_PCEQW(0); _PCEQW(1); _PCEQW(2); _PCEQW(3);
}

#define _PMINW(n) \
	cpuRegs.GPR.r[_Rd_].SL[n] = \
		(cpuRegs.GPR.r[_Rs_].SL[n] < cpuRegs.GPR.r[_Rt_].SL[n]) ? \
		 cpuRegs.GPR.r[_Rs_].SL[n] : cpuRegs.GPR.r[_Rt_].SL[n];

void PMINW() {
	if (!_Rd_) return;

	_PMINW(0); _PMINW(1); _PMINW(2); _PMINW(3);
}

void PADSBH() {
	if (!_Rd_) return;

	_PSUBH(0); _PSUBH(1); _PSUBH(2); _PSUBH(3);
	_PADDH(4); _PADDH(5); _PADDH(6); _PADDH(7);
}

#define _PABSH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = abs(cpuRegs.GPR.r[_Rt_].SS[n]);

void PABSH() {
	if (!_Rd_) return;

	_PABSH(0); _PABSH(1); _PABSH(2); _PABSH(3);
	_PABSH(4); _PABSH(5); _PABSH(6); _PABSH(7);
}

#define _PCEQH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = \
		(cpuRegs.GPR.r[_Rs_].US[n] == cpuRegs.GPR.r[_Rt_].US[n]) ? 0xFFFF : 0x0000;

void PCEQH() {
	if (!_Rd_) return;

	_PCEQH(0); _PCEQH(1); _PCEQH(2); _PCEQH(3);
	_PCEQH(4); _PCEQH(5); _PCEQH(6); _PCEQH(7);
}

#define _PMINH(n) \
	cpuRegs.GPR.r[_Rd_].US[n] = \
		(cpuRegs.GPR.r[_Rs_].SS[n] < cpuRegs.GPR.r[_Rt_].SS[n]) ? \
		 cpuRegs.GPR.r[_Rs_].US[n] : cpuRegs.GPR.r[_Rt_].US[n];

void PMINH() {
	if (!_Rd_) return;

	_PMINH(0); _PMINH(1); _PMINH(2); _PMINH(3);
	_PMINH(4); _PMINH(5); _PMINH(6); _PMINH(7);
}

#define _PCEQB(n) \
	cpuRegs.GPR.r[_Rd_].UC[n] = (cpuRegs.GPR.r[_Rs_].UC[n] == \
								 cpuRegs.GPR.r[_Rt_].UC[n]) ? 0xFF : 0x00;

void PCEQB() {
	if (!_Rd_) return;

	_PCEQB(0);  _PCEQB(1);  _PCEQB(2);  _PCEQB(3);
	_PCEQB(4);  _PCEQB(5);  _PCEQB(6);  _PCEQB(7);
	_PCEQB(8);  _PCEQB(9);  _PCEQB(10); _PCEQB(11);
	_PCEQB(12); _PCEQB(13); _PCEQB(14); _PCEQB(15);
}

#define _PADDUW(n) \
	tmp = (s64)cpuRegs.GPR.r[_Rs_].UL[n] + (s64)cpuRegs.GPR.r[_Rt_].UL[n]; \
	if (tmp > 0xffffffff) \
		 cpuRegs.GPR.r[_Rd_].UL[n] = 0xffffffff; \
	else cpuRegs.GPR.r[_Rd_].UL[n] = (u32)tmp;

void PADDUW () {
	s64 tmp;

	if (!_Rd_) return;

	_PADDUW(0); _PADDUW(1); _PADDUW(2); _PADDUW(3);
}

#define _PSUBUW(n) \
	sTemp64 = (s64)cpuRegs.GPR.r[_Rs_].UL[n] - (s64)cpuRegs.GPR.r[_Rt_].UL[n]; \
	if (sTemp64 <= 0x0) { \
		cpuRegs.GPR.r[_Rd_].UL[n] = 0x0; \
	} else { \
		cpuRegs.GPR.r[_Rd_].UL[n] = (u32)sTemp64; \
	}

void PSUBUW() {
	s64 sTemp64;

	if (!_Rd_) return;

	_PSUBUW(0); _PSUBUW(1); _PSUBUW(2); _PSUBUW(3);
}

void PEXTUW() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rs.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[3];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rs.UL[3];
}

#define _PADDUH(n) \
	sTemp32 = (s32)cpuRegs.GPR.r[_Rs_].US[n] + (s32)cpuRegs.GPR.r[_Rt_].US[n]; \
	if (sTemp32 > 0xFFFF) { \
		cpuRegs.GPR.r[_Rd_].US[n] = 0xFFFF; \
	} else { \
		cpuRegs.GPR.r[_Rd_].US[n] = (u16)sTemp32; \
	}

void PADDUH() {
	s32 sTemp32;

	if (!_Rd_) return;

	_PADDUH(0); _PADDUH(1); _PADDUH(2); _PADDUH(3);
	_PADDUH(4); _PADDUH(5); _PADDUH(6); _PADDUH(7);
}

#define _PSUBUH(n) \
	sTemp32 = (s32)cpuRegs.GPR.r[_Rs_].US[n] - (s32)cpuRegs.GPR.r[_Rt_].US[n]; \
	if (sTemp32 <= 0x0) { \
		cpuRegs.GPR.r[_Rd_].US[n] = 0x0; \
	} else { \
		cpuRegs.GPR.r[_Rd_].US[n] = (u16)sTemp32; \
	}

void PSUBUH() {
	s32 sTemp32;

	if (!_Rd_) return;

	_PSUBUH(0); _PSUBUH(1); _PSUBUH(2); _PSUBUH(3);
	_PSUBUH(4); _PSUBUH(5); _PSUBUH(6); _PSUBUH(7);
}

void PEXTUH() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[1] = Rs.US[4];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[5];
	cpuRegs.GPR.r[_Rd_].US[3] = Rs.US[5];

	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[6];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[7];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[7];
}

#define _PADDUB(n) \
	Temp16 = (u16)cpuRegs.GPR.r[_Rs_].UC[n] + (u16)cpuRegs.GPR.r[_Rt_].UC[n]; \
	if (Temp16 > 0xFF) { \
		cpuRegs.GPR.r[_Rd_].UC[n] = 0xFF; \
	} else { \
		cpuRegs.GPR.r[_Rd_].UC[n] = (u8)Temp16; \
	}

void PADDUB() {
	u16 Temp16;

	if (!_Rd_) return;

	_PADDUB(0);  _PADDUB(1);  _PADDUB(2);  _PADDUB(3);
	_PADDUB(4);  _PADDUB(5);  _PADDUB(6);  _PADDUB(7);
	_PADDUB(8);  _PADDUB(9);  _PADDUB(10); _PADDUB(11);
	_PADDUB(12); _PADDUB(13); _PADDUB(14); _PADDUB(15);
}

#define _PSUBUB(n) \
	sTemp16 = (s16)cpuRegs.GPR.r[_Rs_].UC[n] - (s16)cpuRegs.GPR.r[_Rt_].UC[n]; \
	if (sTemp16 <= 0x0) { \
		cpuRegs.GPR.r[_Rd_].UC[n] = 0x0; \
	} else { \
		cpuRegs.GPR.r[_Rd_].UC[n] = (u8)sTemp16; \
	}

void PSUBUB() {
	s16 sTemp16;

	if (!_Rd_) return;

	_PSUBUB(0);  _PSUBUB(1);  _PSUBUB(2);  _PSUBUB(3);
	_PSUBUB(4);  _PSUBUB(5);  _PSUBUB(6);  _PSUBUB(7);
	_PSUBUB(8);  _PSUBUB(9);  _PSUBUB(10); _PSUBUB(11);
	_PSUBUB(12); _PSUBUB(13); _PSUBUB(14); _PSUBUB(15);
}

void PEXTUB() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UC[0]  = Rt.UC[8];
	cpuRegs.GPR.r[_Rd_].UC[1]  = Rs.UC[8];
	cpuRegs.GPR.r[_Rd_].UC[2]  = Rt.UC[9];
	cpuRegs.GPR.r[_Rd_].UC[3]  = Rs.UC[9];
	cpuRegs.GPR.r[_Rd_].UC[4]  = Rt.UC[10];
	cpuRegs.GPR.r[_Rd_].UC[5]  = Rs.UC[10];
	cpuRegs.GPR.r[_Rd_].UC[6]  = Rt.UC[11];
	cpuRegs.GPR.r[_Rd_].UC[7]  = Rs.UC[11];
	cpuRegs.GPR.r[_Rd_].UC[8]  = Rt.UC[12];
	cpuRegs.GPR.r[_Rd_].UC[9]  = Rs.UC[12];
	cpuRegs.GPR.r[_Rd_].UC[10] = Rt.UC[13];
	cpuRegs.GPR.r[_Rd_].UC[11] = Rs.UC[13];
	cpuRegs.GPR.r[_Rd_].UC[12] = Rt.UC[14];
	cpuRegs.GPR.r[_Rd_].UC[13] = Rs.UC[14];
	cpuRegs.GPR.r[_Rd_].UC[14] = Rt.UC[15];
	cpuRegs.GPR.r[_Rd_].UC[15] = Rs.UC[15];
}

void QFSRV() {				// JayteeMaster: changed a bit to avoid screw up
	GPR_reg Rd;
	if (!_Rd_) return;

	if (cpuRegs.sa == 0) {
		cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[0];
		cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rt_].UD[1];
	} else {
		if (cpuRegs.sa < 64) {
			/*
			cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[0] >> cpuRegs.sa;
			cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rt_].UD[1] >> cpuRegs.sa;
			cpuRegs.GPR.r[_Rd_].UD[0]|= cpuRegs.GPR.r[_Rt_].UD[1] << (64 - cpuRegs.sa);
			cpuRegs.GPR.r[_Rd_].UD[1]|= cpuRegs.GPR.r[_Rs_].UD[0] << (64 - cpuRegs.sa);
			*/
			Rd.UD[0] = cpuRegs.GPR.r[_Rt_].UD[0] >> cpuRegs.sa;
			Rd.UD[1] = cpuRegs.GPR.r[_Rt_].UD[1] >> cpuRegs.sa;
			Rd.UD[0]|= cpuRegs.GPR.r[_Rt_].UD[1] << (64 - cpuRegs.sa);
			Rd.UD[1]|= cpuRegs.GPR.r[_Rs_].UD[0] << (64 - cpuRegs.sa);
			cpuRegs.GPR.r[_Rd_] = Rd;
		} else {
			/*
			cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[1] >> (cpuRegs.sa - 64);
			cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[0] >> (cpuRegs.sa - 64);
			cpuRegs.GPR.r[_Rd_].UD[0]|= cpuRegs.GPR.r[_Rs_].UD[0] << (128 - cpuRegs.sa);
			cpuRegs.GPR.r[_Rd_].UD[1]|= cpuRegs.GPR.r[_Rs_].UD[1] << (128 - cpuRegs.sa);
			*/
			Rd.UD[0] = cpuRegs.GPR.r[_Rt_].UD[1] >> (cpuRegs.sa - 64);
			Rd.UD[1] = cpuRegs.GPR.r[_Rs_].UD[0] >> (cpuRegs.sa - 64);
			Rd.UD[0]|= cpuRegs.GPR.r[_Rs_].UD[0] << (128 - cpuRegs.sa);
			Rd.UD[1]|= cpuRegs.GPR.r[_Rs_].UD[1] << (128 - cpuRegs.sa);
			cpuRegs.GPR.r[_Rd_] = Rd;
		}
	}
}
//#pragma endregion
//********END OF MMI1 OPCODES***********************************

//*********MMI2 OPCODES***************************************
//#pragma region MMI2 Opcodes
#define _PMADDW(dd, ss) { \
	s64 temp =	(s64)((s64)cpuRegs.LO.SL[ss] | ((s64)cpuRegs.HI.SL[ss] << 32)) + \
			   ((s64)cpuRegs.GPR.r[_Rs_].SL[ss] * (s64)cpuRegs.GPR.r[_Rt_].SL[ss]); \
 \
	cpuRegs.LO.SD[dd] = (s32)(temp & 0xffffffff); \
	cpuRegs.HI.SD[dd] = (s32)(temp >> 32); \
 \
	if (_Rd_) cpuRegs.GPR.r[_Rd_].SD[dd] = temp; \
}

void PMADDW() {
	_PMADDW(0, 0);
	_PMADDW(1, 2);
}

void PSLLVW() {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = (s32)(cpuRegs.GPR.r[_Rt_].UL[0] <<
									 (cpuRegs.GPR.r[_Rs_].UL[0] & 0x1F));
	cpuRegs.GPR.r[_Rd_].UD[1] = (s32)(cpuRegs.GPR.r[_Rt_].UL[2] <<
									 (cpuRegs.GPR.r[_Rs_].UL[2] & 0x1F));
}

void PSRLVW() {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = (cpuRegs.GPR.r[_Rt_].UL[0] >>
								(cpuRegs.GPR.r[_Rs_].UL[0] & 0x1F));
	cpuRegs.GPR.r[_Rd_].UD[1] = (cpuRegs.GPR.r[_Rt_].UL[2] >>
								(cpuRegs.GPR.r[_Rs_].UL[2] & 0x1F));
}

#define _PMSUBW(dd, ss) { \
	s64 temp =	(s64)((s64)cpuRegs.LO.SL[ss] | ((s64)cpuRegs.HI.SL[ss] << 32)) - \
			   ((s64)cpuRegs.GPR.r[_Rs_].SL[ss] * (s64)cpuRegs.GPR.r[_Rt_].SL[ss]); \
 \
	cpuRegs.LO.SD[dd] = (s32)(temp & 0xffffffff); \
	cpuRegs.HI.SD[dd] = (s32)(temp >> 32); \
 \
	if (_Rd_) cpuRegs.GPR.r[_Rd_].SD[dd] = temp; \
}

void PMSUBW() {
	_PMSUBW(0, 0);
	_PMSUBW(1, 2);
}

void PMFHI() {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.HI.UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.HI.UD[1];
}

void PMFLO() {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.LO.UD[1];
}

void PINTH() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rs.US[4];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[3] = Rs.US[5];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[6];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[7];
}

#define _PMULTW(dd, ss) { \
    s64 temp = (s64)cpuRegs.GPR.r[_Rs_].SL[ss] * (s64)cpuRegs.GPR.r[_Rt_].SL[ss]; \
 \
    cpuRegs.LO.UD[dd] = (s32)(temp & 0xffffffff); \
    cpuRegs.HI.UD[dd] = (s32)(temp >> 32); \
 \
    if (_Rd_) { \
        cpuRegs.GPR.r[_Rd_].SD[dd] = temp; \
    } \
}

void PMULTW() {
	_PMULTW(0, 0);
	_PMULTW(1, 2);
}

#define _PDIVW(dd, ss) \
    if (cpuRegs.GPR.r[_Rt_].UL[ss] != 0) { \
        cpuRegs.LO.SD[dd] = cpuRegs.GPR.r[_Rs_].SL[ss] / cpuRegs.GPR.r[_Rt_].SL[ss]; \
        cpuRegs.HI.SD[dd] = cpuRegs.GPR.r[_Rs_].SL[ss] % cpuRegs.GPR.r[_Rt_].SL[ss]; \
    }

void PDIVW() {
	_PDIVW(0, 0);
	_PDIVW(1, 2);
}

void PCPYLD() {
	if (!_Rd_) return;

	// note: first _Rs_, since the other way when _Rd_ equals
	// _Rs_ or _Rt_ this would screw up
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[0];
	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[0];
}

void PMADDH() {			// JayteeMaster: changed a bit to avoid screw up
	s32 temp;

    temp = cpuRegs.LO.UL[0] + (s32)cpuRegs.GPR.r[_Rs_].SS[0] * (s32)cpuRegs.GPR.r[_Rt_].SS[0];
    cpuRegs.LO.UL[0] = temp;
    /* if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[0] = temp; */

    temp = cpuRegs.LO.UL[1] + (s32)cpuRegs.GPR.r[_Rs_].SS[1] * (s32)cpuRegs.GPR.r[_Rt_].SS[1];
    cpuRegs.LO.UL[1] = temp;

    temp = cpuRegs.HI.UL[0] + (s32)cpuRegs.GPR.r[_Rs_].SS[2] * (s32)cpuRegs.GPR.r[_Rt_].SS[2];
    cpuRegs.HI.UL[0] = temp;
    /* if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[1] = temp; */

    temp = cpuRegs.HI.UL[1] + (s32)cpuRegs.GPR.r[_Rs_].SS[3] * (s32)cpuRegs.GPR.r[_Rt_].SS[3];
    cpuRegs.HI.UL[1] = temp;

    temp = cpuRegs.LO.UL[2] + (s32)cpuRegs.GPR.r[_Rs_].SS[4] * (s32)cpuRegs.GPR.r[_Rt_].SS[4];
    cpuRegs.LO.UL[2] = temp;
    /* if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[2] = temp; */

    temp = cpuRegs.LO.UL[3] + (s32)cpuRegs.GPR.r[_Rs_].SS[5] * (s32)cpuRegs.GPR.r[_Rt_].SS[5];
    cpuRegs.LO.UL[3] = temp;

    temp = cpuRegs.HI.UL[2] + (s32)cpuRegs.GPR.r[_Rs_].SS[6] * (s32)cpuRegs.GPR.r[_Rt_].SS[6];
    cpuRegs.HI.UL[2] = temp;
    /* if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[3] = temp; */

    temp = cpuRegs.HI.UL[3] + (s32)cpuRegs.GPR.r[_Rs_].SS[7] * (s32)cpuRegs.GPR.r[_Rt_].SS[7];
    cpuRegs.HI.UL[3] = temp;

	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}

}

// JayteeMaster: changed a bit to avoid screw up
#define _PHMADH(hlr, dd, n) { \
	s32 temp = (s32)cpuRegs.GPR.r[_Rs_].SS[n+1] * (s32)cpuRegs.GPR.r[_Rt_].SS[n+1] + \
			   (s32)cpuRegs.GPR.r[_Rs_].SS[n]   * (s32)cpuRegs.GPR.r[_Rt_].SS[n]; \
 \
	cpuRegs.hlr.UL[dd] = temp; \
	/*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[n] = temp; */\
}

void PHMADH() {				// JayteeMaster: changed a bit to avoid screw up. Also used 0,2,4,6 instead of 0,1,2,3
	_PHMADH(LO, 0, 0);
	_PHMADH(HI, 0, 2);
	_PHMADH(LO, 2, 4);
	_PHMADH(HI, 2, 6);
	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}
}

void PAND() {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] & cpuRegs.GPR.r[_Rt_].UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[1] & cpuRegs.GPR.r[_Rt_].UD[1];
}

void PXOR() {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] ^ cpuRegs.GPR.r[_Rt_].UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[1] ^ cpuRegs.GPR.r[_Rt_].UD[1];
}

void PMSUBH() {			// JayteeMaster: changed a bit to avoid screw up
	s32 temp;

    temp = cpuRegs.LO.UL[0] - (s32)cpuRegs.GPR.r[_Rs_].SS[0] * (s32)cpuRegs.GPR.r[_Rt_].SS[0];
    cpuRegs.LO.UL[0] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[0] = temp;*/

    temp = cpuRegs.LO.UL[1] - (s32)cpuRegs.GPR.r[_Rs_].SS[1] * (s32)cpuRegs.GPR.r[_Rt_].SS[1];
    cpuRegs.LO.UL[1] = temp;

    temp = cpuRegs.HI.UL[0] - (s32)cpuRegs.GPR.r[_Rs_].SS[2] * (s32)cpuRegs.GPR.r[_Rt_].SS[2];
    cpuRegs.HI.UL[0] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[1] = temp;*/

    temp = cpuRegs.HI.UL[1] - (s32)cpuRegs.GPR.r[_Rs_].SS[3] * (s32)cpuRegs.GPR.r[_Rt_].SS[3];
    cpuRegs.HI.UL[1] = temp;

    temp = cpuRegs.LO.UL[2] - (s32)cpuRegs.GPR.r[_Rs_].SS[4] * (s32)cpuRegs.GPR.r[_Rt_].SS[4];
    cpuRegs.LO.UL[2] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[2] = temp;*/

    temp = cpuRegs.LO.UL[3] - (s32)cpuRegs.GPR.r[_Rs_].SS[5] * (s32)cpuRegs.GPR.r[_Rt_].SS[5];
    cpuRegs.LO.UL[3] = temp;

    temp = cpuRegs.HI.UL[2] - (s32)cpuRegs.GPR.r[_Rs_].SS[6] * (s32)cpuRegs.GPR.r[_Rt_].SS[6];
    cpuRegs.HI.UL[2] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[3] = temp;*/

    temp = cpuRegs.HI.UL[3] - (s32)cpuRegs.GPR.r[_Rs_].SS[7] * (s32)cpuRegs.GPR.r[_Rt_].SS[7];
    cpuRegs.HI.UL[3] = temp;

	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}
}

// JayteeMaster: changed a bit to avoid screw up
#define _PHMSBH(hlr, dd, n, rdd) { \
	s32 temp = (s32)cpuRegs.GPR.r[_Rs_].SS[n+1] * (s32)cpuRegs.GPR.r[_Rt_].SS[n+1] - \
			   (s32)cpuRegs.GPR.r[_Rs_].SS[n]   * (s32)cpuRegs.GPR.r[_Rt_].SS[n]; \
 \
	cpuRegs.hlr.UL[dd] = temp; \
	/*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[rdd] = temp;*/ \
}

void PHMSBH() {		// JayteeMaster: changed a bit to avoid screw up
	_PHMSBH(LO, 0, 0, 0);
	_PHMSBH(HI, 0, 2, 1);
	_PHMSBH(LO, 2, 4, 2);
	_PHMSBH(HI, 2, 6, 3);
	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}
}

void PEXEH() {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[5] = Rt.US[5];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[7] = Rt.US[7];
}

void PREVH () {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[7];
	cpuRegs.GPR.r[_Rd_].US[5] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[5];
	cpuRegs.GPR.r[_Rd_].US[7] = Rt.US[4];
}

void PMULTH() {			// JayteeMaster: changed a bit to avoid screw up
	s32 temp;

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[0] * (s32)cpuRegs.GPR.r[_Rt_].SS[0];
    cpuRegs.LO.UL[0] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[0] = temp;*/

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[1] * (s32)cpuRegs.GPR.r[_Rt_].SS[1];
    cpuRegs.LO.UL[1] = temp;

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[2] * (s32)cpuRegs.GPR.r[_Rt_].SS[2];
    cpuRegs.HI.UL[0] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[1] = temp;*/

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[3] * (s32)cpuRegs.GPR.r[_Rt_].SS[3];
    cpuRegs.HI.UL[1] = temp;

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[4] * (s32)cpuRegs.GPR.r[_Rt_].SS[4];
    cpuRegs.LO.UL[2] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[2] = temp;*/

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[5] * (s32)cpuRegs.GPR.r[_Rt_].SS[5];
    cpuRegs.LO.UL[3] = temp;

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[6] * (s32)cpuRegs.GPR.r[_Rt_].SS[6];
    cpuRegs.HI.UL[2] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[3] = temp;*/

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[7] * (s32)cpuRegs.GPR.r[_Rt_].SS[7];
    cpuRegs.HI.UL[3] = temp;

	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}
}

#define _PDIVBW(n) \
	cpuRegs.LO.UL[n] = (s32)(cpuRegs.GPR.r[_Rs_].SL[n] / cpuRegs.GPR.r[_Rt_].SS[0]); \
	cpuRegs.HI.UL[n] = (s16)(cpuRegs.GPR.r[_Rs_].SL[n] % cpuRegs.GPR.r[_Rt_].SS[0]); \

void PDIVBW() {
	if (cpuRegs.GPR.r[_Rt_].US[0] == 0) return;

	_PDIVBW(0); _PDIVBW(1); _PDIVBW(2); _PDIVBW(3);
}

void PEXEW() {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rt.UL[1];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rt.UL[3];
}

void PROT3W() {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[1];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rt.UL[3];
}
//#pragma endregion
//*****END OF MMI2 OPCODES***********************************

//*************************MMI3 OPCODES************************
//#pragma region MMI3 Opcodes
#define _PMADDUW(dd, ss) { \
	u64 tempu =	(u64)((u64)cpuRegs.LO.UL[ss] | ((u64)cpuRegs.HI.UL[ss] << 32)) + \
			   ((u64)cpuRegs.GPR.r[_Rs_].UL[ss] * (u64)cpuRegs.GPR.r[_Rt_].UL[ss]); \
 \
	cpuRegs.LO.SD[dd] = (s32)(tempu & 0xffffffff); \
	cpuRegs.HI.SD[dd] = (s32)(tempu >> 32); \
 \
	if (_Rd_) cpuRegs.GPR.r[_Rd_].UD[dd] = tempu; \
}

void PMADDUW() {
	_PMADDUW(0, 0);
	_PMADDUW(1, 2);
}

void PSRAVW() {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = (cpuRegs.GPR.r[_Rt_].SL[0] >>
								(cpuRegs.GPR.r[_Rs_].UL[0] & 0x1F));
	cpuRegs.GPR.r[_Rd_].UD[1] = (cpuRegs.GPR.r[_Rt_].SL[2] >>
								(cpuRegs.GPR.r[_Rs_].UL[2] & 0x1F));
}

void PMTHI() {
	cpuRegs.HI.UD[0] = cpuRegs.GPR.r[_Rs_].UD[0];
	cpuRegs.HI.UD[1] = cpuRegs.GPR.r[_Rs_].UD[1];
}

void PMTLO() {
	cpuRegs.LO.UD[0] = cpuRegs.GPR.r[_Rs_].UD[0];
	cpuRegs.LO.UD[1] = cpuRegs.GPR.r[_Rs_].UD[1];
}

void PINTEH() {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rs.US[0];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[3] = Rs.US[2];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[4];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[6];
}

#define _PMULTUW(dd, ss) { \
    u64 tempu = (u64)cpuRegs.GPR.r[_Rs_].UL[ss] * (u64)cpuRegs.GPR.r[_Rt_].UL[ss]; \
 \
    cpuRegs.LO.UD[dd] = (s32)(tempu & 0xffffffff); \
    cpuRegs.HI.UD[dd] = (s32)(tempu >> 32); \
 \
    if (_Rd_) { \
        cpuRegs.GPR.r[_Rd_].UD[dd] = tempu; \
    } \
}

void PMULTUW() {
	_PMULTUW(0, 0);
	_PMULTUW(1, 2);
}

#define _PDIVUW(dd, ss) \
    if (cpuRegs.GPR.r[_Rt_].UL[ss] != 0) { \
        cpuRegs.LO.UD[dd] = (u64)cpuRegs.GPR.r[_Rs_].UL[ss] / (u64)cpuRegs.GPR.r[_Rt_].UL[ss]; \
        cpuRegs.HI.UD[dd] = (u64)cpuRegs.GPR.r[_Rs_].UL[ss] % (u64)cpuRegs.GPR.r[_Rt_].UL[ss]; \
    }

void PDIVUW() {
	_PDIVUW(0, 0);
	_PDIVUW(1, 2);
}

void PCPYUD() {
	if (!_Rd_) return;

	// note: first _Rs_, since the other way when _Rd_ equals
	// _Rs_ or _Rt_ this would screw up
	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[1];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rt_].UD[1];
}

void POR() {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] | cpuRegs.GPR.r[_Rt_].UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[1] | cpuRegs.GPR.r[_Rt_].UD[1];
}

void PNOR () {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = ~(cpuRegs.GPR.r[_Rs_].UD[0] | cpuRegs.GPR.r[_Rt_].UD[0]);
	cpuRegs.GPR.r[_Rd_].UD[1] = ~(cpuRegs.GPR.r[_Rs_].UD[1] | cpuRegs.GPR.r[_Rt_].UD[1]);
}

void PEXCH() {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[5] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[5];
	cpuRegs.GPR.r[_Rd_].US[7] = Rt.US[7];
}

void PCPYH() {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[5] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[7] = Rt.US[4];
}

void PEXCW() {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[1];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rt.UL[3];
}

//**********************END OF MMI3 OPCODES******************** 
//#pragma endregion

//#pragma region COP0 stuff
void COP0() {
	Int_COP0PrintTable[_Rs_]();
}

void COP0_BC0() {
    Int_COP0BC0PrintTable[(cpuRegs.code >> 16) & 0x03]();
}

void COP0_Func() {
    Int_COP0C0PrintTable[_Funct_]();
}

void COP0_Unknown() {printf("Unknown COP0 opp\n");}

void UpdateCP0Status() {
	u32 value = cpuRegs.CP0.n.Status.val;

	if (value & 0x06 ||
		(value & 0x18) == 0) { // Kernel Mode (KSU = 0 | EXL = 1 | ERL = 1)*/
		//memSetKernelMode();	// Kernel memory always
	} else { // User Mode
		//memSetUserMode();
	}
	//if ((cpuRegs.CP0.n.Status.val & 0x10007) == 0x10001)cpuTestHwInts();
}

void WriteCP0Status(u32 value) {
	cpuRegs.CP0.n.Status.val = value;
    UpdateCP0Status();
}

u32 s_iLastCOP0Cycle;
u32 s_iLastPERFCycle[2];

void MFC0() {
	if (!_Rt_) return;

	//if(bExecBIOS == FALSE && _Rd_ == 25) SysPrintf("MFC0 _Rd_ %x = %x\n", _Rd_, cpuRegs.CP0.r[_Rd_]);
	switch (_Rd_) {
		
		case 12: cpuRegs.GPR.r[_Rt_].UD[0] = (s64)(cpuRegs.CP0.r[_Rd_] & 0xf0c79c1f); break;
		case 25: 
		    switch(_Imm_ & 0x3F){
			    case 0: cpuRegs.GPR.r[_Rt_].UD[0] = (s64)cpuRegs.PERF.n.pccr; break;
			    case 1:
					if((cpuRegs.PERF.n.pccr & 0x800003E0) == 0x80000020) {
						cpuRegs.PERF.n.pcr0 += cpuRegs.cycle-s_iLastPERFCycle[0];
						s_iLastPERFCycle[0] = cpuRegs.cycle;
					}
        
                    cpuRegs.GPR.r[_Rt_].UD[0] = (s64)cpuRegs.PERF.n.pcr0;
                    break;
			    case 3:
					if((cpuRegs.PERF.n.pccr & 0x800F8000) == 0x80008000) {
						cpuRegs.PERF.n.pcr1 += cpuRegs.cycle-s_iLastPERFCycle[1];
						s_iLastPERFCycle[1] = cpuRegs.cycle;
					}
					cpuRegs.GPR.r[_Rt_].UD[0] = (s64)cpuRegs.PERF.n.pcr1;
					break;
		    }
		    /*SysPrintf("MFC0 PCCR = %x PCR0 = %x PCR1 = %x IMM= %x\n", 
		    cpuRegs.PERF.n.pccr, cpuRegs.PERF.n.pcr0, cpuRegs.PERF.n.pcr1, _Imm_ & 0x3F);*/
		    break;
		case 9:
			// update
			cpuRegs.CP0.n.Count += cpuRegs.cycle-s_iLastCOP0Cycle;
			s_iLastCOP0Cycle = cpuRegs.cycle;
		default: cpuRegs.GPR.r[_Rt_].UD[0] = (s64)cpuRegs.CP0.r[_Rd_];
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "mfc0 (needs work)";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.imm = _Rd_;
	//printf("%s  %s, %0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}

void MTC0() {
	switch (_Rd_) {
		case 25: 
			switch(_Imm_ & 0x3F){
				case 0:
					if((cpuRegs.PERF.n.pccr & 0x800003E0) == 0x80000020)
						cpuRegs.PERF.n.pcr0 += cpuRegs.cycle-s_iLastPERFCycle[0];
					if((cpuRegs.PERF.n.pccr & 0x800F8000) == 0x80008000)
						cpuRegs.PERF.n.pcr1 += cpuRegs.cycle-s_iLastPERFCycle[1];
					cpuRegs.PERF.n.pccr = cpuRegs.GPR.r[_Rt_].UL[0];
					s_iLastPERFCycle[0] = cpuRegs.cycle;
					s_iLastPERFCycle[1] = cpuRegs.cycle;
					break;
				case 1: cpuRegs.PERF.n.pcr0 = cpuRegs.GPR.r[_Rt_].UL[0]; s_iLastPERFCycle[0] = cpuRegs.cycle; break;
				case 3: cpuRegs.PERF.n.pcr1 = cpuRegs.GPR.r[_Rt_].UL[0]; s_iLastPERFCycle[1] = cpuRegs.cycle; break;
			}
			break;
		case 12: WriteCP0Status(cpuRegs.GPR.r[_Rt_].UL[0]); break;
		case 9: s_iLastCOP0Cycle = cpuRegs.cycle; cpuRegs.CP0.r[9] = cpuRegs.GPR.r[_Rt_].UL[0]; break;
		default: cpuRegs.CP0.r[_Rd_] = cpuRegs.GPR.r[_Rt_].UL[0]; break;
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "mtc0 (needs work)";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.imm = _Rd_;

	//printf("%s  %s, %0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);
}

int CPCOND0() {
	//if(((psHu16(DMAC_STAT) & psHu16(DMAC_PCR)) & 0x3ff) == (psHu16(DMAC_PCR) & 0x3ff)) return 1;
	return 0;
}

//#define CPCOND0	1

#define BC0(cond) \
	if (CPCOND0() cond) {}
		//intDoBranch(_BranchTarget_); 

void BC0F() {
	BC0(== 0);
	oppPacket.isTargetUsed = 0; oppPacket.name = "bc0f";
	oppPacket.target = _JumpTarget_; oppPacket.isTargetUsed = 1;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  \t%0x needs work\n",oppPacket.name, oppPacket.u32Result);
}

void BC0T() {
	BC0(== 1);
	oppPacket.isTargetUsed = 0; oppPacket.name = "bc0t";
	oppPacket.target = _JumpTarget_; oppPacket.isTargetUsed = 1;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  \t%0x needs work\n",oppPacket.name, oppPacket.u32Result);
}

#define BC0L(cond) \
	if (CPCOND0() cond) {}
		//intDoBranch(_BranchTarget_); 
	//} else cpuRegs.pc+= 4;

void BC0FL() {
	BC0L(== 0);
	oppPacket.isTargetUsed = 0; oppPacket.name = "bc0fl";
	oppPacket.target = _JumpTarget_; oppPacket.isTargetUsed = 1;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  \t%0x needs work\n",oppPacket.name, oppPacket.u32Result);
}

void BC0TL() {
	BC0L(== 1);
	oppPacket.isTargetUsed = 0; oppPacket.name = "bc0tl";
	oppPacket.target = _JumpTarget_; oppPacket.isTargetUsed = 1;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  \t%0x needs work\n",oppPacket.name, oppPacket.u32Result);
}

void TLBR() {
	int i = cpuRegs.CP0.n.Index&0x1f;

	cpuRegs.CP0.n.PageMask = tlb[i].PageMask;
	cpuRegs.CP0.n.EntryHi = tlb[i].EntryHi&~(tlb[i].PageMask|0x1f00);
	cpuRegs.CP0.n.EntryLo0 = (tlb[i].EntryLo0&~1)|((tlb[i].EntryHi>>12)&1);
	cpuRegs.CP0.n.EntryLo1 =(tlb[i].EntryLo1&~1)|((tlb[i].EntryHi>>12)&1);
	oppPacket.isTargetUsed = 0; oppPacket.name = "tlbr";
	//printf("%s  \t\n",oppPacket.name);
}

// Helper Macros
//****************************************************************
#define _Ft_         ( ( cpuRegs.code >> 16 ) & 0x1F )
#define _Fs_         ( ( cpuRegs.code >> 11 ) & 0x1F )
#define _Fd_         ( ( cpuRegs.code >>  6 ) & 0x1F )

#define _FtValf_     fpuRegs.fpr[ _Ft_ ].f
#define _FsValf_     fpuRegs.fpr[ _Fs_ ].f
#define _FdValf_     fpuRegs.fpr[ _Fd_ ].f
#define _FAValf_     fpuRegs.ACC.f

#define _ContVal_    fpuRegs.fprc[ 31 ]

// Testing
#define _FtValUl_    fpuRegs.fpr[ _Ft_ ].UL
#define _FsValUl_    fpuRegs.fpr[ _Fs_ ].UL
#define _FdValUl_    fpuRegs.fpr[ _Fd_ ].UL
#define _FAValUl_    fpuRegs.ACC.UL

//****************************************************************

void COP1() {
	Int_COP1PrintTable[_Rs_]();
}

/*********************************************************
* Load and store for COP1                                *
* Format:  OP rt, offset(base)                           *
*********************************************************/



void LWC1() {
	s32 addr;

	addr = cpuRegs.GPR.r[_Rs_].UL[0] + (s16)(cpuRegs.code);// ((cpuRegs.code & 0x8000 ? 0xFFFF8000 : 0)| (cpuRegs.code & 0x7fff));
	//memRead32(addr, &fpuRegs.fpr[_Rt_].UL);
	oppPacket.isTargetUsed = 0; oppPacket.name = "lwc1";
	oppPacket.fr[0] = getFPReg(_Rt_);
	oppPacket.reg[0] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.fr[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[0]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x($%s)\t\n",oppPacket.name, oppPacket.fr[0],oppPacket.imm, oppPacket.reg[0]);////printf("%s  %s, %0x\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.imm);

}

void SWC1() {
	s32 addr;
	addr = cpuRegs.GPR.r[_Rs_].UL[0] + (s16)(cpuRegs.code);//((cpuRegs.code & 0x8000 ? 0xFFFF8000 : 0)| (cpuRegs.code & 0x7fff));
	//memWrite32(addr,  fpuRegs.fpr[_Rt_].UL); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "swc1";
	oppPacket.fr[0] = getFPReg(_Rt_);
	oppPacket.reg[1] = getGPReg(_Rs_);
	oppPacket.imm = _Imm_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s", oppPacket.fr[0]);
	sprintf(oppPacket.loadoffsetString," $%.4x($%s)",oppPacket.loadOffset,oppPacket.reg[0]);
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, $%0x($%s)\t\n",oppPacket.name, oppPacket.fr[0],oppPacket.imm, oppPacket.reg[0]);
}

void COP1_BC1() {
	Int_COP1BC1PrintTable[_Rt_]();
}

void COP1_S() {
	Int_COP1SPrintTable[_Funct_]();
}

void COP1_W() {
	Int_COP1WPrintTable[_Funct_]();
}

void COP1_Unknown() {printf("Unknown COP0 Opp\n");}



void MFC1() {
	if ( !_Rt_ ) return;
	cpuRegs.GPR.r[_Rt_].UD[0] = (s32)_FsValUl_;
	oppPacket.isTargetUsed = 0; oppPacket.name = "mfc1 (needs work)";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.fr[0] = getFPReg(_Fs_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.fr[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.fr[0]);
}

void CFC1() {
	if ( !_Rt_ || ( _Fs_ != 0 && _Fs_ != 31 ) ) return;
	cpuRegs.GPR.r[_Rt_].UD[0] = (s32)fpuRegs.fprc[_Fs_];
	oppPacket.isTargetUsed = 0; oppPacket.name = "cfc1";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.reg[0] = getGPReg(_Rt_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.fr[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.reg[0],oppPacket.fr[0]);
}

void MTC1() {
	_FsValUl_ = cpuRegs.GPR.r[_Rt_].UL[0];
	oppPacket.isTargetUsed = 0; oppPacket.name = "mtc1";
	oppPacket.reg[0] = getGPReg(_Rt_);
	oppPacket.fr[0] = getFPReg(_Fs_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.fr[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\t\n",oppPacket.name,oppPacket.reg[0],oppPacket.fr[0]);
}

void CTC1() {
	if(_Fs_!=31) return;
	fpuRegs.fprc[_Fs_] = cpuRegs.GPR.r[_Rt_].UL[0];
	oppPacket.isTargetUsed = 0; oppPacket.name = "ctc1";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.reg[0] = getGPReg(_Rt_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.reg[0], oppPacket.fr[0] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.reg[0],oppPacket.fr[0]);
}

#define C_cond_S(cond)                       \
   _ContVal_ = ( _FsValf_ cond _FtValf_ ) ?  \
               ( _ContVal_ | 0x00800000 ) :  \
               ( _ContVal_ & ~0x00800000 );

void C_F()  { _ContVal_ &= ~0x00800000;
	oppPacket.isTargetUsed = 0; oppPacket.name = "c.f.s (clears $f31)";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
} //clears C regardless
void C_EQ() { C_cond_S(==); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "c.eq.s";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
}
void C_LT() { C_cond_S(<); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "c.lt.s (clears $f31)";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
}
void C_LE() { C_cond_S(<=); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "c.le.s (clears $f31)";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
}

#define BC1(cond)                               \
	if ( ( _ContVal_ & 0x00800000 ) cond 0 ) {}
      //intDoBranch( _BranchTarget_ );         

void BC1F() { BC1(==);
	oppPacket.isTargetUsed = 0; oppPacket.name = "bc1f";
	oppPacket.u32Result = _JumpTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  \t%0x needs work\n",oppPacket.name, oppPacket.u32Result);
}
void BC1T() { BC1(!=); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "bc1t";
	oppPacket.u32Result = _JumpTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  \t%0x needs work\n",oppPacket.name, oppPacket.u32Result);
}

#define BC1L(cond)                              \
	if ( ( _ContVal_ & 0x00800000 ) cond 0 ) {}//   \
      //intDoBranch( _BranchTarget_ );            
   //} else cpuRegs.pc += 4;
void BC1FL() { BC1L(==); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "bc1fl";
	oppPacket.u32Result = _JumpTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  \t%0x needs work\n",oppPacket.name, oppPacket.u32Result);
} // Equal to 0
void BC1TL() { BC1L(!=); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "bc1tl";
	oppPacket.u32Result = _JumpTarget_;

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	sprintf(oppPacket.targetString," $%8.8x",oppPacket.target);

	//printf("%s  \t%0x needs work\n",oppPacket.name, oppPacket.u32Result);
} // different from 0


void ADD_S()   { _FdValf_  = _FsValf_ + _FtValf_;
	oppPacket.isTargetUsed = 0; oppPacket.name = "add.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);
	oppPacket.fr[2] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.fr[0], oppPacket.fr[1], oppPacket.fr[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1],oppPacket.fr[2]);
}  
void SUB_S()   { _FdValf_  = _FsValf_ - _FtValf_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "sub.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);
	oppPacket.fr[2] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.fr[0], oppPacket.fr[1], oppPacket.fr[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1],oppPacket.fr[2]);
}  
void MUL_S()   { _FdValf_  = _FsValf_ * _FtValf_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "mul.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);
	oppPacket.fr[2] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.fr[0], oppPacket.fr[1], oppPacket.fr[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1],oppPacket.fr[2]);
}
void MOV_S()   { _FdValf_  = _FsValf_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "mov.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
} 
void NEG_S()   { _FdValf_  = -_FsValf_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "neg.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
} 
void ADDA_S()  { _FAValf_  = _FsValf_ + _FtValf_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "adda.s";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work!!\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
} 
void SUBA_S()  { _FAValf_  = _FsValf_ - _FtValf_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "suba.s";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work!!\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
} 
void MULA_S()  { _FAValf_  = _FsValf_ * _FtValf_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "mula.s";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work!!\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
}
void MADD_S()  { _FdValf_  = _FAValf_ + ( _FsValf_ * _FtValf_ ); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "madd.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);
	oppPacket.fr[2] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.fr[0], oppPacket.fr[1], oppPacket.fr[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work!!\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1],oppPacket.fr[2]);
} 
void MSUB_S()  { _FdValf_  = _FAValf_ - ( _FsValf_ * _FtValf_ ); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "msub.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);
	oppPacket.fr[2] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.fr[0], oppPacket.fr[1], oppPacket.fr[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work!!\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1],oppPacket.fr[2]);
} 
void MADDA_S() { _FAValf_ += _FsValf_ * _FtValf_; 
	oppPacket.isTargetUsed = 0; oppPacket.name = "madda.s";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work!!\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
} 
void MSUBA_S() { _FAValf_ -= _FsValf_ * _FtValf_; 	oppPacket.isTargetUsed = 0; oppPacket.name = "add.s";
	oppPacket.isTargetUsed = 0; oppPacket.name = "msuba.s";
	oppPacket.fr[0] = getFPReg(_Fs_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work!!\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
}
void ABS_S()   {//_FdValf_  = fpufabsf(_FsValf_);
	oppPacket.isTargetUsed = 0; oppPacket.name = "abs.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work!\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
} 
void MAX_S()   { //_FdValf_  = max( _FsValf_, _FtValf_ ); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "max.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);
	oppPacket.fr[2] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.fr[0], oppPacket.fr[1], oppPacket.fr[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1],oppPacket.fr[2]);
}
void MIN_S()   { //_FdValf_  = min( _FsValf_, _FtValf_ ); 
	oppPacket.isTargetUsed = 0; oppPacket.name = "min.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);
	oppPacket.fr[2] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.fr[0], oppPacket.fr[1], oppPacket.fr[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1],oppPacket.fr[2]);
}
void DIV_S()   { if ( _FtValf_ ) { _FdValf_ = _FsValf_ / _FtValf_; } 
	oppPacket.isTargetUsed = 0; oppPacket.name = "div.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);
	oppPacket.fr[2] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s, $%s", oppPacket.fr[0], oppPacket.fr[1], oppPacket.fr[2] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1],oppPacket.fr[2]);
}
void SQRT_S()  { 
	//if ( _FtValf_ >= 0.0f ) { _FdValf_ = fpusqrtf( _FtValf_ ); }
	//else {
	//	_FdValf_ = fpusqrtf(fpufabsf(_FtValf_));
	//}
	oppPacket.isTargetUsed = 0; oppPacket.name = "sqrt.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
} 
void CVT_S()   { _FdValf_ = (float)(s32)_FsValUl_;
	oppPacket.isTargetUsed = 0; oppPacket.name = "cvt.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
}
void CVT_W()   {
	if ( ( _FsValUl_ & 0x7F800000 ) <= 0x4E800000 ) { _FdValUl_ = (s32)(float)_FsValf_; }
	else if ( _FsValUl_ & 0x80000000 ) { _FdValUl_ = 0x80000000; }
	else { _FdValUl_ = 0x7fffffff; }
	oppPacket.isTargetUsed = 0; oppPacket.name = "sqrt.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Fs_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
}

void RSQRT_S() {
	if ( _FtValf_ >= 0.0f ) {
		float tmp;// = fpusqrtf( _FtValf_ );
		if ( tmp != 0 ) { _FdValf_ = _FsValf_ / tmp; }
	}
	oppPacket.isTargetUsed = 0; oppPacket.name = "rsqrt.s";
	oppPacket.fr[0] = getFPReg(_Fd_);
	oppPacket.fr[1] = getFPReg(_Ft_);

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	sprintf(oppPacket.regString, " $%s, $%s", oppPacket.fr[0], oppPacket.fr[1] );
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s  %s, %s\tneeds work\n",oppPacket.name, oppPacket.fr[0],oppPacket.fr[1]);
}
//#pragma endregion
void CACHE(){
	oppPacket.isTargetUsed = 0; oppPacket.name = "cache";

	memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
	memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
	memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
	memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));

	//printf("%s\t\n",oppPacket.name);
}
void LQC2(){}
void SQC2(){}

opPacket execOp(u32 address){
	cpuRegs.pc = address;
	cpuRegs.code = _lw(address);
	oppPacket.address = address;
	oppPacket.data = cpuRegs.code;
	if(oppPacket.data != 0x0){
		Int_OpcodePrintTable[cpuRegs.code >> 26]();
	}else{
		oppPacket.isTargetUsed = 0; oppPacket.name = "nop";
		memset(&oppPacket.immidiateString,0,sizeof(oppPacket.immidiateString));
		memset(&oppPacket.regString, 0, sizeof(oppPacket.regString));
		memset(&oppPacket.loadoffsetString,0,sizeof(oppPacket.loadoffsetString));
		memset(&oppPacket.targetString,0,sizeof(oppPacket.targetString));
	}
	oppPacket.opString = (char*)malloc(sizeof(char)*500);
	oppPacket.opString = printOp();
	return oppPacket;
}

/*int main(void)
{
	cpuRegs.code =  0x3c02002f;
	Int_OpcodePrintTable[cpuRegs.code >> 26]();
	printOp();

	cpuRegs.code =  0x64620001;
	Int_OpcodePrintTable[cpuRegs.code >> 26]();
	printOp();

	cpuRegs.code =  0x44c40800;
	Int_OpcodePrintTable[cpuRegs.code >> 26]();
	printOp();

	cpuRegs.code =  0x2c840001;
	Int_OpcodePrintTable[cpuRegs.code >> 26]();
	printOp();

	cpuRegs.code =  0x20840001;
	Int_OpcodePrintTable[cpuRegs.code >> 26]();
	printOp();

	cpuRegs.code =  0x8d040101;
	Int_OpcodePrintTable[cpuRegs.code >> 26]();
	printOp();

    return 0;
}*/

