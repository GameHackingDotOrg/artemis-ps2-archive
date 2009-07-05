
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

WSADATA *WsaData;

#define SERVER_PORT  		4234
#define SERVER_IP			"192.168.0.80"

char pktbuffer[65536];

// NTPB header magic
#define ntpb_MagicSize  6
const unsigned char ntpb_hdrMagic[6] = {'\xff', '\x00', 'N', 'T', 'P', 'B'};
#define ntpb_hdrSize  	10

static int main_socket = -1;

// command line commands stuff
#define CMD_UNKNOWN			0
#define CMD_EEDUMP			1
#define CMD_IOPDUMP			2
#define CMD_KERNELDUMP		3
#define CMD_SCRATCHPADDUMP	4

const char *id_ram[5] = {
	(const char *)"dummy",
	(const char *)"EE RAM",
	(const char *)"IOP RAM",
	(const char *)"Kernel RAM",
	(const char *)"ScratchPad RAM"	
};

// Remote commands to be sent to server
#define REMOTE_CMD_NONE						0x000
#define REMOTE_CMD_DUMPEE					0x101
#define REMOTE_CMD_DUMPIOP					0x102
#define REMOTE_CMD_DUMPKERNEL				0x103
#define REMOTE_CMD_DUMPSCRATCHPAD			0x104

// commands sent in return by server
#define NTPBCMD_PRINT_EEDUMP 				0x301
#define NTPBCMD_PRINT_IOPDUMP				0x302
#define NTPBCMD_PRINT_KERNELDUMP 			0x303
#define NTPBCMD_PRINT_SCRATCHPADDUMP		0x304
#define NTPBCMD_END_TRANSMIT				0xffff

FILE *fh_dump;


//-------------------------------------------------------------- 
void PrintUsage(void) {
	
	printf("usage: ntpbclient <command> [args]\n");
	printf("ntpbclient command-line version.\n");
	printf("\n");
	printf("Available commands:\n");
	printf("\t ee_dump <start_address> <end_address> <outfile>\n");
	printf("\t iop_dump <start_address> <end_address> <outfile>\n");
	printf("\t kernel_dump <start_address> <end_address> <outfile>\n");
	printf("\t scratchpad_dump <start_address> <end_address> <outfile>\n");
}

//-------------------------------------------------------------- 
#ifdef _WIN32
WSADATA *InitWS2(void) {
    int            r;        // catches return value of WSAStartup
    static WSADATA WsaData;  // receives data from WSAStartup
    int 		   ret;  	 // return value flag
    
    ret = 1;

    // Start WinSock 2.  If it fails, we don't need to call
    // WSACleanup().
    r = WSAStartup(MAKEWORD(2, 0), &WsaData);
    
    if (r) {
        printf("error: can't find high enough version of WinSock\n");
        ret = 0;
    } else {
        // Now confirm that the WinSock 2 DLL supports the exact version
        // we want. If not, make sure to call WSACleanup().
        if ((WsaData.wVersion & 0xff) != 2) {
            printf("error: can't find the correct version of WinSock\n");
            WSACleanup();
            ret = 0;
        }
    }
        
    if (ret)
        return &WsaData;
        
    return NULL;
}
#endif

//-------------------------------------------------------------- 
int clientConnect(void)
{
	int r, tcp_socket;
	struct sockaddr_in peer;

	peer.sin_family = AF_INET;
	peer.sin_port = htons(SERVER_PORT);
	peer.sin_addr.s_addr = inet_addr(SERVER_IP);

	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket < 0) 
		goto error;

	r = connect(tcp_socket, (struct sockaddr*)&peer, sizeof(peer));
	if (r < 0)
		goto error;

	main_socket = tcp_socket;

	return 1;

error:
	printf("error: failed to connect to server...\n");
	closesocket(tcp_socket);

	return 0;
}

//-------------------------------------------------------------- 
int clientDisconnect(void)
{
	closesocket(main_socket);	
	
	return 1;
}

