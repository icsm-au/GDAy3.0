// GridExDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GridEx.h"
#include "GridExDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT ExtractGridFile(LPVOID pParam);
BOOL ExtractingGrid;
int gx_success;

/////////////////////////////////////////////////////////////////////////////
// CGridExDlg dialog

CGridExDlg::CGridExDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridExDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridExDlg)
	m_strLatPos = _T("Hello");
	m_strLongPos = _T("");
	m_strGridFilename = _T("");
	m_strNewGridFilename = _T("");
	m_strNewSubGridName = _T("");
	m_intNSEWLimits = 0;
	m_lfRadius = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGridExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridExDlg)
	DDX_Text(pDX, IDC_LAT_POS, m_strLatPos);
	DDX_Text(pDX, IDC_LONG_POS, m_strLongPos);
	DDX_Text(pDX, IDC_RADIUS, m_strRadius);
	DDX_Control(pDX, IDC_NEW_SUBGRID_NAME, m_ctlNewSubGridName);
	DDX_Text(pDX, IDC_PARENT_GRID_NAME, m_strGridFilename);
	DDX_Text(pDX, IDC_NEW_GRID_NAME, m_strNewGridFilename);
	DDX_CBString(pDX, IDC_NEW_SUBGRID_NAME, m_strNewSubGridName);
	DDX_Radio(pDX, IDC_NSEW_LIMITS, m_intNSEWLimits);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGridExDlg, CDialog)
	//{{AFX_MSG_MAP(CGridExDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_EXTRACT, OnExtract)
	ON_CBN_SELCHANGE(IDC_SUBGRID_NAME, OnSelchangeSubgridName)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_EN_KILLFOCUS(IDC_NORTH_LAT_NEW, OnKillfocusNorthLatNew)
	ON_EN_KILLFOCUS(IDC_EAST_LONG_NEW, OnKillfocusEastLongNew)
	ON_EN_KILLFOCUS(IDC_SOUTH_LAT_NEW, OnKillfocusSouthLatNew)
	ON_EN_KILLFOCUS(IDC_WEST_LONG_NEW, OnKillfocusWestLongNew)
	ON_EN_KILLFOCUS(IDC_LAT_POS, OnKillfocusLatPosition)
	ON_EN_KILLFOCUS(IDC_LONG_POS, OnKillfocusLongPosition)
	ON_EN_SETFOCUS(IDC_NORTH_LAT_NEW, OnSetfocusNorthLatNew)
	ON_EN_SETFOCUS(IDC_SOUTH_LAT_NEW, OnSetfocusSouthLatNew)
	ON_EN_SETFOCUS(IDC_WEST_LONG_NEW, OnSetfocusWestLongNew)
	ON_EN_SETFOCUS(IDC_EAST_LONG_NEW, OnSetfocusEastLongNew)
	ON_EN_SETFOCUS(IDC_LAT_POS, OnSetfocusLatPosition)
	ON_EN_SETFOCUS(IDC_LONG_POS, OnSetfocusLongPosition)
	ON_EN_KILLFOCUS(IDC_NEW_GRID_NAME, OnKillfocusNewGridName)
	ON_BN_CLICKED(ID_HELP_FINDER, OnHelpFinder)
	ON_CBN_SELCHANGE(IDC_NEW_SUBGRID_NAME, OnSelchangeNewSubgridName)
	ON_BN_CLICKED(IDC_PT_RADIUS, OnLimitSelection)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_NSEW_LIMITS, OnLimitSelection)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridExDlg message handlers

const UINT lpIDArray[] =
{
	IDS_TT_IDLEMESSAGE,
	IDS_TT_PROGRESS
};


BOOL CGridExDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CString strTemp = "";
	//CRegProc registry;

	SetIcon(m_hIcon, FALSE);		// Set small icon

	fpGridHeaderArray = NULL;		// grid header array for existing grid file
	fpNewGridHeaderArray = NULL;	// grid header array for new grid file (to be extracted)

	m_bCanFormat = true;

	OnLimitSelection();
	OnCreateToolTips();
	AddDlgStatusBar();
	GetDlgItem(IDC_RADIUS)->SetWindowText(_T(""));
	GotoDlgCtrl(GetDlgItem(IDC_SUBGRID_NAME));

	if (m_strGridFilename.IsEmpty())
	{
		// SF Removed attempt to access registry.
		// GridEx does not write to the regsitry
		// Only gets information to the regsitry if something like GDay has written there.
		// Removed this code and also (now) unnecessary includes for REgProc
		/*strTemp = registry.GetString(HKEY_LOCAL_MACHINE, FILE_LOCATIONS, DISTORTION_GRID);
		if (strTemp.IsEmpty())
			strTemp = registry.GetString(HKEY_CURRENT_USER, FILE_LOCATIONS, DISTORTION_GRID);
		if (!strTemp.IsEmpty())
			m_strGridFilename = strTemp;
		*/
		strTemp = "";
	}
	else
		strTemp = m_strGridFilename;
				
	if (strTemp.IsEmpty() || strTemp == NO_GRID_SPECIFIED)
		DisableControls();
	else
    {
		GetDlgItem(IDC_PARENT_GRID_NAME)->SetWindowText(strTemp);
		ValidateGridName();
	}

	return FALSE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGridExDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGridExDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGridExDlg::OnBrowse() 
{
	// Access the system registry to retrieve distortion grid key
	CString strGrid, strDefExt, strFolder, strbuf;
	char c_grid[_MAX_PATH];
	BOOL bFoundParent = FALSE;
	int filter_index, success;
	
	CFileProc file;

	GetDlgItem(IDC_PARENT_GRID_NAME)->GetWindowText(strGrid);	
	strFolder = file.GetFolder(strGrid);
	strDefExt = file.GetExt(strGrid);
	strcpy(c_grid, file.GetName(strGrid, NULL));
	filter_index = ((strDefExt == ASC) ? 2 : 1);	
	strGrid = file.FileOpenSave(TRUE, FALSE, strFolder, c_grid, SELECT_GRID_FILE, strDefExt, GRID_FILTER, filter_index);
	
	if (strGrid.IsEmpty())
		return;

	m_strGridFilename = strGrid;
	GetDlgItem(IDC_PARENT_GRID_NAME)->SetWindowText(strGrid);
	if ((success = ReadGridFileHeader((char*)((LPCTSTR)strGrid), (char*)((LPCTSTR)file.GetExt(strGrid)))) > 0)
	{
		MessageBox(gx.m_chError_code[success], "Grid file error", MB_OK | MB_ICONEXCLAMATION);
		m_strGridFilename = "";
		GetDlgItem(IDC_PARENT_GRID_NAME)->SetWindowText("");
		DisableControls();
		return;
	}

	UpdateGridInformation();
	GotoDlgCtrl(GetDlgItem(IDC_SUBGRID_NAME));
}

