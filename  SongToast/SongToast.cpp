// SongToast.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SongToast.h"
#include "Plugin.h"
#include "System.h"
#include "WinampUtil.h"
#include "ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

winampGeneralPurposePlugin plugin = {
	GPPHDR_VER,
	"SongToast v1.0",
	CSongToastApp::Load,
	CSongToastApp::Config,
	CSongToastApp::Unload
};

winampGeneralPurposePlugin* winampGetGeneralPurposePlugin()
{
	return &plugin;
}

// CSongToastApp

BEGIN_MESSAGE_MAP(CSongToastApp, CWinApp)
END_MESSAGE_MAP()

// CSongToastApp construction

CSongToastApp::CSongToastApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_szTemplate = _T("%artist% - $if2(%title%,$filepart(%filename%)) '('%percent%'%' of %lengthf%')'");
	m_isToastEnable = TRUE;
	m_dwToastUpdateTime = 10000;
	m_dwToastStayTime = 5000;
	m_isEnableUpdateStatusMessage = TRUE;
	m_szYID = L"";
	m_dwUpdateStatusMessageTime = 10000;
}

// The one and only CSongToastApp object

CSongToastApp theApp;

int CSongToastApp::Load()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return(theApp.Initialize());
}

void CSongToastApp::Config()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	theApp.Configure();
}

void CSongToastApp::Unload()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	theApp.Deinitialize();
}

int CSongToastApp::Initialize()
{
	HWND hwndWinAmp = plugin.hwndParent;
	if (hwndWinAmp == NULL) {
		hwndWinAmp = GetWinampWindowHandle();
	}
	CWnd* pWnd = CWnd::FromHandle(hwndWinAmp);

	m_wndTaskbarNotifier.Create(pWnd);
	m_wndTaskbarNotifier.SetSkin(IDB_SKIN_BOARD, 255, 0, 255);
	m_wndTaskbarNotifier.SetTextFont(_T("Tahoma"), 90, TN_TEXT_NORMAL, TN_TEXT_NORMAL);
	m_wndTaskbarNotifier.SetTextColor(RGB(255, 255, 255), RGB(255, 255, 255));
	m_wndTaskbarNotifier.SetTextRect(
		CRect(50, 10, m_wndTaskbarNotifier.m_nSkinWidth - 30, m_wndTaskbarNotifier.m_nSkinHeight - 10));

	// Start thread
	isRunning = TRUE;
	if (!BeginThread(ThreadProc, this)) {
		return -1;
	}
	return 0;
}

void CSongToastApp::Configure()
{
	CConfigDlg dlg;
	
	dlg.m_szTemplate = m_szTemplate;
	dlg.m_isToastEnable = m_isToastEnable;
	dlg.m_dwToastUpdateTime = m_dwToastUpdateTime;
	dlg.m_dwToastStayTime = m_dwToastStayTime;
	dlg.m_isEnableUpdateStatusMessage = m_isEnableUpdateStatusMessage;
	dlg.m_szYID = m_szYID;
	dlg.m_dwUpdateStatusMessageTime = m_dwUpdateStatusMessageTime;

	if (IDOK == dlg.DoModal())
	{
		m_szTemplate = dlg.m_szTemplate;
		m_isToastEnable = dlg.m_isToastEnable;
		m_dwToastUpdateTime = dlg.m_dwToastUpdateTime;
		m_dwToastStayTime = dlg.m_dwToastStayTime;
		m_isEnableUpdateStatusMessage = dlg.m_isEnableUpdateStatusMessage;
		m_szYID = dlg.m_szYID;
		m_dwUpdateStatusMessageTime = dlg.m_dwUpdateStatusMessageTime;
	}
}

void CSongToastApp::Deinitialize()
{
	isRunning = FALSE;
	WaitForThreadsToExit();
}

DWORD WINAPI CSongToastApp::ThreadProc(LPVOID lpParameter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const DWORD SLEEP_TIME = 2000;

	CSongToastApp* thiz = (CSongToastApp*)lpParameter;
	DWORD dwLastTime = 0;
	TCHAR szMessage[1024];
	while (thiz->isRunning)
	{
		DWORD dwCurrentTime = ::GetTickCount();
		if (dwCurrentTime - dwLastTime > thiz->m_dwToastUpdateTime) {
			dwLastTime = dwCurrentTime;
			if (thiz->m_isToastEnable && GetToastMessage(thiz->m_szTemplate.GetBuffer(), szMessage)) {
				thiz->m_wndTaskbarNotifier.Show(szMessage, 500, thiz->m_dwToastStayTime, 500);
			}
		}
		else {
			Sleep(SLEEP_TIME);
		}
	}
	return 0;
}
