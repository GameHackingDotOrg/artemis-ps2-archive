/****************************************************************************
Artemis - PS2 Code Creator (for lack of a better title) - Main Header
-PS2 dumping/communication functions by Jimmikaelkael
-Code searching by Viper187
*****************************************************************************/

#include <w32api.h>
#define WINVER WindowsNT4
#define _WIN32_IE IE6

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shlobj.h>
#include <winsock2.h>

#include "_types.h"

#define String2Hex(s,v) sscanf((s),"%x",(v))
#define MAKEU64(l,h) (u64)(l)|((u64)(h)<<32)

/****************************************************************************
Global Constants
*****************************************************************************/
#define NUM_TABS 5
#define MAX_SEARCHES 100

//Tab IDs
#define CODE_SEARCH_TAB 0
#define SEARCH_RESULTS_TAB 1
#define MEMORY_EDITOR_TAB 2
#define CHEAT_TAB 3

//Search types
#define SEARCH_INIT 0x0
#define SEARCH_KNOWN 0x1
#define SEARCH_KNOWN_WILD 0x2
#define SEARCH_GREATER 0x4
#define SEARCH_GREATER_BY 0x8
#define SEARCH_GREATER_LEAST 0x10
#define SEARCH_GREATER_MOST 0x20
#define SEARCH_LESS 0x40
#define SEARCH_LESS_BY 0x80
#define SEARCH_LESS_LEAST 0x100
#define SEARCH_LESS_MOST 0x200
#define SEARCH_EQUAL 0x400
#define SEARCH_EQUAL_NUM_BITS 0x800
#define SEARCH_NEQUAL 0x1000
#define SEARCH_NEQUAL_TO 0x2000
#define SEARCH_NEQUAL_BY 0x4000
#define SEARCH_NEQUAL_LEAST 0x8000
#define SEARCH_NEQUAL_MOST 0x10000
#define SEARCH_NEQUAL_TO_BITS 0x20000
#define SEARCH_NEQUAL_BY_BITS 0x40000
#define SEARCH_IN_RANGE 0x80000
#define SEARCH_NOT_RANGE 0x100000
#define SEARCH_BITS_ANY 0x200000
#define SEARCH_BITS_ALL 0x400000
#define SEARCH_FORGOT 0x800000

//Extended Search Types
#define EXCS_SIGNED 0x1
#define EXCS_OR_EQUAL 0x2
#define EXCS_IGNORE_0 0x4
#define EXCS_IGNORE_FF 0x8
#define EXCS_IGNORE_VALUE 0x10
#define EXCS_IGNORE_IN_RANGE 0x20
#define EXCS_IGNORE_NOT_RANGE 0x40
#define EXCS_IGNORE_N64_POINTERS 0x80
#define EXCS_IGNORE_BYTE_VALUE 0x100
#define EXCS_IGNORE_SHORT_VALUE 0x200
#define EXCS_IGNORE_WORD_VALUE 0x400
#define EXCS_IGNORE_DWORD_VALUE 0x800
#define EXCS_IGNORE_BYTE_RANGE 0x1000
#define EXCS_IGNORE_SHORT_RANGE 0x2000
#define EXCS_IGNORE_WORD_RANGE 0x4000
#define EXCS_IGNORE_DWORD_RANGE 0x8000
#define EXCS_EXCLUDE_CONSEC 0x10000
#define EXCS_EXCLUDE_CONSEC_MATCH_VALUES 0x20000
#define EXCS_INCLUDE_CONSEC 0x40000
#define EXCS_INCLUDE_CONSEC_MATCH_VALUES 0x80000
#define EXCS_INCLUDE_ADDRESS_RANGE 0x100000
#define EXCS_EXCLUDE_UPPER16 0x200000
#define EXCS_EXCLUDE_LOWER16 0x400000

//Number Bases (for input)
#define BASE_DEC 0
#define BASE_HEX 1
#define BASE_FLOAT 2
#define BASE_ASCII 0x100 //for certain custom window/control printing functions

//Endian
#define LITTLE_ENDIAN_SYS 0

/*Search Access - this determines how values are access from dumps. Either by
loading them fully, or opening and reading a value at a time*/
#define SEARCH_ACCESS_ARRAY 1
#define SEARCH_ACCESS_FILE 2

/****************************************************************************
Struct Definitions
*****************************************************************************/

//Code Search Vars - This type holds all the input gathered for a search.
typedef struct _CODE_SEARCH_VARS {
    int Count;
    int CompareTo;
    int Size;
    u32 Type;
    u32 TypeEx;
    u64 Values[10];
    u64 ValuesEx1[64];
    u64 ValuesEx2[64];
} CODE_SEARCH_VARS;

//These are the vars included in the header of the binary results files.
typedef struct CODE_SEARCH_RESULTS_INFO {
    char dmpFileName[MAX_PATH];
    int Endian;
    int SearchSize;
    u32 DumpSize;
    u32 ResCount;
    u32 MapFileAddy; //file address
    u32 MapMemAddy; //memory address
} CODE_SEARCH_RESULTS_INFO;

/*Results list with previous values (loaded from each of the previous RAM files for
showing results)*/
typedef struct _SEARCH_RESULTS_LIST {
    char Size;
    u32 Address;
//    u64 Values[MAX_SEARCHES];
} SEARCH_RESULTS_LIST;

//dump data and results during searches
typedef struct _RAM_AND_RES_DATA {
    int Access;
    u8 *NewRAM;
    u8 *OldRAM;
    FILE *NewFile;
    FILE *OldFile;
    u8 *Results;
    CODE_SEARCH_RESULTS_INFO OldResultsInfo;
    CODE_SEARCH_RESULTS_INFO NewResultsInfo;
} RAM_AND_RES_DATA;