void CGridExDlg::OnSelchangeSubgridName()
{
	m_intParentIndex = ((CComboBox*)GetDlgItem(IDC_SUBGRID_NAME))->GetCurSel();
	OnUpdateGridLimits();
}

void CGridExDlg::OnUpdateGridLimits()
{
	CString strbuf;
	CStrProc str;
	
	strbuf.Format("%.4f", GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[m_intParentIndex].dNlat/3600));
	GetDlgItem(IDC_NORTH_LAT)->SetWindowText(str.FormatString(strbuf));	
	strbuf.Format("%.4f", GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[m_intParentIndex].dSlat/3600));
	GetDlgItem(IDC_SOUTH_LAT)->SetWindowText(str.FormatString(strbuf));
	strbuf.Format("%.4f", fabs(GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[m_intParentIndex].dElong/3600)));
	GetDlgItem(IDC_EAST_LONG)->SetWindowText(str.FormatString(strbuf));
	strbuf.Format("%.4f", fabs(GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[m_intParentIndex].dWlong/3600)));
	GetDlgItem(IDC_WEST_LONG)->SetWindowText(str.FormatString(strbuf));
	strbuf.Format("%.4f", GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[m_intParentIndex].dLatinc/3600));
	GetDlgItem(IDC_LAT_INT)->SetWindowText(str.FormatString(strbuf));
	strbuf.Format("%.4f", GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[m_intParentIndex].dLonginc/3600));
	GetDlgItem(IDC_LONG_INT)->SetWindowText(str.FormatString(strbuf));
}

void CGridExDlg::DisableControls()
{
	GetDlgItem(IDC_SUBGRID_NAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_FILETYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_FILETYPE)->SetWindowText(_T(""));
	
	GetDlgItem(IDC_NUMSUBGRIDS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EAST_LONG)->EnableWindow(FALSE);
	GetDlgItem(IDC_WEST_LONG)->EnableWindow(FALSE);
	GetDlgItem(IDC_NORTH_LAT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SOUTH_LAT)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_EAST_LONG)->SetWindowText(_T(""));
	GetDlgItem(IDC_WEST_LONG)->SetWindowText(_T(""));
	GetDlgItem(IDC_NORTH_LAT)->SetWindowText(_T(""));
	GetDlgItem(IDC_SOUTH_LAT)->SetWindowText(_T(""));
	
	GetDlgItem(IDC_VERSION)->EnableWindow(FALSE);
	GetDlgItem(IDC_VERSION)->SetWindowText(_T(""));
	
	GetDlgItem(IDC_GRID_LIMIT_BOX)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_FILETYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_VERSION)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_NOSG)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_SGNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_NLAT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_ELON)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_SLAT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_WLON)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_ST_LATINT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_LONGINT)->EnableWindow(FALSE);
	GetDlgItem(IDC_LAT_INT)->EnableWindow(FALSE);
	GetDlgItem(IDC_LONG_INT)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_LAT_INT)->SetWindowText(_T(""));
	GetDlgItem(IDC_LONG_INT)->SetWindowText(_T(""));
	
	// NSEW limit controls
	GetDlgItem(IDC_NEW_GRID_NAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_NEW_GRID_BOX)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVEAS)->EnableWindow(FALSE);
	GetDlgItem(IDC_NGRID_LIMITS)->EnableWindow(FALSE);
	GetDlgItem(IDC_NEW_SUBGRID_NAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_NORTH_LAT_NEW)->EnableWindow(FALSE);
	GetDlgItem(IDC_EAST_LONG_NEW)->EnableWindow(FALSE);
	GetDlgItem(IDC_SOUTH_LAT_NEW)->EnableWindow(FALSE);
	GetDlgItem(IDC_WEST_LONG_NEW)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_NEWFILENAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_NEWNLAT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_NEWELON)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_NEWSLAT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_NEWWLON)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_NEWSGNAME)->EnableWindow(FALSE);

	// location and radius controls
	GetDlgItem(IDC_NSEW_LIMITS)->EnableWindow(FALSE);
	GetDlgItem(IDC_PT_RADIUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_LATPOS)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_LONPOS)->EnableWindow(FALSE);
	GetDlgItem(IDC_LAT_POS)->EnableWindow(FALSE);
	GetDlgItem(IDC_LONG_POS)->EnableWindow(FALSE);
	GetDlgItem(IDC_ST_RADIUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIUS)->EnableWindow(FALSE);

	GetDlgItem(IDC_EXTRACT)->EnableWindow(FALSE);
	
	((CComboBox*)GetDlgItem(IDC_SUBGRID_NAME))->ResetContent();
}

