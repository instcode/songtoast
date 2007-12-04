#include "afxwin.h"
#if !defined(AFX_CONFIGDLG_H__E4891AF8_5D08_4505_A8A6_4159636E3CC0__INCLUDED_)
#define AFX_CONFIGDLG_H__E4891AF8_5D08_4505_A8A6_4159636E3CC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

class CConfigDlg : public CDialog
{
// Construction
public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_CONFIG_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedHelp();
	CString m_szTemplate;
	BOOL m_isToastEnable;
	DWORD m_dwToastUpdateTime;
	DWORD m_dwToastStayTime;
	BOOL m_isEnableUpdateStatusMessage;
	CString m_szYID;
	DWORD m_dwUpdateStatusMessageTime;
	afx_msg void OnBnClickedCheckShowToastWindow();
	afx_msg void OnBnClickedCheckYmStatusMessage();

protected:
	void UpdateToastSettings(BOOL isChecked);
	void UpdateYahooSettings(BOOL isChecked);
public:
	afx_msg void OnBnClickedShowVariables();
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDLG_H__E4891AF8_5D08_4505_A8A6_4159636E3CC0__INCLUDED_)
