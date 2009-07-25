/*@@ Wedit generated application. Written Wed Jul 08 16:30:58 2009
 @@header: d:\msys\1.0\local\ps2dev\core_launcherv2\ntpbclient_win32\ntpbclient_win32res.h
 @@resources: d:\msys\1.0\local\ps2dev\core_launcherv2\ntpbclient_win32\ntpbclient_win32.rc
 Do not edit outside the indicated areas */
/*<---------------------------------------------------------------------->*/
/*<---------------------------------------------------------------------->*/
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "ntpbclient_win32res.h"


#define SERVER_TCP_PORT 4234
#define SERVER_UDP_PORT 4244
#define SERVER_IP		"192.168.0.80"

static int main_socket = -1;

char pktbuffer[65536];
char netlogbuffer[1024];
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

static int ClientConnected = 0;
static int haltState = 0;

int eedump_index, iopdump_index, kerneldump_index, scratchpaddump_index;
unsigned int dump_size;
unsigned int dump_wpos;
HANDLE fh_dump;

// NTPB header magic
#define ntpb_MagicSize  6
const unsigned char *ntpb_hdrMagic = "\xff\x00NTPB";
#define ntpb_hdrSize  	10

#define NTPBCMD_PRINT_EEDUMP 				0x301
#define NTPBCMD_PRINT_IOPDUMP				0x302
#define NTPBCMD_PRINT_KERNELDUMP 			0x303
#define NTPBCMD_PRINT_SCRATCHPADDUMP		0x304
#define NTPBCMD_END_TRANSMIT				0xffff

#define REMOTE_CMD_NONE						0x000
#define REMOTE_CMD_DUMPEE					0x101
#define REMOTE_CMD_DUMPIOP					0x102
#define REMOTE_CMD_DUMPKERNEL				0x103
#define REMOTE_CMD_DUMPSCRATCHPAD			0x104
#define REMOTE_CMD_HALT						0x201
#define REMOTE_CMD_RESUME					0x202
#define REMOTE_CMD_ADDMEMPATCHES			0x501
#define REMOTE_CMD_CLEARMEMPATCHES			0x502
#define REMOTE_CMD_ADDRAWCODES				0x601
#define REMOTE_CMD_CLEARRAWCODES			0x602

static int remote_cmd;

/*<---------------------------------------------------------------------->*/
HINSTANCE hInst;					// Instance handle

HANDLE clientThid;

HWND hwndMain;						// Main window handle
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
HWND hwndLabelServerLog;		   	// Label control handle
HWND hwndLabelAddr;				   	// Label control handle
HWND hwndLabelVal;				   	// Label control handle
HWND hwndLabelCodes;				// Label control handle

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
HWND hwndTextBoxServerLog;			// Edit control handle
HWND hwndTextBoxPatchMem;			// Edit control handle
HWND hwndTextBoxPatchVal;			// Edit control handle
HWND hwndTextBoxCode1;				// Edit control handle
HWND hwndTextBoxCode2;				// Edit control handle

HWND hwndButtonEEdump;				// Button control handle
HWND hwndButtonIOPdump;				// Button control handle
HWND hwndButtonKerneldump;			// Button control handle
HWND hwndButtonScratchpaddump;		// Button control handle

HWND hwndButtonHaltResume;			// Button control handle
HWND hwndButtonPatchMem;			// Button control handle
HWND hwndButtonUnPatchMem;			// Button control handle
HWND hwndButtonAddCodes;			// Button control handle
HWND hwndButtonClearCodes;			// Button control handle

HWND hwndProgressBardumpState;		// Progress Bar control handle
HWND hWndStatusbar;					// StatusBar handle

WSADATA *WsaData;
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
DWORD HexaToDecimal(const char* pszHexa);
int SendRemoteCmd(int cmd, unsigned char *buf, int size);
int rcvReply(void);
int PrintLog(void *buf, int size, int ResultControlID);
DWORD WINAPI clientThread(LPVOID lpParam);
DWORD WINAPI rcvDataThread(LPVOID lpParam); // retrieving a packet sent by the Client

#define MAX_PATCHES 	256
char *patchaddr[MAX_PATCHES], *patchval[MAX_PATCHES];

#define MAX_CODES 	256
char *code_1[MAX_CODES], *code_2[MAX_CODES];


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
	wc.lpszClassName = "ntpbclient_win32WndClass";
	wc.lpszMenuName = MAKEINTRESOURCE(IDMAINMENU);
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	if (!RegisterClass(&wc))
		return 0;
/*@@0<-@@*/
	// ---TODO--- Call module specific initialization routines here

	return 1;
}