void CGridExDlg::EnableControls()
{
	GetDlgItem(IDC_SUBGRID_NAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_FILETYPE)->EnableWindow(TRUE);
	GetDlgItem(IDC_NUMSUBGRIDS)->EnableWindow(TRUE);
	GetDlgItem(IDC_EAST_LONG)->EnableWindow(TRUE);
	GetDlgItem(IDC_WEST_LONG)->EnableWindow(TRUE);
	GetDlgItem(IDC_NORTH_LAT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SOUTH_LAT)->EnableWindow(TRUE);
	
	GetDlgItem(IDC_VERSION)->EnableWindow(TRUE);
	GetDlgItem(IDC_GRID_LIMIT_BOX)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_FILETYPE)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_VERSION)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_NOSG)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_SGNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_NLAT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_ELON)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_SLAT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_WLON)->EnableWindow(TRUE);
	
	GetDlgItem(IDC_ST_LATINT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_LONGINT)->EnableWindow(TRUE);
	GetDlgItem(IDC_LAT_INT)->EnableWindow(TRUE);
	GetDlgItem(IDC_LONG_INT)->EnableWindow(TRUE);
	
	GetDlgItem(IDC_NEW_GRID_NAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_NEW_GRID_BOX)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVEAS)->EnableWindow(TRUE);
	GetDlgItem(IDC_NGRID_LIMITS)->EnableWindow(TRUE);
	GetDlgItem(IDC_NEW_SUBGRID_NAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_NORTH_LAT_NEW)->EnableWindow(TRUE);
	GetDlgItem(IDC_EAST_LONG_NEW)->EnableWindow(TRUE);
	GetDlgItem(IDC_SOUTH_LAT_NEW)->EnableWindow(TRUE);
	GetDlgItem(IDC_WEST_LONG_NEW)->EnableWindow(TRUE);
	GetDlgItem(IDC_EXTRACT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_NEWFILENAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_NEWNLAT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_NEWELON)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_NEWSLAT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_NEWWLON)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_NEWSGNAME)->EnableWindow(TRUE);

	GetDlgItem(IDC_NSEW_LIMITS)->EnableWindow(TRUE);
	GetDlgItem(IDC_PT_RADIUS)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_LATPOS)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_LONPOS)->EnableWindow(TRUE);
	GetDlgItem(IDC_LAT_POS)->EnableWindow(TRUE);
	GetDlgItem(IDC_LONG_POS)->EnableWindow(TRUE);
	GetDlgItem(IDC_ST_RADIUS)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIUS)->EnableWindow(TRUE);
}

void CGridExDlg::UpdateLimitBoxes()
{
	// show (point and radius) / (NSEW limit) edit boxes
	GetDlgItem(IDC_ST_NEWNLAT)->ShowWindow(m_intNSEWLimits);
	GetDlgItem(IDC_ST_NEWELON)->ShowWindow(m_intNSEWLimits);
	GetDlgItem(IDC_ST_NEWSLAT)->ShowWindow(m_intNSEWLimits);
	GetDlgItem(IDC_ST_NEWWLON)->ShowWindow(m_intNSEWLimits);
	GetDlgItem(IDC_NORTH_LAT_NEW)->ShowWindow(m_intNSEWLimits);
	GetDlgItem(IDC_EAST_LONG_NEW)->ShowWindow(m_intNSEWLimits);
	GetDlgItem(IDC_SOUTH_LAT_NEW)->ShowWindow(m_intNSEWLimits);
	GetDlgItem(IDC_WEST_LONG_NEW)->ShowWindow(m_intNSEWLimits);
	GetDlgItem(IDC_ST_NEWSGNAME)->ShowWindow(m_intNSEWLimits);
	GetDlgItem(IDC_NEW_SUBGRID_NAME)->ShowWindow(m_intNSEWLimits);

	GetDlgItem(IDC_ST_LATPOS)->ShowWindow(!m_intNSEWLimits);
	GetDlgItem(IDC_ST_LONPOS)->ShowWindow(!m_intNSEWLimits);
	GetDlgItem(IDC_LAT_POS)->ShowWindow(!m_intNSEWLimits);
	GetDlgItem(IDC_LONG_POS)->ShowWindow(!m_intNSEWLimits);
	GetDlgItem(IDC_ST_RADIUS)->ShowWindow(!m_intNSEWLimits);
	GetDlgItem(IDC_RADIUS)->ShowWindow(!m_intNSEWLimits);
}

void CGridExDlg::ValidateGridName() 
{
	CString strExt;
	CFileProc file;
	strExt = file.GetExt(m_strGridFilename);
	
	if (m_strGridFilename.IsEmpty() || (ReadGridFileHeader((char*)((LPCTSTR)m_strGridFilename), (char*)((LPCTSTR)strExt)) > 0))
		DisableControls();
	else
		UpdateGridInformation();
}

