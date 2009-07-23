
#include <loadcore.h>
#include <stdio.h>
#include <sysclib.h>
#include <sysmem.h>
#include <thbase.h>
#include <thevent.h>
#include <intrman.h>
#include <sifcmd.h>
#include <sifman.h>
#include <thsemap.h>
#include <errno.h>
#include <io_common.h>
#include <ioman.h>


#define MODNAME "usbserv"
IRX_ID(MODNAME, 1, 0);

int usbserv_io_sema; 	// IO semaphore

static int rpc_tidS_0A10;

static SifRpcDataQueue_t rpc_qdS_0A10 __attribute__((aligned(64)));
static SifRpcServerData_t rpc_sdS_0A10 __attribute__((aligned(64)));

static u8 usbserv_rpc_buf[16384] __attribute__((aligned(64)));

static struct {		
	int rpc_func_ret;
	int usbserv_version;
} rpc_stat __attribute__((aligned(64)));

int (*rpc_func)(void);

void dummy(void) {}
int  rpcUSBopen(void);
int _rpcUSBopen(void *rpc_buf);
int  rpcUSBwrite(void);
int _rpcUSBwrite(void *rpc_buf);
int  rpcUSBclose(void);
int _rpcUSBclose(void *rpc_buf);

// rpc command handling array
void *rpc_funcs_array[16] = {
    (void *)dummy,
    (void *)rpcUSBopen,
    (void *)rpcUSBwrite,
    (void *)rpcUSBclose,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy,
    (void *)dummy
};

typedef struct g_openParam {  // size = 1024
	char filename[1024];	  // 0
} g_openParam_t;

typedef struct g_writeParam { // size =
	int fd;					  // 0
	u8 buf[16376];		 	  // 4
	int size;				  // 
	u8 pad[8];
} g_writeParam_t;

typedef struct g_closeParam { // size = 16
	int fd;	  				  // 0
	u8 pad[12];
} g_closeParam_t;

char filepath[1056];

//--------------------------------------------------------------
void *cb_rpc_S_0A10(u32 fno, void *buf, int size)
{
	// Rpc Callback function
	
	if (fno >= 16)
		return (void *)&rpc_stat;

	// Get function pointer
	rpc_func = (void *)rpc_funcs_array[fno];
	
	// Call needed rpc func
	rpc_stat.rpc_func_ret = rpc_func();
	
	return (void *)&rpc_stat;
}

//--------------------------------------------------------------
void thread_rpc_S_0A10(void* arg)
{
	if (!sceSifCheckInit())
		sceSifInit();

	sceSifInitRpc(0);
	sceSifSetRpcQueue(&rpc_qdS_0A10, GetThreadId());
	sceSifRegisterRpc(&rpc_sdS_0A10, 0x80000a10, (void *)cb_rpc_S_0A10, &usbserv_rpc_buf, NULL, NULL, &rpc_qdS_0A10);
	sceSifRpcLoop(&rpc_qdS_0A10);
}

//-------------------------------------------------------------- 
int start_RPC_server(void)
{
	iop_thread_t thread_param;
	register int thread_id;	
			
 	thread_param.attr = TH_C;
 	thread_param.thread = (void *)thread_rpc_S_0A10;
 	thread_param.priority = 0x68;
 	thread_param.stacksize = 0x1000;
 	thread_param.option = 0;
			
	thread_id = CreateThread(&thread_param);
	rpc_tidS_0A10 = thread_id;
		
	StartThread(thread_id, 0);
	
	return 0;
}

//-------------------------------------------------------------- 
int rpcUSBopen(void)
{
	return _rpcUSBopen(&usbserv_rpc_buf);
}

//-------------------------------------------------------------- 
int _rpcUSBopen(void *rpc_buf)
{
	int fd;
	g_openParam_t *eP = (g_openParam_t *)rpc_buf;	

	WaitSema(usbserv_io_sema);
	
	sprintf(filepath, "mass:%s", eP->filename);					
	fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC);	
	
	SignalSema(usbserv_io_sema);
		
	return fd;
}

//-------------------------------------------------------------- 
int rpcUSBwrite(void)
{
	return _rpcUSBwrite(&usbserv_rpc_buf);
}

//-------------------------------------------------------------- 
int _rpcUSBwrite(void *rpc_buf)
{
	int r;
	g_writeParam_t *eP = (g_writeParam_t *)rpc_buf;	
			
	WaitSema(usbserv_io_sema);
	
	r = write(eP->fd, eP->buf, eP->size);
	
	SignalSema(usbserv_io_sema);
	
	return r;
}

//-------------------------------------------------------------- 
int rpcUSBclose(void)
{
	return _rpcUSBclose(&usbserv_rpc_buf);
}

//-------------------------------------------------------------- 
int _rpcUSBclose(void *rpc_buf)
{
	int r;
	g_closeParam_t *eP = (g_closeParam_t *)rpc_buf;	

	WaitSema(usbserv_io_sema);
					
	r = close(eP->fd);
	
	SignalSema(usbserv_io_sema);
	
	return r;
}


//-------------------------------------------------------------------------
int _start(int argc, char** argv)
{			
	iop_sema_t smp;
				
	SifInitRpc(0);
			
	// Starting usbserv Remote Procedure Call server	
	start_RPC_server();
	
	smp.attr = 1;
	smp.initial = 1;
	smp.max = 1;
	smp.option = 0;
	usbserv_io_sema = CreateSema(&smp);	
						
	return MODULE_RESIDENT_END;
}
