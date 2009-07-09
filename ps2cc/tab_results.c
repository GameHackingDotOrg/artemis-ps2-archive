/****************************************************************************
Results tab procedure(s) and handlers

Everything for the results display should pretty much be here.
*****************************************************************************/
#include "ps2cc.h"
#include "ps2cc_gui.h"

WNDPROC wpResultsListProc;

s64 CurrResNum = 0;
u32 *ResultsList;

BOOL CALLBACK SearchResultsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndResList = GetDlgItem(hwnd, RESULTS_LSV);
	switch(message)
	{
		case WM_INITDIALOG:
        {
            SendMessage(hwndResList,LVM_DELETEALLITEMS,0,0);
            SendMessage(hwndResList,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_LABELTIP);
            SendMessage(hwndResList, WM_SETFONT, (WPARAM)Settings.ValueHFont, TRUE);
            ListViewAddCol(hwndResList, "Address", 0, 0x80);
//		    wpResultsListProc = (WNDPROC)GetWindowLongPtr (hwndResList, GWLP_WNDPROC);
//		    SetWindowLongPtr (hwndResList, GWLP_WNDPROC, (LONG_PTR)ResultsListHandler);
		} break;
        case WM_SHOWWINDOW:
        {
//            if (wParam) { LoadResultsList(); }
        } break;
	}
	return FALSE;
}


/****************************************************************************
Load results
*****************************************************************************/
int LoadResultsList()
{
    CurrResNum = 0;
    FreeRamInfo();
    u32 i, DumpAddy, DumpNum;
    FILE *ramFiles[MAX_SEARCHES];
    char txtValue[32];
    HWND hwndResList = GetDlgItem(hTabDlgs[SEARCH_RESULTS_TAB], RESULTS_LSV);
    HWND hwndCompareTo = GetDlgItem(hTabDlgs[CODE_SEARCH_TAB], COMPARE_TO_CMB);
    int SearchCount = SendMessage(hwndCompareTo,CB_GETCOUNT,0,0) - 1;
    if (!SearchCount) { return 0; }
	//reset columns
	SendMessage(hwndResList,LVM_DELETEALLITEMS,0,0);
    while (SendMessage(hwndResList,LVM_DELETECOLUMN,i,0)) { i++; }
	//load current results info
    char resFileName[MAX_PATH];
    sprintf(resFileName,"%ssearch%u.bin",Settings.CS.DumpDir, SearchCount);
    if (!(LoadStruct(&RamInfo.NewResultsInfo, sizeof(CODE_SEARCH_RESULTS_INFO), resFileName))) { goto LoadResError; }
    if (!(LoadFile(&RamInfo.Results, resFileName, sizeof(CODE_SEARCH_RESULTS_INFO), NULL, FALSE))) { goto LoadResError; }

	//open dump file handles
    for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) {
        sprintf(resFileName,"%ssearch%u.bin",Settings.CS.DumpDir, (SearchCount - DumpNum));
        if (!(LoadStruct(&RamInfo.OldResultsInfo, sizeof(CODE_SEARCH_RESULTS_INFO), resFileName))) { goto LoadResError; } //should also close handles
        ramFiles[DumpNum] = fopen(RamInfo.OldResultsInfo.dmpFileName,"rb");
	    if (!(ramFiles[DumpNum])) {
            sprintf(ErrTxt, "Unable to open dump file (LoadResultsList) -- Error %u", GetLastError());
            MessageBox(NULL,ErrTxt,"Error",MB_OK);
       		goto LoadResError;
	    }
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
	DumpAddy = 0; i = 0;
    while ((DumpAddy < RamInfo.NewResultsInfo.DumpSize) && (i < RamInfo.NewResultsInfo.ResCount)) {
        if (!(GetBitFlag(RamInfo.Results, DumpAddy/RamInfo.NewResultsInfo.SearchSize))) { DumpAddy += RamInfo.NewResultsInfo.SearchSize; continue; }
        ResultsList[i] = DumpAddy;
        DumpAddy += RamInfo.NewResultsInfo.SearchSize;
        i++;
    }

	//cleanup
	for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) { fclose(ramFiles[DumpNum]); }
	FreeRamInfo();
    CurrResNum = ShowResPage(CurrResNum);
	return 1;
LoadResError:
	for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) {
		if(ramFiles[DumpNum]) { fclose(ramFiles[DumpNum]); }
	}
    if (ResultsList) { free(ResultsList); ResultsList = NULL; }
	FreeRamInfo();
	return 0;
}
/****************************************************************************
Show 1 page of results starting in ResAddr
*****************************************************************************/
s64 ShowResPage(s64 ResNum)
{
    u32 i, DumpNum, PageSize, DumpAddy;
    float tmpFloat=0;
    u32 *CastFloat=(u32*)(&tmpFloat);
    double tmpDouble=0;
    u64 *CastDouble=(u64*)&tmpDouble;
    char txtAddress[9], txtValue[32], fmtString[20];
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
	//open dump file handles
    for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) {
        sprintf(resFileName,"%ssearch%u.bin",Settings.CS.DumpDir, (SearchCount - DumpNum));
        if (!(LoadStruct(&RamInfo.OldResultsInfo, sizeof(CODE_SEARCH_RESULTS_INFO), resFileName))) { return 0; }
        ramFiles[DumpNum] = fopen(RamInfo.OldResultsInfo.dmpFileName,"rb");
	    if (!(ramFiles[DumpNum])) {
            sprintf(ErrTxt, "Unable to open dump file (ShowResPage,1) -- Error %u", GetLastError());
            MessageBox(NULL,ErrTxt,"Error",MB_OK); return 0;
	    }
    }
	fseek(ramFiles[0],0,SEEK_END);
	RamInfo.NewResultsInfo.DumpSize = ftell(ramFiles[0]);
	fseek(ramFiles[0],0,SEEK_SET);
	RamInfo.Access = SEARCH_ACCESS_FILE;

	i = 0;
	DumpAddy = 0;
/*
    while ((DumpAddy < RamInfo.NewResultsInfo.DumpSize) && (ResNum < RamInfo.NewResultsInfo.ResCount) && (i < PageSize)) {
        if (!(GetBitFlag(RamInfo.Results, DumpAddy/RamInfo.NewResultsInfo.SearchSize))) { DumpAddy += RamInfo.NewResultsInfo.SearchSize; continue; }
        ResultsList[i].Address = TranslateAddressMap(AddressMap, MapSize, DumpAddy);
        for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) {
            RamInfo.NewFile = ramFiles[DumpNum];
            GetSearchValues(&ResValue, NULL, DumpAddy, RamInfo.NewResultsInfo.SearchSize, RamInfo.NewResultsInfo.Endian);
            ResultsList[i].Values[DumpNum] = ResValue;
        }
        DumpAddy += RamInfo.NewResultsInfo.SearchSize;
        i++;
        ResNum++;
    }
*/
    for (DumpNum = 0; DumpNum < SearchCount; DumpNum++) { fclose(ramFiles[DumpNum]); }
    FreeRamInfo();
}