void CGridExDlg::OnExtract() 
{
	int success, file_progress;
	long hFile;
	CString strbuf, strext;
	BOOL FileProgCreated;
	RECT rc;
	
	CFileProc file;

	// get new grid filename
	GetDlgItem(IDC_NEW_GRID_NAME)->GetWindowText(m_strNewGridFilename);	
	if (m_strGridFilename.IsEmpty() || m_strNewGridFilename.IsEmpty())
		return;

	// is it the same as source grid filename?
	if (m_strGridFilename.CompareNoCase(m_strNewGridFilename) == 0)
	{
		strbuf = gx.m_chError_code[11];
		MessageBox(strbuf, "New file name error", MB_OK | MB_ICONEXCLAMATION);
		GotoDlgCtrl(GetDlgItem(IDC_NEW_GRID_NAME));
		return;
	}

	// validate new grid limtis
	if (!ValidateLimits())
		return;
	
	// get the file type
	strext = file.GetExt(m_strGridFilename);
	if (strext.CompareNoCase(_T("")) == 0)
	{
		MessageBox("No file type specified", "Unknown file type", MB_OK | MB_ICONWARNING);
		return;
	}
	else
		strcpy(fpGridHeaderArray->filetype, strext);

	strcpy(fpGridHeaderArray->filename, m_strGridFilename);
	success = ReadGridFileHeader(fpGridHeaderArray->filename, fpGridHeaderArray->filetype);

	if (success > 0)
	{
		strbuf = gx.m_chError_code[success];		
		MessageBox(strbuf, "Grid file error", MB_OK | MB_ICONEXCLAMATION);		
		DisableControls();
		return;
	}	

	if (!m_bCanFormat)
		MessageBox(AUS_GSB_MSG, "Obsolete file format", MB_OK | MB_ICONEXCLAMATION);
	
	if (fpNewGridHeaderArray == NULL)
	{
		fpNewGridHeaderArray = new (file_par);
		memset(fpNewGridHeaderArray, 0, sizeof(file_par));
	}

	// get the file type
	strext = file.GetExt(m_strNewGridFilename);
	if (strext.CompareNoCase(_T("")) == 0)
	{
		strcpy(fpNewGridHeaderArray->filetype, "gsb");		// default is binary
		m_strNewGridFilename += ".gsb";
	}
	else
		strcpy(fpNewGridHeaderArray->filetype, strext);

	strcpy(fpNewGridHeaderArray->filename, m_strNewGridFilename);
	
	// check existence of log file name: set Write attribute for log file if it exists
	strbuf.Format("%s.log", m_strNewGridFilename);
    if (file.Exists(strbuf, &hFile))
		file.SetReadWriteAttribute(strbuf, false);
	
	BeginWaitCursor();

	m_statusBar.GetItemRect(1, &rc);
	CProgressCtrl *FileProgress = new CProgressCtrl;
	if (FileProgress->Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rc, &m_statusBar, 0))
	{
		FileProgCreated = TRUE;
		FileProgress->SetRange(0, 100);
	}

	m_statusBar.SetPaneStyle(1, SBPS_NORMAL);
	ExtractingGrid = TRUE;

	AfxBeginThread(ExtractGridFile, (void*)this, THREAD_PRIORITY_NORMAL);

	if (FileProgCreated)
	{
		while (ExtractingGrid)
		{
			Sleep(100);
			file_progress = gx.ReturnExtractProgress();
			FileProgress->SetPos(file_progress);
			strbuf.Format("Extracting %s...", gx.ReturnSubGridName());
			m_statusBar.SetPaneText(0, strbuf, TRUE);
			UpdateWindow();
		}
		delete FileProgress;
	}

	EndWaitCursor();

	// set ReadOnly attribute for log file
	file.SetReadWriteAttribute(gx.m_strLogFilePath, true);

	strbuf.LoadString(IDS_TT_IDLEMESSAGE);
	m_statusBar.SetPaneText(0, strbuf, TRUE);

	strbuf.Format("%s\nDo you wish to view the log file?\n", gx.m_chError_code[gx_success]);
	if (MessageBox(strbuf, "Grid extractor", MB_YESNO | MB_ICONINFORMATION) == IDYES)
		OnFileView();
}


void CGridExDlg::OnSaveas() 
{
	CString strGrid, strDefExt, strFolder, msg;
	char c_grid[_MAX_PATH];
	BOOL bFoundParent = FALSE;
	int filter_index;
	
	CFileProc file;
	
	GetDlgItem(IDC_NEW_GRID_NAME)->GetWindowText(strGrid);
	
	strFolder = file.GetFolder(strGrid);
	strDefExt = file.GetExt(strGrid);
	if (strDefExt.IsEmpty())
		strDefExt = "gsb";
	strcpy(c_grid, file.GetName(strGrid, NULL));
	filter_index = ((strDefExt == ASC) ? 2 : 1);
	
	strGrid = file.FileOpenSave(FALSE, FALSE, strFolder, c_grid, SAVE_NEW_GRID_FILE, strDefExt, GRID_FILTER, filter_index);
	
	if (strGrid.IsEmpty())
		return;

	if (m_strGridFilename == strGrid)
	{
		msg = "The new grid filename can not be the same as the original grid file.\n";
		msg += "\t     Please specify a new filename.";
		MessageBox(msg, "Filename conflict", MB_OK | MB_ICONEXCLAMATION);
		OnSaveas();
	}

	GetDlgItem(IDC_NEW_GRID_NAME)->SetWindowText(strGrid);
}

void CGridExDlg::OnKillfocusNorthLatNew() 
{
	CStrProc str;
	GetDlgItem(IDC_NORTH_LAT_NEW)->GetWindowText(m_strNewNorthLat);
	if (!m_strNewNorthLat.IsEmpty())
		GetDlgItem(IDC_NORTH_LAT_NEW)->SetWindowText(str.FormatString(m_strNewNorthLat));
}

void CGridExDlg::OnSetfocusNorthLatNew() 
{
	// copy original value back into input edit box variable
	GetDlgItem(IDC_NORTH_LAT_NEW)->SetWindowText(m_strNewNorthLat);
	((CEdit*)GetDlgItem(IDC_NORTH_LAT_NEW))->SetSel(0, -1, FALSE);
}

void CGridExDlg::OnKillfocusSouthLatNew() 
{
	CStrProc str;
	GetDlgItem(IDC_SOUTH_LAT_NEW)->GetWindowText(m_strNewSouthLat); 
	if (!m_strNewSouthLat.IsEmpty())
		GetDlgItem(IDC_SOUTH_LAT_NEW)->SetWindowText(str.FormatString(m_strNewSouthLat));
}

void CGridExDlg::OnSetfocusSouthLatNew() 
{
	// copy original value back into input edit box variable
	GetDlgItem(IDC_SOUTH_LAT_NEW)->SetWindowText(m_strNewSouthLat);
	((CEdit*)GetDlgItem(IDC_SOUTH_LAT_NEW))->SetSel(0, -1, FALSE);
}

void CGridExDlg::OnKillfocusWestLongNew() 
{
	CStrProc str;
	GetDlgItem(IDC_WEST_LONG_NEW)->GetWindowText(m_strNewWestLong); 
	if (!m_strNewWestLong.IsEmpty())
		GetDlgItem(IDC_WEST_LONG_NEW)->SetWindowText(str.FormatString(m_strNewWestLong));
}


