// SongToast.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SongToast.h"
#include "Plugin.h"
#include "System.h"
#include "WinampUtil.h"
#include "YahooUtil.h"
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
	"SongToast v1.0.0",
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
}

// The one and only CSongToastApp object

CSongToastApp theApp;

int CSongToastApp::Load()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (theApp.Initialize());
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
	HWND hWndWinAmp = plugin.hwndParent;
	if (hWndWinAmp == NULL) {
		hWndWinAmp = GetWinampWindowHandle();
	}
	CWnd* pWnd = CWnd::FromHandle(hWndWinAmp);
	m_wndTaskbarNotifier.Create(pWnd);
	m_wndTaskbarNotifier.SetSkin(IDB_SKIN_BOARD, 255, 0, 255);
	m_wndTaskbarNotifier.SetTextFont(_T("Tahoma"), 90, TN_TEXT_NORMAL, TN_TEXT_NORMAL);
	m_wndTaskbarNotifier.SetTextColor(RGB(255, 255, 255), RGB(255, 255, 255));
	m_wndTaskbarNotifier.SetTextRect(
		CRect(50, 10, m_wndTaskbarNotifier.m_nSkinWidth - 30, m_wndTaskbarNotifier.m_nSkinHeight - 10));

	// Use registry to store settings (support unicode string better than .ini)
	SetRegistryKey(_T("Winamp"));

	// Read & Update configuration
	ReadProfileSettings();
	WriteProfileSettings();

	m_lpWndProcOld = (WNDPROC)SetWindowLong(plugin.hwndParent, GWL_WNDPROC, (LONG)CSongToastApp::WndProc);
	// Start thread
	isRunning = TRUE;
	if (!BeginThread(ThreadProc, this)) {
		return -1;
	}
	return 0;
}

void CSongToastApp::ReadProfileSettings()
{
	m_szTemplate = GetProfileString(L"", L"template", L"%artist% - $if2(%title%,$filepart(%filename%)) '('%percent%'%' of %lengthf%')'");
	m_isToastEnable = GetProfileInt(L"", L"toast_enable", TRUE);
	m_dwToastUpdateTime = GetProfileInt(L"", L"toast_update_time", 10000);
	m_dwToastStayTime = GetProfileInt(L"", L"toast_stay_time", 5000);

	m_isEnableUpdateStatusMessage = GetProfileInt(L"", L"yahoo_status_enable", FALSE);
	m_szYID= GetProfileString(L"", L"yid", L"");
	m_dwUpdateStatusMessageTime = GetProfileInt(L"", L"yahoo_status_update_time", 10000);
}

void CSongToastApp::WriteProfileSettings()
{
	WriteProfileString(L"", L"template", m_szTemplate);
	WriteProfileInt(L"", L"toast_enable", m_isToastEnable);
	WriteProfileInt(L"", L"toast_update_time", m_dwToastUpdateTime);
	WriteProfileInt(L"", L"toast_stay_time", m_dwToastStayTime);

	WriteProfileInt(L"", L"yahoo_status_enable", m_isEnableUpdateStatusMessage);
	WriteProfileString(L"", L"yid", m_szYID);
	WriteProfileInt(L"", L"yahoo_status_update_time", m_dwUpdateStatusMessageTime);
}

void CSongToastApp::Configure()
{
	if (m_pShowedDlg != NULL)
	{
		m_pShowedDlg->SetForegroundWindow();
		return;
	}
	CConfigDlg dlg(CWnd::FromHandle(::GetDesktopWindow()));
	m_pShowedDlg = &dlg;

	dlg.m_szTemplate = m_szTemplate;
	dlg.m_isToastEnable = m_isToastEnable;
	dlg.m_dwToastUpdateTime = m_dwToastUpdateTime;
	dlg.m_dwToastStayTime = m_dwToastStayTime;
	dlg.m_isEnableUpdateStatusMessage = m_isEnableUpdateStatusMessage;
	dlg.m_szYID = m_szYID;
	dlg.m_dwUpdateStatusMessageTime = m_dwUpdateStatusMessageTime;

	if (IDOK == dlg.DoModal())
	{
		// The dialog is no longer valid
		m_pShowedDlg = NULL;

		m_szTemplate = dlg.m_szTemplate;
		m_isToastEnable = dlg.m_isToastEnable;
		m_dwToastUpdateTime = dlg.m_dwToastUpdateTime;
		m_dwToastStayTime = dlg.m_dwToastStayTime;
		m_isEnableUpdateStatusMessage = dlg.m_isEnableUpdateStatusMessage;
		m_szYID = dlg.m_szYID;
		m_dwUpdateStatusMessageTime = dlg.m_dwUpdateStatusMessageTime;

		// Save configuration
		WriteProfileSettings();
	}
	// Make sure the m_pShowedDlg shouldn't be used somewhere else
	m_pShowedDlg = NULL;
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
	DWORD dwUpdateToastLastTime = 0, dwUpdateStatusLastTime = 0;
	TCHAR szMessage[1024];
	while (thiz->isRunning)
	{
		DWORD dwCurrentTime = ::GetTickCount();
		// Toast...
		if (dwCurrentTime - dwUpdateToastLastTime > thiz->m_dwToastUpdateTime) {
			dwUpdateToastLastTime = dwCurrentTime;
			if (thiz->m_isToastEnable && GetFormattedMessage(thiz->m_szTemplate.GetBuffer(), szMessage)) {
				thiz->m_wndTaskbarNotifier.Show(szMessage, 500, thiz->m_dwToastStayTime, 500);
			}
		}
		// Yahoo Status Message...
		if (dwCurrentTime - dwUpdateStatusLastTime > thiz->m_dwUpdateStatusMessageTime) {
			dwUpdateStatusLastTime = dwCurrentTime;
			if (thiz->m_isEnableUpdateStatusMessage && GetFormattedMessage(thiz->m_szTemplate.GetBuffer(), szMessage)) {
				SetCustomStatusMessage(thiz->m_szYID.GetBuffer(), szMessage);
			}
		}

		Sleep(SLEEP_TIME);
	}
	return 0;
}

LRESULT CALLBACK CSongToastApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_TASKBARNOTIFIERCLICKED)
	{
		theApp.Config();
	}
	return CallWindowProc(theApp.m_lpWndProcOld, hWnd, message, wParam, lParam);
}