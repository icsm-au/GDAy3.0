// Page3.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "Page3.h"
#include "OptionsSheet.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage3 property page

IMPLEMENT_DYNCREATE(CPage3, CPropertyPage)

CPage3::CPage3() : CPropertyPage(CPage3::IDD)
{
	//{{AFX_DATA_INIT(CPage3)
	m_strHeaderInfo = _T("");
	m_strLogFilePath = _T("");
	m_P3HasChanged = 0;
	//}}AFX_DATA_INIT
}

void CPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPage3)
	DDX_Text(pDX, IDC_HEADER, m_strHeaderInfo);
	DDV_MaxChars(pDX, m_strHeaderInfo, 500);
	DDX_Text(pDX, IDC_LOG_FILEPATH, m_strLogFilePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPage3, CPropertyPage)
	//{{AFX_MSG_MAP(CPage3)
	ON_BN_CLICKED(IDC_LOG_FILE_CHANGE, OnLogFileChange)
	ON_BN_CLICKED(IDC_LOG_VIEW, OnLogView)
	ON_EN_CHANGE(IDC_HEADER, OnChange)
	ON_EN_CHANGE(IDC_LOG_FILEPATH, OnChange)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPage3 message handlers

void CPage3::OnLogFileChange() 
{
	CString strFile, strFolder;
	char c_newfile[_MAX_PATH];
	
	GetDlgItem(IDC_LOG_FILEPATH)->GetWindowText(strFile);
    
	CFileProc file;

	strFolder = file.GetFolder(strFile);
	strcpy_s(c_newfile, file.GetName(strFile, NULL));

	//Show the File Open dialog and Capture the result
	strFile = file.FileOpenSave(FALSE, FALSE, strFolder, c_newfile, SELECT_LOGFILE, LOG, LOG_FILTER, 0);
		
	//Update the dialog
	if (!strFile.IsEmpty())
	{
		GetDlgItem(IDC_LOG_FILEPATH)->SetWindowText(strFile);
		OnChange();
	}
}

void CPage3::OnChange() 
{
	SetModified(TRUE);

	CString str;

	m_P3HasChanged = 1;
	GetDlgItem(IDC_LOG_FILEPATH)->GetWindowText(str);

	if (m_strLogFilePath != str)
		GetDlgItem(IDC_LOG_VIEW)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_LOG_VIEW)->EnableWindow(TRUE);
}

BOOL CPage3::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Create tooltip control
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip
		m_ToolTip.AddTool(GetDlgItem(IDC_LOG_FILEPATH), IDC_LOG_FILEPATH);
		m_ToolTip.AddTool(GetDlgItem(IDC_LOG_FILE_CHANGE), IDC_LOG_FILE_CHANGE);
		m_ToolTip.AddTool(GetDlgItem(IDC_LOG_VIEW), IDC_LOG_VIEW);
		m_ToolTip.AddTool(GetDlgItem(IDC_HEADER), IDC_HEADER);
		
		// activate ToolTip control
		m_ToolTip.Activate(TRUE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPage3::OnLogView() 
{
	CString logfile, viewer, header, msg;
	long hFile;
	
	GetDlgItem(IDC_LOG_FILEPATH)->GetWindowText(logfile);
	GetDlgItem(IDC_HEADER)->GetWindowText(header);

    CFileProc file;

	if (!file.Exists(logfile, &hFile))
		GCALC_PrintLogFile((char*)((LPCTSTR)logfile), "w", (char*)((LPCTSTR)header));

	viewer = "notepad.exe";
	
	if (file.View(logfile, viewer) == 0)
	{
		msg = "Could not load:  \"";
		msg += viewer;
		msg += "\"\n\nPlease check that the file has not been\nmoved or deleted.\n";
		MessageBox(msg, "GDAy",
			MB_OK | MB_ICONWARNING);
	}
}

BOOL CPage3::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);
	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CPage3::OnKillActive() 
{
	((COptions*)this->GetParent())->m_iLastPageIndex = 2;
	if (!UpdateData(TRUE))
		return FALSE;	
	return CPropertyPage::OnKillActive();
}

void CPage3::OnOK() 
{
	GetDlgItem(IDC_LOG_VIEW)->EnableWindow(TRUE);
	char header[601];
	strcpy_s(header, m_strHeaderInfo);
	CPropertyPage::OnOK();
}

BOOL CPage3::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/LF_options.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}
