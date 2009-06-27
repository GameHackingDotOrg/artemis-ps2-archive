
#include "core.h"

#define CMD_NONE  		0x00
#define CMD_CONNECT  	0x01
#define CMD_SENDCMD	  	0x0e
#define CMD_DISCONNECT  0x0f

static SifRpcClientData_t rpcclient __attribute__((aligned(64)));
static int Rpc_Buffer[16] 			__attribute__((aligned(64)));

static struct { 			// size = 16
	char ServerIP[1024];	// 0
	int ServerPort;			// 4
	u32 pad1;				// 8
	u32 pad2;				// 12
} connectParam __attribute__((aligned(64)));

static struct { 			// size = 16384
	u16 cmd;				// 0
	u8 buf[16378];		// 2
	int size;				// 16380
} sendCmdParam __attribute__((aligned(64)));

// stores command currently being executed on the iop
static unsigned int currentCmd = 0;

static int RPCclient_Inited = 0;

//--------------------------------------------------------------
int rpcNTPBinit(void)
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
int rpcNTPBreset(void)
{
	RPCclient_Inited = 0;
	rpcclient.server = NULL;
	return 1; 
}

//--------------------------------------------------------------
int rpcNTPBconnect(const char *ServerIP, int ServerPort)
{
	int ret;
	
	// check lib is inited
	if (!RPCclient_Inited)
		return -1;
			
	// set global variables
	strncpy(connectParam.ServerIP, ServerIP, strlen(ServerIP));
	connectParam.ServerIP[strlen(ServerIP)] = 0;
	connectParam.ServerPort = ServerPort;
		
	// send sif command
	if ((ret = SifCallRpc(&rpcclient, CMD_CONNECT, 1, &connectParam, sizeof(connectParam), Rpc_Buffer, 4, 0, 0)) != 0)
		return ret;
	
	currentCmd = CMD_CONNECT;
	
   	return *(int*)Rpc_Buffer;
}

//--------------------------------------------------------------
int rpcNTPBsendCmd(u16 cmd, u8 *buf, int size)
{
	int ret = 0;

	// check lib is inited
	if (!RPCclient_Inited)
		return -1;
			
	// set global variables
	sendCmdParam.cmd = cmd;
	if (buf)
		memcpy(sendCmdParam.buf, buf, size);
	else
		memset(sendCmdParam.buf, 0, size);	
		
	sendCmdParam.size = size;
	 	
	if((ret = SifCallRpc(&rpcclient, CMD_SENDCMD, 0, &sendCmdParam, sizeof(sendCmdParam), Rpc_Buffer, 4, 0, 0)) != 0) {
		return ret;
	}
			
	currentCmd = CMD_SENDCMD;
	
	return *(int*)Rpc_Buffer;
}

//--------------------------------------------------------------
int rpcNTPBdisconnect(void)
{
	int ret = 0;

	// check lib is inited
	if (!RPCclient_Inited)
		return -1;
		 	
	if((ret = SifCallRpc(&rpcclient, CMD_DISCONNECT, 1, NULL, 0, Rpc_Buffer, 4, 0, 0)) != 0) {
		return ret;
	}

	currentCmd = CMD_DISCONNECT;
			
	return *(int*)Rpc_Buffer;
}

//--------------------------------------------------------------
int rpcSync(int mode, int *cmd, int *result)
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

