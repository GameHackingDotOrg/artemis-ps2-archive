/****************************************************************************
PS2CC API Library

Collection of functions more specific to this app. API and globals in use.
*****************************************************************************/
#include "ps2cc.h"
#include "ps2cc_gui.h"

/****************************************************************************
Value editbox procedure - forces a textbox to hex/dec/whatever input
-check length of current text on maxlength change???
*****************************************************************************/
LRESULT CALLBACK ValueEditBoxHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC wpOriginalProc = GetSubclassProc(GetDlgCtrlID(hwnd));
    switch (message)
    {
        case WM_CHAR:
        {
        	if ((wParam == VK_BACK) || (wParam == 24) || (wParam == 3) || (wParam == 22)) { break; } //cut/copy/paste/backspace/tab
            if (wParam == 1) { SendMessage(hwnd, EM_SETSEL, 0, -1); } //select all
        	switch (GetDlgCtrlID(hwnd))
        	{
        		case SEARCH_AREA_LOW_TXT: case SEARCH_AREA_HIGH_TXT: case EX_VALUE_TXT:
        		case ACTIVE_RES_ADDR_TXT: case ACTIVE_RES_VALUE_TXT:
        		{
                	wParam = FilterHexChar(wParam);
                } break;
                case SEARCH_VALUE1_TXT: case SEARCH_VALUE2_TXT:
                {
					CODE_SEARCH_VARS Search; memset(&Search,0,sizeof(CODE_SEARCH_VARS));
            		HWND hwndSearchType = GetDlgItem(DlgInfo.TabDlgs[CODE_SEARCH_TAB], SEARCH_TYPE_CMB);
            		Search.Type = SendMessage(hwndSearchType,CB_GETITEMDATA,SendMessage(hwndSearchType,CB_GETCURSEL,0,0),0);
            		if ((Search.Type & SEARCH_KNOWN_WILD) && (wParam == 42)) { break; } //asterisk
            		switch ((Search.Type & (SEARCH_KNOWN_WILD|SEARCH_EQUAL_NUM_BITS|SEARCH_NEQUAL_TO_BITS|SEARCH_NEQUAL_BY_BITS|SEARCH_BITS_ANY|SEARCH_BITS_ALL)) ?
            		BASE_HEX : Settings.CS.NumBase)
            		{
            		    case BASE_HEX: { wParam = FilterHexChar(wParam); } break;
            		    case BASE_DEC: { wParam = ((isdigit(wParam))|| (wParam == '-')) ? wParam : 0; } break;
            		    case BASE_FLOAT: { wParam = (isdigit(wParam) || (wParam == '.') || (wParam == '-')) ? wParam : 0; } break;
            		}
                } break;
            }
        } break;
        case WM_PASTE:
        {
            char txtInput[20], txtInput2[20];
            GetWindowText(hwnd, txtInput, sizeof(txtInput));
            if (wpOriginalProc) { return CallWindowProc (wpOriginalProc, hwnd, message, wParam, lParam); }
            GetWindowText(hwnd, txtInput2, sizeof(txtInput2));
            if ((!isHexWindow(hwnd)) || (strlen(txtInput2) > SendMessage(hwnd, EM_GETLIMITTEXT, 0, 0))) { SetWindowText(hwnd, txtInput); }
        } return 0;
        case WM_KEYDOWN:
        {
        	switch (GetDlgCtrlID(hwnd))
        	{
                case EX_VALUE_TXT:
                {
            		if (wParam == VK_RETURN) { SendMessage(DlgInfo.TabDlgs[CODE_SEARCH_TAB], WM_COMMAND, LSV_CS_ENDEDIT, 1); }
            		if (wParam == VK_ESCAPE) { SendMessage(DlgInfo.TabDlgs[CODE_SEARCH_TAB], WM_COMMAND, LSV_CS_ENDEDIT, 0); }
            	} break;
            }
        } break;
        case WM_KILLFOCUS:
        {
        	switch (GetDlgCtrlID(hwnd))
        	{
                case EX_VALUE_TXT:
                {
            		SendMessage(DlgInfo.TabDlgs[CODE_SEARCH_TAB], WM_COMMAND, LSV_CS_ENDEDIT, 0);
            	} return 0;
            }
        } break;
   }
   if (wpOriginalProc) { return CallWindowProc (wpOriginalProc, hwnd, message, wParam, lParam); }
   else { return DefWindowProc (hwnd, message, wParam, lParam); }
}

/****************************************************************************
Get Subclassed control handle - Take Control ID and find the original HWND
*****************************************************************************/
WNDPROC GetSubclassProc(int ControlId)
{
	int i;
	for (i = 0; i < MAX_SUBCLASSES; i++)
	{
		if (DlgInfo.SubclassIds[i] == ControlId) { return DlgInfo.SubclassProcs[i]; }
	}
	return 0;
}

/****************************************************************************
Set Subclassed control handle - Take Control ID and find an empty array slot for the original HWND
*****************************************************************************/
int SetSubclassProc(WNDPROC ControlProc, int ControlId)
{
	int i;
	for (i = 0; i < MAX_SUBCLASSES; i++)
	{
		if (DlgInfo.SubclassIds[i] == 0) {
			DlgInfo.SubclassIds[i] = ControlId;
			DlgInfo.SubclassProcs[i] = ControlProc;
			return i;
		}
	}
	MessageBox(NULL, "Out of Subclass IDs", "Error", MB_OK);
	return -1;
}
