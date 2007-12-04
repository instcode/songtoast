#include "stdafx.h"
#include "YahooUtil.h"

void SetCustomStatusMessage(LPTSTR szProfile, LPCTSTR szMessage) {
	HKEY hRootKey;
	TCHAR lpszKeyPath[256];

	wsprintf(lpszKeyPath, L"Software\\yahoo\\pager\\profiles\\%s\\Custom Msgs\\", szProfile);
	RegOpenKeyEx(HKEY_CURRENT_USER, lpszKeyPath, 0, KEY_READ | KEY_SET_VALUE, &hRootKey);
	if (hRootKey != NULL) {
		// YM 8.1.0.xxx(-)
		RegSetValueEx(hRootKey, L"1", 0, REG_SZ, (LPBYTE) szMessage, (DWORD)(_tcslen(szMessage) * sizeof(TCHAR)) + 1);
		RegDeleteValue(hRootKey, L"1_bin");
		// YM 9.0.0.xxx(+)
		RegSetValueEx(hRootKey, L"1_W", 0, REG_SZ, (LPBYTE) szMessage, (DWORD)(_tcslen(szMessage) * sizeof(TCHAR)) + 1);
		// Update the combo box..
		HWND hWnd = NULL;//::FindWindow(L"YahooBuddyMain", NULL);
		if (hWnd != NULL) {
			/***************************************
			 Known WM_COMMAND, WPARAM (MS.Spy++, http://yayak.wordpress.com/2007/05/27/yahoo-messenger-idle-status-by-delphi/)
				376 : Available[N]
				---
				377 : Be Right Back[B]
				378 : Busy[B]
				379 : Not at Home[B] - Hidden
				380 : Not at My Desk[B]
				381 : Not in the Office[B] - Hidden
				382 : On the Phone[B]
				383 : On Vacation[B] - Hidden
				384 : Out To Lunch[B] - Hidden
				385 : Stepped Out[B]
				---
				388 : custom_mesg_1
				389 : custom_mesg_2
				390 : custom_mesg_3
				391 : custom_mesg_4
				392 : custom_mesg_5
			****************************************/
			::SendMessage(hWnd, WM_COMMAND, 388, 0);
		}
		RegCloseKey(hRootKey);
	}
}