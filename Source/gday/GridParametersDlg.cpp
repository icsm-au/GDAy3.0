// GridParametersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "GridParametersDlg.h"
#include <GCalcext.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGridParametersDlg dialog

CGridParametersDlg::CGridParametersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridParametersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridParametersDlg)
	m_strGridFile = _T("");
	m_fpGridHeaderArray = NULL;
	//}}AFX_DATA_INIT
}


void CGridParametersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridParametersDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGridParametersDlg, CDialog)
	//{{AFX_MSG_MAP(CGridParametersDlg)
	ON_BN_CLICKED(IDC_GRID_BROWSE, OnGridBrowse)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridParametersDlg message handlers

void CGridParametersDlg::OnGridBrowse() 
{
	CString strGrid, strDefExt, strFolder, strbuf;
	char c_grid[_MAX_PATH] = {'\0'};
	BOOL bFoundParent = FALSE;
	int filter_index = 1;
	
	UpdateData(TRUE);	
	CFileProc file;

	if (m_intGridFileStatus == ERR_GRID_SUCCESS && m_intGridFileStatus == ERR_AUS_BINARY)
	{
		strGrid = m_strGridFile;
		strFolder = file.GetFolder(m_strGridFile);
		strDefExt = file.GetExt(m_strGridFile);
		strcpy_s(c_grid, file.GetName(m_strGridFile, NULL));	
		filter_index = ((strDefExt.CompareNoCase(ASC) == 0) ? 2 : 1);
	}

	strGrid = file.FileOpenSave(TRUE, FALSE, strFolder, c_grid, SELECT_GRID_FILE, strDefExt, GRID_FILTER, filter_index);	
	if (strGrid.IsEmpty())
		return;
	GetDlgItem(IDC_GRID_FILENAME)->SetWindowText(strGrid);	
	m_fpGridHeaderArray = new (file_par);
	memset(m_fpGridHeaderArray, 0, sizeof(file_par));
	// retrieve grid data
	GCALC_ReportGridProperties((char*)((LPCTSTR)strGrid), (char*)((LPCTSTR)file.m_strFileExt), m_fpGridHeaderArray, &m_intGridFileStatus);
	if (m_intGridFileStatus == ERR_AUS_BINARY)
		MessageBox(AUS_GSB_MSG, "Obsolete File Format",
			MB_OK | MB_ICONEXCLAMATION);

	if (m_intGridFileStatus > 0)
	{
		CErrMsg err;
		err.strGridFile = strGrid;
		err.ReportError(m_intGridFileStatus, "NEW");		
		MessageBox(err.ErrorString, "Select Distortion Grid",
			MB_OK | MB_ICONEXCLAMATION);
		((CButton*)GetDlgItem(IDOK))->EnableWindow(FALSE);
		GetDlgItem(IDC_NORTH)->SetWindowText(_T(" "));
		GetDlgItem(IDC_SOUTH)->SetWindowText(_T(" "));
		GetDlgItem(IDC_EAST)->SetWindowText(_T(" "));
		GetDlgItem(IDC_WEST)->SetWindowText(_T(" "));
		delete m_fpGridHeaderArray;
		m_fpGridHeaderArray = NULL;
		return;
	}
	else
	{
		m_strGridFile = strGrid;
		OnUpdateGridLimits();
	}
}

BOOL CGridParametersDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// create tooltip control
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip
		m_ToolTip.AddTool(GetDlgItem(IDC_PARAM), IDC_PARAM);
		m_ToolTip.AddTool(GetDlgItem(IDC_GRID_BROWSE), IDC_GRID_BROWSE);
		m_ToolTip.AddTool(GetDlgItem(IDC_GRID_FILENAME), IDC_GRID_FILENAME);
		
		// activate ToolTip control
		m_ToolTip.Activate(TRUE);
	}

	((CComboBox*)GetDlgItem(IDC_PARAM))->AddString(NAT94);
	((CComboBox*)GetDlgItem(IDC_PARAM))->SelectString(0, m_strTransParam);
	if (strcmp(m_strGridFile, NO_GRID_SPECIFIED) != 0)
	{
		OnUpdateGridLimits();
		((CEdit*)GetDlgItem(IDC_GRID_FILENAME))->SetWindowText(m_strGridFile);
	}

	return TRUE;
}

