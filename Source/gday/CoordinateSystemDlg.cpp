// CoordinateSystemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "CoordinateSystemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoordinateSystemDlg dialog


CCoordinateSystemDlg::CCoordinateSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCoordinateSystemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCoordinateSystemDlg)
	//}}AFX_DATA_INIT
}


void CCoordinateSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCoordinateSystemDlg)
	DDX_CBIndex(pDX, IDC_INPUT_DATUM, (m_coordinate_t.InputisGDA20));
	DDX_LBIndex(pDX, IDC_INPUT_PROJ, (m_coordinate_t.InputisProjected));
	DDX_Radio(pDX, IDC_IN_DMS, (m_coordinate_t.InputDMS));
	DDX_CBIndex(pDX, IDC_OUTPUT_DATUM, (m_coordinate_t.OutputisGDA));
	DDX_LBIndex(pDX, IDC_OUTPUT_PROJ, (m_coordinate_t.OutputisProjected));
	DDX_Check(pDX, IDC_OUTZONE_PROMPT, (m_coordinate_t.PromptforOutputZone));
	DDX_Radio(pDX, IDC_OUT_DMS, (m_coordinate_t.OutputDMS));
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCoordinateSystemDlg, CDialog)
	//{{AFX_MSG_MAP(CCoordinateSystemDlg)
	ON_CBN_SELCHANGE(IDC_INPUT_DATUM, OnSelchangeInputDatum)
	ON_LBN_SELCHANGE(IDC_INPUT_PROJ, OnSelchangeInputProj)
	ON_CBN_SELCHANGE(IDC_OUTPUT_DATUM, OnSelchangeOutputDatum)
	ON_LBN_SELCHANGE(IDC_OUTPUT_PROJ, OnSelchangeOutputProj)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoordinateSystemDlg message handlers

