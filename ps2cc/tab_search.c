/****************************************************************************
Code Search tab procedure(s) and handlers

Most (if not all) of the search tab setup and GUI handling should be found
here. This includes search preparation, like grabbing all the needed info from
the interface to start the search.
*****************************************************************************/

#include "ps2cc.h"
#include "ps2cc_gui.h"

CODE_SEARCH_VARS Search;

LRESULT CALLBACK SearchValueBoxHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC wpSearchValueBoxProc;

BOOL CALLBACK CodeSearchProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndSearchSize = GetDlgItem(hwnd, SEARCH_SIZE_CMB);
    HWND hwndCompareTo = GetDlgItem(hwnd, COMPARE_TO_CMB);
    HWND hwndSearchType = GetDlgItem(hwnd, SEARCH_TYPE_CMB);
    HWND hwndSearchArea = GetDlgItem(hwnd, SEARCH_AREA_CMB);
//    HWND hwndExSearchList = GetDlgItem(hwnd, LSV_CS_EXSEARCH);
    HWND hwndSearchValue1 = GetDlgItem(hwnd, SEARCH_VALUE1_TXT);
    HWND hwndSearchValue2 = GetDlgItem(hwnd, SEARCH_VALUE2_TXT);
    HWND hwndSearchAreaLow = GetDlgItem(hwnd, SEARCH_AREA_LOW_TXT);
    HWND hwndSearchAreaHigh = GetDlgItem(hwnd, SEARCH_AREA_HIGH_TXT);
