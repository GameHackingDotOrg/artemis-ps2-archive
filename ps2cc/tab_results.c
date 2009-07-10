/****************************************************************************
Results tab procedure(s) and handlers

Everything for the results display should pretty much be here.
*****************************************************************************/
#include "ps2cc.h"
#include "ps2cc_gui.h"



WNDPROC wpResultsListProc;
WNDPROC wpResPageTxtProc;
s64 CurrResNum;
u32 *ResultsList;

BOOL CALLBACK SearchResultsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndResList = GetDlgItem(hwnd, RESULTS_LSV);
    HWND hwndResPageText = GetDlgItem(hwnd, RESULTS_PAGE_TXT);
	switch(message)
	{
		case WM_INITDIALOG:
        {
            SendMessage(hwndResList,LVM_DELETEALLITEMS,0,0);
            SendMessage(hwndResList,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_LABELTIP);
            SendMessage(hwndResList, WM_SETFONT, (WPARAM)Settings.ValueHFont, TRUE);
            ListViewAddCol(hwndResList, "Address", 0, 0x80);
            //subclass the page # textbox
		    wpResPageTxtProc = (WNDPROC)GetWindowLongPtr (hwndResPageText, GWLP_WNDPROC);
		    SetWindowLongPtr (hwndResPageText, GWLP_WNDPROC, (LONG_PTR)ResultsPageTxtHandler);

//		    wpResultsListProc = (WNDPROC)GetWindowLongPtr (hwndResList, GWLP_WNDPROC);
//		    SetWindowLongPtr (hwndResList, GWLP_WNDPROC, (LONG_PTR)ResultsListHandler);
		} break;
		case WM_COMMAND:
        {
			switch(LOWORD(wParam))
            {
				case RESULTS_PGUP_CMD:
				{
					if (!ResultsList) { break; }
					u32 PageSize = SendMessage(hwndResList,LVM_GETCOUNTPERPAGE,0,0);
					CurrResNum = ShowResPage(CurrResNum - PageSize);
				} break;
				case RESULTS_PGDOWN_CMD:
				{
					if (!ResultsList) { break; }
					u32 PageSize = SendMessage(hwndResList,LVM_GETCOUNTPERPAGE,0,0);
					CurrResNum = ShowResPage(CurrResNum + PageSize);
				} break;
				case RESULTS_PAGE_GO_CMD:
				{
					if (!ResultsList) { break; }
					u32 PageSize = SendMessage(hwndResList,LVM_GETCOUNTPERPAGE,0,0);
					int PageNum = GetDecWindow(hwndResPageText);
					CurrResNum = ShowResPage(PageSize * PageNum);
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
Search Value box handler
*****************************************************************************/
LRESULT CALLBACK ResultsPageTxtHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CHAR:
        {
            if ((wParam == VK_BACK) || (wParam == 24) || (wParam == 3) || (wParam == 22)) { break; } //cut/copy/paste/backspace
            if (wParam == 1) { SendMessage(hwnd, EM_SETSEL, 0, -1); } //select all
            if (wParam == VK_RETURN) { SendMessage(hTabDlgs[SEARCH_RESULTS_TAB], WM_COMMAND, RESULTS_PAGE_GO_CMD, 0); }
            wParam = ((isdigit(wParam))|| (wParam == '-')) ? wParam : 0;
        } break;
        case WM_PASTE:
        {
        } return 0;
   }
   if (wpResPageTxtProc) {
	   return CallWindowProc (wpResPageTxtProc, hwnd, message, wParam, lParam);
   } else { return DefWindowProc (hwnd, message, wParam, lParam); }
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
    int SearchCount = SendMessage(hwndCompareTo,CB_GETCOUNT,0,0) - 1;
    if (!SearchCount) { return 0; }
	//reset columns
	SendMessage(hwndResList,LVM_DELETEALLITEMS,0,0);
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
    }

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
    u32 i, DumpNum, PageSize;
    float tmpFloat=0;
    u32 *CastFloat=(u32*)(&tmpFloat);
    double tmpDouble=0;
    u64 *CastDouble=(u64*)&tmpDouble;
    HWND hwndResList = GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], RESULTS_LSV);
    HWND hwndCompareTo = GetDlgItem(hTabDlgs[CODE_SEARCH_TAB], COMPARE_TO_CMB);
    PageSize = SendMessage(hwndResList,LVM_GETCOUNTPERPAGE,0,0);
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

    SetDecWindowU(GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], RESULTS_PAGE_TXT), ((ResNum + i)/PageSize) + ((ResNum + i) % PageSize));

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
        { sprintf(tmpstring,"%%0%uI64X", numbytes*2); } break;
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
