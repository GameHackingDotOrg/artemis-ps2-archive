typedef union {   // Declare union type GPR register
	u64 UD[2];      //128 bits
	s64 SD[2];
	u32 UL[4];
	s32 SL[4];
	u16 US[8];
	s16 SS[8];
	u8  UC[16];
	s8  SC[16];
} GPR_reg;

typedef union {
	struct {
		GPR_reg r0, at, v0, v1, a0, a1, a2, a3,
				t0, t1, t2, t3, t4, t5, t6, t7,
				s0, s1, s2, s3, s4, s5, s6, s7,
				t8, t9, k0, k1, gp, sp, s8, ra;
	} n;
	GPR_reg r[32];
} GPRregs;

typedef union {
	struct {
		u32 pccr, pcr0, pcr1, pad;
	} n;
	u32 r[4];
} PERFregs;

typedef union {
	struct {
		u32	Index,    Random,    EntryLo0,  EntryLo1,
			Context,  PageMask,  Wired,     Reserved0,
			BadVAddr, Count,     EntryHi,   Compare;
		union {
			struct {
				int IE:1;
				int EXL:1;
				int ERL:1;
				int KSU:2;
				int unused0:3;
				int IM:8;
				int EIE:1;
				int _EDI:1;
				int CH:1;
				int unused1:3;
				int BEV:1;
				int DEV:1;
				int unused2:2;
				int FR:1;
				int unused3:1;
				int CU:4;
			} b;
			u32 val;
		} Status;
		u32   Cause,    EPC,       PRid,
			Config,   LLAddr,    WatchLO,   WatchHI,
			XContext, Reserved1, Reserved2, Debug,
			DEPC,     PerfCnt,   ErrCtl,    CacheErr,
			TagLo,    TagHi,     ErrorEPC,  DESAVE;
	} n;
	u32 r[32];
} CP0regs;

typedef struct {
    GPRregs GPR;		// GPR regs
	// NOTE: don't change order since recompiler uses it
	GPR_reg HI;
	GPR_reg LO;			// hi & log 128bit wide
	CP0regs CP0;		// is COP0 32bit?
	u32 sa;				// shift amount (32bit), needs to be 16 byte aligned
	u32 constzero;		// always 0, for MFSA
    u32 pc;				// Program counter
    u32 code;			// The instruction
    PERFregs PERF;
	u32 eCycle[32];
	u32 sCycle[32];		// for internal counters
	u32 cycle;			// calculate cpucycles..
	u32 interrupt;
	int branch;
	int opmode;			// operating mode
	u32 tempcycles;	
} cpuRegisters;

int EEsCycle;
u32 EEoCycle, IOPoCycle;
//extern cpuRegisters cpuRegs;
// used for optimization
typedef union {
	u64 UD[1];      //64 bits
	s64 SD[1];
	u32 UL[2];
	s32 SL[3];
	u16 US[4];
	s16 SS[4];
	u8  UC[8];
	s8  SC[8];
} GPR_reg64;

#define GPR_IS_CONST1(reg) ((reg)<32 && (g_cpuHasConstReg&(1<<(reg))))
#define GPR_IS_CONST2(reg1, reg2) ((g_cpuHasConstReg&(1<<(reg1)))&&(g_cpuHasConstReg&(1<<(reg2))))
#define GPR_SET_CONST(reg) { \
	if( (reg) < 32 ) { \
		g_cpuHasConstReg |= (1<<(reg)); \
		g_cpuFlushedConstReg &= ~(1<<(reg)); \
	} \
}

#define GPR_DEL_CONST(reg) { \
	if( (reg) < 32 ) g_cpuHasConstReg &= ~(1<<(reg)); \
}

GPR_reg64 g_cpuConstRegs[32];
u32 g_cpuHasConstReg, g_cpuFlushedConstReg;

typedef union {
	float f;
	u32 UL;
} FPRreg;

typedef struct {
	FPRreg fpr[32];		// 32bit floating point registers
	u32 fprc[32];		// 32bit floating point control registers
	FPRreg ACC;			// 32 bit accumulator 
} fpuRegisters;
fpuRegisters fpuRegs;


typedef struct {
	u32 PageMask,EntryHi;
	u32 EntryLo0,EntryLo1;
	u32 Mask, nMask;
	u32 G;
	u32 ASID;
	u32 VPN2;
	u32 PFN0;
	u32 PFN1;
} tlbs;

tlbs tlb[48];

#ifndef _PC_

#define _i64(x) (s64)x
#define _u64(x) (u64)x

#define _i32(x) (s32)x
#define _u32(x) (u32)x

#define _i16(x) (s16)x
#define _u16(x) (u16)x

#define _i8(x) (s8)x
#define _u8(x) (u8)x

