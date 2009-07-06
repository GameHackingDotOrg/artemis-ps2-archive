/****************************************************************************
Memory Library

This is where the all important PC-PS2 comms code should reside. I intend to
clean this up more later. There's still a lot of unneeded GUI references. I
still have the progress and status bars in use as they were though, hopefully.
*****************************************************************************/

#include "ps2cc.h"
#include "ps2cc_gui.h"

/****************************************************************************
Jimmi's declarations begin
*****************************************************************************/

#define SERVER_PORT  	4234

char pktbuffer[65536];
char launch_path[2048];
char dump_dir[2048];
char eedump_dir[2048];
char iopdump_dir[2048];
char eedump_file[2048];
char iopdump_file[2048];
char kerneldump_dir[2048];
char scratchpaddump_dir[2048];
char kerneldump_file[2048];
char scratchpaddump_file[2048];

int remote_cmd;
int ClientConnected = 0;
static int haltState = 0;

int eedump_index, iopdump_index, kerneldump_index, scratchpaddump_index;
unsigned int dump_size;
unsigned int dump_wpos;
HANDLE fh_dump;

// NTPB header magic
#define ntpb_MagicSize  6
const unsigned char *ntpb_hdrMagic = "\xff\x00NTPB";
#define ntpb_hdrSize  	10

#define NTPBCMD_GET_IOPDUMP_START			0x102
#define NTPBCMD_GET_KERNELDUMP_START		0x103
#define NTPBCMD_GET_SCRATCHPADDUMP_START	0x104
#define NTPBCMD_GET_EEDUMP_END 				0x201
#define NTPBCMD_GET_IOPDUMP_END				0x202
#define NTPBCMD_GET_KERNELDUMP_END			0x203
#define NTPBCMD_GET_SCRATCHPADDUMP_END		0x204
#define NTPBCMD_PRINT_EEDUMP 				0x301
#define NTPBCMD_PRINT_IOPDUMP				0x302
#define NTPBCMD_PRINT_KERNELDUMP 			0x303
#define NTPBCMD_PRINT_SCRATCHPADDUMP		0x304
#define NTPBCMD_GET_REMOTE_CMD				0x400

#define REMOTE_CMD_DUMPEE					0x101
#define REMOTE_CMD_DUMPIOP					0x102
#define REMOTE_CMD_DUMPKERNEL				0x103
#define REMOTE_CMD_DUMPSCRATCHPAD			0x104
#define REMOTE_CMD_HALT						0x201
#define REMOTE_CMD_RESUME					0x202



/*<---------------------------------------------------------------------->*/
HWND hwndLabelEEdump;				// Label control handle
HWND hwndLabelIOPdump;  			// Label control handle
HWND hwndLabelEEdumpStart;  		// Label control handle
HWND hwndLabelEEdumpEnd;  			// Label control handle
HWND hwndLabelIOPdumpStart; 		// Label control handle
HWND hwndLabelIOPdumpEnd;   		// Label control handle

HWND hwndLabelKerneldump;			// Label control handle
HWND hwndLabelScratchpaddump;  		// Label control handle
HWND hwndLabelKerneldumpStart;  	// Label control handle
HWND hwndLabelKerneldumpEnd;  		// Label control handle
HWND hwndLabelScratchpaddumpStart; 	// Label control handle
HWND hwndLabelScratchpaddumpEnd;   	// Label control handle

HWND hwndLabeldumpState;		   	// Label control handle

HWND hwndTextBoxEEdump;				// Edit control handle
HWND hwndTextBoxIOPdump;			// Edit control handle
HWND hwndTextBoxEEdumpStart;		// Edit control handle
HWND hwndTextBoxEEdumpEnd;			// Edit control handle
HWND hwndTextBoxIOPdumpStart;		// Edit control handle
HWND hwndTextBoxIOPdumpEnd;			// Edit control handle

