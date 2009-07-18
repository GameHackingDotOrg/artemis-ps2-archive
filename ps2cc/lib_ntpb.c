/****************************************************************************
Memory Library

This is where the all important PC-PS2 comms code should reside. It's almost
an exact copy of Jimmi's command line ntpb client, just without main(). My
functions to manage things are at the bottom. I changed the IP var, and added
a progress bar step line to receiveDump().
*****************************************************************************/
#include "ps2cc.h"

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
//IP determined by Settings.ServerIp now
//#define SERVER_IP			"192.168.0.80"

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
#define REMOTE_CMD_PATCHMEM					0x501
#define REMOTE_CMD_UNPATCHMEM				0x502

// commands sent in return by server
#define NTPBCMD_PRINT_EEDUMP 				0x301
#define NTPBCMD_PRINT_IOPDUMP				0x302
#define NTPBCMD_PRINT_KERNELDUMP 			0x303
#define NTPBCMD_PRINT_SCRATCHPADDUMP		0x304
#define NTPBCMD_END_TRANSMIT				0xffff

FILE *fh_dump;

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
	peer.sin_addr.s_addr = inet_addr(Settings.ServerIp);

	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket < 0)
		goto error;

	r = connect(tcp_socket, (struct sockaddr*)&peer, sizeof(peer));
	if (r < 0)
		goto error;

	main_socket = tcp_socket;

	return 1;

error:
//	printf("error: failed to connect to server...\n");
	closesocket(tcp_socket);

	return 0;
}

//--------------------------------------------------------------
int clientDisconnect(void)
{
	closesocket(main_socket);

	return 1;
}

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

					//step progress bar
					UpdateProgressBar(PBM_STEPIT, 0, 0);

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

/****************************************************************************
DumpRAM
- The goal with this function is to dump the memory area we want, whenever we
want to, and specify the filename. Thus, inputs to the function are start
address, end address, and filename-- I used the same var names Jimmi setup.
The rest is copied from the main() of ntpbclient.c (command line version) and
tweaked as needed). There should be a complete dump file when the function
returns. It should return 0 on fail, and 1 on success. I think when an error
occurs, we'll set ErrTxt to error text.

To Do: Work out a way to impliment the progress bar in this version of the
source. I'll probably do it. I just want it tested as is first.
*****************************************************************************/
extern char ErrTxt[1000];

int DumpRAM(char *dump_file, unsigned int dump_start, unsigned int dump_end)
{
	unsigned int dump_size;
	int i, r, cmd, remote_cmd;
	unsigned char cmdBuf[16];

	//checking address range to determine course of action
	cmd = CMD_UNKNOWN;
	if (dump_start > dump_end) {
		sprintf(ErrTxt, "Search area start (%08X) is higher thand end (%08X). THINK about it. (DumpRAM)", dump_start, dump_end);
		return 0;
	}
	if ((dump_start >= 00100000) && (dump_end <= 0x02000000)) { //EE Dump
		cmd = CMD_EEDUMP;
		remote_cmd = REMOTE_CMD_DUMPEE;
	} else if ((dump_start >= 0) && (dump_end <= 0x00200000)) { //IOP Dump
		cmd = CMD_IOPDUMP;
		remote_cmd = REMOTE_CMD_DUMPIOP;
	} else if ((dump_start >= 0x80000000) && (dump_end <= 0x82000000)) { //Kernel Dump
		cmd = CMD_KERNELDUMP;
		remote_cmd = REMOTE_CMD_DUMPKERNEL;
	} else if ((dump_start >= 0x70000000) && (dump_end <= 0x70004000)) { //ScratchPad Dump
		cmd = CMD_SCRATCHPADDUMP;
		remote_cmd = REMOTE_CMD_DUMPSCRATCHPAD;
	}
	//Check that we're trying to dump a valid area
	if (cmd == CMD_UNKNOWN) {
		sprintf(ErrTxt, "Invalid search area specifed. (DumpRAM)");
		return 0;
	}

#ifdef _WIN32
	// Init WSA
	WsaData = InitWS2();
	if (WsaData == NULL)
		return 0;
#endif


	// Connect client to PS2 server
	UpdateStatusBar("Contacting PS2 Server...", 0, 0);
	if (!clientConnect()) {
		sprintf(ErrTxt, "Connecting to PS2 failed. (DumpRAM)");
		UpdateStatusBar("Idle", 0, 0);
		return 0;
	}

	UpdateStatusBar("Client connected...", 0, 0);

	//using isDumped to determine the return value. 0 on error, 1 on success
	int isDumped = 0;

	// create the dump file
	fh_dump = fopen(dump_file, "wb");
	if (!fh_dump) {
		sprintf(ErrTxt, "Failed to create dump file! (DumpRAM");
		goto disconnect;
	}
	// fill remote cmd buffer
	*((unsigned int *)&cmdBuf[0]) = dump_start;
	*((unsigned int *)&cmdBuf[4]) = dump_end;

	// send remote cmd
	r = SendRemoteCmd(remote_cmd, cmdBuf, 8);
	if (r < 0) {
		sprintf(ErrTxt, "Failed to send remote command - error %d (DumpRAM)", r);
		goto close_dump;
	}

	dump_size = dump_end - dump_start;

	//init progress bar
	UpdateProgressBar(PBM_SETRANGE, 0, MAKELPARAM(0, dump_size/8192));
	UpdateProgressBar(PBM_SETSTEP, 1, 0);
//	UpdateProgressBar(PBM_SETBARCOLOR, 0, RGB(23, 219, 38));
	UpdateStatusBar("Dumping Memory...", 0, 0);

	// receive dump
	r = receiveDump(dump_size);
	if (r < 0) {
		sprintf(ErrTxt, "Failed to receive dump datas - error %d (DumpRAM)", r);
		goto close_dump;
	}

	//Success!
	isDumped = 1;
	//reset progress bar
	UpdateProgressBar(PBM_SETPOS, 0, 0);

close_dump:
	fclose(fh_dump);

disconnect:
	// disconnect the client
	clientDisconnect();

#ifdef _WIN32
	WSACleanup();
#endif

	UpdateStatusBar("Idle", 0, 0);
	return isDumped;

}

