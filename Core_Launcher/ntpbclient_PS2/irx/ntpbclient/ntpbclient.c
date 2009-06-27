
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
#include <ps2ip.h>

#define MODNAME "NTPB_Client"
IRX_ID(MODNAME, 1, 0);

#define ntpb_MagicSize  6
#define ntpb_hdrSize  	10
// NTPB header magic
const u8 *ntpb_hdrmagic = "\xff\x00NTPB";

int ntpbclient_io_sema;
static int rpc_tidS_0A10;

static SifRpcDataQueue_t rpc_qdS_0A10 __attribute__((aligned(64)));
static SifRpcServerData_t rpc_sdS_0A10 __attribute__((aligned(64)));

static u8 ntpbclient_rpc_buf[16384] __attribute__((aligned(64)));

static struct {		
	int rpc_func_ret;
	int ntpbclient_version;
} rpc_stat __attribute__((aligned(64)));

int (*rpc_func)(void);

void dummy(void) {}
int  rpcNTPBconnect(void);
int _rpcNTPBconnect(void *rpc_buf);
int  rpcNTPBdisconnect(void);
int _rpcNTPBdisconnect(void *rpc_buf);
int  rpcNTPBsendCmd(void);
int _rpcNTPBsendCmd(void *rpc_buf);

// rpc command handling array
void *rpc_funcs_array[16] = {
    (void *)dummy,
    (void *)rpcNTPBconnect,
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
    (void *)dummy,
    (void *)rpcNTPBsendCmd,
    (void *)rpcNTPBdisconnect    
};

typedef struct g_connectParam { // size = 16
	char ServerIP[1024];		// 0
	int ServerPort;				// 4
	u32 pad1;					// 8
	u32 pad2;					// 12
} g_connectParam_t;

typedef struct g_sendCmdParam { // size = 16384
	u16 cmd;				 //	0
	u8 buf[16378];		 	 // 2
	int size;				 // 16380
} g_sendCmdParam_t;

static int main_socket;
static u8 ntpb_buf[16384];


//-------------------------------------------------------------- 
int IOPmem_read(void *IOPaddr, int size, void *EEdest)
{
	SifDmaTransfer_t dmat;
	int oldstate, id;
	void *EEaddr;
	
	size += 3;
	size &= 0xfffffffc;		
	
	EEaddr = (void *)((u32)(EEdest) & 0x1fffffff);
	
	WaitSema(ntpbclient_io_sema);	
	
	dmat.dest = (void *)EEaddr;
	dmat.size = size;
	dmat.src = (void *)IOPaddr;
	dmat.attr = 0;
		
	while (1) {
		CpuSuspendIntr(&oldstate);
		id = sceSifSetDma(&dmat, 1);
		CpuResumeIntr(oldstate);
		if (id)
			break;
		DelayThread(500);
	}
	
	while (sceSifDmaStat(id) >= 0);	

	SignalSema(ntpbclient_io_sema);	
	
	return 0;
}

//-------------------------------------------------------------------------
int OpenTCPSession(struct in_addr dst_IP, u16 dst_port)
{
	int sock;
	int ret, retries;
	struct sockaddr_in sock_addr;
	
	// Creating socket
	sock = lwip_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
		return -1;

    memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_addr = dst_IP;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(dst_port);
		 	
	// 3 attemps to connect
	for (retries = 0; retries < 3; retries++) {
		ret = lwip_connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
		if (ret >= 0)
			break;
		DelayThread(100);	 
	}
	if (retries >= 3)
		return -2;
		
	return sock;
}

//-------------------------------------------------------------- 
int NTPBClientConnect(char *ServerIP, int ServerPort)
{
	struct in_addr dst_addr;
	int retries;
	
	// server IP
	dst_addr.s_addr = inet_addr(ServerIP);

	WaitSema(ntpbclient_io_sema);	

	retries = 0;
	
conn_open:
			
	main_socket = OpenTCPSession(dst_addr, ServerPort);	
	if (main_socket < 0) {
		retries++;
		if (retries < 10)
			goto conn_open;		
	}
	
	memset(ntpb_buf, sizeof(ntpb_buf), 0);
	
	SignalSema(ntpbclient_io_sema);
	
	// connection should be established
	return 0;						
}

