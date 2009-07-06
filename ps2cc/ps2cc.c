/*<---------------------------------------------------------------------->*/
/****************************************************************************
Artemis - PS2 Code Creator (for lack of a better title)
-PS2 dumping/communication functions by Jimmikaelkael
-Code searching by Viper187

This is a bit of a rough start, especially without the PS2 network adaptor to
test on right now. I'll attempt to code this in a way that'll be easy to
implement Jimmikaelkael's communications stuff to either as is or a little
more tailored to fit the task later. I'm just not sure what anyone expects in
terms of GUI or features yet. My inital thought is to preserve as much of
Jimmi's source as is so he can easily update his end if/when he finds issues.
I'm going to dedicate lib_memory.c for the main comms/dump functions. FYI
I just have to butcher the GUI stuff. I'm also a little uneasy about messing
around in the important part of the source without being able to test it.
Getting the whole thing working is the main thing. I can tweak and clean later
as I go. I'll try to keep things somewhat commented. I'm usually around to
answer questions though.

p.s. Please don't screw up my source with Visual Stupid 6.0/.net/2008 or any
other random dev environment. MinGW and Textpad for the win! ...even though
MakeFiles are a little annoying to work on.
*****************************************************************************/

#include "ps2cc.h"
#include "ps2cc_gui.h"

HINSTANCE hInst;					// Instance handle
HWND hwndMain;						// Main window handle

HWND hTabDlgs[NUM_TABS]; //an array of handles for the dialogs on each tab.
char ErrTxt[1000];
WNDPROC wpHexEditBoxes;

//Global structs
MAIN_CFG Settings, Defaults;
RAM_AND_RES_DATA RamInfo;
NTPB_VARS ntpbVars;



/****************************************************************************
Tab Control
-This creates a dialog (window) for each tab and positions them over the tab
 control. Each tab has it's own windows process as if it were really a
 seperate window.
****************************************************************************/
int InitTabControl(HWND hwnd, LPARAM lParam)
{
    TCITEM tabitem;
    HWND hTab;

    hTab = GetDlgItem(hwnd, PS2CC_TABCTRL);
    //insert tabs
    memset(&tabitem, 0, sizeof(tabitem));
    tabitem.mask = TCIF_TEXT;
	tabitem.cchTextMax = MAX_PATH;

    tabitem.pszText = "Search";
    SendMessage(hTab, TCM_INSERTITEM, CODE_SEARCH_TAB, (LPARAM)&tabitem);

    tabitem.pszText = "Results";
    SendMessage(hTab, TCM_INSERTITEM, SEARCH_RESULTS_TAB, (LPARAM)&tabitem);

    tabitem.pszText = "Memory Editor";
    SendMessage(hTab, TCM_INSERTITEM, MEMORY_EDITOR_TAB, (LPARAM)&tabitem);

//    tabitem.pszText = "Cheat";
//    SendMessage(hTab, TCM_INSERTITEM, CHEAT_TAB, (LPARAM)&tabitem);

	// Get the position that the dialogs should be displayed
	RECT rt,itemrt;
	GetWindowRect(hTab, &rt);
    TabCtrl_GetItemRect(hTab,1,&itemrt);
    rt.top -= (itemrt.top - itemrt.bottom);
    rt.bottom -= rt.top;
    rt.right  -= rt.left;
	ScreenToClient(hTab, (LPPOINT)&rt);

	// Create the dialogs modelessly and move them appropriately
    hTabDlgs[CODE_SEARCH_TAB] = CreateDialog((HINSTANCE)lParam, (LPSTR)SEARCH_DLG, hTab, (DLGPROC)CodeSearchProc);
/*
    hTabDlgs[SEARCH_RESULTS_TAB] = CreateDialog((HINSTANCE)lParam, (LPSTR)DLG_SEARCH_RESULTS, hTab, (DLGPROC)SearchResultsProc);
    hTabDlgs[MEMORY_EDITOR_TAB] = CreateDialog((HINSTANCE)lParam, (LPSTR)DLG_MEMORY_EDITOR, hTab, (DLGPROC)MemoryEditorProc);
    hTabDlgs[CHEAT_TAB] = CreateDialog((HINSTANCE)lParam, (LPSTR)DLG_CHEAT, hTab, (DLGPROC)CheatProc);
*/
    MoveWindow(hTabDlgs[CODE_SEARCH_TAB], rt.left, rt.top, rt.right, rt.bottom, 0);
/*
    MoveWindow(hTabDlgs[SEARCH_RESULTS_TAB], rt.left, rt.top, rt.right, rt.bottom, 0);
    MoveWindow(hTabDlgs[MEMORY_EDITOR_TAB], rt.left, rt.top, rt.right, rt.bottom, 0);
    MoveWindow(hTabDlgs[CHEAT_TAB], rt.left, rt.top, rt.right, rt.bottom, 0);
*/
    // Show the default dialog
    ShowWindow(hTabDlgs[CODE_SEARCH_TAB], SW_SHOW);
//    SendMessage(hTab, TCM_SETCURSEL, CODE_SEARCH_TAB, 0);
    SendMessage(hTab, TCM_SETCURFOCUS, CODE_SEARCH_TAB, 0);
    return 0;
}

