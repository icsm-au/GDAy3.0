// GridPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include <GCalcext.h>
#include "GridPropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridPropertiesDlg dialog


CGridPropertiesDlg::CGridPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridPropertiesDlg)
	m_fpGridproperties = NULL;
	//}}AFX_DATA_INIT
}


void CGridPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridPropertiesDlg)
	DDX_Control(pDX, IDC_SUBGRID_NAME, m_ctlSubGridName);
	DDX_Text(pDX, IDC_FILENAME, m_strGridFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGridPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CGridPropertiesDlg)
	ON_CBN_SELCHANGE(IDC_SUBGRID_NAME, OnSelchangeSubgridName)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridPropertiesDlg message handlers

BOOL CGridPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int success, i;
	CString strExt, strbuf;
	
	CFileProc file;
	CStrProc str;
	
	// create tooltip control
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip
		m_ToolTip.AddTool(GetDlgItem(IDC_EAST_LONG), IDC_EAST_LONG);
		m_ToolTip.AddTool(GetDlgItem(IDC_WEST_LONG), IDC_WEST_LONG);
		m_ToolTip.AddTool(GetDlgItem(IDC_VERSION), IDC_VERSION);
		m_ToolTip.AddTool(GetDlgItem(IDC_SYSTEM_TO), IDC_SYSTEM_TO);
		m_ToolTip.AddTool(GetDlgItem(IDC_SYSTEM_FROM), IDC_SYSTEM_FROM);
		m_ToolTip.AddTool(GetDlgItem(IDC_SOUTH_LAT), IDC_SOUTH_LAT);
		m_ToolTip.AddTool(GetDlgItem(IDC_NORTH_LAT), IDC_NORTH_LAT);
		m_ToolTip.AddTool(GetDlgItem(IDC_NUMSUBGRIDS), IDC_NUMSUBGRIDS);
		m_ToolTip.AddTool(GetDlgItem(IDC_FILETYPE), IDC_FILETYPE);
		m_ToolTip.AddTool(GetDlgItem(IDC_SUBGRID_NAME), IDC_SUBGRID_NAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_FILENAME), IDC_FILENAME);

		// activate ToolTip control
		m_ToolTip.Activate(TRUE);
	}

	m_fpGridproperties = new (file_par);
	memset(m_fpGridproperties, 0, sizeof(file_par));
	strExt = file.GetExt(m_strGridFile);
	// call ReportGridProperties to retrieve grid data
	GCALC_ReportGridProperties((char*)((LPCTSTR)m_strGridFile), (char*)((LPCTSTR)strExt), m_fpGridproperties, &success);
	if (success > 0)
	{
		CErrMsg err;
		err.strGridFile = m_strGridFile;
		err.ReportError(success, '\0');
		MessageBox(err.ErrorString, "Grid Properties",
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if ((strExt.CompareNoCase(GSB) == 0) && success == ERR_AUS_BINARY)
		strbuf = "Binary (Australian)";
	else if ((strExt.CompareNoCase(GSB) == 0) && success == ERR_GRID_SUCCESS)
		strbuf = "Binary";
	else if (strExt.CompareNoCase(ASC) == 0)
		strbuf = "ASCII";
	else
		strbuf = "";

	GetDlgItem(IDC_FILETYPE)->SetWindowText(strbuf);
	strbuf.Format("%d", m_fpGridproperties->iNumsubgrids);
	GetDlgItem(IDC_NUMSUBGRIDS)->SetWindowText(strbuf);
	GetDlgItem(IDC_VERSION)->SetWindowText(m_fpGridproperties->chVersion);
	GetDlgItem(IDC_SYSTEM_FROM)->SetWindowText(m_fpGridproperties->chSystem_f);
	GetDlgItem(IDC_SYSTEM_TO)->SetWindowText(m_fpGridproperties->chSystem_t);
	
	m_ctlSubGridName.ResetContent();
	m_intParentIndex = -1;

	// fill combo box with sub names in same order as sub grids appear in the file
	for (i=0; i<m_fpGridproperties->iNumsubgrids; i++)
	{	
		strbuf = m_fpGridproperties->ptrIndex[i].chSubname;
		m_ctlSubGridName.AddString(strbuf);
		// Get index number of first parent grid found
		if (strcmp("NONE    ", m_fpGridproperties->ptrIndex[i].chParent) == 0 && m_intParentIndex == -1)
			m_intParentIndex = i;
	}

	if (m_intParentIndex == -1)			// parent not found at all???
	{
		CErrMsg err;
		err.strGridFile = m_strGridFile;
		err.ReportError(ERR_GRID_CORRUPT, '\0');
		
		MessageBox(err.ErrorString, "Grid Properties",
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	m_ctlSubGridName.SetCurSel(m_intParentIndex);
	UpdateControls();
	return TRUE;
}

void CGridPropertiesDlg::OnSelchangeSubgridName()
{
	m_intParentIndex = m_ctlSubGridName.GetCurSel();
	UpdateControls();
}

void CGridPropertiesDlg::OnOK() 
{
	delete m_fpGridproperties;
	CDialog::OnOK();
}

BOOL CGridPropertiesDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CGridPropertiesDlg::UpdateControls()
{
	CString strbuf;
	T_user theUser;
	CStrProc str;

	theUser.AddPrefix = 0;
	theUser.AddSymbols = 1;
	theUser.GeoPrecision = 4;
	theUser.InputDMS = 0;
	theUser.InputisProjected = 0;
	
	strbuf.Format("%.4f", GCALC_DegtoDms(m_fpGridproperties->ptrIndex[m_intParentIndex].dNlat/3600));
	GetDlgItem(IDC_NORTH_LAT)->SetWindowText(str.AddSymbology(strbuf, theUser, 1));
	
	strbuf.Format("%.4f", GCALC_DegtoDms(m_fpGridproperties->ptrIndex[m_intParentIndex].dSlat/3600));
	GetDlgItem(IDC_SOUTH_LAT)->SetWindowText(str.AddSymbology(strbuf, theUser, 1));

	strbuf.Format("%.4f", GCALC_DegtoDms(m_fpGridproperties->ptrIndex[m_intParentIndex].dElong/-3600));
	GetDlgItem(IDC_EAST_LONG)->SetWindowText(str.AddSymbology(strbuf, theUser, 2));

	strbuf.Format("%.4f", GCALC_DegtoDms(m_fpGridproperties->ptrIndex[m_intParentIndex].dWlong/-3600));
	GetDlgItem(IDC_WEST_LONG)->SetWindowText(str.AddSymbology(strbuf, theUser, 2));
}

BOOL CGridPropertiesDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/NTV2_view_grid_prop.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}

void CGridPropertiesDlg::OnHelp() 
{
	::HtmlHelp(NULL, "gday.chm::/html/NTV2_view_grid_prop.htm", HH_DISPLAY_TOC, 0);
	
}
