#if !defined(AFX_PAGE2_H__E47AC41E_3904_4E15_8283_3FF23A7A0DBB__INCLUDED_)
#define AFX_PAGE2_H__E47AC41E_3904_4E15_8283_3FF23A7A0DBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage2 dialog

class CPage2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPage2)

// Construction
public:
	CPage2();

	int m_P2HasChanged;

// Dialog Data
	//{{AFX_DATA(CPage2)
	enum { IDD = IDD_OPTIONS_OUTFILE };
	BOOL	m_intAddStationName;
	BOOL	m_intCreateOutputFile;
	CString	m_strCustomName;
	CString	m_strInteractiveFilePath;
	int		m_intPromptStationName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPage2)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_ToolTip;		// tooltip control

	BOOL OnKillActive();

	// Generated message map functions
	//{{AFX_MSG(CPage2)
	afx_msg void OnAddStnName();
	afx_msg void OnOutputFileChange();
	afx_msg void OnChange();
	afx_msg void OnStationNaming();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE2_H__E47AC41E_3904_4E15_8283_3FF23A7A0DBB__INCLUDED_)
