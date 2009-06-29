/*@@ Wedit generated application. Written Sat Jun 27 19:55:04 2009
 @@header: d:\msys\1.0\local\ps2dev\task2308\ntpbserver_pc\ntpbserverres.h
 @@resources: d:\msys\1.0\local\ps2dev\task2308\ntpbserver_pc\ntpbserver.rc
 Do not edit outside the indicated areas */
/*<---------------------------------------------------------------------->*/
/*<---------------------------------------------------------------------->*/
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "ntpbserverres.h"

#define SERVER_PORT  	4234

char pktbuffer[65536];
char launch_path[2048];
char dump_dir[2048];
char eedump_dir[2048];
char iopdump_dir[2048];
char eedump_file[2048];
char iopdump_file[2048];

int eedump_index, iopdump_index;
unsigned int eedump_size, iopdump_size;
unsigned int eedump_wpos, iopdump_wpos;
HANDLE fh_eedump, fh_iopdump;


// NTPB header magic
#define ntpb_MagicSize  6
const unsigned char *ntpb_hdrMagic = "\xff\x00NTPB";
#define ntpb_hdrSize  	10

#define NTPBCMD_GET_EEDUMP_START 	0x100
#define NTPBCMD_GET_EEDUMP_END 		0x101
#define NTPBCMD_GET_IOPDUMP_START	0x102
#define NTPBCMD_GET_IOPDUMP_END		0x103
#define NTPBCMD_PRINT_EEDUMP 		0x200
#define NTPBCMD_PRINT_IOPDUMP		0x201

/*<---------------------------------------------------------------------->*/
HINSTANCE hInst;				// Instance handle
HWND hwndMain;					// Main window handle
HWND hwndLabelEEdump;			// Label control handle
HWND hwndLabelIOPdump;  		// Label control handle
HWND hwndLabelEEdumpStart;  	// Label control handle
HWND hwndLabelEEdumpEnd;  		// Label control handle
HWND hwndLabelIOPdumpStart; 	// Label control handle
HWND hwndLabelIOPdumpEnd;   	// Label control handle
HWND hwndTextBoxEEdump;			// Edit control handle
HWND hwndTextBoxIOPdump;		// Edit control handle
HWND hwndTextBoxEEdumpStart;	// Edit control handle
HWND hwndTextBoxEEdumpEnd;		// Edit control handle
HWND hwndTextBoxIOPdumpStart;	// Edit control handle
HWND hwndTextBoxIOPdumpEnd;		// Edit control handle
HWND hwndProgressBarEEdumpState;		// Progress Bar control handle
HWND hwndProgressBarIOPdumpState;		// Progress Bar control handle
HWND hWndStatusbar;				// StatusBar handle

WSADATA *WsaData;
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

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

/*<---------------------------------------------------------------------->*/
static BOOL CreateSBar(HWND hwndParent,char *initialText,int nrOfParts)
{
    hWndStatusbar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_BORDER|SBARS_SIZEGRIP,
                                       initialText,
                                       hwndParent,
                                       IDM_STATUSBAR);
    if(hWndStatusbar)
    {
        InitializeStatusBar(hwndParent,nrOfParts);
        return TRUE;
    }

    return FALSE;
}

/*<---------------------------------------------------------------------->*/
/*@@0->@@*/
static BOOL InitApplication(void)
{
	WNDCLASS wc;

	memset(&wc,0,sizeof(WNDCLASS));
	wc.style = CS_HREDRAW|CS_VREDRAW |CS_DBLCLKS ;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hInstance = hInst;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = "ntpbserverWndClass";
	wc.lpszMenuName = MAKEINTRESOURCE(IDMAINMENU);
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDMAINICON));
	if (!RegisterClass(&wc))
		return 0;
/*@@0<-@@*/
	// ---TODO--- Call module specific initialization routines here

	return 1;
}

/*<---------------------------------------------------------------------->*/
/*@@1->@@*/
HWND CreatentpbserverWndClassWnd(void)
{
	return CreateWindow("ntpbserverWndClass","ntpbserver - jimmikaelkael",
		WS_MINIMIZEBOX|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CAPTION|WS_BORDER|WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,800,512,
		NULL,
		NULL,
		hInst,
		NULL);
}
/*@@1<-@@*/
/*<---------------------------------------------------------------------->*/
/* --- The following code comes from c:\lcc\lib\wizard\defOnCmd.tpl. */
void MainWndProc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id) {
		// ---TODO--- Add new menu commands here
		/*@@NEWCOMMANDS@@*/
		case IDM_EXIT:
		PostMessage(hwnd,WM_CLOSE,0,0);
		break;
	}
}

