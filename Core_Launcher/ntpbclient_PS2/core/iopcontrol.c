
#include "core.h"

// ------------------------------------------------------------------------
int IopReset(const char *arg, int flag)
{
	SifCmdResetData reset_pkt;
	struct t_SifDmaTransfer dmat;

	SifExitRpc();

	SifStopDma();

	memset(&reset_pkt, 0, sizeof(SifCmdResetData));

	reset_pkt.chdr.psize = sizeof(SifCmdResetData);
	reset_pkt.chdr.fcode = 0x80000003;

	reset_pkt.flag = flag;

	if (arg != NULL) {
		strncpy(reset_pkt.arg, arg, 0x50);
		reset_pkt.arg[0x50] = '\0';
		reset_pkt.size = strlen(reset_pkt.arg) + 1;
	}

	dmat.src  = &reset_pkt;
	dmat.dest = (void *)SifGetReg(0x80000000); /* SIF_REG_SUBADDR */
	dmat.size = sizeof reset_pkt;
	dmat.attr = 0x40 | SIF_DMA_INT_O;
	SifWriteBackDCache(&reset_pkt, sizeof reset_pkt);

	DIntr();
	ee_kmode_enter();
	if (!Old_SifSetDma(&dmat, 1)) {
		ee_kmode_exit();
		EIntr();
		return 0;
	}

	Old_SifSetReg(SIF_REG_SMFLAG, 0x10000);
	Old_SifSetReg(SIF_REG_SMFLAG, 0x20000);
	Old_SifSetReg(0x80000002, 0);
	Old_SifSetReg(0x80000000, 0);
	ee_kmode_exit();
	EIntr();

	return 1;
}

// ------------------------------------------------------------------------
int IopSync(void)
{
	if (SifGetReg(SIF_REG_SMFLAG) & 0x40000) {
		return 1;
	}
	return 0;
}
