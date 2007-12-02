// SongToast.h : main header file for the SongToast DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "TaskbarNotifier.h"

// CSongToastApp
// See SongToast.cpp for the implementation of this class
//
class CSongToastApp : public CWinApp
{
public:
        CSongToastApp();
// Overrides
public:
        DECLARE_MESSAGE_MAP()
public:
		static void Unload();
        static void Config();
        static int Load();
private:
        static DWORD WINAPI ThreadProc(LPVOID lpParameter);
		
		void Deinitialize();
        void Configure();
        int Initialize();

		CTaskbarNotifier m_wndTaskbarNotifier;
        BOOL isRunning;
private:
		CString m_szTemplate;
		BOOL m_isToastEnable;
		DWORD m_dwToastUpdateTime;
		DWORD m_dwToastStayTime;
		BOOL m_isEnableUpdateStatusMessage;
		CString m_szYID;
		DWORD m_dwUpdateStatusMessageTime;
};