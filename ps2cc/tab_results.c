/****************************************************************************
Results tab procedure(s) and handlers

Everything for the results display should pretty much be here.
*****************************************************************************/
#include "ps2cc.h"
#include "ps2cc_gui.h"



WNDPROC wpResultsListProc, wpActiveListProc;
s64 CurrResNum;
u32 *ResultsList;


BOOL CALLBACK SearchResultsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndResList = GetDlgItem(hwnd, RESULTS_LSV);
    HWND hwndResPage = GetDlgItem(hwnd, RESULTS_PAGE_CMB);
    HWND hwndActiveResAddr = GetDlgItem(hwnd, ACTIVE_RES_ADDR_TXT);
    HWND hwndActiveResSize = GetDlgItem(hwnd, ACTIVE_RES_SIZE_CMB);
    HWND hwndActiveResValue = GetDlgItem(hwnd, ACTIVE_RES_VALUE_TXT);
    HWND hwndActiveList = GetDlgItem(hwnd, ACTIVE_CODES_LSV);
	switch(message)
	{
		case WM_INITDIALOG:
        {
            SendMessage(hwndResList,LVM_DELETEALLITEMS,0,0);
            SendMessage(hwndResList,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_LABELTIP);
//            SendMessage(hwndResList, WM_SETFONT, (WPARAM)Settings.ValueHFont, TRUE);
            ListViewAddCol(hwndResList, "Address", 0, 0x80);            //subclassing
		    wpResultsListProc = (WNDPROC)GetWindowLongPtr (hwndResList, GWLP_WNDPROC);
		    SetWindowLongPtr (hwndResList, GWLP_WNDPROC, (LONG_PTR)ResultsListHandler);
		    wpActiveListProc = (WNDPROC)GetWindowLongPtr (hwndActiveList, GWLP_WNDPROC);
		    SetWindowLongPtr (hwndActiveList, GWLP_WNDPROC, (LONG_PTR)ActiveListHandler);
//		    wpActiveValueBoxProc = (WNDPROC)GetWindowLongPtr (hwndActiveResValue, GWLP_WNDPROC);
		    SetWindowLongPtr (hwndActiveResValue, GWLP_WNDPROC, (LONG_PTR)HexEditBoxHandler);
		    SetWindowLongPtr (hwndActiveResAddr, GWLP_WNDPROC, (LONG_PTR)HexEditBoxHandler);

            //Active Code Sizes
            SendMessage(hwndActiveResSize,CB_RESETCONTENT,0,0);
//            ComboAddItem(hwndActiveResSize, "8-Bit" , 1);
//            ComboAddItem(hwndActiveResSize, "16-Bit" , 2);
            ComboAddItem(hwndActiveResSize, "32-Bit" , 4);
//            ComboAddItem(hwndActiveResSize, "64-Bit" , 8);
            //Init Active Codes List
            SendMessage(hwndActiveList,LVM_DELETEALLITEMS,0,0);
            SendMessage(hwndActiveList,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES|LVS_EX_GRIDLINES|LVS_EX_LABELTIP);
//            SendMessage(hwndAciveList, WM_SETFONT, (WPARAM)Settings.ValueHFont, TRUE);
            ListViewAddCol(hwndActiveList, "Address", 0, 0x70);
            ListViewAddCol(hwndActiveList, "Value", 1, 0x80);
            ListViewAddCol(hwndActiveList, "Size", 2, 0x30);
            SendMessage(hwndActiveList, LVM_SETCOLUMNWIDTH, 2, LVSCW_AUTOSIZE_USEHEADER);

            SendMessage(hwndActiveResSize,CB_SETCURSEL,0,0);
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ACTIVE_RES_SIZE_CMB, CBN_SELCHANGE),(LPARAM)hwndActiveResSize);
		    SendMessage(hwndActiveResAddr, EM_SETLIMITTEXT, 8, 0);
		} break;
		case WM_COMMAND:
        {
			switch(LOWORD(wParam))
            {
				/************************************************************
				Result Page # Combo Box
				*************************************************************/
                case RESULTS_PAGE_CMB:
                {
                    switch(HIWORD(wParam))
                    {
                        case CBN_SELCHANGE:
                        {
							u32 PageSize = Settings.Results.PageSize ? Settings.Results.PageSize : SendMessage(hwndResList,LVM_GETCOUNTPERPAGE,0,0);
							CurrResNum = ShowResPage(PageSize * SendMessage(hwndResPage,CB_GETITEMDATA,SendMessage(hwndResPage,CB_GETCURSEL,0,0),0));
						} break;
					}
				} break;
				/************************************************************
				Active Code Size Combo Box
				*************************************************************/
                case ACTIVE_RES_SIZE_CMB:
                {
                    switch(HIWORD(wParam))
                    {
                        case CBN_SELCHANGE:
                        {
                            int ResSize = SendMessage(hwndActiveResSize,CB_GETITEMDATA,SendMessage(hwndActiveResSize,CB_GETCURSEL,0,0),0);
                            SendMessage(hwndActiveResValue, EM_SETLIMITTEXT, ResSize*2, 0);
                        } break;
                    }
                } break;
				/************************************************************
				Write Once button
				*************************************************************/
                case RES_WRITE_ONCE_CMD:
                {
                    unsigned char actcodes[128];
                    u32 address = GetHexWindow(hwndActiveResAddr);
    				*((unsigned int *)&actcodes[0]) = 1;
    				*((unsigned int *)&actcodes[4]) = address;
    				*((unsigned int *)&actcodes[8]) = (u32)GetHexWindow(hwndActiveResValue);
                    if (address % 4) { MessageBox(NULL, "Address must be aligned to match the number of bytes being written, fucknut.", "Error", MB_OK); break; }
    				if (!ActivateCheats(actcodes, 1)) {
    					MessageBox(NULL, ErrTxt, "Error", MB_OK); break;
    				}
                    UpdateActiveCheats();
                } break;
				/************************************************************
				Activate button
				*************************************************************/
				case RES_ACTIVATE_CMD:
                {
                    u32 address = GetHexWindow(hwndActiveResAddr);
                    u64 value = GetHexWindow(hwndActiveResValue);
                    if (address % 4) { MessageBox(NULL, "Address must be aligned to match the number of bytes being written, fucknut.", "Error", MB_OK); break; }
                    ListView_SetCheckState(hwndActiveList, Result2ActiveList(address, value, 4), TRUE);
                    UpdateActiveCheats();
                } break;
				/************************************************************
				Delete button
				*************************************************************/
				case RES_DEL_ACTIVE_CMD:
				{
                    int iCount = SendMessage(hwndActiveList, LVM_GETITEMCOUNT, 0, 0);
                    int i;
                    for (i = 0; i < iCount; i++) {
                        if (SendMessage(hwndActiveList, LVM_GETITEMSTATE, i, LVIS_SELECTED)) {
                            SendMessage(hwndActiveList, LVM_DELETEITEM, i, 0);
                        }
                    }
                    UpdateActiveCheats();
				} break;
				/************************************************************
				ALL On/Off buttons
				*************************************************************/
				case RES_ALL_ON_CMD: case RES_ALL_OFF_CMD:
				{
                    int iCount = SendMessage(hwndActiveList, LVM_GETITEMCOUNT, 0, 0);
                    int i;
                    int iState = (LOWORD(wParam) == RES_ALL_ON_CMD) ? TRUE : FALSE;
                    for (i = 0; i < iCount; i++) {
						ListView_SetCheckState(hwndActiveList, i, iState);
					}
				} break;
				/************************************************************
				Clear ALL button
				*************************************************************/
				case RES_CLEAR_ALL_CMD:
				{
					SendMessage(hwndActiveList,LVM_DELETEALLITEMS,0,0);
				} break;
			}
		} break;
        case WM_SHOWWINDOW:
        {
            if (wParam) { LoadResultsList(); }
        } break;
	}
	return FALSE;
}

