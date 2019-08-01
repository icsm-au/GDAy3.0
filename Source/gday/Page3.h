#if !defined(AFX_PAGE3_H__86E9F5A7_78F8_4F95_BFCF_2F2403E209F8__INCLUDED_)
#define AFX_PAGE3_H__86E9F5A7_78F8_4F95_BFCF_2F2403E209F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage3 dialog

class CPage3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPage3)

// Construction
public:
	CPage3();

	int m_P3HasChanged;

// Dialog Data
	//{{AFX_DATA(CPage3)
	enum { IDD = IDD_OPTIONS_LOGFILE };
	CString	m_strHeaderInfo;
	CString	m_strLogFilePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPage3)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_ToolTip;		// tooltip control

	BOOL OnKillActive();

	// Generated message map functions
	//{{AFX_MSG(CPage3)
	virtual BOOL OnInitDialog();
	afx_msg void OnLogFileChange();
	afx_msg void OnLogView();
	afx_msg void OnChange();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE3_H__86E9F5A7_78F8_4F95_BFCF_2F2403E209F8__INCLUDED_)