/*<---------------------------------------------------------------------->*/
/*@@1->@@*/
HWND Createntpbclient_win32WndClassWnd(void)
{
	return CreateWindow("ntpbclient_win32WndClass","ntpbclient_win32",
		WS_MINIMIZEBOX|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CAPTION|WS_BORDER|WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,1024,685,
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
	int ln, numcodes, codestosend, numcodes_sent;
	unsigned char buf[128];
	char startbuf[128], endbuf[128], tmp[128], tmp_file[128];
	char patchbuf[4096];
	char cmdBuf[64];

	switch(id) {
		// ---TODO--- Add new menu commands here
		/*@@NEWCOMMANDS@@*/
		case IDM_EXIT:
			PostMessage(hwnd,WM_CLOSE,0,0);
			break;

		case IDM_RECONNECT:
			if (remote_cmd == REMOTE_CMD_NONE) {
				TerminateThread(clientThid, 0);
				clientThid = CreateThread(NULL, 0, clientThread, NULL, 0, NULL); // no stack, 1MB by default
			}
			break;

		case IDC_BUTTON_EEDUMP:
			if ((ClientConnected) && (remote_cmd == REMOTE_CMD_NONE)) {
				remote_cmd = REMOTE_CMD_DUMPEE;
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_EEDUMPSTART));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_EEDUMPSTART, startbuf, ln + 1);
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_EEDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_EEDUMPEND, endbuf, ln + 1);
				*((unsigned int *)&buf[0]) = (unsigned int)HexaToDecimal(startbuf);
				*((unsigned int *)&buf[4]) = (unsigned int)HexaToDecimal(endbuf);

				sprintf(tmp_file, "dump@%s-%s-%03d.raw", startbuf, endbuf, eedump_index);
				strcpy(eedump_file, eedump_dir);
				strcat(eedump_file, "\\");
				strcat(eedump_file, tmp_file);

				sprintf(tmp, "%s created\r\n", tmp_file);
				PrintLog(tmp, strlen(tmp), IDC_TEXTBOX_EEDUMP);

				fh_dump = CreateFile(eedump_file, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				eedump_index++;

				dump_size = (unsigned int)HexaToDecimal(endbuf) - (unsigned int)HexaToDecimal(startbuf);
				dump_wpos = 0;

				SendMessage(hwndProgressBardumpState, PBM_SETRANGE, 0, MAKELPARAM(0, dump_size/8192));
          		SendMessage(hwndProgressBardumpState, PBM_SETSTEP, 1, 0);
				SendMessage(hwndProgressBardumpState, PBM_SETBARCOLOR, 0, RGB(23, 219, 38));

				SendRemoteCmd(remote_cmd, buf, 8);
				HANDLE rcvDataThid = CreateThread(NULL, 0, rcvDataThread, NULL, 0, NULL); // no stack, 1MB by default
			}
			break;

		case IDC_BUTTON_IOPDUMP:
			if ((ClientConnected) && (remote_cmd == REMOTE_CMD_NONE)) {
				remote_cmd = REMOTE_CMD_DUMPIOP;
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_IOPDUMPSTART));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_IOPDUMPSTART, startbuf, ln + 1);
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_IOPDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_IOPDUMPEND, endbuf, ln + 1);
				*((unsigned int *)&buf[0]) = (unsigned int)HexaToDecimal(startbuf);
				*((unsigned int *)&buf[4]) = (unsigned int)HexaToDecimal(endbuf);

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

				SendRemoteCmd(remote_cmd, buf, 8);
				HANDLE rcvDataThid = CreateThread(NULL, 0, rcvDataThread, NULL, 0, NULL); // no stack, 1MB by default
			}
			break;

		case IDC_BUTTON_KERNELDUMP:
			if ((ClientConnected) && (remote_cmd == REMOTE_CMD_NONE)) {
				remote_cmd = REMOTE_CMD_DUMPKERNEL;
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_KERNELDUMPSTART));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_KERNELDUMPSTART, startbuf, ln + 1);
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_KERNELDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_KERNELDUMPEND, endbuf, ln + 1);
				*((unsigned int *)&buf[0]) = (unsigned int)HexaToDecimal(startbuf);
				*((unsigned int *)&buf[4]) = (unsigned int)HexaToDecimal(endbuf);

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

				SendRemoteCmd(remote_cmd, buf, 8);
				HANDLE rcvDataThid = CreateThread(NULL, 0, rcvDataThread, NULL, 0, NULL); // no stack, 1MB by default
			}
			break;

		case IDC_BUTTON_SCRATCHPADDUMP:
			if ((ClientConnected) && (remote_cmd == REMOTE_CMD_NONE)) {
				remote_cmd = REMOTE_CMD_DUMPSCRATCHPAD;
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPSTART));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPSTART, startbuf, ln + 1);
				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPEND));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_SCRATCHPADDUMPEND, endbuf, ln + 1);
				*((unsigned int *)&buf[0]) = (unsigned int)HexaToDecimal(startbuf);
				*((unsigned int *)&buf[4]) = (unsigned int)HexaToDecimal(endbuf);

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

				SendRemoteCmd(remote_cmd, buf, 8);
				HANDLE rcvDataThid = CreateThread(NULL, 0, rcvDataThread, NULL, 0, NULL); // no stack, 1MB by default
			}
			break;

		case IDC_BUTTON_HALTRESUME:
			if ((ClientConnected) && (remote_cmd == REMOTE_CMD_NONE)) {
				if (haltState) {
					haltState = 0;
					remote_cmd = REMOTE_CMD_RESUME;
					SendRemoteCmd(remote_cmd, NULL, 0);
					SetDlgItemText(hwndMain, IDC_BUTTON_HALTRESUME, "Halt");
					UpdateWindow(hwndMain);
					HANDLE rcvDataThid = CreateThread(NULL, 0, rcvDataThread, NULL, 0, NULL); // no stack, 1MB by default
				}
				else {
					haltState = 1;
					remote_cmd = REMOTE_CMD_HALT;
					SendRemoteCmd(remote_cmd, NULL, 0);
					SetDlgItemText(hwndMain, IDC_BUTTON_HALTRESUME, "Resume");
					UpdateWindow(hwndMain);
					HANDLE rcvDataThid = CreateThread(NULL, 0, rcvDataThread, NULL, 0, NULL); // no stack, 1MB by default
				}
			}
			break;

		case IDC_BUTTON_PATCHMEM:
			if ((ClientConnected) && (remote_cmd == REMOTE_CMD_NONE)) {
				remote_cmd = REMOTE_CMD_ADDMEMPATCHES;

				int i, j, z, addr_count, val_count;

				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_PATCHMEM));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_PATCHMEM, patchbuf, ln + 1);

			    j = 0;
				z = 0;
				for (i=0; i<ln+1; i++) {
					if ((patchbuf[i] == '\r') || (patchbuf[i] == '\0')) {
						if (i-j > 0) {
							patchaddr[z] = (char*)GlobalAlloc(GPTR, i-j+1);
							memcpy(patchaddr[z], &patchbuf[j], i-j);
							patchaddr[z][i-j] = 0;
							//MessageBox(GetActiveWindow(),patchaddr[z],"ntpbclient",MB_ICONERROR | MB_OK);
							z++;
						}
						j = i + 2;
					}
				}
				addr_count = z;

				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_PATCHVAL));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_PATCHVAL, patchbuf, ln + 1);

			    j = 0;
				z = 0;
				for (i=0; i<ln+1; i++) {
					if ((patchbuf[i] == '\r') || (patchbuf[i] == '\0')) {
						if (i-j > 0) {
							patchval[z] = (char*)GlobalAlloc(GPTR, i-j+1);
							memcpy(patchval[z], &patchbuf[j], i-j);
							patchval[z][i-j] = 0;
							//MessageBox(GetActiveWindow(),patchval[z],"ntpbclient",MB_ICONERROR | MB_OK);
							z++;
						}
						j = i + 2;
					}
				}
				val_count = z;

				if ((addr_count == val_count) && (addr_count <= MAX_PATCHES)) {
					*((unsigned int *)&buf[0]) = addr_count;
					z = 4;
					for (i=0; i<addr_count; i++) {
						*((unsigned int *)&buf[z]) = (unsigned int)(HexaToDecimal(patchaddr[i]) | 0x20000000);
						*((unsigned int *)&buf[z+4]) = (unsigned int)HexaToDecimal(patchval[i]);
						z += 8;
					}

					numcodes = addr_count;
					numcodes_sent = 0;

					while (numcodes_sent < numcodes) {

						codestosend = numcodes - numcodes_sent;

						if (codestosend > 6)
							codestosend = 6;

						*((unsigned int *)&cmdBuf[0]) = codestosend;
						memcpy(&cmdBuf[4], &buf[(numcodes_sent * 8) + 4], codestosend * 8);

						SendRemoteCmd(remote_cmd, cmdBuf, (codestosend * 8) + 4);
						rcvReply();
						numcodes_sent += codestosend;
					}
				}
				else {
					if (addr_count > MAX_PATCHES)
						MessageBox(GetActiveWindow(),"Error: too much patches...","ntpbclient",MB_ICONERROR | MB_OK);
					else
						MessageBox(GetActiveWindow(),"Error: addresses items count <> val items count...","ntpbclient",MB_ICONERROR | MB_OK);
				}

				for (i=0; i<addr_count; i++)
					GlobalFree((HANDLE)patchaddr[i]);
				for (i=0; i<val_count; i++)
					GlobalFree((HANDLE)patchval[i]);

				remote_cmd = REMOTE_CMD_NONE;
			}
			break;

		case IDC_BUTTON_UNPATCHMEM:
			if ((ClientConnected) && (remote_cmd == REMOTE_CMD_NONE)) {
				remote_cmd = REMOTE_CMD_CLEARMEMPATCHES;
				SendRemoteCmd(remote_cmd, NULL, 0);
				rcvReply();
				remote_cmd = REMOTE_CMD_NONE;
			}
			break;

		case IDC_BUTTON_ADDCODES:
			if ((ClientConnected) && (remote_cmd == REMOTE_CMD_NONE)) {
				remote_cmd = REMOTE_CMD_ADDRAWCODES;

				int i, j, z, addr_count, val_count;

				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_CODE1));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_CODE1, patchbuf, ln + 1);

			    j = 0;
				z = 0;
				for (i=0; i<ln+1; i++) {
					if ((patchbuf[i] == '\r') || (patchbuf[i] == '\0')) {
						if (i-j > 0) {
							code_1[z] = (char*)GlobalAlloc(GPTR, i-j+1);
							memcpy(code_1[z], &patchbuf[j], i-j);
							code_1[z][i-j] = 0;
							//MessageBox(GetActiveWindow(),patchaddr[z],"ntpbclient",MB_ICONERROR | MB_OK);
							z++;
						}
						j = i + 2;
					}
				}
				addr_count = z;

				ln = GetWindowTextLength(GetDlgItem(hwndMain, IDC_TEXTBOX_CODE2));
				GetDlgItemText(hwndMain, IDC_TEXTBOX_CODE2, patchbuf, ln + 1);

			    j = 0;
				z = 0;
				for (i=0; i<ln+1; i++) {
					if ((patchbuf[i] == '\r') || (patchbuf[i] == '\0')) {
						if (i-j > 0) {
							code_2[z] = (char*)GlobalAlloc(GPTR, i-j+1);
							memcpy(code_2[z], &patchbuf[j], i-j);
							code_2[z][i-j] = 0;
							//MessageBox(GetActiveWindow(),patchval[z],"ntpbclient",MB_ICONERROR | MB_OK);
							z++;
						}
						j = i + 2;
					}
				}
				val_count = z;

				if ((addr_count == val_count) && (addr_count <= MAX_CODES)) {
					*((unsigned int *)&buf[0]) = addr_count;
					z = 4;
					for (i=0; i<addr_count; i++) {
						*((unsigned int *)&buf[z]) = (unsigned int)HexaToDecimal(code_1[i]);
						*((unsigned int *)&buf[z+4]) = (unsigned int)HexaToDecimal(code_2[i]);
						z += 8;
					}

					numcodes = addr_count;
					numcodes_sent = 0;

					while (numcodes_sent < numcodes) {

						codestosend = numcodes - numcodes_sent;

						if (codestosend > 6)
							codestosend = 6;

						*((unsigned int *)&cmdBuf[0]) = codestosend;
						memcpy(&cmdBuf[4], &buf[(numcodes_sent * 8) + 4], codestosend * 8);

						SendRemoteCmd(remote_cmd, cmdBuf, (codestosend * 8) + 4);
						rcvReply();
						numcodes_sent += codestosend;
					}
				}
				else {
					if (addr_count > MAX_CODES)
						MessageBox(GetActiveWindow(),"Error: too much codes...","ntpbclient",MB_ICONERROR | MB_OK);
					else
						MessageBox(GetActiveWindow(),"Error: items count <> ...","ntpbclient",MB_ICONERROR | MB_OK);
				}

				for (i=0; i<addr_count; i++)
					GlobalFree((HANDLE)code_1[i]);
				for (i=0; i<val_count; i++)
					GlobalFree((HANDLE)code_2[i]);

				remote_cmd = REMOTE_CMD_NONE;
			}
			break;

		case IDC_BUTTON_CLEARCODES:
			if ((ClientConnected) && (remote_cmd == REMOTE_CMD_NONE)) {
				remote_cmd = REMOTE_CMD_CLEARRAWCODES;
				SendRemoteCmd(remote_cmd, NULL, 0);
				rcvReply();
				remote_cmd = REMOTE_CMD_NONE;
			}
			break;
	}
}

