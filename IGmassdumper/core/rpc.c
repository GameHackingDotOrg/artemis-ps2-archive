
#include "core.h"

#define CMD_NONE  			0x00
#define CMD_USBOPEN		  	0x01
#define CMD_USBWRITE		0x02
#define CMD_USBCLOSE		0x03

static SifRpcClientData_t rpcclient __attribute__((aligned(64)));
static int Rpc_Buffer[16] 			__attribute__((aligned(64)));

static struct { 			// size = 1024
	char filename[1024];	// 0
} openParam __attribute__((aligned(64)));

static struct { 		// size =
	int fd;				// 0
	u8 buf[16376];		// 4
	int size;			// 
} writeParam __attribute__((aligned(64)));

static struct { 		// size = 16
	int fd;				// 0
	u8 pad[12];
} closeParam __attribute__((aligned(64)));


// stores command currently being executed on the iop
static unsigned int currentCmd = 0;

static int RPCclient_Inited = 0;

//--------------------------------------------------------------
int rpcUSBInit(void)
{
	int ret = 0;

	if (RPCclient_Inited)
		return -1;

	SifInitRpc(0);
		
	// bind to rpc on iop
	do {
		if ((ret = SifBindRpc(&rpcclient, 0x80000a10, 0)) < 0)
			return ret;
			
		if (rpcclient.server == NULL)
			nopdelay();
			
	} while (rpcclient.server == NULL);
 	
	// successfully inited
	RPCclient_Inited = 1;
		
	return 1;
}

//--------------------------------------------------------------
int rpcUSBReset(void)
{
	RPCclient_Inited = 0;
	rpcclient.server = NULL;
	return 1; 
}

//--------------------------------------------------------------
int rpcUSBcreate(char *filename)
{
	int ret = 0;

	// check lib is inited
	if (!RPCclient_Inited)
		return -1;
			
	// set global variables
	if (filename)
		memcpy(openParam.filename, filename, 1020);
	else
		return -2;	
	 	
	if((ret = SifCallRpc(&rpcclient, CMD_USBOPEN, SIF_RPC_M_NOWAIT, &openParam, sizeof(openParam), Rpc_Buffer, 4, 0, 0)) != 0) {
		return ret;
	}
			
	currentCmd = CMD_USBOPEN;
	
	return 1;
}

//--------------------------------------------------------------
int rpcUSBwrite(int fd, u8 *buf, int size)
{
	int ret = 0;

	// check lib is inited
	if (!RPCclient_Inited)
		return -1;
			
	// set global variables
	writeParam.fd = fd;	
	if (buf)
		memcpy(writeParam.buf, buf, size);
	else
		memset(writeParam.buf, 0, size);	
		
	writeParam.size = size;
	 	
	if((ret = SifCallRpc(&rpcclient, CMD_USBWRITE, SIF_RPC_M_NOWAIT, &writeParam, sizeof(writeParam), Rpc_Buffer, 4, 0, 0)) != 0) {
		return ret;
	}
			
	currentCmd = CMD_USBWRITE;
	
	return 1;
}

//--------------------------------------------------------------
int rpcUSBclose(int fd)
{
	int ret = 0;

	// check lib is inited
	if (!RPCclient_Inited)
		return -1;
			
	// set global variables
	closeParam.fd = fd;
	 	
	if((ret = SifCallRpc(&rpcclient, CMD_USBCLOSE, SIF_RPC_M_NOWAIT, &closeParam, sizeof(closeParam), Rpc_Buffer, 4, 0, 0)) != 0) {
		return ret;
	}
			
	currentCmd = CMD_USBCLOSE;
	
	return 1;
}

//--------------------------------------------------------------
int rpcUSBSync(int mode, int *cmd, int *result)
{
	int funcIsExecuting, i;
	
	// check if any functions are registered
	if (currentCmd == CMD_NONE)
		return -1;
	
	// check if function is still processing
	funcIsExecuting = SifCheckStatRpc(&rpcclient);
	
	// if mode = 0, wait for function to finish
	if (mode == 0) {
		while (SifCheckStatRpc(&rpcclient)) {
			for (i=0; i<100000; i++) ;
		}
		// function has finished
		funcIsExecuting = 0;
	}
	
	// get the function that just finished
	if (cmd)
		*cmd = currentCmd;
	
	// if function is still processing, return 0
	if (funcIsExecuting == 1)
		return 0;
	
	// function has finished, so clear last command
	currentCmd = CMD_NONE;
	
	// get result
	if(result)
		*result = *(int*)Rpc_Buffer;
	
	return 1;
}