/*<---------------------------------------------------------------------->*/
/*@@2->@@*/
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg) {
/*@@3->@@*/
	case WM_SIZE:
		SendMessage(hWndStatusbar,msg,wParam,lParam);
		InitializeStatusBar(hWndStatusbar,1);
		break;
	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd,wParam,lParam,MainWndProc_OnCommand);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
/*@@3<-@@*/
	return 0;
}
/*@@2<-@@*/
/*<---------------------------------------------------------------------->*/
/* --- The following code comes from c:\lcc\lib\wizard\tcpini.tpl. */
// WSADATA *InitWS2(void)
// Routine Description:
//
// Calls WSAStartup, makes sure we have a good version of WinSock2
//
//
// Return Value:
//  A pointer to a WSADATA structure - WinSock 2 DLL successfully started up
//  NULL - Error starting up WinSock 2 DLL.
//
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
	int rcvSize, sndSize, packetSize, ntpbpktSize, ntpbCmd, ln, recv_size, sizeWritten;
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

			case NTPBCMD_PRINT_EEDUMP:

				WriteFile(fh_eedump, &pktbuffer[ntpb_hdrSize], ntpbpktSize, &sizeWritten, NULL);
				eedump_wpos += sizeWritten;

				SendMessage(hwndProgressBarEEdumpState, PBM_STEPIT, 0, 0);

				if (eedump_wpos >= eedump_size) {
					SendMessage(hwndProgressBarEEdumpState, PBM_SETPOS, 0, 0);
					UpdateWindow(hwndMain);
				}

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				ntpbpktSize = ntpb_hdrSize + 2;
				break;

			case NTPBCMD_PRINT_IOPDUMP:

				WriteFile(fh_iopdump, &pktbuffer[ntpb_hdrSize], ntpbpktSize, &sizeWritten, NULL);
				iopdump_wpos += sizeWritten;

				SendMessage(hwndProgressBarIOPdumpState, PBM_STEPIT, 0, 0);

				if (iopdump_wpos >= iopdump_size) {
					SendMessage(hwndProgressBarIOPdumpState, PBM_SETPOS, 0, 0);
					UpdateWindow(hwndMain);
				}

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				ntpbpktSize = ntpb_hdrSize + 2;
				break;

			case NTPBCMD_GET_EEDUMP_START:

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

				fh_eedump = CreateFile(eedump_file, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				eedump_index++;

				eedump_size = (unsigned int)HexaToDecimal(endbuf) - (unsigned int)HexaToDecimal(startbuf);
				eedump_wpos = 0;

				SendMessage(hwndProgressBarEEdumpState, PBM_SETRANGE, 0, MAKELPARAM(0, eedump_size/8192));
          		SendMessage(hwndProgressBarEEdumpState, PBM_SETSTEP, 1, 0);
				SendMessage(hwndProgressBarEEdumpState, PBM_SETBARCOLOR, 0, RGB(23, 219, 38));

				*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
				*((unsigned int *)&pktbuffer[ntpb_hdrSize + 2]) = (unsigned int)HexaToDecimal(startbuf);
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

				fh_iopdump = CreateFile(iopdump_file, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				iopdump_index++;

				iopdump_size = (unsigned int)HexaToDecimal(endbuf) - (unsigned int)HexaToDecimal(startbuf);
				iopdump_wpos = 0;

				SendMessage(hwndProgressBarIOPdumpState, PBM_SETRANGE, 0, MAKELPARAM(0, iopdump_size/8192));
          		SendMessage(hwndProgressBarIOPdumpState, PBM_SETSTEP, 1, 0);
				SendMessage(hwndProgressBarIOPdumpState, PBM_SETBARCOLOR, 0, RGB(23, 219, 38));

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
	PostMessage(hwndMain,WM_CLOSE,0,0);
	return 0;
}

/*<---------------------------------------------------------------------->*/
VOID CreateControls(HINSTANCE hInstance)
{
	// Create all custom controls.

	hwndLabelEEdumpStart = CreateWindow (TEXT("static"),
                               "start(HEX):",
                               WS_CHILD|WS_VISIBLE,
                               20,20,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_EEDUMPSTART,
                               hInstance,
                               NULL);

	hwndTextBoxEEdumpStart = CreateWindow (TEXT("edit"),
                               "00100000",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               95,18,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_EEDUMPSTART,
                               hInstance,
                               NULL);

	hwndLabelEEdumpEnd = CreateWindow (TEXT("static"),
                               "end(HEX):",
                               WS_CHILD|WS_VISIBLE,
                               200,20,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_EEDUMPEND,
                               hInstance,
                               NULL);

	hwndTextBoxEEdumpEnd = CreateWindow (TEXT("edit"),
                               "02000000",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               270,18,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_EEDUMPEND,
                               hInstance,
                               NULL);

	hwndLabelIOPdumpStart = CreateWindow (TEXT("static"),
                               "start(HEX):",
                               WS_CHILD|WS_VISIBLE,
                               400,20,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_IOPDUMPSTART,
                               hInstance,
                               NULL);

	hwndTextBoxIOPdumpStart = CreateWindow (TEXT("edit"),
                               "00000000",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               475,18,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_IOPDUMPSTART,
                               hInstance,
                               NULL);

	hwndLabelIOPdumpEnd = CreateWindow (TEXT("static"),
                               "end(HEX):",
                               WS_CHILD|WS_VISIBLE,
                               580,20,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_IOPDUMPEND,
                               hInstance,
                               NULL);

	hwndTextBoxIOPdumpEnd = CreateWindow (TEXT("edit"),
                               "00200000",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               650,18,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_IOPDUMPEND,
                               hInstance,
                               NULL);

	hwndLabelEEdump = CreateWindow (TEXT("static"),
                               "EE dump:",
                               WS_CHILD|WS_VISIBLE,
                               20,52,
                               380,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_EEDUMP,
                               hInstance,
                               NULL);

	hwndTextBoxEEdump = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY,
                               20,70,
                               380,342,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_EEDUMP,
                               hInstance,
                               NULL);

	hwndLabelIOPdump = CreateWindow (TEXT("static"),
                               "IOP dump:",
                               WS_CHILD|WS_VISIBLE,
                               400,52,
                               380,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_EEDUMP,
                               hInstance,
                               NULL);

	hwndTextBoxIOPdump = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY,
                               400,70,
                               380,342,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_IOPDUMP,
                               hInstance,
                               NULL);

	hwndProgressBarEEdumpState = CreateWindowEx (0, PROGRESS_CLASS,
                               NULL,
                               WS_CHILD|WS_VISIBLE|PBS_SMOOTH,
                               20,420,
                               362,20,
                               hwndMain,
                               (HMENU)IDC_PROGRESSBAR_EEDUMPSTATE,
                               hInstance,
                               NULL);

	hwndProgressBarIOPdumpState =  CreateWindowEx (0, PROGRESS_CLASS,
                               NULL,
                               WS_CHILD|WS_VISIBLE|PBS_SMOOTH,
                               400,420,
                               362,20,
                               hwndMain,
                               (HMENU)IDC_PROGRESSBAR_IOPDUMPSTATE,
                               hInstance,
                               NULL);

}

