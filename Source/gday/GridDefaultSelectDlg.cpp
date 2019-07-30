// GridDefaultSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "GridDefaultSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridDefaultSelectDlg dialog


CGridDefaultSelectDlg::CGridDefaultSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridDefaultSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridDefaultSelectDlg)
	m_intPrompt = FALSE;
	//}}AFX_DATA_INIT
}


void CGridDefaultSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridDefaultSelectDlg)
	DDX_Check(pDX, IDC_GRIDFILE_REMINDER, m_intPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGridDefaultSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CGridDefaultSelectDlg)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridDefaultSelectDlg message handlers

BOOL CGridDefaultSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// Create tooltip control
	if (m_ToolTip.Create(this))
	{
		m_ToolTip.AddTool(GetDlgItem(IDC_GRIDFILE_REMINDER), IDC_GRIDFILE_REMINDER);
		m_ToolTip.Activate(TRUE);
	}
	((CStatic*)GetDlgItem(IDC_ICONQUESTION))->SetIcon(LoadIcon(NULL, IDI_QUESTION));
	UpdateData(FALSE);
	return TRUE;
}

void CGridDefaultSelectDlg::OnOK() 
{
	UpdateData(TRUE);
	EndDialog(IDYES);
	return;
}

void CGridDefaultSelectDlg::OnCancel() 
{
	UpdateData(TRUE);
	EndDialog(IDNO);
	return;
}

BOOL CGridDefaultSelectDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/TC_choose_grid.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}

// Relay events to the tooltip control	
BOOL CGridDefaultSelectDlg::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}