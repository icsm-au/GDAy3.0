#if !defined(AFX_POINTSFILEZONEDLG_H__D5ABB9F9_AEAA_46DC_99A3_4439D1FB57C7__INCLUDED_)
#define AFX_POINTSFILEZONEDLG_H__D5ABB9F9_AEAA_46DC_99A3_4439D1FB57C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PointsFileZoneDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPointsFileZoneDlg dialog

class CPointsFileZoneDlg : public CDialog
{
// Construction
public:
	int m_iZone;
	CPointsFileZoneDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPointsFileZoneDlg)
	enum { IDD = IDD_POINT_FILE_ZONE };
	CString	m_strPrompt;
	CString	m_strZone;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointsFileZoneDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CToolTipCtrl m_ToolTip;		// tooltip control
	
	// Generated message map functions
	//{{AFX_MSG(CPointsFileZoneDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTSFILEZONEDLG_H__D5ABB9F9_AEAA_46DC_99A3_4439D1FB57C7__INCLUDED_)
