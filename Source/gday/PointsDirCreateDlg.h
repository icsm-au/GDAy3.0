#if !defined(AFX_POINTSDIRCREATEDLG_H__98766BF3_1091_439D_BC56_8EC90257C55A__INCLUDED_)
#define AFX_POINTSDIRCREATEDLG_H__98766BF3_1091_439D_BC56_8EC90257C55A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PointsDirCreateDlg.h : header file
//

#define DLG_TOP		356
#define DLG_BOT		480
#define DLG_LEFT	320
#define DLG_RIGHT	832
#define TXT_TOP		385
#define TXT_BOT		426
#define TXT_LEFT	388
#define TXT_RIGHT	817

/////////////////////////////////////////////////////////////////////////////
// CPointsDirCreateDlg dialog

class CPointsDirCreateDlg : public CDialog
{
// Construction
public:
	CString m_strDirPath;
	int m_nParentWidth;
	CPointsDirCreateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPointsDirCreateDlg)
	enum { IDD = IDD_POINT_DIR_CREATEDIR };
	CStatic	m_IconArea;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointsDirCreateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void	RepositionControls();
	void	OnInitDialogSize();
	void	OnResizeDialog(int iDiffWidth, int iDiffHeight);
	void	OnMoveDlgCtrl(UINT nButtonID, int iHzOffset, int iVOffset);

	// Generated message map functions
	//{{AFX_MSG(CPointsDirCreateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnNo();
	afx_msg void OnNoall();
	afx_msg void OnYes();
	afx_msg void OnYesall();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTSDIRCREATEDLG_H__98766BF3_1091_439D_BC56_8EC90257C55A__INCLUDED_)