/**** R3000A Instruction Macros ****/
#define _PC_       cpuRegs.pc       // The next PC to be executed

#define _Funct_  ((cpuRegs.code      ) & 0x3F)  // The funct part of the instruction register 
#define _Rd_     ((cpuRegs.code >> 11) & 0x1F)  // The rd part of the instruction register 
#define _Rt_     ((cpuRegs.code >> 16) & 0x1F)  // The rt part of the instruction register 
#define _Rs_     ((cpuRegs.code >> 21) & 0x1F)  // The rs part of the instruction register 
#define _Sa_     ((cpuRegs.code >>  6) & 0x1F)  // The sa part of the instruction register
#define _Im_     ((u16)cpuRegs.code)			// The immediate part of the instruction register
#define _Target_ (cpuRegs.code & 0x03ffffff)    // The target part of the instruction register

#define _Imm_	((s16)cpuRegs.code) // sign-extended immediate
#define _ImmU_	(cpuRegs.code&0xffff) // zero-extended immediate


//#define _JumpTarget_     ((_Target_ * 4) + (_PC_ & 0xf0000000))   // Calculates the target during a jump instruction
//#define _BranchTarget_  ((s16)_Im_ * 4 + _PC_)                 // Calculates the target during a branch instruction

#define _JumpTarget_     ((_Target_ << 2) + (_PC_ & 0xf0000000))   // Calculates the target during a jump instruction
#define _BranchTarget_  (((s32)(s16)_Im_ * 4) + _PC_)                 // Calculates the target during a branch instruction

#define _SetLink(x)     cpuRegs.GPR.r[x].UD[0] = _PC_ + 4;       // Sets the return address in the link register

#endif

//void (*Int_OpcodePrintTable[64])();
//void (*Int_SpecialPrintTable[64])();
//void (*Int_REGIMMPrintTable[32])();
//void (*Int_MMIPrintTable[64])();
//void (*Int_MMI0PrintTable[32])();
//void (*Int_MMI1PrintTable[32])();
//void (*Int_MMI2PrintTable[32])();
//void (*Int_MMI3PrintTable[32])();
//void (*Int_COP0PrintTable[32])();
//void (*Int_COP0BC0PrintTable[32])();
//void (*Int_COP0C0PrintTable[64])();
//void (*Int_COP1PrintTable[32])();
//void (*Int_COP1BC1PrintTable[32])();
//void (*Int_COP1SPrintTable[64])();
//void (*Int_COP1WPrintTable[64])();
//void (*Int_COP2PrintTable[32])();
//void (*Int_COP2BC2PrintTable[32])();
//void (*Int_COP2SPECIAL1PrintTable[64])();
//void (*Int_COP2SPECIAL2PrintTable[128])();

void SPECIAL();
void REGIMM();
void UnknownOpcode();
void COP0();
void COP1();
void COP2();
void MMI_Unknown();
void MMI();
void MMI0();
void MMI1();
void MMI2();
void MMI3();
void COP0_Unknown();
void COP0_BC0();
void COP0_Func(); 
void COP1_BC1();
void COP1_S();
void COP1_W();
void COP1_Unknown();
void COP2_BC2();
void COP2_SPECIAL();
void COP2_Unknown();
void COP2_SPECIAL2();

// **********************Standard Opcodes**************************
void J();
void JAL();
void BEQ();
void BNE();
void BLEZ();
void BGTZ();
void ADDI();
void ADDIU();
void SLTI();
void SLTIU();
void ANDI();
void ORI();
void XORI();
void LUI();
void BEQL();
void BNEL();
void BLEZL();
void BGTZL();
void DADDI();
void DADDIU();
void LDL();
void LDR();
void LB();
void LH();
void LWL();
void LW();
void LBU();
void LHU();
void LWR();
void LWU();
void SB();
void SH();
void SWL();
void SW();
void SDL();
void SDR();
void SWR();
void CACHE();
void LWC1();
void PREF();
void LQC2();
void LD();
void SQC2();
void SD();
void LQ();
void SQ();
void SWC1();
//***************end of standard opcodes*************************


//***************SPECIAL OPCODES**********************************
void SLL();
void SRL();
void SRA();
void SLLV();
void SRLV();
void SRAV();
void JR();
void JALR();
void SYSCALL();
void BREAK();
void SYNC();
void MFHI();
void MTHI();
void MFLO();
void MTLO();
void DSLLV();
void DSRLV();
void DSRAV();
void MULT();
void MULTU();
void DIV();
void DIVU();
void ADD();
void ADDU();
void SUB();
void SUBU();
void AND();
void OR();
void XOR();
void NOR();
void SLT();
void SLTU();
void DADD();
void DADDU();
void DSUB();
void DSUBU();
void TGE();
void TGEU();
void TLT();
void TLTU();
void TEQ();
void TNE();
void DSLL();
void DSRL();
void DSRA();
void DSLL32();
void DSRL32();
void DSRA32();
void MOVZ();
void MOVN();
void MFSA();
void MTSA();
//******************END OF SPECIAL OPCODES**************************

