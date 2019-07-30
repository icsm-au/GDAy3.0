// Page1.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "Page1.h"
#include "OptionsSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage1 property page

IMPLEMENT_DYNCREATE(CPage1, CPropertyPage)

CPage1::CPage1() : CPropertyPage(CPage1::IDD)
{
	//{{AFX_DATA_INIT(CPage1)
	m_strStartingPointNumber = _T("");
	m_intAutoIncrement = FALSE;
	m_intAddSymbols = FALSE;
	m_intPointNumbering = FALSE;
	m_intAddPrefix = FALSE;
	m_intLatisPosSouth = FALSE;
	m_intSecondsPrec = 0;
	m_intMetresPrec = 0;
	m_P1HasChanged = 0;
	//}}AFX_DATA_INIT
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPage1)
	DDX_Control(pDX, IDC_SECONDS_SPIN, m_intSecondsSpin);
	DDX_Control(pDX, IDC_METRES_SPIN, m_intMetresSpin);
	DDX_CBString(pDX, IDC_POINT_START_NUM, m_strStartingPointNumber);
	DDV_MaxChars(pDX, m_strStartingPointNumber, 8);
	DDX_Check(pDX, IDC_AUTO_INC, m_intAutoIncrement);
	DDX_Check(pDX, IDC_DMS_FORMAT, m_intAddSymbols);
	DDX_Check(pDX, IDC_POINT_NUM, m_intPointNumbering);
	DDX_Check(pDX, IDC_POS_NEG_FORMAT, m_intAddPrefix);
	DDX_Check(pDX, IDC_POS_SOUTH, m_intLatisPosSouth);
	DDX_Text(pDX, IDC_SECONDS_PREC, m_intSecondsPrec);
	DDV_MinMaxInt(pDX, m_intSecondsPrec, 0, 7);
	DDX_Text(pDX, IDC_METRES_PREC, m_intMetresPrec);
	DDV_MinMaxInt(pDX, m_intMetresPrec, 0, 5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CPage1)
	ON_BN_CLICKED(IDC_AUTO_INC, OnAutoIncrement)
	ON_BN_CLICKED(IDC_POINT_NUM, OnPointNumber)
	ON_BN_CLICKED(IDC_POS_SOUTH, OnChange)
	ON_BN_CLICKED(IDC_POS_NEG_FORMAT, OnChange)
	ON_BN_CLICKED(IDC_DMS_FORMAT, OnChange)
	ON_CBN_SELCHANGE(IDC_POINT_START_NUM, OnChange)
	ON_CBN_EDITCHANGE(IDC_POINT_START_NUM, OnChange)
	ON_EN_CHANGE(IDC_SECONDS_PREC, OnChange)
	ON_EN_CHANGE(IDC_METRES_PREC, OnChange)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage1 message handlers

BOOL CPage1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Create tooltip control
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip
		m_ToolTip.AddTool(GetDlgItem(IDC_METRES_PREC), IDC_METRES_PREC);
		m_ToolTip.AddTool(GetDlgItem(IDC_SECONDS_PREC), IDC_SECONDS_PREC);
		m_ToolTip.AddTool(GetDlgItem(IDC_POS_SOUTH), IDC_POS_SOUTH);
		m_ToolTip.AddTool(GetDlgItem(IDC_POS_NEG_FORMAT), IDC_POS_NEG_FORMAT);
		m_ToolTip.AddTool(GetDlgItem(IDC_DMS_FORMAT), IDC_DMS_FORMAT);
		m_ToolTip.AddTool(GetDlgItem(IDC_POINT_START_NUM), IDC_POINT_START_NUM);
		m_ToolTip.AddTool(GetDlgItem(IDC_POINT_NUM), IDC_POINT_NUM);
		m_ToolTip.AddTool(GetDlgItem(IDC_AUTO_INC), IDC_AUTO_INC);
		
		// activate ToolTip control
		m_ToolTip.Activate(TRUE);
	}
	
	m_intSecondsSpin.SetBuddy(GetDlgItem(IDC_SECONDS_PREC));
	m_intSecondsSpin.SetRange(0, 7);	
	m_intMetresSpin.SetBuddy(GetDlgItem(IDC_METRES_PREC));
	m_intMetresSpin.SetRange(0, 5);

	if (m_intPointNumbering)
	{
		if (!m_intAutoIncrement)
		{
			GetDlgItem(IDC_POINT_START_NUM)->EnableWindow(FALSE);
			GetDlgItem(IDP_STARTAT)->EnableWindow(FALSE);
		}
	}
	else
	{
		GetDlgItem(IDC_AUTO_INC)->EnableWindow(FALSE);
		GetDlgItem(IDC_POINT_START_NUM)->EnableWindow(FALSE);
		GetDlgItem(IDP_STARTAT)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPage1::OnAutoIncrement() 
{
	OnChange();
	if (((CButton*)GetDlgItem(IDC_AUTO_INC))->GetCheck())
	{
		GetDlgItem(IDC_POINT_START_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDP_STARTAT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_POINT_START_NUM)->EnableWindow(FALSE);
		GetDlgItem(IDP_STARTAT)->EnableWindow(FALSE);
	}
}

void CPage1::OnPointNumber() 
{
	OnChange();
	if (((CButton*)GetDlgItem(IDC_POINT_NUM))->GetCheck())
	{
		GetDlgItem(IDC_AUTO_INC)->EnableWindow(TRUE);
			
		if (((CButton*)GetDlgItem(IDC_AUTO_INC))->GetCheck())
		{
			GetDlgItem(IDC_POINT_START_NUM)->EnableWindow(TRUE);
			GetDlgItem(IDP_STARTAT)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_POINT_START_NUM)->EnableWindow(FALSE);
			GetDlgItem(IDP_STARTAT)->EnableWindow(FALSE);
		}
	}
	else
	{
		GetDlgItem(IDC_AUTO_INC)->EnableWindow(FALSE);
		GetDlgItem(IDC_POINT_START_NUM)->EnableWindow(FALSE);
		GetDlgItem(IDP_STARTAT)->EnableWindow(FALSE);
	}
}

void CPage1::OnChange() 
{
	m_P1HasChanged = 1;
	SetModified(TRUE);	
}

BOOL CPage1::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CPage1::OnKillActive() 
{
	((COptions*)this->GetParent())->m_iLastPageIndex = 0;
	if (!UpdateData(TRUE))
		return FALSE;	
	return CPropertyPage::OnKillActive();
}

BOOL CPage1::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/UI_options.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}