/*<---------------------------------------------------------------------->*/
/*@@2->@@*/
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int ret;

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
	case WM_CLOSE:
retry:
		if (remote_cmd != REMOTE_CMD_NONE)
			switch (remote_cmd) {
				case REMOTE_CMD_DUMPEE:
					ret = MessageBox(GetActiveWindow(),"You can't exit right now !!! Please wait EE dump finished\r\nand request application shutdown again...","ntpbclient",MB_ICONERROR | MB_ABORTRETRYIGNORE);
					switch (ret) {
						case IDABORT:
							break;
						case IDRETRY:
							goto retry;
							break;
						case IDIGNORE:
							return DefWindowProc(hwnd,msg,wParam,lParam);
							break;
					}
					break;
				case REMOTE_CMD_DUMPIOP:
					ret = MessageBox(GetActiveWindow(),"You can't exit right now !!! Please wait IOP dump finished\r\nand request application shutdown again...","ntpbclient",MB_ICONERROR | MB_ABORTRETRYIGNORE);
					switch (ret) {
						case IDABORT:
							break;
						case IDRETRY:
							goto retry;
							break;
						case IDIGNORE:
							return DefWindowProc(hwnd,msg,wParam,lParam);
							break;
					}
					break;
				case REMOTE_CMD_DUMPKERNEL:
					ret = MessageBox(GetActiveWindow(),"You can't exit right now !!! Please wait Kernel dump finished\r\nand request application shutdown again...","ntpbclient",MB_ICONERROR | MB_ABORTRETRYIGNORE);
					switch (ret) {
						case IDABORT:
							break;
						case IDRETRY:
							goto retry;
							break;
						case IDIGNORE:
							return DefWindowProc(hwnd,msg,wParam,lParam);
							break;
					}
					break;
				case REMOTE_CMD_DUMPSCRATCHPAD:
					ret = MessageBox(GetActiveWindow(),"You can't exit right now !!! Please wait ScratchPad dump finished\r\nand request application shutdown again...","ntpbclient",MB_ICONERROR | MB_ABORTRETRYIGNORE);
					switch (ret) {
						case IDABORT:
							break;
						case IDRETRY:
							goto retry;
							break;
						case IDIGNORE:
							return DefWindowProc(hwnd,msg,wParam,lParam);
							break;
					}
					break;
				case REMOTE_CMD_HALT:
					ret = MessageBox(GetActiveWindow(),"You can't exit right now !!! Please wait current cmd finished\r\nand request application shutdown again...","ntpbclient",MB_ICONERROR | MB_ABORTRETRYIGNORE);
					switch (ret) {
						case IDABORT:
							break;
						case IDRETRY:
							goto retry;
							break;
						case IDIGNORE:
							return DefWindowProc(hwnd,msg,wParam,lParam);
							break;
					}
					break;
				case REMOTE_CMD_RESUME:
					ret = MessageBox(GetActiveWindow(),"You can't exit right now !!! Please wait current cmd finished\r\nand request application shutdown again...","ntpbclient",MB_ICONERROR | MB_ABORTRETRYIGNORE);
					switch (ret) {
						case IDABORT:
							break;
						case IDRETRY:
							goto retry;
							break;
						case IDIGNORE:
							return DefWindowProc(hwnd,msg,wParam,lParam);
							break;
					}
					break;
			}
		else if (haltState) {
			ret = MessageBox(GetActiveWindow(),"You can't exit right now !!! Please resume game and request\r\napplication shutdown again...","ntpbclient",MB_ICONERROR | MB_ABORTRETRYIGNORE);
			switch (ret) {
				case IDABORT:
					break;
				case IDRETRY:
					goto retry;
					break;
				case IDIGNORE:
					return DefWindowProc(hwnd,msg,wParam,lParam);
					break;
			}
		}
		else {
			TerminateThread(clientThid, 0);
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}
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
DWORD WINAPI netlogThread(LPVOID lpParam)
{
	int udp_socket;
	struct sockaddr_in peer;
	int r, peerlen, retval;
	fd_set fd;

	peer.sin_family = AF_INET;
	peer.sin_port = htons(SERVER_UDP_PORT);
	peer.sin_addr.s_addr = htonl(INADDR_ANY);

	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_socket < 0) {
		MessageBox(GetActiveWindow(),"Error: failed to create UDP socket !","ntpbclient",MB_ICONERROR | MB_OK);
		return 0;
	}

  	if (bind(udp_socket, (struct sockaddr *)&peer, sizeof(struct sockaddr)) < 0) {
		MessageBox(GetActiveWindow(),"Error: failed to bind UDP socket !","ntpbclient",MB_ICONERROR | MB_OK);
		return 0;
	}

	FD_ZERO(&fd);

	peerlen = sizeof(peer);

	while (1) {
		FD_SET(udp_socket, &fd);

  		select(FD_SETSIZE, &fd, NULL, NULL, NULL);

		memset(netlogbuffer, 0, sizeof(netlogbuffer));

		recvfrom(udp_socket, netlogbuffer, sizeof(netlogbuffer), 0, (struct sockaddr *)&peer, &peerlen);

		strcat(netlogbuffer, "\r\n");

		PrintLog(netlogbuffer, strlen(netlogbuffer), IDC_TEXTBOX_SERVERLOG);
	}

	closesocket(udp_socket);

	return 0;
}

