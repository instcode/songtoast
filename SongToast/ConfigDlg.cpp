// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SongToast.h"
#include "ConfigDlg.h"
#include ".\configdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog


CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
	, m_szTemplate(_T(""))
	, m_isToastEnable(FALSE)
	, m_dwToastUpdateTime(0)
	, m_dwToastStayTime(0)
	, m_isEnableUpdateStatusMessage(FALSE)
	, m_szYID(_T(""))
	, m_dwUpdateStatusMessageTime(0)
{
	//{{AFX_DATA_INIT(CConfigDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_TEMPLATE, m_szTemplate);
	DDX_Check(pDX, IDC_CHECK_SHOW_TOAST_WINDOW, m_isToastEnable);
	DDX_Text(pDX, IDC_TEXT_TOAST_UPDATE_TIME, m_dwToastUpdateTime);
	DDV_MinMaxUInt(pDX, m_dwToastUpdateTime, 2000, 600000);
	DDX_Text(pDX, IDC_EDIT_TOAST_TIME_TO_STAY, m_dwToastStayTime);
	DDV_MinMaxUInt(pDX, m_dwToastStayTime, 2000, 600000);
	DDX_Check(pDX, IDC_CHECK_YM_STATUS_MESSAGE, m_isEnableUpdateStatusMessage);
	DDX_Text(pDX, IDC_EDIT_YID, m_szYID);
	DDX_Text(pDX, IDC_EDIT_STATUS_UPDATE_TIME, m_dwUpdateStatusMessageTime);
	DDV_MinMaxUInt(pDX, m_dwUpdateStatusMessageTime, 10000, 86400000);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDHELP, OnBnClickedHelp)
	ON_BN_CLICKED(IDC_CHECK_SHOW_TOAST_WINDOW, OnBnClickedCheckShowToastWindow)
	ON_BN_CLICKED(IDC_CHECK_YM_STATUS_MESSAGE, OnBnClickedCheckYmStatusMessage)
	ON_BN_CLICKED(IDC_SHOW_VARIABLES, OnBnClickedShowVariables)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg message handlers

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateToastSettings(m_isToastEnable);
	UpdateYahooSettings(m_isEnableUpdateStatusMessage);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDlg::OnBnClickedOk()
{
	OnOK();
}

void CConfigDlg::OnBnClickedHelp()
{
	// TODO: Add your control notification handler code here
}

void CConfigDlg::OnBnClickedShowVariables()
{
	// TODO: Add your control notification handler code here
}

void CConfigDlg::UpdateToastSettings(BOOL isChecked)
{
	GetDlgItem(IDC_EDIT_TOAST_TIME_TO_STAY)->EnableWindow(isChecked);
	GetDlgItem(IDC_TEXT_TOAST_UPDATE_TIME)->EnableWindow(isChecked);
}

void CConfigDlg::UpdateYahooSettings(BOOL isChecked)
{
	GetDlgItem(IDC_EDIT_STATUS_UPDATE_TIME)->EnableWindow(isChecked);
	GetDlgItem(IDC_EDIT_YID)->EnableWindow(isChecked);
}

void CConfigDlg::OnBnClickedCheckShowToastWindow()
{
	UpdateData();
	BOOL isChecked = m_isToastEnable;
	UpdateToastSettings(isChecked);
}

void CConfigDlg::OnBnClickedCheckYmStatusMessage()
{
	UpdateData();
	BOOL isChecked = m_isEnableUpdateStatusMessage;
	UpdateYahooSettings(isChecked);
}