/****************************************************************************
Results Listview Handler
*****************************************************************************/
LRESULT CALLBACK ResultsListHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_LBUTTONDBLCLK: case WM_LBUTTONDOWN:
        {
            if (!(ResultsList)) { break; }
            int iSelected = ListViewHitTst(hwnd, GetMessagePos(), -1);
            if (iSelected < 0) { break; }
            int iSubItem = ListViewHitTst(hwnd, GetMessagePos(), iSelected);
            if (iSubItem <= 0) { iSubItem = 1; }
            u32 address = ListViewGetHex(hwnd, iSelected, 0);
			u64 value = 0;
			HWND hwndActiveResValue = GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], ACTIVE_RES_VALUE_TXT);
			HWND hwndSearchSize = GetDlgItem(hTabDlgs[CODE_SEARCH_TAB], SEARCH_SIZE_CMB);
			int SearchSize = SendMessage(hwndSearchSize,CB_GETITEMDATA,SendMessage(hwndSearchSize,CB_GETCURSEL,0,0),0);
            ComboSelFromData(GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], ACTIVE_RES_SIZE_CMB), SearchSize);
            SetHexWindow(GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], ACTIVE_RES_ADDR_TXT), address);
            SendMessage(hwndActiveResValue, EM_SETLIMITTEXT, SearchSize*2, 0);
			switch (Settings.Results.DisplayFmt)
			{
				case MNU_RES_SHOW_HEX:
				{
		            value = ListViewGetHex(hwnd, iSelected, iSubItem);
				} break;
				case MNU_RES_SHOW_DECU:
				case MNU_RES_SHOW_DECS:
				{
		            value = ListViewGetDec(hwnd, iSelected, iSubItem);
				} break;
				case MNU_RES_SHOW_FLOAT:
				{
		            value = ListViewGetFloat(hwnd, iSelected, iSubItem, SearchSize);
				} break;
			}
			SetHexWindow(hwndActiveResValue, value);