//    HWND hwndProgress = GetDlgItem(hwnd, PGB_CS_PROGRESS);
	switch(message)
	{
		case WM_INITDIALOG:
        {
			//subclassing for controls (value box handler etc)
		    wpHexEditBoxes = (WNDPROC)GetWindowLongPtr (hwndSearchAreaLow, GWLP_WNDPROC);
		    SetWindowLongPtr (hwndSearchAreaLow, GWLP_WNDPROC, (LONG_PTR)HexEditBoxHandler);
		    SetWindowLongPtr (hwndSearchAreaHigh, GWLP_WNDPROC, (LONG_PTR)HexEditBoxHandler);
		    wpSearchValueBoxProc = (WNDPROC)GetWindowLongPtr (hwndSearchValue1, GWLP_WNDPROC);
		    SetWindowLongPtr (hwndSearchValue1, GWLP_WNDPROC, (LONG_PTR)SearchValueBoxHandler);
		    SetWindowLongPtr (hwndSearchValue2, GWLP_WNDPROC, (LONG_PTR)SearchValueBoxHandler);

            //Search Sizes
            SendMessage(hwndSearchSize,CB_RESETCONTENT,0,0);
            ComboAddItem(hwndSearchSize, "8-Bit (1 Byte)" , 1);
            ComboAddItem(hwndSearchSize, "16-Bit (2 Bytes)" , 2);
            ComboAddItem(hwndSearchSize, "32-Bit (4 Bytes)" , 4);
            ComboAddItem(hwndSearchSize, "64-Bit (8 Bytes)" , 8);

            //Compare To
            SendMessage(hwndCompareTo,CB_RESETCONTENT,0,0);
            ComboAddItem(hwndCompareTo, "New Search" , 0);

			//Search Area
            SendMessage(hwndSearchArea,CB_RESETCONTENT,0,0);
            ComboAddItem(hwndSearchArea, "EE" , 0);
            ComboAddItem(hwndSearchArea, "IOP" , 1);
            ComboAddItem(hwndSearchArea, "Kernal" , 2);
            ComboAddItem(hwndSearchArea, "ScratchPad" , 3);
            ComboAddItem(hwndSearchArea, "Custom" , 4);
            SendMessage(hwndSearchAreaLow, EM_SETLIMITTEXT, 8, 0);
            SendMessage(hwndSearchAreaHigh, EM_SETLIMITTEXT, 8, 0);
            //unfinished

            //Search Types
            SendMessage(hwndSearchType,CB_RESETCONTENT,0,0);
            ComboAddItem(hwndSearchType, "Initial Dump" , SEARCH_INIT);
            ComboAddItem(hwndSearchType, "I Forgot (just dump again and keep results)" , SEARCH_FORGOT);
            ComboAddItem(hwndSearchType, "Known Value" , SEARCH_KNOWN);
            ComboAddItem(hwndSearchType, "Known Value w/ Wildcards (Hex Input Only)" , SEARCH_KNOWN_WILD);
            ComboAddItem(hwndSearchType, "Greater Than" , SEARCH_GREATER);
            ComboAddItem(hwndSearchType, "Greater Than By <value>" , SEARCH_GREATER_BY);
            ComboAddItem(hwndSearchType, "Greater Than By At Least <value>" , SEARCH_GREATER_LEAST);
            ComboAddItem(hwndSearchType, "Greater Than By At Most <value>" , SEARCH_GREATER_MOST);
            ComboAddItem(hwndSearchType, "Less Than" , SEARCH_LESS);
            ComboAddItem(hwndSearchType, "Less Than By <value>" , SEARCH_LESS_BY);
            ComboAddItem(hwndSearchType, "Less Than By At Least <value>" , SEARCH_LESS_LEAST);
            ComboAddItem(hwndSearchType, "Less Than By At Most <value>" , SEARCH_LESS_MOST);
            ComboAddItem(hwndSearchType, "Equal To" , SEARCH_EQUAL);
            ComboAddItem(hwndSearchType, "Equal To # Bits" , SEARCH_EQUAL_NUM_BITS);
            ComboAddItem(hwndSearchType, "Not Equal To" , SEARCH_NEQUAL);
            ComboAddItem(hwndSearchType, "Not Equal To <value>" , SEARCH_NEQUAL_TO);
            ComboAddItem(hwndSearchType, "Not Equal By <value>" , SEARCH_NEQUAL_BY);
            ComboAddItem(hwndSearchType, "Not Equal By At Least <value>" , SEARCH_NEQUAL_LEAST);
            ComboAddItem(hwndSearchType, "Not Equal By At Most <value>" , SEARCH_NEQUAL_MOST);
            ComboAddItem(hwndSearchType, "Not Equal To # Bits" , SEARCH_NEQUAL_TO_BITS);
            ComboAddItem(hwndSearchType, "Not Equal By # Bits" , SEARCH_NEQUAL_BY_BITS);
            ComboAddItem(hwndSearchType, "In-Range" , SEARCH_IN_RANGE);
            ComboAddItem(hwndSearchType, "Not In-Range" , SEARCH_NOT_RANGE);
            ComboAddItem(hwndSearchType, "Active Bits (Any)" , SEARCH_BITS_ANY);
            ComboAddItem(hwndSearchType, "Active Bits (All)" , SEARCH_BITS_ALL);

            //To Do: extended search options (signed, filter results, etc)

            //Set starting positions for the dropdown lists (last)
            SendMessage(hwndSearchSize,CB_SETCURSEL,0,0);
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(SEARCH_SIZE_CMB, CBN_SELCHANGE),(LPARAM)hwndSearchSize);
            SendMessage(hwndCompareTo,CB_SETCURSEL,0,0);
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(COMPARE_TO_CMB, CBN_SELCHANGE),(LPARAM)hwndCompareTo);
            SendMessage(hwndSearchType,CB_SETCURSEL,0,0);
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(SEARCH_TYPE_CMB, CBN_SELCHANGE),(LPARAM)hwndSearchType);
            SendMessage(hwndSearchArea,CB_SETCURSEL,0,0);
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(SEARCH_AREA_CMB, CBN_SELCHANGE),(LPARAM)hwndSearchArea);

        } break;
		case WM_COMMAND:
        {
			switch(LOWORD(wParam))
            {
                case SEARCH_AREA_CMB: //handle user interaction with the Search Area combo box
                {
                    switch(HIWORD(wParam))
                    {
                        case CBN_SELCHANGE:
                        {
                            switch(SendMessage(hwndSearchArea,CB_GETITEMDATA,SendMessage(hwndSearchArea,CB_GETCURSEL,0,0),0))
                            {
                            	case 0: //EE
                            	{
									SetWindowText(hwndSearchAreaLow, "00100000");
									SetWindowText(hwndSearchAreaHigh, "02000000");
									EnableWindow (hwndSearchAreaLow, FALSE);
									EnableWindow (hwndSearchAreaHigh, FALSE);
								} break;
                            	case 1: //IOP
                            	{
									SetWindowText(hwndSearchAreaLow, "00000000");
									SetWindowText(hwndSearchAreaHigh, "00200000");
									EnableWindow (hwndSearchAreaLow, FALSE);
									EnableWindow (hwndSearchAreaHigh, FALSE);
								} break;
                            	case 2: //Kernal
                            	{
									SetWindowText(hwndSearchAreaLow, "80000000");
									SetWindowText(hwndSearchAreaHigh, "82000000");
									EnableWindow (hwndSearchAreaLow, FALSE);
									EnableWindow (hwndSearchAreaHigh, FALSE);
								} break;
                            	case 3: //ScratchPad
                            	{
									SetWindowText(hwndSearchAreaLow, "70000000");
									SetWindowText(hwndSearchAreaHigh, "70004000");
									EnableWindow (hwndSearchAreaLow, FALSE);
									EnableWindow (hwndSearchAreaHigh, FALSE);
								} break;
                            	case 4: //Custom
                            	{
									SetWindowText(hwndSearchAreaLow, "00100000");
									SetWindowText(hwndSearchAreaHigh, "02000000");
									EnableWindow (hwndSearchAreaLow, TRUE);
									EnableWindow (hwndSearchAreaHigh, TRUE);
								} break;
							}
                        } break;
                    }
                } break;
                case SEARCH_SIZE_CMB: //handle user interaction with the Search Size combo box
                {
                    switch(HIWORD(wParam))
                    {
                        case CBN_SELCHANGE:
                        {
                            Search.Size = SendMessage(hwndSearchSize,CB_GETITEMDATA,SendMessage(hwndSearchSize,CB_GETCURSEL,0,0),0);
                            switch((Search.Type == SEARCH_KNOWN_WILD) ? BASE_HEX : Settings.CS.NumBase)
                            {
                                case BASE_DEC:
                                {
                                    SendMessage(hwndSearchValue1, EM_SETLIMITTEXT, 31, 0);
                                    SendMessage(hwndSearchValue2, EM_SETLIMITTEXT, 31, 0);
                                } break;
                                case BASE_HEX:
                                {
                                    SendMessage(hwndSearchValue1, EM_SETLIMITTEXT, Search.Size*2, 0);
                                    SendMessage(hwndSearchValue2, EM_SETLIMITTEXT, Search.Size*2, 0);
                                } break;
                                case BASE_FLOAT:
                                {
                                    SendMessage(hwndSearchValue1, EM_SETLIMITTEXT, 32, 0);
                                    SendMessage(hwndSearchValue2, EM_SETLIMITTEXT, 32, 0);
                                } break;
                            }
                        } break;
                    }
                } break;
                case SEARCH_TYPE_CMB: //handle user interaction with the Search Type combo box
                {
                    switch(HIWORD(wParam))
                    {
                        case CBN_SELCHANGE:
                        {
                            Search.Type = SendMessage(hwndSearchType,CB_GETITEMDATA,SendMessage(hwndSearchType,CB_GETCURSEL,0,0),0);
                            Search.CompareTo = SendMessage(hwndCompareTo,CB_GETCURSEL,0,0);
                            SetWindowText(hwndSearchValue1, "0");
                            SetWindowText(hwndSearchValue2, "0");
                            switch(Search.Type)
                            {
                                case SEARCH_INIT:
                                {
                                    SetWindowPos(hwndSearchValue1, 0, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
                                    SetWindowPos(hwndSearchValue2, 0, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
                                    SendMessage(hwndCompareTo,CB_SETCURSEL,0,0);
                                    SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(COMPARE_TO_CMB, CBN_SELCHANGE),(LPARAM)hwndCompareTo);
                                } break;
                                case SEARCH_KNOWN: case SEARCH_KNOWN_WILD:
                                case SEARCH_GREATER_BY: case SEARCH_GREATER_LEAST: case SEARCH_GREATER_MOST:
                                case SEARCH_LESS_BY: case SEARCH_LESS_LEAST: case SEARCH_LESS_MOST:
                                case SEARCH_EQUAL_NUM_BITS: case SEARCH_NEQUAL_TO: case SEARCH_NEQUAL_BY: case SEARCH_NEQUAL_LEAST:
                                case SEARCH_NEQUAL_MOST: case SEARCH_NEQUAL_TO_BITS: case SEARCH_NEQUAL_BY_BITS:
                                case SEARCH_BITS_ANY: case SEARCH_BITS_ALL:
                                {
                                    SetWindowPos(hwndSearchValue1, 0, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
                                    SetWindowPos(hwndSearchValue2, 0, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
                                } break;
                                case SEARCH_GREATER: case SEARCH_LESS: case SEARCH_EQUAL: case SEARCH_NEQUAL:
                                {
                                    if (Search.CompareTo == 0) { MessageBox(NULL, "You can't compare without an initial dump, asshole.", "Error", MB_OK); return 0; }
                                    SetWindowPos(hwndSearchValue1, 0, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
                                    SetWindowPos(hwndSearchValue2, 0, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
                                } break;
                                case SEARCH_IN_RANGE: case SEARCH_NOT_RANGE:
                                {
                                    SetWindowPos(hwndSearchValue1, 0, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
                                    SetWindowPos(hwndSearchValue2, 0, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
                                } break;
                            }
                            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(SEARCH_SIZE_CMB, CBN_SELCHANGE),(LPARAM)hwndSearchSize);
                        } break;
                    }
                } break;
                case TAKE_DUMP_CMD: //Dump button
                {
					//get start and end address
					u32 DumpAreaLow = GetHexWindow(hwndSearchAreaLow);
					u32 DumpAreaHigh = GetHexWindow(hwndSearchAreaHigh);
					//get filename
					char dmpFileName[MAX_PATH];
                    if (!DoFileSave(hwnd, dmpFileName)) { break; }
                    //Dump the RAM
                    if (!(DumpRAM(dmpFileName, DumpAreaLow, DumpAreaHigh))) {
						MessageBox(NULL, ErrTxt, "Error", MB_OK); break;
					}
					MessageBox(NULL, "Did it work? You shouldn't see this until dumping is complete.", "Debug", MB_OK);
				} break;
			    case DO_SEARCH_CMD: //Search button click
			    {
                    memset(&Search,0,sizeof(CODE_SEARCH_VARS));
                    FreeRamInfo();
                    Search.Size = SendMessage(hwndSearchSize,CB_GETITEMDATA,SendMessage(hwndSearchSize,CB_GETCURSEL,0,0),0);
                    Search.Type = SendMessage(hwndSearchType,CB_GETITEMDATA,SendMessage(hwndSearchType,CB_GETCURSEL,0,0),0);
                    Search.CompareTo = SendMessage(hwndCompareTo,CB_GETCURSEL,0,0);
                    Search.Count = SendMessage(hwndCompareTo,CB_GETCOUNT,0,0);
                    if (Search.CompareTo == 0) { Search.Count = 1; }
                    int i;
                    char txtValue[17];

                    //Switch case based on search type to grab whatever input we need in the format we need.
                    switch(Search.Type)
                    {
                        case SEARCH_INIT:
                        {
                            Search.CompareTo = 0;
                            Search.Count = 1;
                        } break;
                        case SEARCH_KNOWN_WILD:
                        {
                            GetWindowText(hwndSearchValue1, txtValue, sizeof(txtValue));
                            Search.Values[1] = 0xFFFFFFFFFFFFFFFFLL;
                            for (i = 0; i < strlen(txtValue); i++) {
                                if (txtValue[i] == '*') {
                                    Search.Values[1] &= ~((u64)0xF << ((strlen(txtValue) - i - 1) * 4));
                                    txtValue[i] = '0';
                                }
                            }
                            String2Hex(txtValue,&Search.Values[0]);
                        } break;
                        case SEARCH_KNOWN:
                        case SEARCH_GREATER_BY: case SEARCH_GREATER_LEAST: case SEARCH_GREATER_MOST:
                        case SEARCH_LESS_BY: case SEARCH_LESS_LEAST: case SEARCH_LESS_MOST:
                        case SEARCH_EQUAL_NUM_BITS: case SEARCH_NEQUAL_TO: case SEARCH_NEQUAL_BY: case SEARCH_NEQUAL_LEAST:
                        case SEARCH_NEQUAL_MOST: case SEARCH_NEQUAL_TO_BITS: case SEARCH_NEQUAL_BY_BITS:
                        case SEARCH_BITS_ANY: case SEARCH_BITS_ALL:
                        case SEARCH_IN_RANGE: case SEARCH_NOT_RANGE:
                        {
                            switch((Search.Type & (SEARCH_EQUAL_NUM_BITS|SEARCH_NEQUAL_TO_BITS|SEARCH_NEQUAL_BY_BITS|SEARCH_BITS_ANY|SEARCH_BITS_ALL)) &&
                            (Settings.CS.NumBase == BASE_FLOAT) ? BASE_HEX : Settings.CS.NumBase)
                            {
                                case BASE_HEX:
                                {
                                    Search.Values[0] = GetHexWindow(hwndSearchValue1);
                                    Search.Values[1] = GetHexWindow(hwndSearchValue2);
                                } break;
                                case BASE_DEC:
                                {
                                    Search.Values[0] = GetDecWindow(hwndSearchValue1);
                                    Search.Values[1] = GetDecWindow(hwndSearchValue2);
                                } break;
                                case BASE_FLOAT:
                                {
                                    Search.Values[0] = GetFloatWindow(hwndSearchValue1, Search.Size);
                                    Search.Values[1] = GetFloatWindow(hwndSearchValue2, Search.Size);
                                } break;
                            }
                        } break;
                        case SEARCH_GREATER: case SEARCH_LESS: case SEARCH_EQUAL: case SEARCH_NEQUAL:
                        {
                            if ((Search.Count == 1) || (Search.CompareTo == 0)) { MessageBox(NULL, "You can't compare without an initial dump, n00b.", "Error", MB_OK); return 0; }
                        } break;
                    }
                    if (Search.Count > MAX_SEARCHES) { MessageBox(NULL,"Holy shit! 100 searches? If you didn't find the code by now, give it up.","Error",MB_OK); break; }


                    char sdFileName[MAX_PATH];
                    //Load previous results if this continuing a search.
                    if (Search.CompareTo) {
                        sprintf(sdFileName, "%ssearch%u.bin", Settings.CS.DumpDir, Search.CompareTo);
                        if (!(LoadStruct(&RamInfo.OldResultsInfo, sizeof(CODE_SEARCH_RESULTS_INFO), sdFileName))) { break; }
                        if (!(LoadFile(&RamInfo.Results, sdFileName, sizeof(CODE_SEARCH_RESULTS_INFO), NULL, FALSE))) { break; }
                    }

                    sprintf(RamInfo.NewResultsInfo.sdFileName, "%sdump%u.raw", Settings.CS.DumpDir, Search.Count);
//                    RamInfo.NewResultsInfo.Endian = ;
                    RamInfo.NewResultsInfo.SearchSize = Search.Size;

				} break;
			}
		} break;


//        default:
//        	return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return FALSE;
}

/****************************************************************************
Search Value box handler
*****************************************************************************/
LRESULT CALLBACK SearchValueBoxHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CHAR:
        {
            if ((wParam == VK_BACK) || (wParam == 24) || (wParam == 3) || (wParam == 22)) { break; } //cut/copy/paste/backspace
            if (wParam == 1) { SendMessage(hwnd, EM_SETSEL, 0, -1); } //select all
            HWND hwndSearchType = GetDlgItem(hTabDlgs[CODE_SEARCH_TAB], SEARCH_TYPE_CMB);
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
        case WM_PASTE:
        {
            char txtInput[20], txtInput2[20];
            GetWindowText(hwnd, txtInput, sizeof(txtInput));
            CallWindowProc (wpHexEditBoxes, hwnd, message, wParam, lParam);
            GetWindowText(hwnd, txtInput2, sizeof(txtInput2));
            if ((!isHexWindow(hwnd)) || (strlen(txtInput2) > SendMessage(hwnd, EM_GETLIMITTEXT, 0, 0))) { SetWindowText(hwnd, txtInput); }
        } return 0;
        default:
        {
            if (wpSearchValueBoxProc) {
                return CallWindowProc (wpSearchValueBoxProc, hwnd, message, wParam, lParam);
            } else { return DefWindowProc (hwnd, message, wParam, lParam); }
        }
   }
   return CallWindowProc (wpSearchValueBoxProc, hwnd, message, wParam, lParam);
}
