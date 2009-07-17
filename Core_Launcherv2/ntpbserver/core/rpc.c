
#include "core.h"

#define CMD_NONE  			0x00
#define CMD_GETREMOTECMD  	0x06
#define CMD_SENDDATA	  	0x0e
#define CMD_ENDREPLY	  	0x0f

static SifRpcClientData_t rpcclient __attribute__((aligned(64)));
static int Rpc_Buffer[16] 			__attribute__((aligned(64)));

static struct { 		// size = 16384
	u16 cmd;			// 0
	u8 buf[16378];		// 2
	int size;			// 16380
} sendDataParam __attribute__((aligned(64)));

static struct { 		// size = 16
	u16 cmd;			// 0
	u8 buf[8];			// 2
	int size;			// 10
	u16 pad;
} getRemoteCmdParam __attribute__((aligned(64)));

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
int rpcNTPBgetRemoteCmd(u16 *cmd, u8 *buf, int *size)
{
	int ret = 0;

	// check lib is inited
	if (!RPCclient_Inited)
		return -1;
					 	
	if((ret = SifCallRpc(&rpcclient, CMD_GETREMOTECMD, SIF_RPC_M_NOWAIT, NULL, 0, &getRemoteCmdParam, sizeof(getRemoteCmdParam), 0, 0)) != 0) {
		return ret;
	}

	*cmd = getRemoteCmdParam.cmd;
	*size = getRemoteCmdParam.size;
	if (getRemoteCmdParam.size > 0)
		memcpy(buf, getRemoteCmdParam.buf, getRemoteCmdParam.size);
				
	currentCmd = CMD_GETREMOTECMD;
	*(int*)Rpc_Buffer = 1;
	
	return 1;
}

//--------------------------------------------------------------
int rpcNTPBsendData(u16 cmd, u8 *buf, int size)
{
	int ret = 0;

	// check lib is inited
	if (!RPCclient_Inited)
		return -1;
			
	// set global variables
	sendDataParam.cmd = cmd;
	if (buf)
		memcpy(sendDataParam.buf, buf, size);
	else
		memset(sendDataParam.buf, 0, size);	
		
	sendDataParam.size = size;
	 	
	if((ret = SifCallRpc(&rpcclient, CMD_SENDDATA, SIF_RPC_M_NOWAIT, &sendDataParam, sizeof(sendDataParam), Rpc_Buffer, 4, 0, 0)) != 0) {
		return ret;
	}
			
	currentCmd = CMD_SENDDATA;
	
	*(int*)Rpc_Buffer = 1;
	
	return 1;
}

//--------------------------------------------------------------
int rpcNTPBEndReply(void)
{
	int ret = 0;

	// check lib is inited
	if (!RPCclient_Inited)
		return -1;
				 	
	if((ret = SifCallRpc(&rpcclient, CMD_ENDREPLY, SIF_RPC_M_NOWAIT, NULL, 0, Rpc_Buffer, 4, 0, 0)) != 0) {
		return ret;
	}
			
	currentCmd = CMD_ENDREPLY;
	
	*(int*)Rpc_Buffer = 1;
	
	return 1;
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