/*<---------------------------------------------------------------------->*/
int rcvReply(void) // retrieving reply sent by server
{
	int rcvSize, sndSize, packetSize, ntpbpktSize, ntpbCmd, ln, recv_size;
	char *pbuf;
	int endTransmit = 0;

	while (1) {

		pbuf = (char *)&pktbuffer[0];

		// receive the first packet
		rcvSize = recv(main_socket, &pktbuffer[0], sizeof(pktbuffer), 0);
		if (rcvSize < 0) {
			MessageBox(GetActiveWindow(),"Error: recv failed !","ntpbclient",MB_ICONERROR | MB_OK);
			goto error;
		}

		// packet sanity check
		if (!check_ntpb_header(pbuf)) {
			MessageBox(GetActiveWindow(),"Error: not ntpb packet !","ntpbclient",MB_ICONERROR | MB_OK);
			goto error;
		}

		ntpbpktSize = *((unsigned short *)&pbuf[6]);
		packetSize = ntpbpktSize + ntpb_hdrSize;

		recv_size = rcvSize;

		// fragmented packet handling
		while (recv_size < packetSize) {
			rcvSize = recv(main_socket, &pktbuffer[recv_size], sizeof(pktbuffer) - recv_size, 0);
			if (rcvSize < 0) {
				MessageBox(GetActiveWindow(),"Error: recv failed !","ntpbclient",MB_ICONERROR | MB_OK);
				goto error;
			}
			else {
				recv_size += rcvSize;
			}
		}

		// parses packet
		if (check_ntpb_header(pbuf)) {
			ntpbCmd = *((unsigned short *)&pbuf[8]);

			switch(ntpbCmd) { // treat Client Request here

				case NTPBCMD_END_TRANSMIT:
					Sleep(100);
					CloseHandle(fh_dump);
					endTransmit = 1;
					break;
			}

			*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
			*((unsigned short *)&pktbuffer[6]) = 0;
			packetSize = ntpb_hdrSize + 2;

			// send the response packet
			sndSize = send(main_socket, &pktbuffer[0], packetSize, 0);
			if (sndSize <= 0) {
				MessageBox(GetActiveWindow(),"Error: send failed !","ntpbclient",MB_ICONERROR | MB_OK);
				goto error;
			}

			if (endTransmit)
				break;
		}
	}

	//MessageBox(GetActiveWindow(),"receive data done !","ntpbclient",MB_ICONERROR | MB_OK);
	return 1;

error:
	return 0;
}