HWND hwndTextBoxKerneldump;			// Edit control handle
HWND hwndTextBoxScratchpaddump;		// Edit control handle
HWND hwndTextBoxKerneldumpStart;	// Edit control handle
HWND hwndTextBoxKerneldumpEnd;		// Edit control handle
HWND hwndTextBoxScratchpaddumpStart;// Edit control handle
HWND hwndTextBoxScratchpaddumpEnd;	// Edit control handle

HWND hwndButtonEEdump;				// Button control handle
HWND hwndButtonIOPdump;				// Button control handle
HWND hwndButtonKerneldump;			// Button control handle
HWND hwndButtonScratchpaddump;		// Button control handle

HWND hwndButtonHaltResume;			// Button control handle

//these two are probably the only important ones
HWND hwndProgressBardumpState;		// Progress Bar control handle
HWND hWndStatusbar;					// StatusBar handle

WSADATA *WsaData;

//temp
#define IDC_TEXTBOX_EEDUMP 201
#define IDC_TEXTBOX_IOPDUMP 202
#define IDC_TEXTBOX_EEDUMPSTART 203
#define IDC_TEXTBOX_EEDUMPEND 204
#define IDC_TEXTBOX_IOPDUMPSTART 205
#define IDC_TEXTBOX_IOPDUMPEND 206
#define IDC_TEXTBOX_KERNELDUMP 207
#define IDC_TEXTBOX_SCRATCHPADDUMP 208
#define IDC_TEXTBOX_KERNELDUMPSTART 209
#define IDC_TEXTBOX_KERNELDUMPEND 211
#define IDC_TEXTBOX_SCRATCHPADDUMPSTART 212
#define IDC_TEXTBOX_SCRATCHPADDUMPEND 213
//temp
/****************************************************************************
Jimmi's declarations end here
*****************************************************************************/

/*<---------------------------------------------------------------------->*/
void UpdateStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags)
{
    SendMessage(hWndStatusbar,
                SB_SETTEXT,
                partNumber | displayFlags,
                (LPARAM)lpszStatusString);
}

/*<---------------------------------------------------------------------->*/
void InitializeStatusBar(HWND hwndParent,int nrOfParts)
{
    const int cSpaceInBetween = 8;
    int   ptArray[40];   // Array defining the number of parts/sections
    RECT  rect;
    HDC   hDC;

   /* * Fill in the ptArray...  */

    hDC = GetDC(hwndParent);
    GetClientRect(hwndParent, &rect);

    ptArray[nrOfParts-1] = rect.right;
    //---TODO--- Add code to calculate the size of each part of the status
    // bar here.

    ReleaseDC(hwndParent, hDC);
    SendMessage(hWndStatusbar,
                SB_SETPARTS,
                nrOfParts,
                (LPARAM)(LPINT)ptArray);

    //UpdateStatusBar("Ready", 0, 0);
}


/****************************************************************************
// WSADATA *InitWS2(void)
// Routine Description:
//
// Calls WSAStartup, makes sure we have a good version of WinSock2
//
// Return Value:
//  A pointer to a WSADATA structure - WinSock 2 DLL successfully started up
//  NULL - Error starting up WinSock 2 DLL.
// (Jimmi's)
****************************************************************************/