/****************************************************************************
Main window thread - Basically a tab control, status bar, and progress bar.
Menu handlers might go here later as well.
****************************************************************************/

BOOL CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
        {
//			LoadSettings();
		    InitTabControl(hwnd, lParam);
		}
        case WM_NOTIFY:
        {
            NMHDR *hdr = (LPNMHDR)lParam;
            if (hdr->code == TCN_SELCHANGING || hdr->code == TCN_SELCHANGE)  //switching tabs
            {
                int index = TabCtrl_GetCurSel(hdr->hwndFrom);
                if (index >= 0 && index < NUM_TABS) ShowWindow(hTabDlgs[index], (hdr->code == TCN_SELCHANGE) ? SW_SHOW : SW_HIDE);
            }
        } break;
		case WM_SIZE:
			SendMessage(hWndStatusbar,msg,wParam,lParam);
			InitializeStatusBar(hWndStatusbar,1);
			break;
		case WM_COMMAND:
//			HANDLE_WM_COMMAND(hwnd,wParam,lParam,MainWndProc_OnCommand);
			break;
		case WM_CLOSE:
		{
//            SaveSettings();
            DestroyWindow(hwnd);
		} break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return FALSE;
	}
	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	MSG msg;
	unsigned char *p;

	hInst = hInstance;
//	if (!InitApplication())
//		return 0;


//	if ((hwndMain = CreatentpbserverWndClassWnd()) == (HWND)0)
//		return 0;
   hwndMain = CreateDialog(hInst,MAKEINTRESOURCE(PS2CC_DLG),HWND_DESKTOP, MainWndProc);
   sprintf(ErrTxt, "%u", GetLastError());
   if (!hwndMain) { MessageBox(NULL,ErrTxt, "debug",MB_OK); }


	// Create our controls
    INITCOMMONCONTROLSEX blah;
    blah.dwSize = sizeof(INITCOMMONCONTROLSEX);
    blah.dwICC = -1;
    InitCommonControlsEx(&blah);

	hwndProgressBardumpState = GetDlgItem(hwndMain, DUMPSTATE_PRB);
//	CreateControls(hInst); //fix
//	CreateSBar(hwndMain,"",1);
	hWndStatusbar = GetDlgItem(hwndMain, NTPB_STATUS_BAR);
    InitializeStatusBar(hwndMain,1);

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
//	strcat(eedump_dir, "\\EE");
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

	// Create & start the server thread
	HANDLE serverThid = CreateThread(NULL, 0, serverThread, NULL, 0, NULL); // no stack, 1MB by default

	// API message loop
	while (GetMessage(&msg,NULL,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// shutdown WSA
	WSACleanup();

	return msg.wParam;
}


/****************************************************************************
Free shit -Everything that is or might be malloc()'d should be tested and/or
free()'d here.
*****************************************************************************/
int FreeShit()
{
    FreeRamInfo();
//reinstate later    if (ResultsList) { free(ResultsList); ResultsList = NULL; }
    return 0;
}

/****************************************************************************
Free RAM info -Reset the RamInfo struct.
*****************************************************************************/
int FreeRamInfo()
{
    if (RamInfo.NewRAM) { free(RamInfo.NewRAM); RamInfo.NewRAM = NULL; }
    if (RamInfo.OldRAM) { free(RamInfo.OldRAM); RamInfo.OldRAM = NULL; }
    if (RamInfo.NewFile) { fclose(RamInfo.NewFile); RamInfo.NewFile = NULL; }
    if (RamInfo.OldFile) { fclose(RamInfo.OldFile); RamInfo.OldFile = NULL; }
    if (RamInfo.AddressMap) { free(RamInfo.AddressMap); RamInfo.AddressMap = NULL; }
    if (RamInfo.Results) { free(RamInfo.Results); RamInfo.Results = NULL; }
    RamInfo.MapSize = 0;
    memset(&RamInfo.OldResultsInfo, 0, sizeof(CODE_SEARCH_RESULTS_INFO));
    memset(&RamInfo.NewResultsInfo, 0, sizeof(CODE_SEARCH_RESULTS_INFO));
    return 0;
}