//            extern CurrMemAddress;
//            if (Settings.Results.RamView == 1) { CurrMemAddress = ShowRAM(address & 0xFFFFFFF0); }
            if (message == WM_LBUTTONDBLCLK) {
                ListView_SetCheckState(GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], ACTIVE_CODES_LSV), Result2ActiveList(address, value, 4), TRUE);
            }
//            sprintf(ErrTxt, "%08I64X: %I64X", address, value);
//            MessageBox(NULL, ErrTxt, "Debug", MB_OK);
        } break;
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_NEXT: case VK_PRIOR:
                {
                    if (!(ResultsList)) { break; }
                    HWND hwndResPage = GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], RESULTS_PAGE_CMB);
                    int iSelected = SendMessage(hwnd, LVM_GETSELECTIONMARK, 0, 0);
                    if (wParam == VK_NEXT) {
                    	SendMessage(hwndResPage,CB_SETCURSEL, SendMessage(hwndResPage,CB_GETCURSEL,0,0) + 1, 0);
            			SendMessage(hTabDlgs[SEARCH_RESULTS_TAB], WM_COMMAND, MAKEWPARAM(SEARCH_SIZE_CMB, CBN_SELCHANGE),(LPARAM)hwndResPage);
					}
                    else {
                    	SendMessage(hwndResPage,CB_SETCURSEL, SendMessage(hwndResPage,CB_GETCURSEL,0,0) - 1, 0);
            			SendMessage(hTabDlgs[SEARCH_RESULTS_TAB], WM_COMMAND, MAKEWPARAM(SEARCH_SIZE_CMB, CBN_SELCHANGE),(LPARAM)hwndResPage);
                    }
                    ListView_SetItemState(hwnd, iSelected, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
                } return 0;
            }
        } break;
    }
    if (wpResultsListProc) { return CallWindowProc (wpResultsListProc, hwnd, message, wParam, lParam); }
    else { return DefWindowProc (hwnd, message, wParam, lParam); }
}

/****************************************************************************
Active List Handler
*****************************************************************************/
//LRESULT CALLBACK ActiveListHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
LRESULT CALLBACK ActiveListHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_LBUTTONUP:
		{
			UpdateActiveCheats();
		} break;
		case WM_KEYUP:
		{
			if (wParam == VK_DELETE) { SendMessage(hTabDlgs[SEARCH_RESULTS_TAB], WM_COMMAND, RES_DEL_ACTIVE_CMD, 0); }
		} break;
	}
    if (wpActiveListProc) { return CallWindowProc (wpActiveListProc, hwnd, message, wParam, lParam); }
    else { return DefWindowProc (hwnd, message, wParam, lParam); }
