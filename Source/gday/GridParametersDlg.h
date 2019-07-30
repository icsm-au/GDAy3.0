#if !defined(AFX_GRIDPARAMETERSDLG_H__56671C3A_280F_424A_A43B_A2D51AA22D97__INCLUDED_)
#define AFX_GRIDPARAMETERSDLG_H__56671C3A_280F_424A_A43B_A2D51AA22D97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGridParametersDlg dialog

class CGridParametersDlg : public CDialog
{
// Construction
public:
	CGridParametersDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGridParametersDlg)
	enum { IDD = IDD_GRID_AND_PARAMS };
	int		m_intGridFileStatus;
	CString m_strTransParam;
	CString	m_strGridFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridParametersDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_ToolTip;				// Tooltip control
	file_par *m_fpGridHeaderArray;		// grid header array for dialog interface

	void OnUpdateGridLimits();

	// Generated message map functions
	//{{AFX_MSG(CGridParametersDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnGridBrowse();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDPARAMETERSDLG_H__56671C3A_280F_424A_A43B_A2D51AA22D97__INCLUDED_)