//-------------------------------------------------------------- 
unsigned int HexaToDecimal(const char* pszHexa)
{
	unsigned int ret = 0, t = 0, n = 0;
	const char *c = pszHexa;

	while (*c && (n < 16)) {

		if ((*c >= '0') && (*c <= '9'))
			t = (*c - '0');
		else if((*c >= 'A') && (*c <= 'F'))
			t = (*c - 'A' + 10);
		else if((*c >= 'a') && (*c <= 'f'))
			t = (*c - 'a' + 10);
		else
			break;

		n++; ret *= 16; ret += t; c++;

		if (n >= 8)
			break;
	}

	return ret;
}

//-------------------------------------------------------------- 
int check_ntpb_header(void *buf) // sanity check to see if the packet have the format we expect
{
	int i;
	unsigned char *pbuf = (unsigned char *)buf;

	for (i=0; i<ntpb_MagicSize; i++) {
		if (pbuf[i] != ntpb_hdrMagic[i])
			break;
	}

	if (i == ntpb_MagicSize)
		return 1;

	return 0;
}

//-------------------------------------------------------------- 
int SendRemoteCmd(int cmd, unsigned char *buf, int size)
{
	int ntpbpktSize, sndSize, rcvSize;

	memcpy(&pktbuffer[0], ntpb_hdrMagic, ntpb_MagicSize); //copying NTPB Magic
	*((unsigned short *)&pktbuffer[ntpb_MagicSize]) = size;
	*((unsigned short *)&pktbuffer[ntpb_MagicSize+2]) = cmd;

	if ((buf) && (size > 0)) {
		memcpy(&pktbuffer[ntpb_hdrSize], buf, size);
	}

	ntpbpktSize = ntpb_hdrSize + size;

	// send the ntpb packet
	sndSize = send(main_socket, &pktbuffer[0], ntpbpktSize, 0);
	if (sndSize <= 0)
		return -1;

	rcvSize = recv(main_socket, &pktbuffer[0], sizeof(pktbuffer), 0);
	if (rcvSize <= 0)
		return -2;
		
	// packet sanity check
	if (!check_ntpb_header(pktbuffer))
		return -3;
		
	// reply check
	if (*((unsigned short *)&pktbuffer[ntpb_hdrSize]) != 1)
		return -4;	

	return 1;
}

//-------------------------------------------------------------- 
int receiveDump(unsigned int dump_size) // retrieving datas sent by server
{
	int rcvSize, sndSize, packetSize, ntpbpktSize, ntpbCmd, recv_size, sizeWritten;
	unsigned int dump_wpos = 0;
	int endTransmit = 0;

	while (1) {
		
		// receive the first packet
		rcvSize = recv(main_socket, &pktbuffer[0], sizeof(pktbuffer), 0);
		if (rcvSize < 0)
			return -1;
			
		// packet sanity check
		if (!check_ntpb_header(pktbuffer))
			return -2;

		ntpbpktSize = *((unsigned short *)&pktbuffer[6]);
		packetSize = ntpbpktSize + ntpb_hdrSize;

		recv_size = rcvSize;

		// fragmented packet handling
		while (recv_size < packetSize) {
			rcvSize = recv(main_socket, &pktbuffer[recv_size], sizeof(pktbuffer) - recv_size, 0);
			if (rcvSize < 0)
				return -1;
			else
				recv_size += rcvSize;
		}

		// parses packet
		if (check_ntpb_header(pktbuffer)) {
			ntpbCmd = *((unsigned short *)&pktbuffer[8]);

			switch(ntpbCmd) { // treat Client Request here

				case NTPBCMD_PRINT_EEDUMP:
				case NTPBCMD_PRINT_IOPDUMP:
				case NTPBCMD_PRINT_KERNELDUMP:
				case NTPBCMD_PRINT_SCRATCHPADDUMP:

					if ((dump_wpos + ntpbpktSize) > dump_size)
						return -3;

					sizeWritten = fwrite(&pktbuffer[ntpb_hdrSize], 1, ntpbpktSize, fh_dump);
					if (sizeWritten != ntpbpktSize)
						return -4;
					
					dump_wpos += sizeWritten;
					break;

				case NTPBCMD_END_TRANSMIT:
					endTransmit = 1;
					break;
			}

			*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
			*((unsigned short *)&pktbuffer[6]) = 0;
			packetSize = ntpb_hdrSize + 2;

			// send the response packet
			sndSize = send(main_socket, &pktbuffer[0], packetSize, 0);
			if (sndSize <= 0)
				return -5;
				
			// catch end of dump transmission	
			if (endTransmit)
				break;
		}
	}

	return 1;
}

