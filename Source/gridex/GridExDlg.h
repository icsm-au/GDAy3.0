// GridExDlg.h : header file
//

#if !defined(AFX_GRIDEXDLG_H__1F22E606_13CA_11D5_A1BE_00C04F88C574__INCLUDED_)
#define AFX_GRIDEXDLG_H__1F22E606_13CA_11D5_A1BE_00C04F88C574__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <GCalcext.h>

/////////////////////////////////////////////////////////////////////////////
// CGridExDlg dialog

class CGridExDlg : public CDialog
{
// Construction
public:
	void ComputeLimitsbyLR(double azimuth, double *lfLatPos2, double *m_lfLongPos2);
	int ValidateLimits();
	file_par	*fpGridHeaderArray;		// grid header array for existing grid file
	file_par	*fpNewGridHeaderArray;	// grid header array for new grid file (to be extracted)
	CString		m_strGridFilename;
	double		m_lfNewNorthLat;
	double		m_lfNewSouthLat;
	double		m_lfNewEastLong;
	double		m_lfNewWestLong;
	double		m_lfRadius;
	long		hFile;					// handle to log file
	
	CGridExDlg(CWnd* pParent = NULL);	// standard constructor
	
	CGridEx gx;
	

// Dialog Data
	//{{AFX_DATA(CGridExDlg)
	enum { IDD = IDD_GRIDEX_DIALOG };
	CString	m_strLatPos;
	CString	m_strLongPos;
	CString m_strRadius;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridExDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int		m_intNSEWLimits;
	
	int		ReadGridFileHeader(char *cNewFile, char *cFiletype);
	void	DisableControls();
	void	EnableControls();
	void	UpdateLimitBoxes();
	void	OnUpdateGridLimits();
	void	UpdateGridInformation();
	void	OnCreateToolTips();
	void	ValidateGridName();
	void	OnFileView();
	BOOL	AddDlgStatusBar();
	HICON	m_hIcon;

	CToolTipCtrl	m_ToolTip;
	CStatusBar		m_statusBar;
	
	CComboBox	m_ctlNewSubGridName;
	CString		m_strNewEastLong;
	CString		m_strNewNorthLat;
	CString		m_strNewSouthLat;
	CString		m_strNewWestLong;
	CString		m_strNewGridFilename;
	CString		m_strNewSubGridName;
	double		m_dGridLatinc;
	double		m_dGridLoninc;
	int			m_bCanFormat;
	int			m_intParentIndex;
	
	// Generated message map functions
	//{{AFX_MSG(CGridExDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	afx_msg void OnExtract();
	afx_msg void OnSelchangeSubgridName();
	afx_msg void OnSaveas();
	afx_msg void OnKillfocusNorthLatNew();
	afx_msg void OnKillfocusEastLongNew();
	afx_msg void OnKillfocusSouthLatNew();
	afx_msg void OnKillfocusWestLongNew();
	afx_msg void OnKillfocusLatPosition();
	afx_msg void OnKillfocusLongPosition();
	afx_msg void OnSetfocusNorthLatNew();
	afx_msg void OnSetfocusSouthLatNew();
	afx_msg void OnSetfocusWestLongNew();
	afx_msg void OnSetfocusEastLongNew();
	afx_msg void OnSetfocusLatPosition();
	afx_msg void OnSetfocusLongPosition();
	afx_msg void OnKillfocusNewGridName();
	afx_msg void OnHelpFinder();
	afx_msg void OnSelchangeNewSubgridName();
	afx_msg void OnLimitSelection();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDEXDLG_H__1F22E606_13CA_11D5_A1BE_00C04F88C574__INCLUDED_)