/*<---------------------------------------------------------------------->*/
DWORD WINAPI rcvDataThread(LPVOID lpParam) // retrieving datas sent by server
{
	int rcvSize, sndSize, packetSize, ntpbpktSize, ntpbCmd, ln, recv_size, sizeWritten;
	char *pbuf;
	char startbuf[128], endbuf[128], tmp[128], tmp_file[128];
	int endTransmit = 0;

	while (1) {

		pbuf = (char *)&pktbuffer[0];

		// receive the first packet
		rcvSize = recv(main_socket, &pktbuffer[0], sizeof(pktbuffer), 0);
		if (rcvSize < 0) {
			MessageBox(GetActiveWindow(),"Error: recv failed !","ntpbclient",MB_ICONERROR | MB_OK);
			goto error;
		}

		// packet sanity check
		if (!check_ntpb_header(pbuf)) {
			MessageBox(GetActiveWindow(),"Error: not ntpb packet !","ntpbclient",MB_ICONERROR | MB_OK);
			goto error;
		}

		ntpbpktSize = *((unsigned short *)&pbuf[6]);
		packetSize = ntpbpktSize + ntpb_hdrSize;

		recv_size = rcvSize;

		// fragmented packet handling
		while (recv_size < packetSize) {
			rcvSize = recv(main_socket, &pktbuffer[recv_size], sizeof(pktbuffer) - recv_size, 0);
			if (rcvSize < 0) {
				MessageBox(GetActiveWindow(),"Error: recv failed !","ntpbclient",MB_ICONERROR | MB_OK);
				goto error;
			}
			else {
				recv_size += rcvSize;
			}
		}

		// parses packet
		if (check_ntpb_header(pbuf)) {
			ntpbCmd = *((unsigned short *)&pbuf[8]);

			switch(ntpbCmd) { // treat Client Request here

				case NTPBCMD_PRINT_EEDUMP:
				case NTPBCMD_PRINT_IOPDUMP:
				case NTPBCMD_PRINT_KERNELDUMP:
				case NTPBCMD_PRINT_SCRATCHPADDUMP:

					if ((dump_wpos + ntpbpktSize) > dump_size) {
						MessageBox(GetActiveWindow(),"Error: dump size exeeded !","ntpbclient",MB_ICONERROR | MB_OK);
						goto error;
					}

					WriteFile(fh_dump, &pktbuffer[ntpb_hdrSize], ntpbpktSize, &sizeWritten, NULL);
					dump_wpos += sizeWritten;

					// stepping progress bar
					SendMessage(hwndProgressBardumpState, PBM_STEPIT, 0, 0);
					UpdateWindow(hwndMain);
					break;

				case NTPBCMD_END_TRANSMIT:
					Sleep(100);
					CloseHandle(fh_dump);
					endTransmit = 1;
					break;
			}

			*((unsigned short *)&pktbuffer[ntpb_hdrSize]) = 1;
			*((unsigned short *)&pktbuffer[6]) = 0;
			packetSize = ntpb_hdrSize + 2;

			// send the response packet
			sndSize = send(main_socket, &pktbuffer[0], packetSize, 0);
			if (sndSize <= 0) {
				MessageBox(GetActiveWindow(),"Error: send failed !","ntpbclient",MB_ICONERROR | MB_OK);
				goto error;
			}

			if (endTransmit)
				break;
		}
	}

	// resetting progress bar
	SendMessage(hwndProgressBardumpState, PBM_SETPOS, 0, 0);
	UpdateWindow(hwndMain);

	remote_cmd = REMOTE_CMD_NONE;

	//MessageBox(GetActiveWindow(),"receive data done !","ntpbclient",MB_ICONERROR | MB_OK);

error:
	ExitThread(1);

	return 0;
}