void CCoordinateSystemDlg::OnSelchangeInputDatum() 
{
	int selection;

	UpdateData(TRUE);

	selection = ((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->GetCurSel();

	// delete all contents of the input projection list box
	((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->ResetContent();
	((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->AddString(GEO);

	switch ( ((CComboBox*)GetDlgItem(IDC_INPUT_DATUM))->GetCurSel() )
	{
	case 0:
		((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->AddString(MGA94_STR);
		break;
	case 1:
		((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->AddString(MGA20_STR);
		break;
	default:
		MessageBox(SEL_ERR, "Error", MB_OK | MB_ICONEXCLAMATION);
		break;
	}
	
	((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->SetCurSel(selection);

	OnSelchangeInputProj();	
}

void CCoordinateSystemDlg::OnSelchangeInputProj() 
{
	switch ( ((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->GetCurSel() )
	{
	case 0:
		// The user selected "None"
		GetDlgItem(IDC_IN_DDEG)->EnableWindow(TRUE);
		GetDlgItem(IDC_IN_DMIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_IN_DMS)->EnableWindow(TRUE);
		GetDlgItem(IDC_IN_FORMAT)->EnableWindow(TRUE);
		break;
	case 1:
		// The user selected a projection type...no choice for D.ms/D.Deg
		GetDlgItem(IDC_IN_DDEG)->EnableWindow(FALSE);
		GetDlgItem(IDC_IN_DMIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_IN_DMS)->EnableWindow(FALSE);
		GetDlgItem(IDC_IN_FORMAT)->EnableWindow(FALSE);
		break;
	default:
		MessageBox(SEL_ERR, "Error", MB_OK | MB_ICONEXCLAMATION);
		break;
	}	
}

void CCoordinateSystemDlg::OnSelchangeOutputDatum() 
{	
	int selection;

	selection = ((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->GetCurSel();

	// delete all contents of the output projection list box
	((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->ResetContent();
	((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->AddString(GEO);
	
	switch ( ((CComboBox*)GetDlgItem(IDC_OUTPUT_DATUM))->GetCurSel() )
	{
	case 0:
		((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->AddString(MGA94_STR);
		break;
	case 1:
		((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->AddString(MGA20_STR);
		break;
	default:
		MessageBox(SEL_ERR, "Error", MB_OK | MB_ICONEXCLAMATION);
		break;
	}

	((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->SetCurSel(selection);

	OnSelchangeOutputProj();
}

void CCoordinateSystemDlg::OnSelchangeOutputProj() 
{
	switch ( ((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->GetCurSel() )
	{
	case 0:
		// The user selected "None"
		GetDlgItem(IDC_OUT_DDEG)->EnableWindow(TRUE);
		GetDlgItem(IDC_OUT_DMIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_OUT_DMS)->EnableWindow(TRUE);
		GetDlgItem(IDC_OUT_FORMAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_OUTZONE_PROMPT)->EnableWindow(FALSE);
		break;
	case 1:
		// The user selected a projection type...no choice for D.ms/D.Deg
		GetDlgItem(IDC_OUT_DDEG)->EnableWindow(FALSE);
		GetDlgItem(IDC_OUT_DMIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_OUT_DMS)->EnableWindow(FALSE);
		GetDlgItem(IDC_OUT_FORMAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_OUTZONE_PROMPT)->EnableWindow(TRUE);
		break;
	default:
		MessageBox(SEL_ERR, "Error", MB_OK | MB_ICONEXCLAMATION);
		break;
	}	
}

BOOL CCoordinateSystemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Create tooltip control
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip
		m_ToolTip.AddTool(GetDlgItem(IDC_IN_DMS), IDC_DMS);
		m_ToolTip.AddTool(GetDlgItem(IDC_IN_DMIN), IDC_DMIN);
		m_ToolTip.AddTool(GetDlgItem(IDC_IN_DDEG), IDC_DDEG);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUT_DMS), IDC_DMS);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUT_DMIN), IDC_DMIN);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUT_DDEG), IDC_DDEG);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_DATUM), IDC_OUTPUT_DATUM);
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_DATUM), IDC_INPUT_DATUM);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_PROJ), IDC_OUTPUT_PROJ);
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_PROJ), IDC_INPUT_PROJ);
			
		// activate ToolTip control
		m_ToolTip.Activate(TRUE);
	}	

	// clear projection list boxes
	((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->ResetContent();
	((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->AddString(GEO);
	((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->ResetContent();	
	((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->AddString(GEO);
	
	// Input Datum
	switch (m_coordinate_t.InputisGDA20)
	{
	case 0:
		((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->AddString(MGA94_STR);
		break;
	case 1:
		((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->AddString(MGA20_STR);
		break;
	default:
		return FALSE;
	}
	
	((CComboBox*)GetDlgItem(IDC_INPUT_DATUM))->SetCurSel(m_coordinate_t.InputisGDA20);
	
	// Input Coordinates projected?
	GetDlgItem(IDC_IN_DDEG)->EnableWindow(!m_coordinate_t.InputisProjected);
	GetDlgItem(IDC_IN_DMIN)->EnableWindow(!m_coordinate_t.InputisProjected);
	GetDlgItem(IDC_IN_DMS)->EnableWindow(!m_coordinate_t.InputisProjected);
	GetDlgItem(IDC_IN_FORMAT)->EnableWindow(!m_coordinate_t.InputisProjected);
	
	((CListBox*)GetDlgItem(IDC_INPUT_PROJ))->SetCurSel(m_coordinate_t.InputisProjected);

	// Output Datum
	switch (m_coordinate_t.OutputisGDA)
	{
	case 0:
		((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->AddString(MGA94_STR);
		break;
	case 1:
		((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->AddString(MGA20_STR);
		break;
	default:
		return FALSE;
	}
	
	((CComboBox*)GetDlgItem(IDC_OUTPUT_DATUM))->SetCurSel(m_coordinate_t.OutputisGDA);
	
	// Output Coordinates projected?
	GetDlgItem(IDC_OUT_DDEG)->EnableWindow(!m_coordinate_t.OutputisProjected);
	GetDlgItem(IDC_OUT_DMIN)->EnableWindow(!m_coordinate_t.OutputisProjected);
	GetDlgItem(IDC_OUT_DMS)->EnableWindow(!m_coordinate_t.OutputisProjected);
	GetDlgItem(IDC_OUT_FORMAT)->EnableWindow(!m_coordinate_t.OutputisProjected);
	GetDlgItem(IDC_OUTZONE_PROMPT)->EnableWindow(m_coordinate_t.OutputisProjected);
	
	((CListBox*)GetDlgItem(IDC_OUTPUT_PROJ))->SetCurSel(m_coordinate_t.OutputisProjected);
	
	UpdateData(FALSE);	
	
	return TRUE;
}

void CCoordinateSystemDlg::OnOK() 
{
	UpdateData(TRUE);

	CDialog::OnOK();
}

void CCoordinateSystemDlg::OnCancel() 
{
	UpdateData(FALSE);

	CDialog::OnCancel();
}

void CCoordinateSystemDlg::OnHelp() 
{
	::HtmlHelp(NULL, "gday.chm::/html/TC_choose_coord_sys.htm", HH_DISPLAY_TOC, 0);
}

BOOL CCoordinateSystemDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CCoordinateSystemDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/TC_choose_coord_sys.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;		
}
