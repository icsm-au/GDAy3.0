// PointsDirProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "PointsDirProgressDlg.h"

#include "PointsDirDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bTransformingPointsFile;
extern BOOL g_CancelPressed;

/////////////////////////////////////////////////////////////////////////////
// CPointsDirProgressDlg dialog


CPointsDirProgressDlg::CPointsDirProgressDlg(CWnd* pParent)
	: CDialog(CPointsDirProgressDlg::IDD, pParent)
{
	m_pParent = pParent;
	m_nID = CPointsDirProgressDlg::IDD;

	//{{AFX_DATA_INIT(CPointsDirProgressDlg)
	//}}AFX_DATA_INIT
}

void CPointsDirProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointsDirProgressDlg)
	DDX_Control(pDX, IDC_DIR_PROGRESS, m_DirProgress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPointsDirProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CPointsDirProgressDlg)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointsDirProgressDlg message handlers

BOOL CPointsDirProgressDlg::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void CPointsDirProgressDlg::OnCancel() 
{
	g_CancelPressed = TRUE;
	// ((CPointsDirDlg*)m_pParent)->m_statusBar.SetPaneText(0, "Ending transformation...", TRUE);
}

void CPointsDirProgressDlg::PostNcDestroy()
{
	delete this;
}

BOOL CPointsDirProgressDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/TF_overview.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}