//******************REGIMM OPCODES**********************************
void BLTZ();
void BGEZ();
void BLTZL();
void BGEZL();
void TGEI();
void TGEIU();
void TLTI();
void TLTIU();
void TEQI();
void TNEI();
void BLTZAL();
void BGEZAL();
void BLTZALL();
void BGEZALL();
void MTSAB();
void MTSAH();
//*****************END OF REGIMM OPCODES*****************************
//*****************MMI OPCODES*********************************
void MADD();
void MADDU();
void PLZCW();
void MADD1();
void MADDU1();
void MFHI1();
void MTHI1();
void MFLO1();
void MTLO1();
void MULT1();
void MULTU1();
void DIV1();
void DIVU1();
void PMFHL();
void PMTHL();
void PSLLH();
void PSRLH();
void PSRAH();
void PSLLW();
void PSRLW();
void PSRAW();
//*****************END OF MMI OPCODES**************************
//*************************MMI0 OPCODES************************

void PADDW();  
void PSUBW();  
void PCGTW();  
void PMAXW(); 
void PADDH();  
void PSUBH();  
void PCGTH();  
void PMAXH(); 
void PADDB();  
void PSUBB();  
void PCGTB();
void PADDSW(); 
void PSUBSW(); 
void PEXTLW();  
void PPACW(); 
void PADDSH();
void PSUBSH();
void PEXTLH(); 
void PPACH();
void PADDSB();
void PSUBSB();
void PEXTLB(); 
void PPACB();
void PEXT5(); 
void PPAC5();
//***END OF MMI0 OPCODES******************************************
//**********MMI1 OPCODES**************************************
void PABSW();
void PCEQW();
void PMINW(); 
void PADSBH();
void PABSH();
void PCEQH();
void PMINH();  
void PCEQB(); 
void PADDUW();
void PSUBUW(); 
void PEXTUW();  
void PADDUH();
void PSUBUH(); 
void PEXTUH(); 
void PADDUB();
void PSUBUB();
void PEXTUB();
void QFSRV(); 
//********END OF MMI1 OPCODES***********************************
//*********MMI2 OPCODES***************************************
void PMADDW();
void PSLLVW();
void PSRLVW(); 
void PMSUBW();
void PMFHI();
void PMFLO();
void PINTH();
void PMULTW();
void PDIVW();
void PCPYLD();
void PMADDH();
void PHMADH();
void PAND();
void PXOR(); 
void PMSUBH();
void PHMSBH();
void PEXEH();
void PREVH(); 
void PMULTH();
void PDIVBW();
void PEXEW();
void PROT3W();
//*****END OF MMI2 OPCODES***********************************
//*************************MMI3 OPCODES************************
void PMADDUW();
void PSRAVW(); 
void PMTHI();
void PMTLO();
void PINTEH(); 
void PMULTUW();
void PDIVUW();
void PCPYUD(); 
void POR();
void PNOR();  
void PEXCH();
void PCPYH(); 
void PEXCW();
//**********************END OF MMI3 OPCODES******************** 
//****************************************************************************
//** COP0                                                                   **
//****************************************************************************
void MFC0();
void MTC0();
void BC0F();
void BC0T();
void BC0FL();
void BC0TL();
void TLBR();
void TLBWI();
void TLBWR();
void TLBP();
void ERET();
void _DI();
void _EI();
//****************************************************************************
//** END OF COP0                                                            **
//****************************************************************************
//****************************************************************************
//** COP1 - Floating Point Unit (FPU)                                       **
//****************************************************************************
void MFC1();
void CFC1();
void MTC1();
void CTC1();
void BC1F();
void BC1T();
void BC1FL();
void BC1TL();
void ADD_S();  
void SUB_S();  
void MUL_S();  
void DIV_S();  
void SQRT_S(); 
void ABS_S();  
void MOV_S(); 
void NEG_S(); 
void RSQRT_S();  
void ADDA_S(); 
void SUBA_S(); 
void MULA_S();
void MADD_S(); 
void MSUB_S(); 
void MADDA_S(); 
void MSUBA_S();
void CVT_W();
void MAX_S();
void MIN_S();
void C_F();
void C_EQ();
void C_LT();
void C_LE();
 void CVT_S();