void CGridExDlg::OnSetfocusWestLongNew() 
{
	if (m_lfNewEastLong > m_lfNewWestLong)
		return;

	// copy original value back into input edit box variable
	GetDlgItem(IDC_WEST_LONG_NEW)->SetWindowText(m_strNewWestLong);
	((CEdit*)GetDlgItem(IDC_WEST_LONG_NEW))->SetSel(0, -1, FALSE);
}

void CGridExDlg::OnKillfocusEastLongNew() 
{
	CStrProc str;
	GetDlgItem(IDC_EAST_LONG_NEW)->GetWindowText(m_strNewEastLong); 
	if (!m_strNewEastLong.IsEmpty())
		GetDlgItem(IDC_EAST_LONG_NEW)->SetWindowText(str.FormatString(m_strNewEastLong));
}

void CGridExDlg::OnSetfocusEastLongNew() 
{
	if (m_lfNewEastLong > m_lfNewWestLong)
		return;

	// copy original value back into input edit box variable
	GetDlgItem(IDC_EAST_LONG_NEW)->SetWindowText(m_strNewEastLong);
	((CEdit*)GetDlgItem(IDC_EAST_LONG_NEW))->SetSel(0, -1, FALSE);
}

void CGridExDlg::OnKillfocusLatPosition() 
{
	CStrProc str;
	GetDlgItem(IDC_LAT_POS)->GetWindowText(m_strLatPos); 
	if (!m_strLatPos.IsEmpty())
		GetDlgItem(IDC_LAT_POS)->SetWindowText(str.FormatString(m_strLatPos));
}

void CGridExDlg::OnSetfocusLatPosition() 
{
	// copy original value back into input edit box variable
	GetDlgItem(IDC_LAT_POS)->SetWindowText(m_strLatPos);
	((CEdit*)GetDlgItem(IDC_LAT_POS))->SetSel(0, -1, FALSE);
}

void CGridExDlg::OnKillfocusLongPosition() 
{
	CStrProc str;
	GetDlgItem(IDC_LONG_POS)->GetWindowText(m_strLongPos); 
	if (!m_strLongPos.IsEmpty())
		GetDlgItem(IDC_LONG_POS)->SetWindowText(str.FormatString(m_strLongPos));
}

void CGridExDlg::OnSetfocusLongPosition() 
{
	// copy original value back into input edit box variable
	GetDlgItem(IDC_LONG_POS)->SetWindowText(m_strLongPos);
	((CEdit*)GetDlgItem(IDC_LONG_POS))->SetSel(0, -1, FALSE);
}

void CGridExDlg::OnKillfocusNewGridName() 
{
	CString strExt;
	CFileProc file;

	GetDlgItem(IDC_NEW_GRID_NAME)->GetWindowText(m_strNewGridFilename);
	strExt = file.GetExt(m_strNewGridFilename);

	if (strExt.IsEmpty())
		return;

	// test for type of grid file, then assign appropriate access mode...
	if (strExt != ASC && strExt != GSB)
		MessageBox("The file extension you have entered is an\nunrecognised Distortion Grid filetype.\n\nThe file will be saved in Binary format.", "Unknown file type",
			MB_OK | MB_ICONWARNING);
}


int CGridExDlg::ReadGridFileHeader(char *cNewFile, char *cFiletype)
{
	int i, success;
	CString strbuf;
	
	if (fpGridHeaderArray == NULL)
	{
		fpGridHeaderArray = new (file_par);
		memset(fpGridHeaderArray, 0, sizeof(file_par));
	}

	// call ReportGridProperties to retrieve grid data
	GCALC_ReportGridProperties(cNewFile, cFiletype, fpGridHeaderArray, &success);

	// is the grid australian/canadian binary?
	if (success == -2)
		m_bCanFormat = false;
	else
		m_bCanFormat = true;

	m_dGridLatinc = -1;
	m_dGridLoninc = -1;

	// find largest 'parent' grid shift interval
	if (success < 1)
	{
		for (i=0; i<fpGridHeaderArray->iNumsubgrids; i++)
		{
			if (strcmp(NOPARENT, fpGridHeaderArray->ptrIndex[i].chParent) == 0)
			{
				if (m_dGridLatinc < fpGridHeaderArray->ptrIndex[i].dLatinc)
					m_dGridLatinc = fpGridHeaderArray->ptrIndex[i].dLatinc;
				
				if (m_dGridLoninc < fpGridHeaderArray->ptrIndex[i].dLonginc)
					m_dGridLoninc = fpGridHeaderArray->ptrIndex[i].dLonginc;
			}
		}

		// convert seconds values to decimal degrees
		m_dGridLatinc /= 3600;
		m_dGridLoninc /= 3600;
	}

	return success;
}

void CGridExDlg::UpdateGridInformation()
{
	CString strbuf, strExt;
	int i;	
	CFileProc file;

	m_intParentIndex = -1;
	
	// get parent grid extents and display
	for (i=0; i<fpGridHeaderArray->iNumsubgrids; i++)
	{
		if (strcmp(fpGridHeaderArray->ptrIndex[i].chParent, NOPARENT) == 0)
		{
			m_intParentIndex = i;
			i = fpGridHeaderArray->iNumsubgrids + 1;
		}
	}

	if (m_intParentIndex < 0)
	{
		strbuf = gx.m_chError_code[3];
		strbuf += "\nPlease select a new grid file.";
		
		MessageBox(strbuf, "Grid file error", MB_OK | MB_ICONEXCLAMATION);
		m_strGridFilename = "";
		DisableControls();
		return;
	}
	else
	{
		((CComboBox*)GetDlgItem(IDC_SUBGRID_NAME))->SetCurSel(m_intParentIndex);
		OnUpdateGridLimits();
	}

	strExt = file.GetExt(m_strGridFilename);	

	if (strExt.CompareNoCase("gsb") == 0 && !m_bCanFormat)
		GetDlgItem(IDC_FILETYPE)->SetWindowText("Binary (Australian)");
	else if (strExt.CompareNoCase("gsb") == 0 && m_bCanFormat)
		GetDlgItem(IDC_FILETYPE)->SetWindowText("Binary");
	else
		GetDlgItem(IDC_FILETYPE)->SetWindowText("ASCII");

	GetDlgItem(IDC_VERSION)->SetWindowText(fpGridHeaderArray->chVersion);
	strbuf.Format("%d", fpGridHeaderArray->iNumsubgrids);
	GetDlgItem(IDC_NUMSUBGRIDS)->SetWindowText(strbuf);
	
	((CComboBox*)GetDlgItem(IDC_SUBGRID_NAME))->ResetContent();
	m_ctlNewSubGridName.ResetContent();
	m_ctlNewSubGridName.AddString("");
	
	// fill combo box with sub names in same order as sub grids appear in the file
	for (i=0; i<fpGridHeaderArray->iNumsubgrids; i++)
	{	
		strbuf = fpGridHeaderArray->ptrIndex[i].chSubname;
		((CComboBox*)GetDlgItem(IDC_SUBGRID_NAME))->AddString(strbuf);
		m_ctlNewSubGridName.AddString(strbuf);
	}

	((CComboBox*)GetDlgItem(IDC_SUBGRID_NAME))->SetCurSel(m_intParentIndex);

	EnableControls();
}