//Sub-Structures for settings (typically 1 per tab)
typedef struct _CODE_SEARCH_SETTINGS {
    char DumpDir[MAX_PATH];
    int NumBase;
    int NumBaseId;
} CODE_SEARCH_SETTINGS;

typedef struct _SEARCH_RESULTS_SETTINGS {
    int DisplayFmt;
    int ExportFmt;
    int ResWriteRate;
    int ResWriteRateId;
    int RamView;
} SEARCH_RESULTS_SETTINGS;

//assuming there's a memory editor/viewer
typedef struct _MEMORY_EDITOR_SETTINGS {
    int RefreshRate;
    int ByteSwap;
} MEMORY_EDITOR_SETTINGS;

//Main Settings Structure
typedef struct _MAIN_CFG {
    int CFGVersion; //to prevent loading shit from a previous version if struct has changed.
    char ServerIp[16];
    LOGFONT ValueFontInfo;
    HFONT ValueHFont;
    CODE_SEARCH_SETTINGS CS;
    SEARCH_RESULTS_SETTINGS Results;
    MEMORY_EDITOR_SETTINGS MemEdit;
} MAIN_CFG;

/****************************************************************************
Externs (Global Vars)
*****************************************************************************/

extern HINSTANCE hInst;
extern HWND hwndMain;
extern HWND hTabDlgs[NUM_TABS];
extern char ErrTxt[1000];
extern WNDPROC wpHexEditBoxes;

//structs
extern MAIN_CFG Settings;
extern RAM_AND_RES_DATA RamInfo;


/****************************************************************************
Function Declarations -should also be a guidline to help people find where
any function is locatated.
*****************************************************************************/

//ps2cc.c
int InitTabControl(HWND hwnd, LPARAM lParam);
BOOL CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
int FreeShit();
int FreeRamInfo();
int LoadSettings();
int SaveSettings();
int UpdateStatusBar(const char *StatusText, int PartNum, int Flags);
int UpdateProgressBar(unsigned int Message, WPARAM wParam, LPARAM lParam);

//lib_api
LRESULT CALLBACK HexEditBoxHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int ComboAddItem(HWND hCombo, const char* combostring, DWORD value);
int isHexWindow(HWND txtbox);
u64 GetHexWindow(HWND txtbox);
int SetHexWindow(HWND txtbox, u64 value);
u64 isDecWindow(HWND txtbox);
u64 GetDecWindow(HWND txtbox);
int SetDecWindowU(HWND txtbox, u64 value);
u64 IsFloatWindow(HWND txtbox);
u64 GetFloatWindow(HWND txtbox, int fsize);
int DoFileOpen(HWND hwnd, char* filename);
int BrowseForFolder(HWND hwnd, char* filename);
int SetMenuState(HMENU hMenu, UINT id, UINT state);
int SetMenuItemText(HMENU hMenu, UINT id, const char* MenuText);
int SetMenuItemData(HMENU hMenu, UINT id, UINT data);
int GetMenuItemData(HMENU hMenu, UINT id);

//lib_dialog
BOOL CALLBACK IpConfigDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//lib_fileio
u32 FileExists(char *filename);
int LoadFile(u8 **buffer, char* filename, int headerlen, u8 **headerdata, BOOL loadheader);
int SaveFile(u8 *buffer, u32 filesize, char* filename, int headerlen, VOID *headerdata);
int LoadStruct(VOID *buffer, u32 filesize, char* filename);
int SaveStruct(VOID *buffer, u32 filesize, char* filename);

//lib_listview
int ListViewAddRow(HWND hListView, int count, ...);
int ListViewSetRow(HWND hListView, int item, int subitem, int count, ...);
int ListViewAddCol(HWND hListView, const char* colName, int colNum, int colWidth);
int ListViewGetText(HWND hListView, int iNum, int iSub, char* iText, int txtLen);
u64 ListViewGetHex(HWND hListView, int iNum, int iSub);
u64 ListViewGetDec(HWND hListView, int iNum, int iSub);
int ListViewHitTst(HWND hListView, DWORD dwPos, int iItem);

//lib_misc
int FilterHexChar(int lvalue);
int isHex(char* text);
int isDec(char* text);
int isFloat(char* text);
u64 Float2Hex(float floatval);
u64 Double2Hex(double dblval);
int str_lcase(char *str);
int StringCompareCI(char* string1, char* string2);
int SetBitFlag(u8 *flags, int num, int val);
int GetBitFlag(u8 *flags, int num);
int BitCount(u64 countval);
u64 SignExtend64(u64 signval, int sbytes);
u64 ByteFromU64(u64 dword, int valpart);
u64 ShortFromU64(u64 dword, int valpart);
u64 WordFromU64(u64 dword, int valpart);
int Hex2ASCII(u64 value, int bytes, char *string);
u64 FlipBytes(u64 value, int size);
int isIPAddr(char *text);

//lib_ntpb
int DumpRAM(char *dump_file, unsigned int dump_start, unsigned int dump_end);
int TestConnect();

//lib_search
int CodeSearch(CODE_SEARCH_VARS Search);
u64 GetSearchValues(u64 *NewVal, u64 *OldVal, int index, int size, int endian);

//tab_search
BOOL CALLBACK CodeSearchProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SearchValueBoxHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//tab_results
BOOL CALLBACK SearchResultsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int LoadResultsList();
s64 ShowResPage(s64 ResNum);
