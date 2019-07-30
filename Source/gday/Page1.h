#if !defined(AFX_PAGE1_H__928BDF6E_31D1_44C4_B937_1D06357414B4__INCLUDED_)
#define AFX_PAGE1_H__928BDF6E_31D1_44C4_B937_1D06357414B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPage1 dialog

class CPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPage1)

// Construction
public:
	CPage1();

	int m_P1HasChanged;

// Dialog Data
	//{{AFX_DATA(CPage1)
	enum { IDD = IDD_OPTIONS_VIEW };
	CSpinButtonCtrl	m_intSecondsSpin;
	CSpinButtonCtrl	m_intMetresSpin;
	CString	m_strStartingPointNumber;
	CString m_strCustomName;
	int		m_intAutoIncrement;
	int		m_intAddSymbols;
	int		m_intPointNumbering;
	int		m_intAddPrefix;
	int		m_intLatisPosSouth;
	int		m_intSecondsPrec;
	int		m_intMetresPrec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPage1)
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
	//{{AFX_MSG(CPage1)
	afx_msg void OnAutoIncrement();
	afx_msg void OnPointNumber();
	afx_msg void OnChange();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE1_H__928BDF6E_31D1_44C4_B937_1D06357414B4__INCLUDED_)
