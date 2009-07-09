/****************************************************************************
Artemis - PS2 Code Creator (for lack of a better title)
-PS2 dumping/communication functions by Jimmikaelkael
-Code searching by Viper187

This is a bit of a rough start, especially without the PS2 network adaptor to
test on right now. I've implemented the source from Jimmikaelkael's core
dumper v2. I used the command line build because it's linear, easier to call
on demand, and doesn't have a bunch of unneeded GUI stuff to cut out. I still
need to add status/progress monitoring while dumping. I'm a little uneasy
about messing around in the important part of that source without being able
to test it, but comparing to the GUI source should make it easy enough. I'll
try to keep things somewhat commented everywhere so the whole app can be
followed in case anyone feels the need to mess with it later. I'm usually
around to answer questions though.

p.s. Please don't screw up my source with Visual Stupid 6.0/.net/2008 or any
other random dev environment. MinGW and Textpad for the win! ...even though
MakeFiles are a little annoying to work on.
*****************************************************************************/

#include "ps2cc.h"
#include "ps2cc_gui.h"

HINSTANCE hInst;					// Instance handle
HWND hwndMain;						// Main window handle

char CFGFile[MAX_PATH];
HWND hTabDlgs[NUM_TABS]; //an array of handles for the dialogs on each tab.
char ErrTxt[1000];
WNDPROC wpHexEditBoxes;

//Global structs
MAIN_CFG Settings, Defaults;
RAM_AND_RES_DATA RamInfo;


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
	HMENU hMenu = GetMenu(hwnd);
	HWND hwndStatusBar = GetDlgItem(hwnd, NTPB_STATUS_BAR);
	switch (msg) {
 		case WM_INITDIALOG:
        {
			LoadSettings();
		    InitTabControl(hwnd, lParam);
		    int statwidth = -1;
            SendMessage(hwndStatusBar, SB_SETPARTS, 1, (LPARAM)&statwidth);
		    //To Do: set fonts here later
            SetMenuItemText(hMenu, MNU_DUMP_DIR, Settings.CS.DumpDir);
		} break;
        case WM_NOTIFY:
        {
            NMHDR *hdr = (LPNMHDR)lParam;
            if (hdr->code == TCN_SELCHANGING || hdr->code == TCN_SELCHANGE)  //switching tabs
            {
                int index = TabCtrl_GetCurSel(hdr->hwndFrom);
                if (index >= 0 && index < NUM_TABS) ShowWindow(hTabDlgs[index], (hdr->code == TCN_SELCHANGE) ? SW_SHOW : SW_HIDE);
            }
        } break;
		case WM_COMMAND:
        {
			switch(LOWORD(wParam))
            {
			    case MNU_DUMP_DIR:
			    {
                    char DumpPath[MAX_PATH];
                    if (BrowseForFolder(hwnd, DumpPath)) {
                        strcpy(Settings.CS.DumpDir, DumpPath);
                        SetMenuItemText(hMenu, MNU_DUMP_DIR, Settings.CS.DumpDir);
                    }
			    } break;
			    case MNU_IP_CONFIG:
			    {
					DialogBox(hInst, MAKEINTRESOURCE(IP_CONFIG_DLG), hwnd, IpConfigDlg);
				} break;
			}
		} break;
		case WM_SIZE:
		{
		} break;
		case WM_CLOSE:
		{
            SaveSettings();
            DestroyWindow(hwnd);
		} break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		} break;
		default:
			return FALSE;
	}
	return FALSE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	MSG msg;
	unsigned char *p;

	hInst = hInstance;

   hwndMain = CreateDialog(hInst,MAKEINTRESOURCE(PS2CC_DLG),HWND_DESKTOP, MainWndProc);
   sprintf(ErrTxt, "%u", GetLastError());
   if (!hwndMain) { MessageBox(NULL,ErrTxt, "debug",MB_OK); }


	// Create our controls
    INITCOMMONCONTROLSEX blah;
    blah.dwSize = sizeof(INITCOMMONCONTROLSEX);
    blah.dwICC = -1;
    InitCommonControlsEx(&blah);

	ShowWindow(hwndMain,SW_SHOW);


	// API message loop
	while (GetMessage(&msg,NULL,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


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
    if (RamInfo.Results) { free(RamInfo.Results); RamInfo.Results = NULL; }
    memset(&RamInfo.OldResultsInfo, 0, sizeof(CODE_SEARCH_RESULTS_INFO));
    memset(&RamInfo.NewResultsInfo, 0, sizeof(CODE_SEARCH_RESULTS_INFO));
    return 0;
}

/****************************************************************************
LoadSettings
*****************************************************************************/
int LoadSettings()
{
    memset(&Defaults,0,sizeof(Defaults));
    memset(&Settings,0,sizeof(Settings));
    if (GetModuleFileName(NULL,CFGFile,sizeof(CFGFile)) ) {
        char *fndchr = strrchr(CFGFile,'\\');
        *(fndchr + 1) = '\0';
        strcpy(Defaults.CS.DumpDir, CFGFile);
        strcat(Defaults.CS.DumpDir, "Searches\\");
        strcat(CFGFile,"ps2cc.cfg");
    } else {
        sprintf(CFGFile,"ps2cc.cfg");
        strcpy(Defaults.CS.DumpDir, "Searches\\");
    }
    Defaults.CFGVersion = 1; //increment this if settings struct or sub-struct definitions in ps2cc.h change
    sprintf(Defaults.ServerIp, "192.168.0.80");
    Defaults.ValueFontInfo = (LOGFONT){ 0, 10, 0, 0, 10, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN, "Terminal"} ;
    Defaults.ValueHFont = CreateFontIndirect(&Defaults.ValueFontInfo);
    Defaults.CS.NumBase = BASE_HEX;
//    Defaults.CS.NumBaseId = MNU_CS_INPUT_HEX;
/*results options
    Defaults.Results.ResWriteRate = 100;
    Defaults.Results.ResWriteRateId = MNU_RES_WRITE_100MS;
    Defaults.Results.DisplayFmt = MNU_RES_SHOW_HEX;
*/
	if (FileExists(CFGFile)) { LoadStruct(&Settings, sizeof(MAIN_CFG), CFGFile); }
    if (Settings.CFGVersion != Defaults.CFGVersion) {
		memset(&Settings,0,sizeof(Settings));
        memcpy(&Settings,&Defaults,sizeof(Defaults));
	}
    mkdir(Settings.CS.DumpDir);
    return 0;
}

/****************************************************************************
SaveSettings
*****************************************************************************/
int SaveSettings()
{
    SaveStruct(&Settings, sizeof(Settings), CFGFile);
    return 0;
}

/****************************************************************************
Update Status Bar
*****************************************************************************/
int UpdateStatusBar(const char *StatusText, int PartNum, int Flags)
{
	HWND hwndStatusBar = GetDlgItem(hwndMain, NTPB_STATUS_BAR);
	if (hwndStatusBar) {
		SendMessage(hwndStatusBar, SB_SETTEXT, PartNum|Flags, (LPARAM)StatusText);
		UpdateWindow(hwndMain);
	}
	return 0;
}

/****************************************************************************
Update Progress Bar
*****************************************************************************/
int UpdateProgressBar(unsigned int Message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndProgressBar = GetDlgItem(hwndMain, DUMPSTATE_PRB);
	if (hwndProgressBar) {
		SendMessage(hwndProgressBar, Message, wParam, lParam);
		UpdateWindow(hwndMain);
	}
	return 0;
}
