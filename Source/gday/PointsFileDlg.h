#if !defined(AFX_POINTSFILEDLG_H__73B4DC57_4F4B_440A_8ABE_441D11494722__INCLUDED_)
#define AFX_POINTSFILEDLG_H__73B4DC57_4F4B_440A_8ABE_441D11494722__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PointsFileZoneDlg.h"	// Added by ClassView
#include "PointsDirProgressDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPointsFileDlg dialog

class CPointsFileDlg : public CDialog
{
// Construction
public:
	long	m_lNumSuccess;
	long	m_lNumFail;
	long	m_lNumInterpolated;
	int		m_intFilesTransformed;
	BOOL	m_bMultiFiles;
	
	CPointsFileDlg(CWnd* pParent = NULL);   // standard constructor

	CPointsDirProgressDlg *m_dDirProgDlg;							// modeless

// Dialog Data
	//{{AFX_DATA(CPointsFileDlg)
	enum { IDD = IDD_POINT_FILE };
	CString			m_strInputFileName;
	CString			m_strOutputFileName;
	CString			m_strInputFileType;
	CString			m_strOutputFileType;
	T_user			m_pfile_t;
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointsFileDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void			OnEnableButtons(BOOL bEnable = TRUE);
	
	CToolTipCtrl	m_ToolTip;		// tooltip control	
	BOOL			m_StatusBarCreated;
	
	// Generated message map functions
	//{{AFX_MSG(CPointsFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnInputBrowse();
	afx_msg void OnOutputBrowse();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLogfileview();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void OnInitTransformationData(pointsfile *ptrCoordFile, char *cInputExt, char *cOutputExt);
	int OnValidateTransformationFiles(char *cInputExt, char *cOutputExt);
	CStatusBar   m_statusBar;	// statusbar
	CFileProc file;

	BOOL AddDlgStatusBar();
	void ModifyDlgStatusBar();
	void OnUpdateDialogControls();
	void UpdateProgress(long count);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTSFILEDLG_H__73B4DC57_4F4B_440A_8ABE_441D11494722__INCLUDED_)