//-------------------------------------------------------------- 
int NTPBClientDisconnect(void)
{
	WaitSema(ntpbclient_io_sema);	
	
	// Close the socket
	lwip_close(main_socket);

	SignalSema(ntpbclient_io_sema);
	
	return 0;
}

//-------------------------------------------------------------- 
int check_ntpb_header(void *buf) // sanity check to see if the packet have the format we expect
{
	int i;
	u8 *pbuf = (u8 *)buf;

	for (i=0; i<ntpb_MagicSize; i++) {
		if (pbuf[i] != ntpb_hdrmagic[i])
			break;
	}

	if (i == ntpb_MagicSize)
		return 1;

	return 0;
}

//-------------------------------------------------------------- 
int NTPBClientSendCmd(u16 cmd, u8 *buf, int size)
{
	int sz,	servRet;

	WaitSema(ntpbclient_io_sema);	
	
	memcpy(&ntpb_buf[0], ntpb_hdrmagic, ntpb_MagicSize); //copying NTPB Magic
	*((u16 *)&ntpb_buf[ntpb_MagicSize]) = size;		
	*((u16 *)&ntpb_buf[ntpb_MagicSize+2]) = cmd;		
	
	if (buf)
		memcpy(&ntpb_buf[ntpb_hdrSize], buf, size); 
			
	sz = lwip_send(main_socket, ntpb_buf, size+ntpb_hdrSize, 0); 
	if (sz <= 0)
		return -1;

	/// No fragmentation handled at the moment !!! ///
	sz = lwip_recv(main_socket, ntpb_buf, sizeof(ntpb_buf), 0); 
	if (sz <= 0)
		return -1;
	
	// header sanity check
	if (!check_ntpb_header(ntpb_buf))
		return -2;
		
	// cmd sanity check	
	if (*((u16 *)&ntpb_buf[ntpb_MagicSize+2]) != cmd)
		return -3;					

	// server reply sanity check	
	if (*((u16 *)&ntpb_buf[ntpb_hdrSize]) != 1)
		return -4;					
			
	servRet = (int)(*((u16 *)&ntpb_buf[ntpb_hdrSize+2]));
	servRet |= (int)((*((u16 *)&ntpb_buf[ntpb_hdrSize+4])) << 16);
	
	SignalSema(ntpbclient_io_sema);
		
	return servRet;
}

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
	sceSifRegisterRpc(&rpc_sdS_0A10, 0x80000a10, (void *)cb_rpc_S_0A10, &ntpbclient_rpc_buf, NULL, NULL, &rpc_qdS_0A10);
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
int rpcNTPBconnect(void)
{
	return _rpcNTPBconnect(&ntpbclient_rpc_buf);
}

//-------------------------------------------------------------- 
int rpcNTPBsendCmd(void)
{
	return _rpcNTPBsendCmd(&ntpbclient_rpc_buf);
}
//-------------------------------------------------------------- 
int rpcNTPBdisconnect(void)
{
	return _rpcNTPBdisconnect(&ntpbclient_rpc_buf);
}

//-------------------------------------------------------------- 
int _rpcNTPBconnect(void *rpc_buf)
{
	g_connectParam_t *cP = (g_connectParam_t *)rpc_buf;	
	int r;
		
	r = NTPBClientConnect(cP->ServerIP, cP->ServerPort);
	
	return r;
}

//-------------------------------------------------------------- 
int _rpcNTPBsendCmd(void *rpc_buf)
{
	int r;
	g_sendCmdParam_t *eP = (g_sendCmdParam_t *)rpc_buf;	
				
	r = NTPBClientSendCmd(eP->cmd, eP->buf, eP->size);
	
	return r;
}

//-------------------------------------------------------------- 
int _rpcNTPBdisconnect(void *rpc_buf)
{
	int r;
		
	r = NTPBClientDisconnect();
			
	return r;
}

//-------------------------------------------------------------------------
int _start(int argc, char** argv)
{			
	iop_sema_t smp;
			
	SifInitRpc(0);
	
	// Starting Remote Procedure Call server	
	start_RPC_server();		

	smp.attr = 1;
	smp.initial = 1;
	smp.max = 1;
	smp.option = 0;
	ntpbclient_io_sema = CreateSema(&smp);	
				
	return MODULE_RESIDENT_END;
}