void CGridExDlg::OnHelpFinder() 
{
	AfxGetApp()->WinHelp(0L, HELP_FINDER);	
}

void CGridExDlg::OnSelchangeNewSubgridName() 
{
	CStrProc str;
	int i = m_ctlNewSubGridName.GetCurSel();
	
	if (i == 0)
	{
		GetDlgItem(IDC_NORTH_LAT_NEW)->SetWindowText(_T(""));
		GetDlgItem(IDC_SOUTH_LAT_NEW)->SetWindowText(_T(""));
		GetDlgItem(IDC_EAST_LONG_NEW)->SetWindowText(_T(""));
		GetDlgItem(IDC_WEST_LONG_NEW)->SetWindowText(_T(""));
		return;
	}

	i --;

	m_lfNewNorthLat = fpGridHeaderArray->ptrIndex[i].dNlat/3600;
	m_lfNewSouthLat = fpGridHeaderArray->ptrIndex[i].dSlat/3600;
	m_lfNewEastLong = fpGridHeaderArray->ptrIndex[i].dElong/3600;
	m_lfNewWestLong = fpGridHeaderArray->ptrIndex[i].dWlong/3600;
	
	m_strNewNorthLat.Format("%.4f", GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[i].dNlat/3600));
	GetDlgItem(IDC_NORTH_LAT_NEW)->SetWindowText(str.FormatString(m_strNewNorthLat));	
	m_strNewSouthLat.Format("%.4f", GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[i].dSlat/3600));
	GetDlgItem(IDC_SOUTH_LAT_NEW)->SetWindowText(str.FormatString(m_strNewSouthLat));
	m_strNewEastLong.Format("%.4f", fabs(GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[i].dElong/3600)));
	GetDlgItem(IDC_EAST_LONG_NEW)->SetWindowText(str.FormatString(m_strNewEastLong));
	m_strNewWestLong.Format("%.4f", fabs(GCALC_DegtoDms(fpGridHeaderArray->ptrIndex[i].dWlong/3600)));
	GetDlgItem(IDC_WEST_LONG_NEW)->SetWindowText(str.FormatString(m_strNewWestLong));
}


BOOL CGridExDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CGridExDlg::OnCreateToolTips()
{
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip
		m_ToolTip.AddTool(GetDlgItem(IDC_PARENT_GRID_NAME), IDC_PARENT_GRID_NAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_FILETYPE), IDC_FILETYPE);
		m_ToolTip.AddTool(GetDlgItem(IDC_VERSION), IDC_VERSION);
		m_ToolTip.AddTool(GetDlgItem(IDC_NUMSUBGRIDS), IDC_NUMSUBGRIDS);
		m_ToolTip.AddTool(GetDlgItem(IDC_NEW_GRID_NAME), IDC_NEW_GRID_NAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_NORTH_LAT), IDC_NORTH_LAT);
		m_ToolTip.AddTool(GetDlgItem(IDC_SOUTH_LAT), IDC_SOUTH_LAT);
		m_ToolTip.AddTool(GetDlgItem(IDC_EAST_LONG), IDC_EAST_LONG);
		m_ToolTip.AddTool(GetDlgItem(IDC_WEST_LONG), IDC_WEST_LONG);
		m_ToolTip.AddTool(GetDlgItem(IDC_NORTH_LAT_NEW), IDC_NORTH_LAT_NEW);
		m_ToolTip.AddTool(GetDlgItem(IDC_SOUTH_LAT_NEW), IDC_SOUTH_LAT_NEW);
		m_ToolTip.AddTool(GetDlgItem(IDC_EAST_LONG_NEW), IDC_EAST_LONG_NEW);
		m_ToolTip.AddTool(GetDlgItem(IDC_WEST_LONG_NEW), IDC_WEST_LONG_NEW);
		m_ToolTip.AddTool(GetDlgItem(IDC_SUBGRID_NAME), IDC_SUBGRID_NAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_NEW_SUBGRID_NAME), IDC_NEW_SUBGRID_NAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_LAT_INT), IDC_LAT_INT);
		m_ToolTip.AddTool(GetDlgItem(IDC_LONG_INT), IDC_LONG_INT);
		m_ToolTip.AddTool(GetDlgItem(IDC_BROWSE), IDC_BROWSE);
		m_ToolTip.AddTool(GetDlgItem(IDC_SAVEAS), IDC_SAVEAS);
		m_ToolTip.AddTool(GetDlgItem(IDC_EXTRACT), IDC_EXTRACT);
		m_ToolTip.AddTool(GetDlgItem(IDC_NSEW_LIMITS), IDC_NSEW_LIMITS);
		m_ToolTip.AddTool(GetDlgItem(IDC_PT_RADIUS), IDC_PT_RADIUS);
		m_ToolTip.AddTool(GetDlgItem(IDC_LAT_POS), IDC_LAT_POS);
		m_ToolTip.AddTool(GetDlgItem(IDC_LONG_POS), IDC_LONG_POS);
		m_ToolTip.AddTool(GetDlgItem(IDC_RADIUS), IDC_RADIUS);

		// activate ToolTip control
		m_ToolTip.Activate(TRUE);
	}
}


