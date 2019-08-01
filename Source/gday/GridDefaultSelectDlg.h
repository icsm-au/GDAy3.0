#if !defined(AFX_GRIDDEFAULTSELECTDLG_H__A6B8455D_A0E7_4A34_85F2_67FB69296FAA__INCLUDED_)
#define AFX_GRIDDEFAULTSELECTDLG_H__A6B8455D_A0E7_4A34_85F2_67FB69296FAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GridDefaultSelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGridDefaultSelectDlg dialog

class CGridDefaultSelectDlg : public CDialog
{
// Construction
public:
	CGridDefaultSelectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGridDefaultSelectDlg)
	enum { IDD = IDD_GRID_DEFAULT_SELECT };
	BOOL	m_intPrompt;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridDefaultSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CToolTipCtrl m_ToolTip;		// tooltip control

	// Generated message map functions
	//{{AFX_MSG(CGridDefaultSelectDlg)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDDEFAULTSELECTDLG_H__A6B8455D_A0E7_4A34_85F2_67FB69296FAA__INCLUDED_)