//	return DefSubclassProc(hwnd, message, wParam, lParam);
}

/****************************************************************************
Load results
*****************************************************************************/
int LoadResultsList()
{
    CurrResNum = 0;
    FreeRamInfo();
    u32 i, DumpAddy, DumpNum;
    char txtValue[32];
    HWND hwndResList = GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], RESULTS_LSV);
    HWND hwndCompareTo = GetDlgItem(hTabDlgs[CODE_SEARCH_TAB], COMPARE_TO_CMB);
    HWND hwndResPage = GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], RESULTS_PAGE_CMB);
    int SearchCount = SendMessage(hwndCompareTo,CB_GETCOUNT,0,0) - 1;
    u32 PageSize = Settings.Results.PageSize ? Settings.Results.PageSize : SendMessage(hwndResList,LVM_GETCOUNTPERPAGE,0,0);
    if (!SearchCount) { return 0; }
	//reset columns and pages
	SendMessage(hwndResList,LVM_DELETEALLITEMS,0,0);
	SendMessage(hwndResPage,CB_RESETCONTENT,0,0);
	ComboAddItem(hwndResPage, "1" , 0);
	i = 1;
    while (SendMessage(hwndResList,LVM_DELETECOLUMN,i,0)) { i++; }
	//load current results info
    char resFileName[MAX_PATH];
    sprintf(resFileName,"%ssearch%u.bin",Settings.CS.DumpDir, SearchCount);
    if (!(LoadStruct(&RamInfo.NewResultsInfo, sizeof(CODE_SEARCH_RESULTS_INFO), resFileName))) { goto LoadResError; }
    if (!(LoadFile(&RamInfo.Results, resFileName, sizeof(CODE_SEARCH_RESULTS_INFO), NULL, FALSE))) { goto LoadResError; }
	//Set columns for values
    for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) {
	    sprintf(txtValue, "%u", SearchCount - DumpNum);
	    ListViewAddCol(hwndResList, txtValue, DumpNum + 1, 0x80);
    }
	//allocate results list memory;
    if (ResultsList) { free(ResultsList); ResultsList = NULL; }
    if (!(ResultsList = (u32*)malloc(sizeof(u32) * (RamInfo.NewResultsInfo.ResCount+1)))) {
        sprintf(ErrTxt, "Unable to allocate results list memory (LoadResultsList) -- Error %u", GetLastError());
        MessageBox(NULL, ErrTxt, "Error", MB_OK);
        goto LoadResError;
    }

	//get the file address of every result
	i = 0; DumpAddy = 0;
    while ((DumpAddy < RamInfo.NewResultsInfo.DumpSize) && (i < RamInfo.NewResultsInfo.ResCount)) {
        if (!(GetBitFlag(RamInfo.Results, DumpAddy/RamInfo.NewResultsInfo.SearchSize))) { DumpAddy += RamInfo.NewResultsInfo.SearchSize; continue; }
        ResultsList[i] = DumpAddy;
        DumpAddy += RamInfo.NewResultsInfo.SearchSize;
        i++;
        if (((i % PageSize) == 0) && ((i/ PageSize) < Settings.Results.MaxResPages)) {
			sprintf(txtValue, "%u", (i / PageSize) + 1);
			ComboAddItem(hwndResPage, txtValue , i / PageSize);
		}
    }
	SendMessage(hwndResPage,CB_SETCURSEL,0,0);

	//cleanup
	FreeRamInfo();
    CurrResNum = ShowResPage(CurrResNum);
	return 1;
