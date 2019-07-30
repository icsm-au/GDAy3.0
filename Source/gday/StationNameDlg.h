#if !defined(AFX_STATIONNAMEDLG_H__34A46ED2_9A6A_4847_AAFE_3B9F2490FDDB__INCLUDED_)
#define AFX_STATIONNAMEDLG_H__34A46ED2_9A6A_4847_AAFE_3B9F2490FDDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationNameDlg dialog

class CStationNameDlg : public CDialog
{
// Construction
public:
	CStationNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStationNameDlg)
	enum { IDD = IDD_STATION_NAME };
	CString	m_strStationName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationNameDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	CToolTipCtrl m_ToolTip;		// tooltip control
	
	// Generated message map functions
	//{{AFX_MSG(CStationNameDlg)
	virtual void OnOK();
	afx_msg void OnSetfocusStationName();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONNAMEDLG_H__34A46ED2_9A6A_4847_AAFE_3B9F2490FDDB__INCLUDED_)