BOOL CGridExDlg::AddDlgStatusBar()
{
	// Create status bar at the bottom of the dialog window
	if (m_statusBar.Create(this))
	{
		m_statusBar.SetIndicators(lpIDArray, sizeof(lpIDArray)/sizeof(UINT));
		
		// Make a sunken or recessed border around the first pane
		m_statusBar.SetPaneInfo(0, IDS_TT_IDLEMESSAGE, SBPS_STRETCH, NULL );
		m_statusBar.SetPaneInfo(1, IDS_TT_PROGRESS, SBPS_NOBORDERS, 130);
		m_statusBar.SetPaneStyle(0, SBPS_NOBORDERS | SBPS_STRETCH);

		// We need to resize the dialog to make room for control bars.
		// First, figure out how big the control bars are.
		CRect rcClientStart;
		CRect rcClientNow;
		GetClientRect(rcClientStart);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST,
					   0, reposQuery, rcClientNow);

		// Now move all the controls so they are in the same relative
		// position within the remaining client area as they would be
		// with no control bars.
		CPoint ptOffset(rcClientNow.left - rcClientStart.left,
						rcClientNow.top - rcClientStart.top);

		// Adjust the dialog window dimensions
		CRect rcWindow;
		GetWindowRect(rcWindow);
		rcWindow.right += rcClientStart.Width() - rcClientNow.Width();
		rcWindow.bottom += rcClientStart.Height() - rcClientNow.Height();
		MoveWindow(rcWindow, FALSE);

		// And position the control bars
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

		return true;
	}
	else
		return false;
}

UINT ExtractGridFile(LPVOID pParam)
{
	((CGridExDlg*)pParam)->gx.ExtractGrid(
		((CGridExDlg*)pParam)->m_lfNewEastLong, 
		((CGridExDlg*)pParam)->m_lfNewWestLong, 
		((CGridExDlg*)pParam)->m_lfNewSouthLat, 
		((CGridExDlg*)pParam)->m_lfNewNorthLat, 
		((CGridExDlg*)pParam)->fpGridHeaderArray, 
		((CGridExDlg*)pParam)->fpNewGridHeaderArray,
		&gx_success);

	ExtractingGrid = FALSE;
	return 0;
}

void CGridExDlg::OnLimitSelection() 
{
	m_intNSEWLimits = ((CButton*)GetDlgItem(IDC_NSEW_LIMITS))->GetCheck();
	UpdateLimitBoxes();
}

void CGridExDlg::OnFileView() 
{	
	CString logfile, viewer, msg;	
	CFileProc file;

	viewer = "notepad.exe";
	if (file.View(gx.m_strLogFilePath, viewer) == 0)
	{
		msg = "Could not load:  \"";
		msg += viewer;
		msg += "\"\n\nPlease check that the file has not been\nmoved or deleted.\n";
		MessageBox(msg, "Grid Extractor", MB_OK | MB_ICONWARNING);
	}
}

int CGridExDlg::ValidateLimits()
{
	CString strbuf;
	double dBuf;
	int i_ID = 0;

	// check for empty edit boxes
	if (m_intNSEWLimits)
	{
		if (m_strNewNorthLat.IsEmpty() || m_strNewSouthLat.IsEmpty() || m_strNewEastLong.IsEmpty() || m_strNewWestLong.IsEmpty())
			return false;

		// NSEW limits (force all values to be negative)
		m_lfNewEastLong = -1 * GCALC_DmstoDeg(fabs(atof(m_strNewEastLong)));		// m_str* obtained by lose focus
		m_lfNewNorthLat = -1 * GCALC_DmstoDeg(fabs(atof(m_strNewNorthLat)));		// m_str* obtained by lose focus
		m_lfNewSouthLat = -1 * GCALC_DmstoDeg(fabs(atof(m_strNewSouthLat)));		// m_str* obtained by lose focus
		m_lfNewWestLong = -1 * GCALC_DmstoDeg(fabs(atof(m_strNewWestLong)));		// m_str* obtained by lose focus
		strbuf = "\n\nEnter in a valid geographic coordinate value.  ";
	}
	else
	{
		GetDlgItem(IDC_RADIUS)->GetWindowText(m_strRadius);
		
		if (m_strLatPos.IsEmpty() || m_strLongPos.IsEmpty() || m_strRadius.IsEmpty())
			return false;

		// compute new latitude and longitude positions 
		// using vincenty's formula
		ComputeLimitsbyLR(0.0, &m_lfNewNorthLat, &dBuf);						// upper lat
		ComputeLimitsbyLR(90.0, &dBuf, &m_lfNewEastLong);						// east long
		ComputeLimitsbyLR(180.0, &m_lfNewSouthLat, &dBuf);						// south lat
		ComputeLimitsbyLR(270.0, &dBuf, &m_lfNewWestLong);						// west long
		strbuf = "Invalid location and radius.\n\nEither reduce the radius distance or specify\nlocation coordinates that are further within\nthe limits of the Map Grid of Australia.           ";
	}

	if (m_lfNewNorthLat < -90. || m_lfNewNorthLat > 0.)
	{
		if (m_intNSEWLimits)
		{
			strbuf.Insert(0, "Invalid north latitude limit.");
			i_ID = IDC_NORTH_LAT_NEW;
		}
		else
			i_ID = IDC_RADIUS;
	}
	else if (m_lfNewSouthLat < -90. || m_lfNewSouthLat > 0.)
	{
		if (m_intNSEWLimits)
		{
			strbuf.Insert(0, "Invalid south latitude limit.");
			i_ID = IDC_SOUTH_LAT_NEW;
		}
		else
			i_ID = IDC_RADIUS;
	}
	else if (m_lfNewEastLong < -180. || m_lfNewEastLong > 0.)
	{
		if (m_intNSEWLimits)
		{
			strbuf.Insert(0, "Invalid east longitude limit.");
			i_ID = IDC_EAST_LONG_NEW;
		}
		else
			i_ID = IDC_RADIUS;
	}
	else if (m_lfNewWestLong < -180. || m_lfNewWestLong > 0.)
	{
		if (m_intNSEWLimits)
		{
			strbuf.Insert(0, "Invalid west longitude limit.");
			i_ID = IDC_WEST_LONG_NEW;
		}
		else
			i_ID = IDC_RADIUS;
	}

	if (i_ID != 0)
	{
		MessageBox(strbuf, "Invalid Limits", MB_OK | MB_ICONWARNING);
		GotoDlgCtrl(GetDlgItem(i_ID));
		return false;
	}

	// validate data
	if (m_lfNewNorthLat < m_lfNewSouthLat)
	{
		strbuf = gx.m_chError_code[13];
		MessageBox(strbuf, "Grid limit error", MB_OK | MB_ICONEXCLAMATION);
		GotoDlgCtrl(GetDlgItem(IDC_NORTH_LAT_NEW));
		return false;
	}
	
	// validate data
	if (m_lfNewEastLong > m_lfNewWestLong)
	{
		strbuf = gx.m_chError_code[14];		
		MessageBox(strbuf, "Grid limit error", MB_OK | MB_ICONEXCLAMATION);		
		GotoDlgCtrl(GetDlgItem(IDC_WEST_LONG_NEW));
		return false;
	}
	
	return true;
}