LoadResError:
    if (ResultsList) { free(ResultsList); ResultsList = NULL; }
	FreeRamInfo();
	return 0;
}
/****************************************************************************
ShowResPage - Show 1 page of results starting at Result number (ResNum)
*****************************************************************************/
s64 ShowResPage(s64 ResNum)
{
    u32 i, DumpNum;
    float tmpFloat=0;
    u32 *CastFloat=(u32*)(&tmpFloat);
    double tmpDouble=0;
    u64 *CastDouble=(u64*)&tmpDouble;
    HWND hwndResList = GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], RESULTS_LSV);
    HWND hwndCompareTo = GetDlgItem(hTabDlgs[CODE_SEARCH_TAB], COMPARE_TO_CMB);
    u32 PageSize = Settings.Results.PageSize ? Settings.Results.PageSize : SendMessage(hwndResList,LVM_GETCOUNTPERPAGE,0,0);
    int SearchCount = SendMessage(hwndCompareTo,CB_GETCOUNT,0,0) - 1;
    FILE *ramFiles[MAX_SEARCHES];

    char resFileName[MAX_PATH];
    sprintf(resFileName,"%ssearch%u.bin",Settings.CS.DumpDir, SearchCount);
    if (!(LoadStruct(&RamInfo.NewResultsInfo, sizeof(CODE_SEARCH_RESULTS_INFO), resFileName))) { return 0; }
    if (!(LoadFile(&RamInfo.Results, resFileName, sizeof(CODE_SEARCH_RESULTS_INFO), NULL, FALSE))) { return 0; }
	if (ResNum > RamInfo.NewResultsInfo.ResCount) { ResNum = RamInfo.NewResultsInfo.ResCount - PageSize; }
	if (ResNum < 0) { ResNum = 0; }

	SendMessage(hwndResList,LVM_DELETEALLITEMS,0,0);
	//open dump file handles - Notice they're being loaded in order from the current search back.
    for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) {
        sprintf(resFileName,"%ssearch%u.bin",Settings.CS.DumpDir, (SearchCount - DumpNum));
        if (!(LoadStruct(&RamInfo.OldResultsInfo, sizeof(CODE_SEARCH_RESULTS_INFO), resFileName))) { goto ShowResPageError; }
        ramFiles[DumpNum] = fopen(RamInfo.OldResultsInfo.dmpFileName,"rb");
	    if (!(ramFiles[DumpNum])) {
            sprintf(ErrTxt, "Unable to open dump file (ShowResPage,1) -- Error %u", GetLastError());
            MessageBox(NULL,ErrTxt,"Error",MB_OK);
            goto ShowResPageError;
	    }
    }
	fseek(ramFiles[0],0,SEEK_END);
	RamInfo.NewResultsInfo.DumpSize = ftell(ramFiles[0]);
	fseek(ramFiles[0],0,SEEK_SET);
	RamInfo.Access = SEARCH_ACCESS_FILE;
    char txtAddress[9], txtValue[32], fmtString[20];
	ResFormatString(fmtString, Settings.Results.DisplayFmt, RamInfo.NewResultsInfo.SearchSize);
	i = 0;
	u64 ResValue = 0;
	/*loop through the results list starting from ResNum, and loop through the
	dump handles to get the value from each.*/
    while (((ResNum + i) < RamInfo.NewResultsInfo.ResCount) && (i < PageSize)) {
		//make address a string and add row with address in 1st column
        sprintf(txtAddress, "%08X", ResultsList[ResNum + i] + RamInfo.NewResultsInfo.MapMemAddy);
        ListViewAddRow(hwndResList, 1, txtAddress);
        SendMessage(hwndResList, LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE);
        //loop and update the row with the value from each search
        for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) {
            RamInfo.NewFile = ramFiles[DumpNum];
            GetSearchValues(&ResValue, NULL, ResultsList[ResNum + i], RamInfo.NewResultsInfo.SearchSize, RamInfo.NewResultsInfo.Endian);
            //Check if we need to cast the value to floating point
            if (Settings.Results.DisplayFmt == MNU_RES_SHOW_FLOAT) {
                *CastDouble = ResValue;
                *CastFloat = ResValue & 0xFFFFFFFF;
                sprintf(txtValue, fmtString, (RamInfo.NewResultsInfo.SearchSize == 4) ? tmpFloat : tmpDouble);
            } else { sprintf(txtValue, fmtString, ResValue); }
            ListViewSetRow(hwndResList, i, DumpNum + 1, 1, txtValue);
            SendMessage(hwndResList, LVM_SETCOLUMNWIDTH, DumpNum + 1, LVSCW_AUTOSIZE);
        }
        i++;
    }

