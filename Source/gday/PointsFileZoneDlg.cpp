// PointsFileZoneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "PointsFileZoneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointsFileZoneDlg dialog


CPointsFileZoneDlg::CPointsFileZoneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPointsFileZoneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPointsFileZoneDlg)
	m_strPrompt = _T("");
	m_strZone = _T("");
	//}}AFX_DATA_INIT
}


void CPointsFileZoneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointsFileZoneDlg)
	DDX_Text(pDX, IDC_PROMPT, m_strPrompt);
	DDX_Text(pDX, IDC_PFILE_ZONE, m_strZone);
	DDV_MaxChars(pDX, m_strZone, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointsFileZoneDlg, CDialog)
	//{{AFX_MSG_MAP(CPointsFileZoneDlg)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointsFileZoneDlg message handlers

void CPointsFileZoneDlg::OnOK() 
{
	UpdateData(TRUE);
	m_iZone = atoi(m_strZone);
	if (m_iZone < 47 || m_iZone > 58)
	{
		MessageBox("Specify a zone between 47 and 58", "Invalid zone", MB_OK | MB_ICONEXCLAMATION);
		GotoDlgCtrl(GetDlgItem(IDC_PFILE_ZONE));
		return;
	}
	CDialog::OnOK();
}

BOOL CPointsFileZoneDlg::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CPointsFileZoneDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_ToolTip.Create(this))
	{
		m_ToolTip.AddTool(GetDlgItem(IDC_PFILE_ZONE), IDC_PFILE_ZONE);
		m_ToolTip.Activate(TRUE);
	}
	return TRUE;
}

BOOL CPointsFileZoneDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/TF_zone.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}
