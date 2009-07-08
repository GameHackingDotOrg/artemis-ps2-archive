/****************************************************************************
Search Library

This is where the main search loop runs, as well as supplemental search
functions. A few specific globals are in use (I think just RamInfo), and 1 API
(progress bar). If updating/adding options at any point, please make an effort
not to utilize for any globals or APIs that aren't already in use.

Each comparison type actually compares to opposite. You start with every
address as a possibly. Then find the bad results and turn them OFF in the list.
SetBitflag may seem overly complicated, but you should be able to add your own
options without really understanding it. Just do what I did. Every call to
SetBitFlag in the main search loop is
*****************************************************************************/
#include "ps2cc.h"


/****************************************************************************
CodeSearch - main search loop
*****************************************************************************/
int CodeSearch(CODE_SEARCH_VARS Search, HWND hProgressBar)
{
    u32 address;
    u64 NewValue;
    u64 OldValue;
//    RamInfo.NewResultsInfo.ResCount = 0;
    if (Search.TypeEx & EXCS_SIGNED) {
        int i;
        for (i = 0; i < 10; i++) { Search.Values[i] = SignExtend64(Search.Values[i], Search.Size); }
    }
    for (address = 0; address < RamInfo.NewResultsInfo.DumpSize; address += Search.Size) {
        if(!(address % 0x100000)) { SendMessage(hProgressBar, PBM_STEPIT, 0, 0); }
        if (!(GetBitFlag(RamInfo.Results, address/Search.Size))) { continue; }
        GetSearchValues(&NewValue, &OldValue, address, Search.Size, LITTLE_ENDIAN_SYS);
//        if (!CodeSearchEx(address, NewValue, OldValue, Search)) { continue; }
        if (Search.TypeEx & EXCS_SIGNED) {
            NewValue = SignExtend64(NewValue, Search.Size);
            OldValue = SignExtend64(OldValue, Search.Size);
        }
        switch(Search.Type)
        {
            case SEARCH_KNOWN:
            {
                if (NewValue != Search.Values[0]) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_KNOWN_WILD:
            {
                if ((NewValue & Search.Values[1]) != Search.Values[0]) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_GREATER:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if ((s64)NewValue <= (s64)OldValue) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if (NewValue <= OldValue) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_GREATER_BY:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if ((s64)NewValue != ((s64)OldValue + Search.Values[0])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if (NewValue != (OldValue + Search.Values[0])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_GREATER_LEAST:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if ((s64)NewValue < ((s64)OldValue + Search.Values[0])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if (NewValue < (OldValue + Search.Values[0])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_GREATER_MOST:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if (((s64)NewValue > ((s64)OldValue + Search.Values[0])) || ((s64)NewValue <= (s64)OldValue)) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if ((NewValue > (OldValue + Search.Values[0])) || (NewValue <= OldValue)) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_LESS:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if ((s64)NewValue >= (s64)OldValue) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if (NewValue >= OldValue) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_LESS_BY:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if ((s64)NewValue != ((s64)OldValue - Search.Values[0])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if (NewValue != (OldValue - Search.Values[0])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_LESS_LEAST:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if ((s64)NewValue > ((s64)OldValue - Search.Values[0])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if (NewValue > (OldValue - Search.Values[0])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_LESS_MOST:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if (((s64)NewValue < ((s64)OldValue - Search.Values[0])) || ((s64)NewValue >= (s64)OldValue)) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if ((NewValue < (OldValue - Search.Values[0])) || (NewValue >= OldValue)) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_EQUAL:
            {
                if (NewValue != OldValue) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_EQUAL_NUM_BITS:
            {
                if (BitCount(NewValue) != Search.Values[0]) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_NEQUAL:
            {
                if (NewValue == OldValue) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_NEQUAL_TO:
            {
                if (NewValue == Search.Values[0]) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_NEQUAL_BY:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if (((s64)NewValue != ((s64)OldValue - Search.Values[0])) && ((s64)NewValue != ((s64)OldValue + Search.Values[0]))) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if ((NewValue != (OldValue - Search.Values[0])) && (NewValue != (OldValue + Search.Values[0]))) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_NEQUAL_LEAST:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if (((s64)NewValue > ((s64)OldValue - Search.Values[0])) && ((s64)NewValue < ((s64)OldValue + Search.Values[0]))) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if ((NewValue > (OldValue - Search.Values[0])) && (NewValue < (OldValue + Search.Values[0]))) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_NEQUAL_MOST:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if (((s64)NewValue < ((s64)OldValue - Search.Values[0])) || ((s64)NewValue > ((s64)OldValue + Search.Values[0])) || (NewValue == OldValue)) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if ((NewValue < (OldValue - Search.Values[0])) || (NewValue > (OldValue + Search.Values[0])) || (NewValue == OldValue)) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_NEQUAL_TO_BITS:
            {
                if (BitCount(NewValue) == Search.Values[0]) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_NEQUAL_BY_BITS:
            {
               if (BitCount(NewValue ^ OldValue) != Search.Values[0]) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_IN_RANGE:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if (((s64)NewValue <= Search.Values[0]) || ((s64)NewValue > Search.Values[1])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if ((NewValue <= Search.Values[0]) || (NewValue > Search.Values[1])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_NOT_RANGE:
            {
                if (Search.TypeEx & EXCS_SIGNED) {
                    if (((s64)NewValue >= Search.Values[0]) && ((s64)NewValue <= Search.Values[1])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
                } else if ((NewValue >= Search.Values[0]) && (NewValue <= Search.Values[1])) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_BITS_ANY:
            {
                if ((NewValue & Search.Values[0]) == 0) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
            case SEARCH_BITS_ALL:
            {
                if ((NewValue & Search.Values[0]) != Search.Values[0]) { SetBitFlag(RamInfo.Results, address/Search.Size, 0); }
            } break;
        }
        if (GetBitFlag(RamInfo.Results, address/Search.Size)) { RamInfo.NewResultsInfo.ResCount++; }
    }
//    if (Search.TypeEx & (EXCS_EXCLUDE_CONSEC|EXCS_INCLUDE_CONSEC|EXCS_EXCLUDE_CONSEC_MATCH_VALUES|EXCS_INCLUDE_CONSEC_MATCH_VALUES)) { FilterResultsEx(Search, hProgressBar); }
    return 1;
}

/****************************************************************************
Get Search Values -- RamInfo.NewRAM or RamInfo.NewFile must be loaded before
calling this. It grabs any size value from the address (index) and loads them
into NewVal and OldVal via pointer.
*****************************************************************************/
u64 GetSearchValues(u64 *NewVal, u64 *OldVal, int index, int size, int endian)
{
    int i;
//    u64 tmpvalue = 0;
    *NewVal = 0;
    if(RamInfo.OldRAM) {  *OldVal = 0; }
    if (RamInfo.Access == SEARCH_ACCESS_ARRAY) {
        switch (endian)
        {
            case LITTLE_ENDIAN_SYS:
            {
                switch (size)
                {
                    case 1:
                    {
                        *NewVal = RamInfo.NewRAM[index];
                        if(RamInfo.OldRAM) { *OldVal = RamInfo.OldRAM[index]; }
                    } return 1;
                    case 2:
                    {
                        *NewVal = *(u16*)(&RamInfo.NewRAM[index]);
                        if(RamInfo.OldRAM) { *OldVal = *(u16*)(&RamInfo.OldRAM[index]); }
                    } return 1;
                    case 4:
                    {
                        *NewVal = *(u32*)(&RamInfo.NewRAM[index]);
                        if(RamInfo.OldRAM) { *OldVal = *(u32*)(&RamInfo.OldRAM[index]); }
                    } return 1;
                    case 8:
                    {
                        *NewVal = MAKEU64(*(u32*)(&RamInfo.NewRAM[index+4]),*(u32*)(&RamInfo.NewRAM[index]));
                        if(RamInfo.OldRAM) { *OldVal = MAKEU64(*(u32*)(&RamInfo.OldRAM[index+4]),*(u32*)(&RamInfo.OldRAM[index])); }
                    } return 1;
                }
            } break;
        }
    } else {
        switch (endian)
        {
            case LITTLE_ENDIAN_SYS:
            {
                fseek(RamInfo.NewFile,index,SEEK_SET);
                if (RamInfo.OldFile) { fseek(RamInfo.OldFile,index,SEEK_SET); }
                switch (size)
                {
                    case 1:
                    {
                        *NewVal = getc(RamInfo.NewFile);
                        if (RamInfo.OldFile) { *OldVal = getc(RamInfo.OldFile); }
                    } return 1;
                    case 2:
                    {
                        *NewVal = getc(RamInfo.NewFile) | (getc(RamInfo.NewFile)<<8);
                        if (RamInfo.OldFile) { *OldVal = getc(RamInfo.OldFile) | (getc(RamInfo.OldFile)<<8); }
                    } return 1;
                    case 4:
                    {
                        *NewVal = getw(RamInfo.NewFile);
                        if (RamInfo.OldFile) { *OldVal = getw(RamInfo.OldFile); }
                    } return 1;
                    case 8:
                    {
                        *NewVal = ((u64)getw(RamInfo.NewFile)<<32) | getw(RamInfo.NewFile);
                        if (RamInfo.OldFile) { *OldVal = ((u64)getw(RamInfo.OldFile)<<32) | getw(RamInfo.OldFile); }
                    } return 1;
                }
            } break;
        }
    }
    return 0;
}
