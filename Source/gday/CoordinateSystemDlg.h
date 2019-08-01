#if !defined(AFX_COORDINATESYSTEMDLG_H__4138FFA1_6777_11D3_AD1C_0008C725A4CD__INCLUDED_)
#define AFX_COORDINATESYSTEMDLG_H__4138FFA1_6777_11D3_AD1C_0008C725A4CD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CoordinateSystemDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCoordinateSystemDlg dialog

class CCoordinateSystemDlg : public CDialog
{
// Construction
public:
	CCoordinateSystemDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCoordinateSystemDlg)
	enum { IDD = IDD_COORDINATE_SYSTEM };
	T_user  m_coordinate_t;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoordinateSystemDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_ToolTip;		// Tooltip control

	// Generated message map functions
	//{{AFX_MSG(CCoordinateSystemDlg)
	afx_msg void OnSelchangeInputDatum();
	afx_msg void OnSelchangeInputProj();
	afx_msg void OnSelchangeOutputDatum();
	afx_msg void OnSelchangeOutputProj();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COORDINATESYSTEMDLG_H__4138FFA1_6777_11D3_AD1C_0008C725A4CD__INCLUDED_)