/****************************************************************************
TestConnect - Test the connection when user edits IP, etc
*****************************************************************************/
int TestConnect()
{
#ifdef _WIN32
	// Init WSA
	WsaData = InitWS2();
	if (WsaData == NULL)
		return 0;
#endif

	// Connect client to PS2 server
	if (!clientConnect()) {
		sprintf(ErrTxt, "Connecting to PS2 failed. (DumpRAM)");
		return 0;
	}
	clientDisconnect();
	return 1;
}
/****************************************************************************
Activate Cheats*****************************************************************************/
int ActivateCheats(unsigned char codes[128], int numcodes)
{
	int i, r, cmd, remote_cmd, opstatus;
	unsigned char cmdBuf[16];

#ifdef _WIN32
	// Init WSA
	WsaData = InitWS2();
	if (WsaData == NULL)
		return 0;
#endif

	// Connect client to PS2 server
	UpdateStatusBar("Contacting PS2 Server...", 0, 0);
	if (!clientConnect()) {
		sprintf(ErrTxt, "Connecting to PS2 failed. (DumpRAM)");
		UpdateStatusBar("Idle", 0, 0);
		return 0;
	}

	UpdateStatusBar("Client connected...", 0, 0);

	// send remote cmd
	UpdateStatusBar("Updating Codes...", 0, 0);	
	remote_cmd = REMOTE_CMD_PATCHMEM;
	r = SendRemoteCmd(remote_cmd, codes, (numcodes * 8) + 4);
	if (r < 0) {
		sprintf(ErrTxt, "Failed to send remote command - error %d (ActivateCodes)", r);
		opstatus = 0;
		goto ac_disconnect;
	}
	opstatus = 1;

ac_disconnect:
	// disconnect the client
	clientDisconnect();

#ifdef _WIN32
	WSACleanup();
#endif

	UpdateStatusBar("Idle", 0, 0);
	return opstatus;
}

/****************************************************************************
DeActivate Cheats*****************************************************************************/
int DeActivateCodes()
{
	int i, r, cmd, remote_cmd, opstatus;
	unsigned char cmdBuf[16];

#ifdef _WIN32
	// Init WSA
	WsaData = InitWS2();
	if (WsaData == NULL)
		return 0;
#endif

	// Connect client to PS2 server
	UpdateStatusBar("Contacting PS2 Server...", 0, 0);
	if (!clientConnect()) {
		sprintf(ErrTxt, "Connecting to PS2 failed. (DumpRAM)");
		UpdateStatusBar("Idle", 0, 0);
		return 0;
	}

	UpdateStatusBar("Client connected...", 0, 0);

	// send remote cmd
	UpdateStatusBar("Updating Codes...", 0, 0);	
	remote_cmd = REMOTE_CMD_UNPATCHMEM;
	r = SendRemoteCmd(remote_cmd, NULL, 0);
	if (r < 0) {
		sprintf(ErrTxt, "Failed to send remote command - error %d (DeActivateCodes)", r);
		opstatus = 0;
		goto dac_disconnect;
	}
	opstatus = 1;

dac_disconnect:
	// disconnect the client
	clientDisconnect();

#ifdef _WIN32
	WSACleanup();
#endif

	UpdateStatusBar("Idle", 0, 0);
	return opstatus;
}
