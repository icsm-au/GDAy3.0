// StationNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "StationNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationNameDlg dialog


CStationNameDlg::CStationNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStationNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStationNameDlg)
	m_strStationName = _T("");
	//}}AFX_DATA_INIT
}


void CStationNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationNameDlg)
	DDX_Text(pDX, IDC_STATION_NAME, m_strStationName);
	DDV_MaxChars(pDX, m_strStationName, 25);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStationNameDlg, CDialog)
	//{{AFX_MSG_MAP(CStationNameDlg)
	ON_EN_SETFOCUS(IDC_STATION_NAME, OnSetfocusStationName)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationNameDlg message handlers

void CStationNameDlg::OnOK() 
{
	UpdateData(TRUE);	
	CDialog::OnOK();
}

void CStationNameDlg::OnSetfocusStationName() 
{
	((CEdit*)GetDlgItem(IDC_STATION_NAME))->SetSel(0, -1, FALSE);
}


BOOL CStationNameDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CStationNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Create tooltip control
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip & activate
		m_ToolTip.AddTool(GetDlgItem(IDC_STATION_NAME), IDC_STATION_NAME);
		m_ToolTip.Activate(TRUE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CStationNameDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/IO_station_naming.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}
