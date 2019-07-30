#if !defined(AFX_GRIDPROPERTIESDLG_H__3B93BFB3_7A66_4694_BBD5_7E2935D39FC4__INCLUDED_)
#define AFX_GRIDPROPERTIESDLG_H__3B93BFB3_7A66_4694_BBD5_7E2935D39FC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GridPropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGridPropertiesDlg dialog

class CGridPropertiesDlg : public CDialog
{
// Construction
public:
	CString m_strGridFile;
	CGridPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGridPropertiesDlg)
	enum { IDD = IDD_GRID_PROPERTIES };
	CComboBox	m_ctlSubGridName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridPropertiesDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_intParentIndex;
	CToolTipCtrl m_ToolTip;
	
	void UpdateControls();

	file_par *m_fpGridproperties;

	// Generated message map functions
	//{{AFX_MSG(CGridPropertiesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSubgridName();
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDPROPERTIESDLG_H__3B93BFB3_7A66_4694_BBD5_7E2935D39FC4__INCLUDED_)