void CGridExDlg::ComputeLimitsbyLR(double azimuth, double *lfLatPos2, double *lfLongPos2)
{
	double Sigma, TwoSigmam, deltaSigma, A, B;
	double TanU1, TanSigma1, SinAlpha, f, u2, b;
	double Lambda, C, Omega;
	int i;

	#define toRadians (pi / 180)			// Constant to convert DDEGREES to RADIANS

	// get radius data
	m_lfRadius = fabs(atof(m_strRadius));

	// calculate fundamentals
	f = (1.0 / 298.257222101);				// inverse flattening based on GDA semi major
	b = 6378137.0 * (1.0 - f);				// GDA semi minor
	TanU1 = (1.0 - f) * tan(-toRadians * GCALC_DmstoDeg(fabs(atof(m_strLatPos))));		// from lat pos in radians
	TanSigma1 = TanU1 / cos(azimuth * toRadians);
	SinAlpha = cos(atan(TanU1)) * sin(azimuth * toRadians);
	u2 = pow(cos(asin(SinAlpha)), 2.0) * (pow(6378137.0, 2.0) - pow(b, 2.0)) / pow(b, 2.0);
	A = 1.0 + (u2/16384.0) * (4096.0 + (u2 * (-768.0 + (u2 * (320.0 - (175.0 * u2))))));
	B = (u2/1024.0) * (256.0 + (u2 * (-128.0 + (u2 * (74.0 - (47.0 * u2))))));
	Sigma = m_lfRadius / (b * A);
	
	// iterate until no signigicant change in sigma
	for (i=0; i<4; i++)
	{
		TwoSigmam = (2.0 * atan(TanSigma1)) + Sigma;
		deltaSigma = B * sin(Sigma) * (cos(TwoSigmam) + (B / 4.0 * ((cos(Sigma) * (-1.0 + (2.0 * pow(cos(TwoSigmam), 2.0)))) - (B / 6.0 * cos(TwoSigmam) * (-3.0 + (4.0 * pow(sin(Sigma), 2.0))) * ((-3.0 + (4.0 * pow(cos(TwoSigmam), 2.0))))))));
		Sigma = (m_lfRadius / (b * A)) + deltaSigma;
	}
	
	// latitude of new position
	*lfLatPos2 = atan2(((sin(atan(TanU1)) * cos(Sigma)) + (cos(atan(TanU1)) * sin(Sigma) * cos(azimuth * toRadians))), ((1.0 - f) * pow((pow(SinAlpha, 2.0) + pow(((sin(atan(TanU1)) * sin(Sigma)) - (cos(atan(TanU1)) * cos(Sigma) * cos(azimuth * toRadians))), 2.0)), 0.5))) / toRadians;
	
	Lambda = atan2((sin(Sigma) * sin(azimuth * toRadians)), ((cos(atan(TanU1)) * cos(Sigma)) - (sin(atan(TanU1))*sin(Sigma)*cos(azimuth * toRadians))));
	C = (f / 16.0) * pow(cos(asin(SinAlpha)), 2.0) * (4.0 + (f * (4.0 - (3.0 * pow(cos(asin(SinAlpha)), 2.0)))));
	Omega = Lambda - ((1.0 - C) * f * SinAlpha * (Sigma + (C * sin(Sigma) * (cos(TwoSigmam) + (C * cos(Sigma) * (-1 + (2 * pow(cos(TwoSigmam), 2.0))))))));
	
	// longitude of new position
	*lfLongPos2 = ((-toRadians * GCALC_DmstoDeg(fabs(atof(m_strLongPos)))) - Omega) / toRadians;	

	// TRACE("Lat: %.12f,   Long: %.12f\n", *lfLatPos2, *lfLongPos2);
}

void CGridExDlg::OnDropFiles(HDROP hDropInfo) 
{
	TCHAR szFileName[_MAX_PATH];
	::DragQueryFile(hDropInfo, 0, szFileName, _MAX_PATH);
	::DragFinish(hDropInfo);
	GetDlgItem(IDC_PARENT_GRID_NAME)->SetWindowText(szFileName);
	m_strGridFilename = szFileName;
	ValidateGridName();
}
