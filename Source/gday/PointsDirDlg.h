#if !defined(AFX_POINTSDIRDLG_H__B9734207_73AE_42EA_AB65_074C9E2325DD__INCLUDED_)
#define AFX_POINTSDIRDLG_H__B9734207_73AE_42EA_AB65_074C9E2325DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PointsFileZoneDlg.h"	// Added by ClassView
#include "PointsDirCreateDlg.h"
#include "PointsDirProgressDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPointsDirDlg dialog

class CPointsDirDlg : public CDialog
{
// Construction
public:
	int		m_intFilesSuccess;
	
	CPointsDirDlg(CWnd* pParent = NULL);   // standard constructor
	CPointsDirProgressDlg *m_dDirProgDlg;							// modeless

// Dialog Data
	//{{AFX_DATA(CPointsDirDlg)
	enum { IDD = IDD_POINT_DIR };
	T_user	m_pfile_t;
	CString m_strOutputPath;
	CString	m_strInputPath;
	BOOL	m_Recurse;
	int		m_intInputFormat;
	int		m_intOutputFormat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointsDirDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int		m_CreateDir;
	int		m_nFileProgress;
	long	m_lNumSuccess;
	long	m_lNumFail;
	int		m_intFilesTransformed;
	BOOL	m_bSameOutputZone;
	
	CStatusBar  m_statusBar;		// statusbar
	CString m_strInputFileType;
	CString m_strOutputFileType;

	void	OnEnableButtons(BOOL bEnable = TRUE);
	void	OnInitialisePFileVars();
	int		OnValidateDirectories();
	void	OnCheckDirectoryExists();
	BOOL	AddDlgStatusBar();
	void	OnUpdateDialogControls();
	void	TransformDirectory(char *strfiledir);
	void	RecurseFilesinDir(char *strfiledir);
	void	PrintDirtoLog(CString Path);
	void	GetFilesinDir(LPCTSTR strfiledir);
	int		TransformFiles(LPCTSTR strfiledir);
	CString ReturnFileTypeSelection(int iComboPos);
	
	CPointsFileZoneDlg m_dPointsFileZoneDlg;
	
	// Generated message map functions
	//{{AFX_MSG(CPointsDirDlg)
	afx_msg void OnDirLookin();
	afx_msg void OnDirSave();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeInputFormat();
	afx_msg void OnSelchangeOutputFormat();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLogfileview();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL		TRANSDIRSUCCESS;
	BOOL		m_ExitDirectory;
	long		m_NumFiles;			// total number of files
	long		m_lTotalBytes;		// total number of bytes
	char		*m_pSubDir;
	CFileProc	file;				// file handling methods
	CToolTipCtrl m_ToolTip;			// tooltip control	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTSDIRDLG_H__B9734207_73AE_42EA_AB65_074C9E2325DD__INCLUDED_)