/*<---------------------------------------------------------------------->*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	MSG msg;
	HANDLE hAccelTable;
	unsigned char *p;

	hInst = hInstance;
	if (!InitApplication())
		return 0;

	hAccelTable = LoadAccelerators(hInst,MAKEINTRESOURCE(IDACCEL));
	if ((hwndMain = CreatentpbserverWndClassWnd()) == (HWND)0)
		return 0;

	// Create our controls
	CreateControls(hInst);

	CreateSBar(hwndMain,"",1);
	ShowWindow(hwndMain,SW_SHOW);

	// Init WSA
	WsaData = InitWS2();
	if (WsaData == NULL)
		return 0;

	// Get the launch_path from command line
	strcpy(launch_path, GetCommandLine());
	p = strrchr(launch_path, '\"');
	*p = 0;
	if ((!(p = strrchr(launch_path, '/'))) && (!(p = strrchr(launch_path, '\\'))))
		p = strrchr(launch_path, ':');
	if (p)
		*(p+1) = 0;
	p = (unsigned char *)launch_path;
	while (*p == '\"')
	   p++;
  	strcpy(launch_path, p);

	// Create needed dirs
	strcpy(dump_dir, launch_path);
	strcat(dump_dir, "\\dump");
	strcpy(eedump_dir, dump_dir);
	strcat(eedump_dir, "\\EE");
	strcpy(iopdump_dir, dump_dir);
	strcat(iopdump_dir, "\\IOP");

	CreateDirectory(dump_dir, NULL);
	CreateDirectory(eedump_dir, NULL);
	CreateDirectory(iopdump_dir, NULL);

	eedump_index = 0;
	iopdump_index = 0;

	// Create & start the server thread
	HANDLE serverThid = CreateThread(NULL, 0, serverThread, NULL, 0, NULL); // no stack, 1MB by default

	// API message loop
	while (GetMessage(&msg,NULL,0,0)) {
		if (!TranslateAccelerator(msg.hwnd,hAccelTable,&msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// shutdown WSA
	WSACleanup();

	return msg.wParam;
}