/*<---------------------------------------------------------------------->*/
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
	if (sndSize <= 0) {
		MessageBox(GetActiveWindow(),"Error: send failed !","ntpbclient",MB_ICONERROR | MB_OK);
		return -1;
	}

	rcvSize = recv(main_socket, &pktbuffer[0], sizeof(pktbuffer), 0);
	if (rcvSize <= 0) {
		MessageBox(GetActiveWindow(),"Error: recv failed !","ntpbclient",MB_ICONERROR | MB_OK);
		return -1;
	}

	return 1;
}

/*<---------------------------------------------------------------------->*/
DWORD WINAPI clientThread(LPVOID lpParam)
{
	int r, tcp_socket;
	struct sockaddr_in peer;
	char recvbuffer[65536];

	if (main_socket != -1)
		closesocket(main_socket);

	UpdateStatusBar("Contacting PS2 Server...", 0, 0);

	peer.sin_family = AF_INET;
	peer.sin_port = htons(SERVER_TCP_PORT);
	peer.sin_addr.s_addr = inet_addr(SERVER_IP);

	while (1) {
		tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (tcp_socket < 0) {
	       	goto error;
		}

		r = connect(tcp_socket, (struct sockaddr*)&peer, sizeof(peer));
		if (r < 0) {
			goto error;
		}

		UpdateStatusBar("Client connected...", 0, 0);

		main_socket = tcp_socket;

		ClientConnected = 1;

		ExitThread(1);

error:
		//MessageBox(GetActiveWindow(),"Error !","ntpbclient",MB_ICONERROR | MB_OK);
		closesocket(tcp_socket);
	}

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
                               380,82,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_EEDUMP,
                               hInstance,
                               NULL);

	hwndButtonEEdump = CreateWindow (TEXT("button"),
                               "Dump EE",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               20,156,
                               362,24,
                               hwndMain,
                               (HMENU)IDC_BUTTON_EEDUMP,
                               hInstance,
                               NULL);

	hwndLabelIOPdump = CreateWindow (TEXT("static"),
                               "IOP dump:",
                               WS_CHILD|WS_VISIBLE,
                               400,52,
                               380,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_IOPDUMP,
                               hInstance,
                               NULL);

	hwndTextBoxIOPdump = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY,
                               400,70,
                               380,82,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_IOPDUMP,
                               hInstance,
                               NULL);

	hwndButtonIOPdump = CreateWindow (TEXT("button"),
                               "Dump IOP",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               400,156,
                               362,24,
                               hwndMain,
                               (HMENU)IDC_BUTTON_IOPDUMP,
                               hInstance,
                               NULL);

	hwndLabelKerneldumpStart = CreateWindow (TEXT("static"),
                               "start(HEX):",
                               WS_CHILD|WS_VISIBLE,
                               20,220,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_KERNELDUMPSTART,
                               hInstance,
                               NULL);

	hwndTextBoxKerneldumpStart = CreateWindow (TEXT("edit"),
                               "80000000",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               95,218,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_KERNELDUMPSTART,
                               hInstance,
                               NULL);

	hwndLabelKerneldumpEnd = CreateWindow (TEXT("static"),
                               "end(HEX):",
                               WS_CHILD|WS_VISIBLE,
                               200,220,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_KERNELDUMPEND,
                               hInstance,
                               NULL);

	hwndTextBoxKerneldumpEnd = CreateWindow (TEXT("edit"),
                               "82000000",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               270,218,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_KERNELDUMPEND,
                               hInstance,
                               NULL);

	hwndLabelScratchpaddumpStart = CreateWindow (TEXT("static"),
                               "start(HEX):",
                               WS_CHILD|WS_VISIBLE,
                               400,220,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_SCRATCHPADDUMPSTART,
                               hInstance,
                               NULL);

	hwndTextBoxScratchpaddumpStart = CreateWindow (TEXT("edit"),
                               "70000000",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               475,218,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_SCRATCHPADDUMPSTART,
                               hInstance,
                               NULL);

	hwndLabelScratchpaddumpEnd = CreateWindow (TEXT("static"),
                               "end(HEX):",
                               WS_CHILD|WS_VISIBLE,
                               580,220,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_SCRATCHPADDUMPEND,
                               hInstance,
                               NULL);

	hwndTextBoxScratchpaddumpEnd = CreateWindow (TEXT("edit"),
                               "70004000",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               650,218,
                               70,20,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_SCRATCHPADDUMPEND,
                               hInstance,
                               NULL);

	hwndLabelKerneldump = CreateWindow (TEXT("static"),
                               "Kernel dump:",
                               WS_CHILD|WS_VISIBLE,
                               20,252,
                               380,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_KERNELDUMP,
                               hInstance,
                               NULL);

	hwndTextBoxKerneldump = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY,
                               20,270,
                               380,82,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_KERNELDUMP,
                               hInstance,
                               NULL);

	hwndButtonKerneldump = CreateWindow (TEXT("button"),
                               "Dump Kernel",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               20,356,
                               362,24,
                               hwndMain,
                               (HMENU)IDC_BUTTON_KERNELDUMP,
                               hInstance,
                               NULL);

	hwndLabelScratchpaddump = CreateWindow (TEXT("static"),
                               "ScratchPad dump:",
                               WS_CHILD|WS_VISIBLE,
                               400,252,
                               380,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_SCRATCHPADDUMP,
                               hInstance,
                               NULL);

	hwndTextBoxScratchpaddump = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY,
                               400,270,
                               380,82,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_SCRATCHPADDUMP,
                               hInstance,
                               NULL);

	hwndButtonScratchpaddump = CreateWindow (TEXT("button"),
                               "Dump ScratchPad",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               400,356,
                               362,24,
                               hwndMain,
                               (HMENU)IDC_BUTTON_SCRATCHPADDUMP,
                               hInstance,
                               NULL);

	hwndLabeldumpState = CreateWindow (TEXT("static"),
                               "Dump Status:",
                               WS_CHILD|WS_VISIBLE,
                               355,400,
                               100,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_DUMPSTATE,
                               hInstance,
                               NULL);

	hwndProgressBardumpState = CreateWindowEx (0, PROGRESS_CLASS,
                               NULL,
                               WS_CHILD|WS_VISIBLE|PBS_SMOOTH,
                               20,420,
                               754,20,
                               hwndMain,
                               (HMENU)IDC_PROGRESSBAR_DUMPSTATE,
                               hInstance,
                               NULL);

	hwndButtonHaltResume = CreateWindow (TEXT("button"),
                               "Halt",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               20,470,
                               754,24,
                               hwndMain,
                               (HMENU)IDC_BUTTON_HALTRESUME,
                               hInstance,
                               NULL);

	hwndLabelServerLog = CreateWindow (TEXT("static"),
                               "Server Log:",
                               WS_CHILD|WS_VISIBLE,
                               790,20,
                               120,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_SERVERLOG,
                               hInstance,
                               NULL);

	hwndTextBoxServerLog = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_READONLY,
                               790,40,
                               210,456,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_SERVERLOG,
                               hInstance,
                               NULL);

	hwndLabelAddr = CreateWindow (TEXT("static"),
                               "address:",
                               WS_CHILD|WS_VISIBLE,
                               20,518,
                               100,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_ADDR,
                               hInstance,
                               NULL);

	hwndLabelVal = CreateWindow (TEXT("static"),
                               "value:",
                               WS_CHILD|WS_VISIBLE,
                               120,518,
                               100,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_VAL,
                               hInstance,
                               NULL);

	hwndTextBoxPatchMem = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE,
                               20,538,
                               100,70,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_PATCHMEM,
                               hInstance,
                               NULL);

	hwndTextBoxPatchVal = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE,
                               120,538,
                               100,70,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_PATCHVAL,
                               hInstance,
                               NULL);

	hwndButtonPatchMem = CreateWindow (TEXT("button"),
                               "Add Patches",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               240,538,
                               100,24,
                               hwndMain,
                               (HMENU)IDC_BUTTON_PATCHMEM,
                               hInstance,
                               NULL);

	hwndButtonPatchMem = CreateWindow (TEXT("button"),
                               "Clear Patches",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               240,566,
                               100,24,
                               hwndMain,
                               (HMENU)IDC_BUTTON_UNPATCHMEM,
                               hInstance,
                               NULL);

	hwndLabelCodes = CreateWindow (TEXT("static"),
                               "raw codes:",
                               WS_CHILD|WS_VISIBLE,
                               400,518,
                               100,20,
                               hwndMain,
                               (HMENU)IDC_LABEL_CODES,
                               hInstance,
                               NULL);

	hwndTextBoxCode1 = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE,
                               400,538,
                               100,70,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_CODE1,
                               hInstance,
                               NULL);

	hwndTextBoxCode2 = CreateWindow (TEXT("edit"),
                               "",
                               WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_MULTILINE,
                               500,538,
                               100,70,
                               hwndMain,
                               (HMENU)IDC_TEXTBOX_CODE2,
                               hInstance,
                               NULL);

	hwndButtonAddCodes = CreateWindow (TEXT("button"),
                               "Add Codes",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               620,538,
                               100,24,
                               hwndMain,
                               (HMENU)IDC_BUTTON_ADDCODES,
                               hInstance,
                               NULL);

	hwndButtonClearCodes = CreateWindow (TEXT("button"),
                               "Clear Codes",
                               WS_CHILD|WS_VISIBLE|WS_BORDER,
                               620,566,
                               100,24,
                               hwndMain,
                               (HMENU)IDC_BUTTON_CLEARCODES,
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
	if ((hwndMain = Createntpbclient_win32WndClassWnd()) == (HWND)0)
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
	strcpy(kerneldump_dir, dump_dir);
	strcat(kerneldump_dir, "\\Kernel");
	strcpy(scratchpaddump_dir, dump_dir);
	strcat(scratchpaddump_dir, "\\ScratchPad");

	CreateDirectory(dump_dir, NULL);
	CreateDirectory(eedump_dir, NULL);
	CreateDirectory(iopdump_dir, NULL);
	CreateDirectory(kerneldump_dir, NULL);
	CreateDirectory(scratchpaddump_dir, NULL);

	eedump_index = 0;
	iopdump_index = 0;
	kerneldump_index = 0;
	scratchpaddump_index = 0;

	remote_cmd = REMOTE_CMD_NONE;

	// Create & start the netlog thread
	HANDLE netlogThid = CreateThread(NULL, 0, netlogThread, NULL, 0, NULL); // no stack, 1MB by default

	// Create & start the client thread
	clientThid = CreateThread(NULL, 0, clientThread, NULL, 0, NULL); // no stack, 1MB by default

	// API message loop
	while (GetMessage(&msg,NULL,0,0)) {
		if (!TranslateAccelerator(msg.hwnd,hAccelTable,&msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	WSACleanup();

	return msg.wParam;
}

