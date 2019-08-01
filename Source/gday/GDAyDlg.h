// GDAyDlg.h : header file
//

#if !defined(AFX_GDAYDLG_H__9145B9A9_6765_11D3_AD1C_0008C725A4CD__INCLUDED_)
#define AFX_GDAYDLG_H__9145B9A9_6765_11D3_AD1C_0008C725A4CD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CoordinateSystemDlg.h"
#include "GridPropertiesDlg.h"
#include "GridDefaultSelectDlg.h"
#include "GridParametersDlg.h"
#include "OptionsSheet.h"
#include "PointsFileDlg.h"
#include "PointsDirDlg.h"
#include "StationNameDlg.h"

#ifndef _GLOBVARS_
#define _GLOBVARS_ 1

typedef struct {
	char gridfile[601];
	char gridtype[4];
	pointsfile *pCFile;
} T_Pointsfile;

typedef struct {
	CString strNewGrid;
	CString strGridFile;
} gridfile;

extern T_Pointsfile *t_Pfile;
extern BOOL g_bTransformingPointsFile;
extern int pf_success;

UINT ProcessFile(LPVOID pParam);
UINT ExportBinaryGridFile(LPVOID pParam);
UINT ExportAsciiGridFile(LPVOID pParam);

#endif

/////////////////////////////////////////////////////////////////////////////
// CGDAyDlg dialog

class CGDAyDlg : public CDialog
{
// Construction
public:
	CGDAyDlg(CWnd* pParent = NULL);	// standard constructor
	
	CString			m_strDefaultOutputFile;
	int				m_intGridFileStatus;
	
	T_user			theUser;
	void			PrintOutputHeader();		// used by CPointsFileDlg and CPointsDirDlg	


// Dialog Data
	//{{AFX_DATA(CGDAyDlg)
	enum { IDD = IDD_GDAY_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGDAyDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnClearDialogData();
	CString			m_strLatEastInput;
	CString			m_strLongNorthInput;
	CString			m_strZoneInput;
	double			m_lfLatEastInput;
	double			m_lfLongNorthInput;
	double			m_lfLatEastOutput;
	double			m_lfLongNorthOutput;
	double			m_lfLatacc;
	double			m_lfLongacc;
	double			m_lfZoneInput;
	long			m_lPointNumber;
	int				m_iLastOptionsPage;
	bool			m_ToolTipCreated;
	CToolTipCtrl	m_ToolTip;		// tooltip control
	CStatusBar		m_statusBar;	// statusbar
	HICON			m_hIcon;
	BOOL			m_StatusBarCreated;
	BOOL			m_bMenuSelected;
	UINT            m_nIDTracking;
	UINT            m_nIDLastMessage;
	UINT			m_nMsgPaneID;
	UINT			m_nMsgPaneStyle;
	int				m_iMsgPaneWidth;
	
	CGridParametersDlg		m_dGridParametersDlg;
	CPointsFileDlg			m_dPointsFileDlg;			// created as globals to retain file names
	CPointsDirDlg			m_dPointsDirDlg;			// created as globals to retain folder names
	
	void			OnPrintInteractiveOutput(coordinate cCoordVar);
	void			OnDisplayOutput(coordinate *cCoordVar);
	void			OnLogfileview();
	void			OnFileExportGridFile(CString strExt);
	void			OnInitGridFileSettings();
	void			OnSetStatusBarText(int pane_index, CString strText);
	void 			UpdateDialogText();
	void 			UpdateDialogOptions();
	void 			UpdateMenuItems(CMenu *GridPopupMenu);
	BOOL 			AddDlgStatusBar();
	
	// Generated message map functions
	//{{AFX_MSG(CGDAyDlg)
	afx_msg void OnCompute();
	afx_msg void OnKillfocusLatEastInput();
	afx_msg void OnKillfocusLongNthInput();
	afx_msg void OnKillfocusZone();
	afx_msg void OnSetfocusLatEastInput();
	afx_msg void OnSetfocusLongNthInput();
	afx_msg void OnSetfocusZoneInput();
	afx_msg void OnSetfocusPtNumber();
	afx_msg void OnFileGridfile();
	afx_msg void OnFileGridProperties();
	afx_msg void OnFileGridex();
	afx_msg void OnFileExportToAscii();
	afx_msg void OnFileExportToBinary();
	afx_msg void OnFileNewOutput();
	afx_msg void OnFileView();
	afx_msg void OnFileTransPointsfile();
	afx_msg void OnToolsCoordSystem();
	afx_msg void OnToolsOptions();
	afx_msg void OnHelpAboutGday();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam = 0L/*, int iPaneIndex = 0*/);	
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFileTransDirectory();
	afx_msg void OnHelpContents();
	afx_msg void OnHelpIndex();
	afx_msg void OnHelpSearch();
	//afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GDAYDLG_H__9145B9A9_6765_11D3_AD1C_0008C725A4CD__INCLUDED_)
