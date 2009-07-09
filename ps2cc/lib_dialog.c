/****************************************************************************
Dialog Library

Small, secondary dialog handlers should go here.
*****************************************************************************/
#include "ps2cc.h"
#include "ps2cc_gui.h"

BOOL CALLBACK IpConfigDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndIpAddr = GetDlgItem(hwnd, IP_ADDR_TXT);
    switch(message)
    {
        case WM_INITDIALOG:
        {
			SetWindowText(hwndIpAddr, Settings.ServerIp);
        } break;
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case SET_IP_CMD:
				case TEST_IP_CMD:
				{
					char ip[16];
					GetWindowText(hwndIpAddr, ip, sizeof(ip));
					if (!isIPAddr(ip)) {
						MessageBox(NULL, "Invalid IP", "Error", MB_OK);
						return 0;
					}
					if (LOWORD(wParam) == TEST_IP_CMD) {
						if(TestConnect()) { MessageBox(NULL, "Connection Successful!", "", MB_OK); }
						else { MessageBox(NULL, "Unable to Connect", "Error", MB_OK); }
					} else {
						strcpy(Settings.ServerIp, ip);
					}
				} break;
			}
        } break;
		case WM_CLOSE:
        {
			EndDialog(hwnd, 0);
        } break;
        default:
            return FALSE;
   }
   return TRUE;
}