//****************************************************************************
//** END OF COP1                                                            **
//****************************************************************************
//****************************************************************************
//** COP2 - (VU0)                                                           **
//****************************************************************************
void QMFC2(); 
void CFC2(); 
void QMTC2();
void CTC2();  
void BC2F();
void BC2T();
void BC2FL();
void BC2TL();
//*****************SPECIAL 1 VUO TABLE*******************************
void VADDx();       
void VADDy();       
void VADDz();       
void VADDw();       
void VSUBx();        
void VSUBy();        
void VSUBz();
void VSUBw(); 
void VMADDx();
void VMADDy();
void VMADDz();
void VMADDw();
void VMSUBx();
void VMSUBy();
void VMSUBz();       
void VMSUBw(); 
void VMAXx();       
void VMAXy();       
void VMAXz();       
void VMAXw();       
void VMINIx();       
void VMINIy();       
void VMINIz();       
void VMINIw(); 
void VMULx();       
void VMULy();       
void VMULz();       
void VMULw();       
void VMULq();        
void VMAXi();        
void VMULi();        
void VMINIi();
void VADDq();
void VMADDq();      
void VADDi();       
void VMADDi();      
void VSUBq();        
void VMSUBq();       
void VSUBi();        
void VMSUBi(); 
void VADD();        
void VMADD();       
void VMUL();        
void VMAX();        
void VSUB();         
void VMSUB();       
void VOPMSUB();      
void VMINI();  
void VIADD();       
void VISUB();       
void VIADDI();       
void VIAND();        
void VIOR();        
void VCALLMS();     
void VCALLMSR();   
//***********************************END OF SPECIAL1 VU0 TABLE*****************************
//******************************SPECIAL2 VUO TABLE*****************************************
void VADDAx();      
void VADDAy();      
void VADDAz();      
void VADDAw();      
void VSUBAx();      
void VSUBAy();      
void VSUBAz();      
void VSUBAw();
void VMADDAx();     
void VMADDAy();     
void VMADDAz();     
void VMADDAw();     
void VMSUBAx();     
void VMSUBAy();     
void VMSUBAz();     
void VMSUBAw();
void VITOF0();      
void VITOF4();      
void VITOF12();     
void VITOF15();     
void VFTOI0();      
void VFTOI4();      
void VFTOI12();     
void VFTOI15();
void VMULAx();      
void VMULAy();      
void VMULAz();      
void VMULAw();      
void VMULAq();      
void VABS();        
void VMULAi();      
void VCLIPw();
void VADDAq();      
void VMADDAq();     
void VADDAi();      
void VMADDAi();     
void VSUBAq();      
void VMSUBAq();     
void VSUBAi();      
void VMSUBAi();
void VADDA();       
void VMADDA();      
void VMULA();       
void VSUBA();       
void VMSUBA();      
void VOPMULA();     
void VNOP();   
void VMOVE();       
void VMR32();       
void VLQI();        
void VSQI();        
void VLQD();        
void VSQD();   
void VDIV();        
void VSQRT();       
void VRSQRT();      
void VWAITQ();     
void VMTIR();       
void VMFIR();       
void VILWR();       
void VISWR();  
void VRNEXT();      
void VRGET();       
void VRINIT();      
void VRXOR();  

typedef struct {
	int isImmUsed;			//is immidiate used
	int isLoadOffsetUsed;	//is load offset used for store/load insrtuctions
	int isTargetUsed;		//is target used for branch/jump instructions

	u32 address;	//address of op
	u32 data;		//data portion
	char *name;		//name of the op
	char *reg[3];	//general purpose register string names
	char *fr[3];	//float register string names

	u16 imm;		//Immidiate value		eg slti a0, a0, $imm
	u16 loadOffset;	//offset value			eg lw a0, $offset($a0)
	u32 target;		//branches and jumps	eg beq a0, a0, $target
	u32 u32Result;	//u32 sized result

	char regString[20];
	char immidiateString[9];
	char loadoffsetString[12];
	char targetString[9];

} opPacket;

//string format
//ADDRESS : DATA -> OP
//ADDRESS : DATA -> OP	reg, reg, reg
//ADDRESS : DATA -> OP	reg, reg, immidiate
//ADDRESS : DATA -> OP	reg, reg
//ADDRESS : DATA -> OP	reg, immidiate
//ADDRESS : DATA -> OP	immidiate
//ADDRESS : DATA -> OP	reg, immidiate(reg)
//ADDRESS : DATA -> OP	reg, reg, branch target
//ADDRESS : DATA -> OP	reg, branch target
//ADDRESS : DATA -> OP	branch target
//ADDRESS : DATA -> OP	jump target

//"%8.8x : %8.8x -> %s \t %s  %s  %s  Result: %8.8x"
opPacket execOp(u32 address);