//-------------------------------------------------------------- 
int main(int argc, char **argv, char **env) {

	int i, r, cmd, remote_cmd;
	unsigned int dump_start, dump_end, dump_size;
	char dump_file[1024];
	unsigned char cmdBuf[16];
	
	// check number of args
	if (argc < 2) {
		PrintUsage();
		return 0;
	}
	
	// get typed command
	cmd = CMD_UNKNOWN;
	
	for (i=0; argv[i]; i++) {
		if ((!strcmp(argv[i], "help")) || (!strcmp(argv[i], "-help")) || (!strcmp(argv[i], "--help"))) {
			PrintUsage();
			return 0;
		}
		else if (!strcmp(argv[i], "ee_dump")) {
			if (argc < 5) {
				PrintUsage();
				return 0;		
			}
			cmd = CMD_EEDUMP;
			remote_cmd = REMOTE_CMD_DUMPEE;
			dump_start = HexaToDecimal(argv[i+1]);
			dump_end = HexaToDecimal(argv[i+2]);
			strcpy(dump_file, argv[i+3]);			
		}
		else if (!strcmp(argv[i], "iop_dump")) {
			if (argc < 5) {
				PrintUsage();
				return 0;		
			}						
			cmd = CMD_IOPDUMP;			
			remote_cmd = REMOTE_CMD_DUMPIOP;			
			dump_start = HexaToDecimal(argv[i+1]);
			dump_end = HexaToDecimal(argv[i+2]);
			strcpy(dump_file, argv[i+3]);						
		}
		else if (!strcmp(argv[i], "kernel_dump")) {
			if (argc < 5) {
				PrintUsage();
				return 0;		
			}			
			cmd = CMD_KERNELDUMP;
			remote_cmd = REMOTE_CMD_DUMPKERNEL;			
			dump_start = HexaToDecimal(argv[i+1]);
			dump_end = HexaToDecimal(argv[i+2]);
			strcpy(dump_file, argv[i+3]);												
		}
		else if (!strcmp(argv[i], "scratchpad_dump")) {
			if (argc < 5) {
				PrintUsage();
				return 0;		
			}			
			cmd = CMD_SCRATCHPADDUMP;
			remote_cmd = REMOTE_CMD_DUMPSCRATCHPAD;			
			dump_start = HexaToDecimal(argv[i+1]);
			dump_end = HexaToDecimal(argv[i+2]);
			strcpy(dump_file, argv[i+3]);												
		}
 	}	

 	// check typed command
	if (cmd == CMD_UNKNOWN) {
		PrintUsage();
		return 0;
	}
 		
#ifdef _WIN32
	// Init WSA
	WsaData = InitWS2();
	if (WsaData == NULL)
		return 0;	
#endif
		
	// Connect client to PS2 server
	if (!clientConnect())
		return 0;	

	printf("Please wait while dumping %s @0x%08x-0x%08x to %s... ", id_ram[cmd], dump_start, dump_end, dump_file);
	
	// create the dump file
	fh_dump = fopen(dump_file, "wb");
	if (!fh_dump) {
		printf("failed to create file!\n");
		goto disconnect;
	}
	
	// fill remote cmd buffer
	*((unsigned int *)&cmdBuf[0]) = dump_start;
	*((unsigned int *)&cmdBuf[4]) = dump_end;
	
	// send remote cmd
	r = SendRemoteCmd(remote_cmd, cmdBuf, 8);
	if (r < 0) {
		printf("failed to send remote command - error %d\n", r);
		goto close_dump;
	}
	
	dump_size = dump_end - dump_start;
		
	// receive dump	
	r = receiveDump(dump_size);
	if (r < 0) {
		printf("failed to receive dump datas - error %d\n", r);		
		goto close_dump;
	}
	
	printf("done\n");
		
close_dump:			
	fclose(fh_dump);		

disconnect:			
	// disconnect the client
	clientDisconnect();		
				
#ifdef _WIN32
	WSACleanup();
#endif
		
	// End program.
	return 0;

}