//    SetDecWindowU(GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], RESULTS_PAGE_TXT), ((ResNum + i)/PageSize) + ((ResNum + i) % PageSize));

ShowResPageError:
	for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) {
		if(ramFiles[DumpNum]) { fclose(ramFiles[DumpNum]); }
	}
	FreeRamInfo();
	return ResNum;
}

/****************************************************************************
Res Format String

Pick the format string to be used in sprintf() statements that show result values.
*****************************************************************************/
int ResFormatString(char *tmpstring, int outfmt, int numbytes)
{
    switch (outfmt) {
        case MNU_RES_SHOW_HEX: case MNU_RES_EXPORT_HEX:
        {
			if (numbytes <= 4) { sprintf(tmpstring,"%%0%uX", numbytes*2); }
			else { strcpy(tmpstring, "%16I64X"); }
//				sprintf(tmpstring,"%%0%uI64X", numbytes*2); //doesn't like me
		} break;
        MessageBox(NULL, tmpstring, "Debug", 0);
        case MNU_RES_SHOW_DECU: case MNU_RES_EXPORT_DECU:
        {
            sprintf(tmpstring,"%%I64u");
        } break;
        case MNU_RES_SHOW_DECS: case MNU_RES_EXPORT_DECS:
        {
            sprintf(tmpstring,"%%I64d");
        } break;
        case MNU_RES_SHOW_FLOAT: case MNU_RES_EXPORT_FLOAT:
        {
            if (numbytes == 4) sprintf(tmpstring,"%%f");
            else sprintf(tmpstring,"%%Lf");
        } break;
    }
    return 0;
}
/****************************************************************************
Add Code to active list
*****************************************************************************/
int Result2ActiveList(u32 address, u64 value, int size)
{
    HWND hwndActList = GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], ACTIVE_CODES_LSV);
    int iCount = SendMessage(hwndActList, LVM_GETITEMCOUNT, 0, 0);
    char txtValue[32], txtSize[4], txtAddress[20];
    int i;
    sprintf(txtAddress, "%08I64X", address);
    sprintf(txtValue, "%08I64X", value);
    sprintf(txtSize, "%u", size);
    for (i = 0; i < iCount; i++) {
        if (ListViewGetHex(hwndActList, i, 0) == address) {
            ListViewSetRow(hwndActList, i, 1, 2, txtValue, txtSize);
            return i;
        }
    }
    return ListViewAddRow(hwndActList, 3, txtAddress, txtValue, txtSize);
}
/****************************************************************************
Update Active Cheats - Make array of cheats to send to PS2 and calls said function
*****************************************************************************/
int UpdateActiveCheats()
{
//    DeActivateCodes();
	unsigned char actcodes[128];
    HWND hwndActList = GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], ACTIVE_CODES_LSV);
    int i = 0, iCount = SendMessage(hwndActList, LVM_GETITEMCOUNT, 0, 0);
    int aCount = 4;
    u32 address, value;
    while ((i < iCount) && (aCount < 128))
    {
    	if (ListView_GetCheckState(hwndActList, i)) {
    		*((unsigned int *)&actcodes[aCount]) = ListViewGetHex(hwndActList, i, 0);
    		*((unsigned int *)&actcodes[aCount + 4]) = ListViewGetHex(hwndActList, i, 1);
    		aCount += 8;
    	}
    	i++;
    }
    if (aCount == 4) { //deactivate
		if(!DeActivateCodes()) { MessageBox(NULL, ErrTxt, "Error", MB_OK); return 0; }
		return 1;
	}
    *((unsigned int *)&actcodes[0]) = (aCount - 4)/8;
    if (!ActivateCheats(actcodes, aCount-1)) {
    	MessageBox(NULL, ErrTxt, "Error", MB_OK); return 0;
    }
    return 1;
}