WSADATA *InitWS2(void)
{
    int           Error;              // catches return value of WSAStartup
    WORD          VersionRequested;   // passed to WSAStartup
    static WSADATA       WsaData;            // receives data from WSAStartup
    BOOL          ReturnValue = TRUE; // return value flag

    // Start WinSock 2.  If it fails, we don't need to call
    // WSACleanup().
    VersionRequested = MAKEWORD(2, 0);
    Error = WSAStartup(VersionRequested, &WsaData);
    if (Error) {
        MessageBox(hwndMain,
                   "Could not find high enough version of WinSock",
                   "Error", MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
        ReturnValue = FALSE;
    } else {

        // Now confirm that the WinSock 2 DLL supports the exact version
        // we want. If not, make sure to call WSACleanup().
        if (LOBYTE(WsaData.wVersion) != 2) {
            MessageBox(hwndMain,
                       "Could not find the correct version of WinSock",
                       "Error",  MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
            WSACleanup();
            ReturnValue = FALSE;
        }
    }
    if (ReturnValue)
        return &WsaData;
    return(NULL);

} // InitWS2()

/*<---------------------------------------------------------------------->*/
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

/*<---------------------------------------------------------------------->*/
int PrintLog(void *buf, int size, int ResultControlID)
{
	// send messages to the edit control after preserving the already written content
	int len = GetWindowTextLength(GetDlgItem(hwndMain, ResultControlID));
	char *buffer;

	buffer = (char*)GlobalAlloc(GPTR, len + size + 256);
	GetDlgItemText(hwndMain, ResultControlID, buffer, len + 1);

	strcat(buffer, buf);

	SetDlgItemText(hwndMain, ResultControlID, buffer);
	UpdateWindow(hwndMain);

	GlobalFree((HANDLE)buffer);

	return size;
}
/*<---------------------------------------------------------------------->*/
DWORD HexaToDecimal(const char* pszHexa)
{
	DWORD ret = 0, t = 0, n = 0;
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

		if(n >= 8)
			break;
	}

	return ret;
}

/*<---------------------------------------------------------------------->*/
int receivePacket(int client_socket) // retrieving a packet sent by the Client
{
	int rcvSize, sndSize, packetSize, ntpbpktSize, ntpbCmd, ln, recv_size;
	DWORD sizeWritten;
	char *pbuf;
	char startbuf[128], endbuf[128], tmp[128], tmp_file[128];

	pbuf = (char *)&pktbuffer[0];

	// receive the request packet
	rcvSize = recv(client_socket, &pktbuffer[0], sizeof(pktbuffer), 0);
	if (rcvSize <= 0) {
		MessageBox(GetActiveWindow(),"Error: recv failed !","NTPBserver",MB_ICONERROR | MB_OK);
		return -1;
	}

	ntpbpktSize = *((unsigned short *)&pbuf[6]);
	packetSize = ntpbpktSize + ntpb_hdrSize;

	recv_size = rcvSize;

	// fragmented packet handling
	while (recv_size < packetSize) {
		rcvSize = recv(client_socket, &pktbuffer[recv_size], sizeof(pktbuffer) - recv_size, 0);
		if (rcvSize <= 0) {
			MessageBox(GetActiveWindow(),"Error: recv failed !","NTPBserver",MB_ICONERROR | MB_OK);
			return -1;
		}
		recv_size += rcvSize;
	}

	// parses packet
	if (check_ntpb_header(pbuf)) {
		ntpbCmd = *((unsigned short *)&pbuf[8]);

		switch(ntpbCmd) { // treat Client Request here

			case NTPBCMD_GET_REMOTE_CMD:

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = (unsigned int)remote_cmd;
				ntpbpktSize = ntpb_hdrSize + 6;

				if ((remote_cmd == REMOTE_CMD_HALT) || (remote_cmd == REMOTE_CMD_RESUME))
						remote_cmd = REMOTE_CMD_NONE;
				break;

			case NTPBCMD_PRINT_EEDUMP:
			case NTPBCMD_PRINT_IOPDUMP:
			case NTPBCMD_PRINT_KERNELDUMP:
			case NTPBCMD_PRINT_SCRATCHPADDUMP:

				WriteFile(fh_dump, &pktbuffer[ntpb_hdrSize], ntpbpktSize, &sizeWritten, NULL);
				dump_wpos += sizeWritten;

				SendMessage(hwndProgressBardumpState, PBM_STEPIT, 0, 0);
				UpdateWindow(hwndMain);

				if (dump_wpos >= dump_size) {
					Sleep(100);
					SendMessage(hwndProgressBardumpState, PBM_SETPOS, 0, 0);
					UpdateWindow(hwndMain);

					remote_cmd = REMOTE_CMD_NONE;
				}

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				ntpbpktSize = ntpb_hdrSize + 2;
				break;

			case NTPBCMD_GET_EEDUMP_START:
/*
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_EEDUMPSTART));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_EEDUMPSTART, startbuf, ln + 1);
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_EEDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_EEDUMPEND, endbuf, ln + 1);

				sprintf(tmp_file, "dump@%s-%s-%03d.raw", startbuf, endbuf, eedump_index);
				strcpy(eedump_file, eedump_dir);
				strcat(eedump_file, "\\");
				strcat(eedump_file, tmp_file);

				sprintf(tmp, "%s created\r\n", tmp_file);
				PrintLog(tmp, strlen(tmp), IDC_TEXTBOX_EEDUMP);
*/
				//I'll use a global struct here to get the needed info from my search/dump fuctions

				fh_dump = CreateFile(ntpbVars.FileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				eedump_index++;

				dump_size = ntpbVars.DumpAreaEnd - ntpbVars.DumpAreaStart;
				dump_wpos = 0;

				SendMessage(hwndProgressBardumpState, PBM_SETRANGE, 0, MAKELPARAM(0, dump_size/8192));
          		SendMessage(hwndProgressBardumpState, PBM_SETSTEP, 1, 0);
				SendMessage(hwndProgressBardumpState, PBM_SETBARCOLOR, 0, RGB(23, 219, 38));

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = ntpbVars.DumpAreaStart;
				ntpbpktSize = ntpb_hdrSize + 6;
				break;

			case NTPBCMD_GET_EEDUMP_END:

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_EEDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_EEDUMPEND, startbuf, ln + 1);
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = (unsigned int)HexaToDecimal(startbuf);
				ntpbpktSize = ntpb_hdrSize + 6;
				break;

			case NTPBCMD_GET_IOPDUMP_START:

				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_IOPDUMPSTART));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_IOPDUMPSTART, startbuf, ln + 1);
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_IOPDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_IOPDUMPEND, endbuf, ln + 1);

				sprintf(tmp_file, "dump@%s-%s-%03d.raw", startbuf, endbuf, iopdump_index);
				strcpy(iopdump_file, iopdump_dir);
				strcat(iopdump_file, "\\");
				strcat(iopdump_file, tmp_file);

				sprintf(tmp, "%s created\r\n", tmp_file);
				PrintLog(tmp, strlen(tmp), IDC_TEXTBOX_IOPDUMP);

				fh_dump = CreateFile(iopdump_file, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				iopdump_index++;

				dump_size = (unsigned int)HexaToDecimal(endbuf) - (unsigned int)HexaToDecimal(startbuf);
				dump_wpos = 0;

				SendMessage(hwndProgressBardumpState, PBM_SETRANGE, 0, MAKELPARAM(0, dump_size/8192));
          		SendMessage(hwndProgressBardumpState, PBM_SETSTEP, 1, 0);
				SendMessage(hwndProgressBardumpState, PBM_SETBARCOLOR, 0, RGB(23, 219, 38));

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = (unsigned int)HexaToDecimal(startbuf);
				ntpbpktSize = ntpb_hdrSize + 6;
				break;

			case NTPBCMD_GET_IOPDUMP_END:

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_IOPDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_IOPDUMPEND, startbuf, ln + 1);
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = (unsigned int)HexaToDecimal(startbuf);
				ntpbpktSize = ntpb_hdrSize + 6;
				break;

			case NTPBCMD_GET_KERNELDUMP_START:

				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_KERNELDUMPSTART));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_KERNELDUMPSTART, startbuf, ln + 1);
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_KERNELDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_KERNELDUMPEND, endbuf, ln + 1);

				sprintf(tmp_file, "dump@%s-%s-%03d.raw", startbuf, endbuf, kerneldump_index);
				strcpy(kerneldump_file, kerneldump_dir);
				strcat(kerneldump_file, "\\");
				strcat(kerneldump_file, tmp_file);

				sprintf(tmp, "%s created\r\n", tmp_file);
				PrintLog(tmp, strlen(tmp), IDC_TEXTBOX_KERNELDUMP);

				fh_dump = CreateFile(kerneldump_file, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				kerneldump_index++;

				dump_size = (unsigned int)HexaToDecimal(endbuf) - (unsigned int)HexaToDecimal(startbuf);
				dump_wpos = 0;

				SendMessage(hwndProgressBardumpState, PBM_SETRANGE, 0, MAKELPARAM(0, dump_size/8192));
          		SendMessage(hwndProgressBardumpState, PBM_SETSTEP, 1, 0);
				SendMessage(hwndProgressBardumpState, PBM_SETBARCOLOR, 0, RGB(23, 219, 38));

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = (unsigned int)HexaToDecimal(startbuf);
				ntpbpktSize = ntpb_hdrSize + 6;
				break;

			case NTPBCMD_GET_KERNELDUMP_END:

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_KERNELDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_KERNELDUMPEND, startbuf, ln + 1);
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = (unsigned int)HexaToDecimal(startbuf);
				ntpbpktSize = ntpb_hdrSize + 6;
				break;

			case NTPBCMD_GET_SCRATCHPADDUMP_START:

				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPSTART));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPSTART, startbuf, ln + 1);
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPEND, endbuf, ln + 1);

				sprintf(tmp_file, "dump@%s-%s-%03d.raw", startbuf, endbuf, scratchpaddump_index);
				strcpy(scratchpaddump_file, scratchpaddump_dir);
				strcat(scratchpaddump_file, "\\");
				strcat(scratchpaddump_file, tmp_file);

				sprintf(tmp, "%s created\r\n", tmp_file);
				PrintLog(tmp, strlen(tmp), IDC_TEXTBOX_SCRATCHPADDUMP);

				fh_dump = CreateFile(scratchpaddump_file, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				scratchpaddump_index++;

				dump_size = (unsigned int)HexaToDecimal(endbuf) - (unsigned int)HexaToDecimal(startbuf);
				dump_wpos = 0;

				SendMessage(hwndProgressBardumpState, PBM_SETRANGE, 0, MAKELPARAM(0, dump_size/8192));
          		SendMessage(hwndProgressBardumpState, PBM_SETSTEP, 1, 0);
				SendMessage(hwndProgressBardumpState, PBM_SETBARCOLOR, 0, RGB(23, 219, 38));

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = (unsigned int)HexaToDecimal(startbuf);
				ntpbpktSize = ntpb_hdrSize + 6;
				break;

			case NTPBCMD_GET_SCRATCHPADDUMP_END:

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPEND, startbuf, ln + 1);
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = (unsigned int)HexaToDecimal(startbuf);
				ntpbpktSize = ntpb_hdrSize + 6;
				break;

		}

		// send the response packet
		sndSize = send(client_socket, &pktbuffer[0], ntpbpktSize, 0);
		if (sndSize <= 0) {
			MessageBox(GetActiveWindow(),"Error: send failed !","NTPBserver",MB_ICONERROR | MB_OK);
			return -1;
		}
	}
	else {
		MessageBox(GetActiveWindow(),"received unknown packet(not NTPB) !!!","NTPBserver",MB_ICONERROR | MB_OK);
	}

	return 0;
}

