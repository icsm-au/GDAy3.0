// Page2.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "GDAyDlg.h"
#include "Page2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage2 property page

IMPLEMENT_DYNCREATE(CPage2, CPropertyPage)

CPage2::CPage2() : CPropertyPage(CPage2::IDD)
{
	//{{AFX_DATA_INIT(CPage2)
	m_intAddStationName = FALSE;
	m_intCreateOutputFile = FALSE;
	m_strCustomName = _T("");
	m_strInteractiveFilePath = _T("");
	m_intPromptStationName = -1;
	m_P2HasChanged = 0;
	//}}AFX_DATA_INIT
}

void CPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPage2)
	DDX_Check(pDX, IDC_ADD_STN_NAME, m_intAddStationName);
	DDX_Check(pDX, IDC_CREATE_OUTPUT_FILE, m_intCreateOutputFile);
	DDX_Text(pDX, IDC_CUSTOM_NAME, m_strCustomName);
	DDV_MaxChars(pDX, m_strCustomName, 25);
	DDX_Text(pDX, IDC_OUTPUT_FILEPATH, m_strInteractiveFilePath);
	DDX_Radio(pDX, IDC_PROMPT_STN_NAME, m_intPromptStationName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPage2, CPropertyPage)
	//{{AFX_MSG_MAP(CPage2)
	ON_BN_CLICKED(IDC_ADD_STN_NAME, OnAddStnName)
	ON_BN_CLICKED(IDC_OUTPUT_FILE_CHANGE, OnOutputFileChange)
	ON_EN_CHANGE(IDC_OUTPUT_FILEPATH, OnChange)
	ON_BN_CLICKED(IDC_AUTO_NAME, OnStationNaming)
	ON_BN_CLICKED(IDC_PROMPT_STN_NAME, OnStationNaming)
	ON_BN_CLICKED(IDC_CREATE_OUTPUT_FILE, OnChange)
	ON_EN_CHANGE(IDC_CUSTOM_NAME, OnChange)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage2 message handlers

BOOL CPage2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Create tooltip control
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_FILEPATH), IDC_OUTPUT_FILEPATH);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_FILE_CHANGE), IDC_OUTPUT_FILE_CHANGE);
		m_ToolTip.AddTool(GetDlgItem(IDC_CREATE_OUTPUT_FILE), IDC_CREATE_OUTPUT_FILE);
		m_ToolTip.AddTool(GetDlgItem(IDC_ADD_STN_NAME), IDC_ADD_STN_NAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_PROMPT_STN_NAME), IDC_PROMPT_STN_NAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_AUTO_NAME), IDC_AUTO_NAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_CUSTOM_NAME), IDC_CUSTOM_NAME);
		
		// activate ToolTip control
		m_ToolTip.Activate(TRUE);
	}
	
	if (m_intAddStationName) 
	{
		// User wants to add station naming
		GetDlgItem(IDC_PROMPT_STN_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_AUTO_NAME)->EnableWindow(TRUE);

			if (m_intPromptStationName) 
			{
				// User wants to be prompted for station name
				GetDlgItem(IDC_CUSTOM_NAME)->EnableWindow(TRUE);
				GetDlgItem(IDP_DEFNAME)->EnableWindow(TRUE);
			}
			else
			{
				// User does not want to be prompted for station name...ie Auto name
				GetDlgItem(IDC_CUSTOM_NAME)->EnableWindow(FALSE);
				GetDlgItem(IDP_DEFNAME)->EnableWindow(FALSE);
			}
	}
	else
	{
		// User does not want to add station naming
		GetDlgItem(IDC_CUSTOM_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDP_DEFNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_PROMPT_STN_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_AUTO_NAME)->EnableWindow(FALSE);
	}	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPage2::OnAddStnName() 
{
	OnChange();

	if (((CButton*)GetDlgItem(IDC_ADD_STN_NAME))->GetCheck())
	{
		GetDlgItem(IDC_PROMPT_STN_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_AUTO_NAME)->EnableWindow(TRUE);
		OnStationNaming();
	}
	else
	{
		GetDlgItem(IDC_PROMPT_STN_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_AUTO_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_CUSTOM_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDP_DEFNAME)->EnableWindow(FALSE);
	}
}

void CPage2::OnStationNaming() 
{
	OnChange();

	if (((CButton*)GetDlgItem(IDC_PROMPT_STN_NAME))->GetCheck())
	{
		GetDlgItem(IDC_CUSTOM_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDP_DEFNAME)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CUSTOM_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDP_DEFNAME)->EnableWindow(TRUE);
	}
}

void CPage2::OnChange() 
{
	m_P2HasChanged = 1;
	SetModified(TRUE);
}

void CPage2::OnOutputFileChange() 
{
	CString strFile, strFolder;
	char c_newfile[_MAX_PATH];
	
	CFileProc file;

	GetDlgItem(IDC_OUTPUT_FILEPATH)->GetWindowText(strFile);
    
	strFolder = file.GetFolder(strFile);
	strcpy_s(c_newfile, file.GetName(strFile, NULL));

	//Show the File Open dialog and Capture the result
	strFile = file.FileOpenSave(FALSE, FALSE, strFolder, c_newfile, SELECT_OUTFILE, OUTPUT, OUT_FILTER, 0);
		
	//Update the dialog
	if (!strFile.IsEmpty())
	{
		GetDlgItem(IDC_OUTPUT_FILEPATH)->SetWindowText(strFile);
		OnChange();
	}
}

BOOL CPage2::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CPage2::OnKillActive() 
{
	((COptions*)this->GetParent())->m_iLastPageIndex = 1;
	if (!UpdateData(TRUE))
		return FALSE;	
	return CPropertyPage::OnKillActive();
}

BOOL CPage2::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/IO_options.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}
