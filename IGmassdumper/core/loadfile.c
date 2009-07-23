
#include "core.h"

static SifRpcClientData_t _lf_cd;
static int _lf_init  = 0;

// ------------------------------------------------------------------------
int LoadFileInit()
{
	int r;

	if (_lf_init)
		return 0;

	while ((r = SifBindRpc(&_lf_cd, 0x80000006, 0)) >= 0 && (!_lf_cd.server))
		nopdelay();

	if (r < 0)
		return -E_SIF_RPC_BIND;

	_lf_init = 1;

	return 0;
}

// ------------------------------------------------------------------------
void LoadFileExit()
{
	_lf_init = 0;
	memset(&_lf_cd, 0, sizeof(_lf_cd));
}

// ------------------------------------------------------------------------
int LoadModuleAsync(const char *path, int arg_len, const char *args)
{
	struct _lf_module_load_arg arg;

	if (LoadFileInit() < 0)
		return -E_LIB_API_INIT;

	memset(&arg, 0, sizeof arg);

	strncpy(arg.path, path, LF_PATH_MAX - 1);
	arg.path[LF_PATH_MAX - 1] = 0;

	if ((args) && (arg_len)) {
		arg.p.arg_len = arg_len > LF_ARG_MAX ? LF_ARG_MAX : arg_len;
		memcpy(arg.args, args, arg.p.arg_len);
	}
	else arg.p.arg_len = 0;

	if (SifCallRpc(&_lf_cd, LF_F_MOD_LOAD, SIF_RPC_M_NOWAIT, &arg, sizeof(arg), &arg, 8, NULL, NULL) < 0)
		return -E_SIF_RPC_CALL;

	return 0;
}

// ------------------------------------------------------------------------
int LoadModuleBuffer(void *modptr, unsigned int modsize, int arg_len, const char *args)
{
	SifDmaTransfer_t sifdma;
	void            *iopmem;
	int              dma_id;

	if (LoadFileInit() < 0)
		return -E_LIB_API_INIT;

	/* Round the size up to the nearest 16 bytes. */
	// modsize = (modsize + 15) & -16;

	iopmem = SifAllocIopHeap(modsize);
	if (iopmem == NULL) return -E_IOP_NO_MEMORY;
 
	sifdma.src  = modptr;
	sifdma.dest = iopmem;
	sifdma.size = modsize;
	sifdma.attr = 0;

	SifWriteBackDCache(modptr, modsize);

	do {
		dma_id = SifSetDma(&sifdma, 1);
	} while (!dma_id);

	while (SifDmaStat(dma_id) > 0) {;}
 
	struct _lf_module_buffer_load_arg arg;

	memset(&arg, 0, sizeof arg);

	arg.p.ptr = iopmem;
	if ((args) && (arg_len)) {
		arg.q.arg_len = arg_len > LF_ARG_MAX ? LF_ARG_MAX : arg_len;
		memcpy(arg.args, args, arg.q.arg_len);
	}
	else arg.q.arg_len = 0;

	if (SifCallRpc(&_lf_cd, LF_F_MOD_BUF_LOAD, 0, &arg, sizeof(arg), &arg, 8, NULL, NULL) < 0)
		return -E_SIF_RPC_CALL;

	SifFreeIopHeap(iopmem); 

	return arg.p.result;
}

// ------------------------------------------------------------------------
int LoadElf(const char *path, t_ExecData *data)
{
	struct _lf_elf_load_arg arg;

	if (LoadFileInit() < 0)
		return -E_LIB_API_INIT;

	u32 secname = 0x6c6c61;  // "all" 

	strncpy(arg.path, path, LF_PATH_MAX - 1);
	strncpy(arg.secname, (char*)&secname, LF_ARG_MAX - 1);
	arg.path[LF_PATH_MAX - 1] = 0;
	arg.secname[LF_ARG_MAX - 1] = 0;

	if (SifCallRpc(&_lf_cd, LF_F_ELF_LOAD, 0, &arg, sizeof arg, &arg, sizeof(t_ExecData), NULL, NULL) < 0)
		return -E_SIF_RPC_CALL;

	if (arg.p.result < 0)
		return arg.p.result;

	if (data) {
		data->epc = arg.p.epc;
		data->gp  = arg.gp;
	}

	return 0;
}
