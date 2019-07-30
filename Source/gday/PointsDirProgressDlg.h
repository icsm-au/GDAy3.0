#if !defined(AFX_POINTSDIRPROGRESSDLG_H__8DD8D15B_33EF_480D_9884_AFD8F9A9BD9A__INCLUDED_)
#define AFX_POINTSDIRPROGRESSDLG_H__8DD8D15B_33EF_480D_9884_AFD8F9A9BD9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PointsDirProgressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPointsDirProgressDlg dialog

class CPointsDirProgressDlg : public CDialog
{
// Construction
public:
	CPointsDirProgressDlg(CWnd* pParent);   // standard constructor
	
	void OnCancel();
	BOOL Create();
	
	int m_intProgressPos;
	
// Dialog Data
	//{{AFX_DATA(CPointsDirProgressDlg)
	enum { IDD = IDD_PROGRESS };
	CProgressCtrl	m_DirProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointsDirProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	CWnd* m_pParent;
	int m_nID;

	// Generated message map functions
	//{{AFX_MSG(CPointsDirProgressDlg)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTSDIRPROGRESSDLG_H__8DD8D15B_33EF_480D_9884_AFD8F9A9BD9A__INCLUDED_)