void CGridParametersDlg::OnOK() 
{	
	int iIndex;
	CString strSelectedGrid;

	iIndex = ((CComboBox*)GetDlgItem(IDC_PARAM))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_PARAM))->GetLBText(iIndex, m_strTransParam.GetBuffer(((CComboBox*)GetDlgItem(IDC_PARAM))->GetLBTextLen(iIndex)));
	CDialog::OnOK();
}

void CGridParametersDlg::OnHelp() 
{
	::HtmlHelp(NULL, "gday.chm::/html/TC_choose_grid.htm", HH_DISPLAY_TOC, 0);
}

void CGridParametersDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CGridParametersDlg::OnUpdateGridLimits() 
{
	int i;
	double dnlat, dslat, delon, dwlon;
	CString strbuf;
	
	T_user theUser;

	if (m_fpGridHeaderArray == NULL)
	{
		m_fpGridHeaderArray = new (file_par);
		memset(m_fpGridHeaderArray, 0, sizeof(file_par));
	}
	
	CFileProc file;

	// retrieve grid data
	GCALC_ReportGridProperties((char*)((LPCTSTR)m_strGridFile), (char*)((LPCTSTR)file.GetExt(m_strGridFile)), m_fpGridHeaderArray, &m_intGridFileStatus);
	
	if (m_intGridFileStatus > 0)
	{
		// an error in the grid or grid does not exist
		delete m_fpGridHeaderArray;
		return;
	}

	dnlat = dwlon = -200.0 * 3600.0;
	dslat = delon = 200.0 * 3600.0;

	CStrProc string;
	
	// get parent grid extents and display
	for (i=0; i<m_fpGridHeaderArray->iNumsubgrids; i++)
	{
		if (strcmp(m_fpGridHeaderArray->ptrIndex[i].chParent, "NONE    \0") == 0)
		{
			if (dnlat < m_fpGridHeaderArray->ptrIndex[i].dNlat)
				dnlat = m_fpGridHeaderArray->ptrIndex[i].dNlat;

			if (dslat > m_fpGridHeaderArray->ptrIndex[i].dSlat)
				dslat = m_fpGridHeaderArray->ptrIndex[i].dSlat;

			if (delon > m_fpGridHeaderArray->ptrIndex[i].dElong)
				delon = m_fpGridHeaderArray->ptrIndex[i].dElong;

			if (dwlon < m_fpGridHeaderArray->ptrIndex[i].dWlong)
				dwlon = m_fpGridHeaderArray->ptrIndex[i].dWlong;
		}
	}

	theUser.AddPrefix = 0;
	theUser.AddSymbols = 1;
	theUser.GeoPrecision = 4;
	theUser.InputDMS = 0;
	theUser.InputisProjected = 0;
	
	strbuf.Format("%.4f", GCALC_DegtoDms(dnlat/3600));
	GetDlgItem(IDC_NORTH)->SetWindowText(_T(string.AddSymbology(strbuf, theUser, 1)));
	strbuf.Format("%.4f", GCALC_DegtoDms(dslat/3600));
	GetDlgItem(IDC_SOUTH)->SetWindowText(_T(string.AddSymbology(strbuf, theUser, 1)));
	strbuf.Format("%.4f", GCALC_DegtoDms(delon/-3600));
	GetDlgItem(IDC_EAST)->SetWindowText(_T(string.AddSymbology(strbuf, theUser, 2)));
	strbuf.Format("%.4f", GCALC_DegtoDms(dwlon/-3600));
	GetDlgItem(IDC_WEST)->SetWindowText(_T(string.AddSymbology(strbuf, theUser, 2)));

	delete m_fpGridHeaderArray;
	m_fpGridHeaderArray = NULL;
	((CButton*)GetDlgItem(IDOK))->EnableWindow(TRUE);
}

BOOL CGridParametersDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CGridParametersDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/TC_choose_grid.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}