/*<---------------------------------------------------------------------->*/
DWORD WINAPI serverThread(LPVOID lpParam) // Server thread: Handle Client & packets
{
	int tcp_socket, client_socket;
	struct sockaddr_in peer;
	int peerlen, r;
	int rcvSize;

	fd_set fd; 			// create a file descriptor set called fd
	fd_set active_fd;

	FD_ZERO(&fd); 		// zero out our fd (otherwise this program tends to crash)
	struct timeval tv;  // create a time value object
	tv.tv_usec = 0;

	peer.sin_family = AF_INET;
	peer.sin_port = htons(SERVER_PORT);
	peer.sin_addr.s_addr = htonl(INADDR_ANY);

	// create the socket
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket < 0) {
		MessageBox(GetActiveWindow(),"Error: failed to create socket !","NTPBserver",MB_ICONERROR | MB_OK);
		goto error;
	}

	r = bind(tcp_socket,(struct sockaddr *)&peer,sizeof(peer));
	if (r < 0) {
		MessageBox(GetActiveWindow(),"Error: socket failed to bind !","NTPBserver",MB_ICONERROR | MB_OK);
		goto error;
	}

	r = listen(tcp_socket, 3);
	if (r < 0) {
		MessageBox(GetActiveWindow(),"Error: listen failed !","NTPBserver",MB_ICONERROR | MB_OK);
		goto error;
	}
	UpdateStatusBar("Awaiting PS2 Client connection...", 0, 0);


	// awaiting Client connection
	FD_SET(tcp_socket, &fd);	// Oddly enough, it seems like you need to call select() once to set the fd
								// value; otherwise it will think that the socket is ready to receive, even if isn't
								// To make the select() go fast, we'll set our time value to zero, then re-set
								// when select() is done.
	tv.tv_sec = 0;
	r = select(1, &fd, NULL, NULL, &tv);
	if (r < 0) {
		MessageBox(GetActiveWindow(),"Error: select failed !","NTPBserver",MB_ICONERROR | MB_OK);
		goto error;
	}

	tv.tv_sec = 86400;
	while (!FD_ISSET(tcp_socket, &fd)) { // if socket hasn't received any data yet...

        FD_SET(tcp_socket, &fd); 		 // re-set the fd so select() will work
        r = select(1, &fd, NULL, NULL, &tv);
		if (r < 0) {
			MessageBox(GetActiveWindow(),"Error: select failed !","NTPBserver",MB_ICONERROR | MB_OK);
			goto error;
		}

        if (!FD_ISSET(tcp_socket, &fd)) {
			MessageBox(GetActiveWindow(),"86400 seconds without Client connection...","NTPBserver",MB_ICONERROR | MB_OK);
			goto error;
    	}
	}

	peerlen = sizeof(peer);
	r = accept(tcp_socket,(struct sockaddr *)&peer, &peerlen);
	if (r < 0) {
		MessageBox(GetActiveWindow(),"Error: accept failed !","NTPBserver",MB_ICONERROR | MB_OK);
		goto error;
	}

	client_socket = r;

	UpdateStatusBar("PS2 Client connection OK", 0, 0);
	UpdateWindow(hwndMain);


	// waiting for packets
	FD_SET(client_socket, &fd);
	tv.tv_sec = 0;
	r = select(1, &fd, NULL, NULL, &tv);
	if (r < 0) {
		MessageBox(GetActiveWindow(),"Error: select failed !","NTPBserver",MB_ICONERROR | MB_OK);
		goto error;
	}

	UpdateStatusBar("Listening PS2 Client...", 0, 0);
	ClientConnected = 1;

	while (1) {
		tv.tv_sec = 3;
		while (!FD_ISSET(client_socket, &fd)) { // if socket hasn't received any data yet...

        	FD_SET(client_socket, &fd); 		// re-set the fd so select() will work
        	r = select(1, &fd, NULL, NULL, &tv);
			if (r < 0) {
				MessageBox(GetActiveWindow(),"Error: select failed !","NTPBserver",MB_ICONERROR | MB_OK);
				goto error;
			}
    	}

		r = receivePacket(client_socket);
		if (r < 0)
			goto error;
	}

	return 1;


error:
	//Setting remote_cmd to a value to designate error status for verification of dump success/fail.
	//hopefully
	remote_cmd = REMOTE_CMD_ERROR;
//	PostMessage(hwndMain,WM_CLOSE,0,0);
	return 0;
}


/*<---------------------------------------------------------------------->*/
