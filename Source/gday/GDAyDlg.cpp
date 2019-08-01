// GDAyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"

// add any header files that define new classes for GDAyDlg
// in GDAyDlg.h
#include "GDAyDlg.h"

#include <afxpriv.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL ExportingGridFile;
int iSuccess;

UINT ExportAsciiGridFile(LPVOID pParam)
{	
	GCALC_ExportToAscii((char *)(LPCTSTR)((gridfile*)pParam)->strGridFile, GSB, (char *)(LPCTSTR)((gridfile*)pParam)->strNewGrid, &iSuccess);
	ExportingGridFile = FALSE;	
	return 0;
}

UINT ExportBinaryGridFile(LPVOID pParam)
{
	GCALC_ExportToBinary((char *)(LPCTSTR)((gridfile*)pParam)->strGridFile, ASC, (char *)(LPCTSTR)((gridfile*)pParam)->strNewGrid, &iSuccess);
	ExportingGridFile = FALSE;
	return 0;
}

const UINT lpIDArray[] =
{
	IDS_TT_IDLEMESSAGE,
	IDS_TT_GRIDFILE,
	IDS_TT_INTERACTIVEOUTPUT
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//BOOL CAboutDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
//{
//	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
//		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/EULA_license_agreement.htm", HH_DISPLAY_TOC, 0)!=NULL;
//	return true;
//}

/////////////////////////////////////////////////////////////////////////////
// CGDAyDlg dialog

CGDAyDlg::CGDAyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGDAyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGDAyDlg)
	m_strLatEastInput = _T("");
	m_strLongNorthInput = _T("");
	m_strZoneInput = _T("");
	m_bMenuSelected = FALSE;
	m_iMsgPaneWidth = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_GDAY_ICON);
	m_ToolTipCreated = false;
}

void CGDAyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGDAyDlg)
	DDX_Text(pDX, IDC_LAT_EAST, m_strLatEastInput);
	DDX_Text(pDX, IDC_LONG_NTH, m_strLongNorthInput);
	DDX_Text(pDX, IDC_ZONE, m_strZoneInput);
	DDV_MaxChars(pDX, m_strZoneInput, 10);
	DDX_Text(pDX, IDC_PT_NUMBER, m_lPointNumber);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGDAyDlg, CDialog)
	//{{AFX_MSG_MAP(CGDAyDlg)
	ON_BN_CLICKED(IDC_COMPUTE, OnCompute)
	ON_EN_KILLFOCUS(IDC_LAT_EAST, OnKillfocusLatEastInput)
	ON_EN_KILLFOCUS(IDC_LONG_NTH, OnKillfocusLongNthInput)
	ON_EN_KILLFOCUS(IDC_ZONE, OnKillfocusZone)
	ON_EN_SETFOCUS(IDC_LAT_EAST, OnSetfocusLatEastInput)
	ON_EN_SETFOCUS(IDC_LONG_NTH, OnSetfocusLongNthInput)
	ON_EN_SETFOCUS(IDC_ZONE, OnSetfocusZoneInput)
	ON_EN_SETFOCUS(IDC_PT_NUMBER, OnSetfocusPtNumber)
	ON_COMMAND(IDM_FILE_GRIDFILE, OnFileGridfile)
	ON_COMMAND(IDM_FILE_GRIDPROP, OnFileGridProperties)
	ON_COMMAND(IDM_FILE_GRIDEX, OnFileGridex)
	ON_COMMAND(IDM_FILE_EXPORTTOASCII, OnFileExportToAscii)
	ON_COMMAND(IDM_FILE_EXPORTTOBINARY, OnFileExportToBinary)
	ON_COMMAND(IDM_FILE_OUTPUT, OnFileNewOutput)
	ON_COMMAND(IDM_FILE_VIEW, OnFileView)
	ON_COMMAND(IDM_FILE_TRANS_POINTSFILE, OnFileTransPointsfile)
	ON_COMMAND(IDM_TOOLS_COORD_SYSTEM, OnToolsCoordSystem)
	ON_COMMAND(IDM_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(IDM_HELP_ABOUTGDAY, OnHelpAboutGday)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ENTERIDLE()
	ON_WM_INITMENUPOPUP()
	ON_WM_MENUSELECT()
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_FILE_TRANS_DIRECTORY, OnFileTransDirectory)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	ON_COMMAND(ID_HELP_SEARCH, OnHelpSearch)
	ON_WM_DESTROY()
	ON_COMMAND(IDM_TRANSFORM_GDA94TOGDA20, OnCompute)
	ON_COMMAND(IDM_TRANSFORM_GDA20TOGDA94, OnCompute)
	ON_COMMAND(IDM_LOGFILEVIEW, OnLogfileview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGDAyDlg message handlers

BOOL CGDAyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.
	SetIcon(m_hIcon, TRUE);			// Set big icon
	
	CString strBuffer;
	CGridProc grid;
	CFileProc file;
	CStrProc cs;
	char cHeader[501];
	
	m_nIDTracking     = 0;
	m_nIDLastMessage  = 0;

	// Edit log header information and print to file
	strcpy_s(cHeader, theUser.LogHeader);
	if (strlen(cHeader)<498)
		sprintf(cHeader + strlen(cHeader), "%s", "\n\n");
	GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "w", cHeader);

	// initialise starting values
	OnClearDialogData();
	
	m_lPointNumber = theUser.StartNumber;
	SetDlgItemInt(IDC_PT_NUMBER, m_lPointNumber, TRUE);

	GetDlgItem(IDC_LAT_EAST)->SetWindowText(cs.AddSymbology(m_strLatEastInput, theUser, 1));
	GetDlgItem(IDC_LONG_NTH)->SetWindowText(cs.AddSymbology(m_strLongNorthInput, theUser, 2));

	// statusbar must be created prior to OnFileGridfile call
	m_StatusBarCreated = AddDlgStatusBar();

	if (strcmp(theUser.GridFile, NO_GRID_SPECIFIED) == 0)
	{
		theUser.GridFileStatus = ERR_GRID_NOT_OPENED;
		if(!theUser.DoNotPromptforGridFile)
		{
			CGridDefaultSelectDlg dGridDefaultSelectDlg;
			if (dGridDefaultSelectDlg.DoModal() == IDYES)
			{
				OnFileGridfile();
				if (strcmp(theUser.GridFile, NO_GRID_SPECIFIED) == 0)			
					theUser.GridFileStatus = ERR_GRID_NOT_OPENED;		 // cancel was pressed
			}
			theUser.DoNotPromptforGridFile = dGridDefaultSelectDlg.m_intPrompt;
		}
	}
	else
	{
		theUser.GridFileStatus = grid.CheckGridStatus(theUser.GridFile);		
		switch (theUser.GridFileStatus)
		{
		case ERR_AUS_BINARY:
			MessageBox(AUS_GSB_MSG, "Obsolete File Format",
				MB_OK | MB_ICONEXCLAMATION);
			break;
		case ERR_GRIDFILE_READ:
		case ERR_GRIDFILE_TYPE:
		case ERR_GRID_CORRUPT:
			CString msg;
			CErrMsg err;
			err.strGridFile = theUser.GridFile;
			err.ReportError(theUser.GridFileStatus, (char*)((LPCTSTR)theUser.GridFile));
			msg.Format("%sPlease select a new transformation grid file.", err.ErrorString);
			MessageBox(msg, "Transformation Grid Error",
				MB_OK | MB_ICONEXCLAMATION);
			break;
		}
	}

	// update statusbar
	if (theUser.GridFileStatus != ERR_GRID_SUCCESS && theUser.GridFileStatus != ERR_AUS_BINARY)		// gridfilestatus failed
		OnSetStatusBarText(1, " ...");
	else
		OnSetStatusBarText(1, file.GetName(theUser.GridFile, NULL));

	// set pane 2: interactive output filename
	OnSetStatusBarText(2, file.GetName(theUser.OutputFile, NULL));
		
	// print grid and params to log
	grid.PrintGridFiletoLog(theUser);
	grid.PrintParamstoLog(theUser);

	UpdateDialogText();
	UpdateDialogOptions();
	UpdateMenuItems(NULL);

	// SF Test COde for GDA2020 Icon Position
	CRect rectLogo;
	CRect rectGroupOut;
	CWnd *pWnd = GetDlgItem(IDC_STATIC_LOGO);
	pWnd->GetWindowRect(&rectLogo);
	ScreenToClient(&rectLogo);
	pWnd = GetDlgItem(IDC_STATIC_GROUP_OUT);
	pWnd->GetWindowRect(&rectGroupOut);
	ScreenToClient(&rectGroupOut);

	rectLogo.top = 10;
	rectLogo.bottom = rectGroupOut.top - 10;
	rectLogo.right = rectGroupOut.right - 10;
	rectLogo.left = rectLogo.right - (rectLogo.Height() * 1.33);
	
	//rectLogo.left = rectGroupOut.right - rectLogo.Width() - 20;
	//rectLogo.top = rectGroupOut.top - rectLogo.Height() - 20;

	pWnd = GetDlgItem(IDC_STATIC_LOGO);
	pWnd->MoveWindow(&rectLogo, TRUE);
	



	




	m_strDefaultOutputFile = theUser.OutputFile;		// save default file
			if (theUser.OnStartNewOutput)						// If user wants new output file at start up...
		OnFileNewOutput();
	else
		PrintOutputHeader();
	return FALSE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGDAyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(rect);
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
HCURSOR CGDAyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGDAyDlg::OnHelpAboutGday() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CGDAyDlg::OnCompute() 
{
	CString msg, strBuf;
	int result = IDCANCEL;

	coordinate cCoordVar;
	CErrMsg err;

	err.strGridFile = theUser.GridFile;

	// store input variables into data struct
	cCoordVar.dNum1 = m_lfLatEastInput;
	if (theUser.LatisPosSouth && !theUser.InputisProjected && cCoordVar.dNum1 > 0.0)
		cCoordVar.dNum1 *= -1;
	cCoordVar.dNum2 = m_lfLongNorthInput;
	cCoordVar.dNum3 = m_lfZoneInput;
	cCoordVar.iNum4 = theUser.InputisGDA20;
	m_lPointNumber = GetDlgItemInt(IDC_PT_NUMBER, NULL, TRUE);
	
	if (theUser.InputisProjected)
		GCALC_GridtoGeo(&cCoordVar);
	else
	{
		// convert to decimal degrees 
		if (theUser.InputDMS == DMS)					// degrees minutes seconds
		{
			cCoordVar.dNum1 = GCALC_DmstoDeg(cCoordVar.dNum1);
			cCoordVar.dNum2 = GCALC_DmstoDeg(cCoordVar.dNum2);
		}
		else if (theUser.InputDMS == DMIN)				// degrees minutes
		{
			cCoordVar.dNum1 = GCALC_DmintoDeg(cCoordVar.dNum1);
			cCoordVar.dNum2 = GCALC_DmintoDeg(cCoordVar.dNum2);
		}
	}
	
	// check for valid Longitude input
	if (cCoordVar.dNum2 < -180.0 || cCoordVar.dNum2 > 180.0)
	{
		if (theUser.InputisProjected)
		{
			if (cCoordVar.dNum3 <= 0.0 || cCoordVar.dNum3 > 60.0)
			{
				// invalid input
				MessageBox("Please enter in a number between 1 and 60", "Invalid Zone...", MB_OK | MB_ICONQUESTION);
				GotoDlgCtrl(GetDlgItem(IDC_ZONE));
				return;
			}
			else
			{
				MessageBox("Please enter in a number between 150,000.0 and 1,000,000.0", "Invalid Easting...",	MB_OK | MB_ICONQUESTION);
				GotoDlgCtrl(GetDlgItem(IDC_LAT_EAST));
				return;
			}
		}
		else
		{
			MessageBox("Please enter in a number between -180 and 180", "Invalid Longitude", MB_OK | MB_ICONWARNING);
			GotoDlgCtrl(GetDlgItem(IDC_LONG_NTH));
			return;
		}
	}

	// must be in geographic...check for valid Latitude input
	if (cCoordVar.dNum1 < -90.0 || cCoordVar.dNum1 > 90.0)
	{
		if (theUser.InputisProjected)
		{
			MessageBox("Please enter in a number between 1,000,000 and 19,000,000.", "Invalid Northing...", MB_OK | MB_ICONQUESTION);		
			GotoDlgCtrl(GetDlgItem(IDC_LONG_NTH));
			return;
		}
		else
		{
			MessageBox("Please enter in a number between -90 and 90.", "Invalid Latitude", MB_OK | MB_ICONWARNING);
			GotoDlgCtrl(GetDlgItem(IDC_LAT_EAST));
			return;
		}
	}

	// set ellipsoidal height/UTM zone to zero
	cCoordVar.dNum3 = 0.0;
	cCoordVar.IO_Status = ERR_TRANS_SUCCESS;
	
	// datum transformation
	if (theUser.InputisGDA20 != theUser.OutputisGDA)
		GCALC_BiLinearTransformation(&cCoordVar);
	else
	{
		cCoordVar.dLatacc = NULL_ACCURACY;
		cCoordVar.dLongacc = NULL_ACCURACY;
	}

	if (cCoordVar.IO_Status > ERR_TRANS_SUCCESS ||		// bilineartransformation was called - and failed
		(theUser.GridFileStatus != ERR_GRID_SUCCESS && theUser.GridFileStatus != ERR_AUS_BINARY))		// gridfilestatus failed
	{
		// Print error message to log file
		if (cCoordVar.IO_Status > ERR_TRANS_SUCCESS)
		{
			if (theUser.InputisProjected)
				strBuf.Format("%.3f  %.3f %.0f\n", m_lfLatEastInput, m_lfLongNorthInput, m_lfZoneInput);
			else
				strBuf.Format("%.9f  %.9f\n", m_lfLatEastInput, m_lfLongNorthInput);

			err.ReportError(cCoordVar.IO_Status, (char*)((LPCTSTR)strBuf));
			GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "a", (char*)((LPCTSTR)err.ErrorString));
		}

		// Report error message to user
		if (cCoordVar.IO_Status > ERR_TRANS_SUCCESS)
			err.ReportError(cCoordVar.IO_Status, '\0');
		else if (theUser.GridFileStatus != ERR_GRID_SUCCESS && theUser.GridFileStatus != ERR_AUS_BINARY)
			err.ReportError(theUser.GridFileStatus, (char*)((LPCTSTR)theUser.GridFile));
 
		msg.Format("%sDo you wish to continue with a 7 Parameter Transformation only  \nusing the %s parameters?",
			err.ErrorString, theUser.TransParam);
		result = MessageBox(msg, "Compute", MB_OKCANCEL | MB_ICONQUESTION);
		
		if (result == IDOK)		// perform conformal transformation
		{
			CGridProc grid;
			double dParameters[8];
			grid.GetTransformationParameters(theUser.TransParam, dParameters);

			cCoordVar.iNum4 = theUser.InputisGDA20;
			GCALC_GeotoCart(&cCoordVar);
			GCALC_DatumTran(&cCoordVar, dParameters);
			cCoordVar.iNum4 = theUser.OutputisGDA;
			GCALC_CarttoGeo(&cCoordVar);
	
			// initialise success
			cCoordVar.IO_Status = ERR_TRANS_SUCCESS;
			cCoordVar.dLatacc = NULL_ACCURACY;
			cCoordVar.dLongacc = NULL_ACCURACY;

			GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "a", ">> 7-Parameter Transformation performed.\n\n");
		}
		else
		{
			GetDlgItem(IDC_LAT_EAST2)->SetWindowText(_T(""));
			GetDlgItem(IDC_LONG_NTH2)->SetWindowText(_T(""));
			GetDlgItem(IDC_PREC_LATEAST)->SetWindowText(_T(""));
			GetDlgItem(IDC_PREC_LONGNORTH)->SetWindowText(_T(""));
		}
	}

	// convert output
	if (cCoordVar.IO_Status == ERR_TRANS_SUCCESS)
	{
		// no errors
		m_lfLatEastOutput = cCoordVar.dNum1;
		m_lfLongNorthOutput = cCoordVar.dNum2;
		m_lfLatacc = cCoordVar.dLatacc;
		m_lfLongacc = cCoordVar.dLongacc;
		OnDisplayOutput(&cCoordVar);
	}

	// get station name
	if (theUser.AddStationName && !theUser.AutoNameAllStations)
	{
		CStationNameDlg	dStationNameDlg;
		if (dStationNameDlg.DoModal() == IDOK)
			theUser.PromptStationName = dStationNameDlg.m_strStationName;
		else
			theUser.PromptStationName = theUser.AutoStationName;
	}

	// print to interactive output file
	OnPrintInteractiveOutput(cCoordVar);
	
	// set focus to appropriate control
	if (!theUser.AutoIncrement && theUser.ShowPointNumbers)
		GotoDlgCtrl(GetDlgItem(IDC_PT_NUMBER));		// interactive point numbering 
	else
	{
		GotoDlgCtrl(GetDlgItem(IDC_LAT_EAST));		// auto point numbering 
		if (!theUser.ShowPointNumbers)
			GetDlgItem(IDC_PT_NUMBER)->SetWindowText(_T(""));
		else
		{
			m_lPointNumber ++;
			SetDlgItemInt(IDC_PT_NUMBER, m_lPointNumber, TRUE);
		}
	}
}

void CGDAyDlg::OnFileGridfile() 
{
	CFileProc file;
	CRegProc registry;
	CGridProc grid;
	bool bNewGrid = false;
	bool bNewParam = false;

	HKEY HKEY_InstallKey;

	OnInitGridFileSettings();
	if (m_dGridParametersDlg.DoModal() == IDOK)
	{
		if (theUser.GridFile != m_dGridParametersDlg.m_strGridFile)
		{
			// update global struct theUser
			theUser.GridFileStatus = m_dGridParametersDlg.m_intGridFileStatus;
			theUser.GridFile = m_dGridParametersDlg.m_strGridFile;
			theUser.GridType = file.GetExt(m_dGridParametersDlg.m_strGridFile);

			// update statusbar
			if (theUser.GridFileStatus != ERR_GRID_SUCCESS && theUser.GridFileStatus != ERR_AUS_BINARY)		// gridfilestatus failed
				OnSetStatusBarText(1, " ...");
			else
				OnSetStatusBarText(1, file.GetName(theUser.GridFile, NULL));

			// update dialog menu and status bar
			UpdateMenuItems(NULL);
			
			// save grid information to registry (this is only done for the benefit of Grid Extractor)
			if (registry.GetKeyHandle(REGKEY_GDAY30, THE_APP, &HKEY_InstallKey) == ERROR_SUCCESS)
			{
				registry.SaveString(HKEY_InstallKey, CUSTOM, GRIDTYPE, (char*)((LPCTSTR)theUser.GridType));
				registry.SaveString(HKEY_InstallKey, FILE_LOCATIONS, DISTORTION_GRID, (char*)((LPCTSTR)theUser.GridFile));
				RegCloseKey(HKEY_InstallKey);
			}

			// print change to log
			GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "a", "New grid file selected...\n");
			grid.PrintGridFiletoLog(theUser);
			bNewGrid = true;
		}

		if (theUser.TransParam != m_dGridParametersDlg.m_strTransParam)
		{
			theUser.TransParam = m_dGridParametersDlg.m_strTransParam;
			GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "a", "New transformation parameters selected...\n");
			grid.PrintParamstoLog(theUser);
			bNewParam = true;
		}
		
		if (bNewGrid || !bNewParam)
			GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "a", "\n");
	}
}

void CGDAyDlg::OnToolsCoordSystem() 
{
	int precision;
	int bNewOutputCoordinates = false, bNewInputDatum = false;

	CCoordinateSystemDlg dCoordinateSystemDlg;
	dCoordinateSystemDlg.m_coordinate_t = theUser;
	GotoDlgCtrl(GetDlgItem(IDC_COMPUTE));
	
	//Show the Coordinate System dialog and capture the results
	if (dCoordinateSystemDlg.DoModal() == IDOK)
	{
		CStrProc cs;

		if (theUser.InputisGDA20 != dCoordinateSystemDlg.m_coordinate_t.InputisGDA20)		// different input datum
		{
			bNewInputDatum = true;
			OnClearDialogData();
		}
		else if (
			(m_lfLatEastInput != 0.0 && m_lfLongNorthInput != 0.0 && (!theUser.InputisProjected || (m_lfZoneInput != 0.0 && theUser.InputisProjected))) &&			// valid data
			(theUser.InputisProjected == dCoordinateSystemDlg.m_coordinate_t.InputisProjected && theUser.InputDMS != dCoordinateSystemDlg.m_coordinate_t.InputDMS) || theUser.InputisProjected != dCoordinateSystemDlg.m_coordinate_t.InputisProjected) // different coordinate format
		{
			coordinate input_t;

			input_t.dNum1 = m_lfLatEastInput;
			input_t.dNum2 = m_lfLongNorthInput;
			input_t.dNum3 = m_lfZoneInput;
			input_t.iNum4 = theUser.InputisGDA20;
			
			// reduce coordinates to decimal degrees
			if (theUser.InputisProjected)
				GCALC_GridtoGeo(&input_t);
			else
			{		
				input_t.dNum1 = -1 * fabs(input_t.dNum1);
				if (theUser.InputDMS == DMS)
				{
					input_t.dNum1 = GCALC_DmstoDeg(input_t.dNum1);
					input_t.dNum2 = GCALC_DmstoDeg(input_t.dNum2);
				}
				else if (theUser.InputDMS == DMIN)
				{
					input_t.dNum1 = GCALC_DmintoDeg(input_t.dNum1);
					input_t.dNum2 = GCALC_DmintoDeg(input_t.dNum2);
				}
			}
			
			// invalid coordinates?
			if ((input_t.dNum2 < -180.0 || input_t.dNum2 > 180.0) ||
				(input_t.dNum1 < -90.0 || input_t.dNum1 > 90.0))
			{
				OnClearDialogData();
			}
			else
			{
				if (dCoordinateSystemDlg.m_coordinate_t.InputisProjected)
				{
					GCALC_GeotoGrid(&input_t);
					CString strBuf;
					strBuf.Format("%.0f", input_t.dNum3);
					GetDlgItem(IDC_ZONE)->SetWindowText(strBuf);
					precision = theUser.MetPrecision;

				}
				else
				{
					// new coordinate system parameters			
					if (dCoordinateSystemDlg.m_coordinate_t.InputDMS == DMS)
					{
						input_t.dNum1 = GCALC_DegtoDms(input_t.dNum1);
						input_t.dNum2 = GCALC_DegtoDms(input_t.dNum2);
					}
					else if (dCoordinateSystemDlg.m_coordinate_t.InputDMS == DMIN)
					{
						input_t.dNum1 = GCALC_DegtoDmin(input_t.dNum1);
						input_t.dNum2 = GCALC_DegtoDmin(input_t.dNum2);
					}

					precision = theUser.GeoPrecision + 4;
					if (dCoordinateSystemDlg.m_coordinate_t.InputDMS == DMIN)
						precision -= 2;
				}
				
				// copy converted values
				m_lfLatEastInput = input_t.dNum1;
				m_lfLongNorthInput = input_t.dNum2;
				m_lfZoneInput = input_t.dNum3;

				m_strLatEastInput.Format("%.*f", precision, input_t.dNum1);
				GetDlgItem(IDC_LAT_EAST)->SetWindowText(cs.AddSymbology(m_strLatEastInput, dCoordinateSystemDlg.m_coordinate_t, 1));
				
				m_strLongNorthInput.Format("%.*f", precision, input_t.dNum2);
				GetDlgItem(IDC_LONG_NTH)->SetWindowText(cs.AddSymbology(m_strLongNorthInput, dCoordinateSystemDlg.m_coordinate_t, 2));
			}						
		}		
		
		// new output coordinates?
		if (theUser.OutputisGDA != dCoordinateSystemDlg.m_coordinate_t.OutputisGDA ||
			theUser.OutputDMS != dCoordinateSystemDlg.m_coordinate_t.OutputDMS ||
			theUser.OutputisProjected != dCoordinateSystemDlg.m_coordinate_t.OutputisProjected)
			bNewOutputCoordinates = true;
		
		theUser = dCoordinateSystemDlg.m_coordinate_t;
			
		// convert output coordinates
		if (bNewOutputCoordinates && !bNewInputDatum)
			OnCompute();
		
		UpdateDialogText();
		UpdateMenuItems(NULL);
	} // OnOK
	
	if (theUser.ShowPointNumbers && !theUser.AutoIncrement)
		GotoDlgCtrl(GetDlgItem(IDC_PT_NUMBER));
	else
		GotoDlgCtrl(GetDlgItem(IDC_LAT_EAST));
}

void CGDAyDlg::OnToolsOptions() 
{
	int iStartNum;
	bool OptionsModified = false;
	CGridProc grid;
	CFileProc file;

	GotoDlgCtrl(GetDlgItem(IDC_COMPUTE));

	// Show the Options "Property" tab pages
	COptions OptionsSheet("Options", this, m_iLastOptionsPage);

	// Page 1
	OptionsSheet.m_dView.m_intAutoIncrement = theUser.AutoIncrement;
	OptionsSheet.m_dView.m_intAddSymbols = theUser.AddSymbols;
	OptionsSheet.m_dView.m_intLatisPosSouth = theUser.LatisPosSouth;
	OptionsSheet.m_dView.m_intMetresPrec = theUser.MetPrecision;
	OptionsSheet.m_dView.m_intSecondsPrec = theUser.GeoPrecision;
	OptionsSheet.m_dView.m_intAddPrefix = theUser.AddPrefix;
	OptionsSheet.m_dView.m_intPointNumbering = theUser.ShowPointNumbers;
	OptionsSheet.m_dView.m_strStartingPointNumber.Format("%d", theUser.StartNumber);

	// Page 2
	OptionsSheet.m_dInteractiveOutput.m_intAddStationName = theUser.AddStationName;
	OptionsSheet.m_dInteractiveOutput.m_intCreateOutputFile = theUser.OnStartNewOutput;
	OptionsSheet.m_dInteractiveOutput.m_intPromptStationName = theUser.AutoNameAllStations;
	OptionsSheet.m_dInteractiveOutput.m_strCustomName = theUser.AutoStationName;
	OptionsSheet.m_dInteractiveOutput.m_strInteractiveFilePath = m_strDefaultOutputFile;	// default

	// Page 3
	OptionsSheet.m_dLogFile.m_strHeaderInfo = theUser.LogHeader;
	OptionsSheet.m_dLogFile.m_strLogFilePath = theUser.LogFile;

	OptionsSheet.SetActivePage(m_iLastOptionsPage);
	
	OptionsSheet.DoModal();
	m_iLastOptionsPage = OptionsSheet.m_iLastPageIndex;

	// Page 1
	if (OptionsSheet.m_dView.m_P1HasChanged)
	{
		theUser.AddSymbols = OptionsSheet.m_dView.m_intAddSymbols;
		theUser.LatisPosSouth = OptionsSheet.m_dView.m_intLatisPosSouth;
		theUser.MetPrecision = OptionsSheet.m_dView.m_intMetresPrec;
		theUser.GeoPrecision = OptionsSheet.m_dView.m_intSecondsPrec;
		theUser.AddPrefix = OptionsSheet.m_dView.m_intAddPrefix;
		iStartNum = atoi(OptionsSheet.m_dView.m_strStartingPointNumber);

		if ((!theUser.ShowPointNumbers == OptionsSheet.m_dView.m_intPointNumbering && OptionsSheet.m_dView.m_intAutoIncrement) ||
			(OptionsSheet.m_dView.m_intPointNumbering && OptionsSheet.m_dView.m_intAutoIncrement && theUser.StartNumber != iStartNum) ||
			(OptionsSheet.m_dView.m_intPointNumbering && !theUser.AutoIncrement == OptionsSheet.m_dView.m_intAutoIncrement)
			)
		{
			m_lPointNumber = atoi(OptionsSheet.m_dView.m_strStartingPointNumber);
			SetDlgItemInt(IDC_PT_NUMBER, m_lPointNumber, TRUE);			
		}
		
		theUser.ShowPointNumbers = OptionsSheet.m_dView.m_intPointNumbering;
		theUser.AutoIncrement = OptionsSheet.m_dView.m_intAutoIncrement;
		theUser.StartNumber = iStartNum;
		OptionsModified = true;
	}

	// Page 2
	if (OptionsSheet.m_dInteractiveOutput.m_P2HasChanged)
	{
		theUser.AddStationName = OptionsSheet.m_dInteractiveOutput.m_intAddStationName;
		theUser.OnStartNewOutput = OptionsSheet.m_dInteractiveOutput.m_intCreateOutputFile;
		theUser.AutoNameAllStations = OptionsSheet.m_dInteractiveOutput.m_intPromptStationName;
		theUser.AutoStationName = OptionsSheet.m_dInteractiveOutput.m_strCustomName;
		if (theUser.OutputFile != OptionsSheet.m_dInteractiveOutput.m_strInteractiveFilePath)
			m_strDefaultOutputFile = OptionsSheet.m_dInteractiveOutput.m_strInteractiveFilePath;		// copy default
		OptionsModified = true;
	}

	// Page 3
	if (OptionsSheet.m_dLogFile.m_P3HasChanged)
	{
		if (theUser.LogFile != OptionsSheet.m_dLogFile.m_strLogFilePath)
		{
			theUser.LogFile = OptionsSheet.m_dLogFile.m_strLogFilePath;
			grid.PrintGridFiletoLog(theUser);
		}
		theUser.LogHeader = OptionsSheet.m_dLogFile.m_strHeaderInfo;
		OptionsModified = true;
	}

	if (OptionsModified)
		UpdateDialogOptions();
}

void CGDAyDlg::OnKillfocusLatEastInput() 
{
	CStrProc cs;
	
	GetDlgItem(IDC_LAT_EAST)->GetWindowText(m_strLatEastInput);


	m_strLatEastInput.TrimRight();
	m_strLatEastInput.TrimLeft();
	if (m_strLatEastInput.IsEmpty())
		m_strLatEastInput = "0.";
	
	m_lfLatEastInput = atof(m_strLatEastInput);		
	GetDlgItem(IDC_LAT_EAST)->SetWindowText(cs.AddSymbology(m_strLatEastInput, theUser, 1));
}

void CGDAyDlg::OnKillfocusLongNthInput() 
{
	CStrProc cs;

	GetDlgItem(IDC_LONG_NTH)->GetWindowText(m_strLongNorthInput);

	m_strLongNorthInput.TrimRight();
	m_strLongNorthInput.TrimLeft();
	if (m_strLongNorthInput.IsEmpty())
		m_strLongNorthInput = "0.";

	m_lfLongNorthInput = atof(m_strLongNorthInput);	
	GetDlgItem(IDC_LONG_NTH)->SetWindowText(cs.AddSymbology(m_strLongNorthInput, theUser, 2));
}

void CGDAyDlg::OnKillfocusZone() 
{
	GetDlgItem(IDC_ZONE)->GetWindowText(m_strZoneInput);

	m_strZoneInput.TrimRight();
	m_strZoneInput.TrimLeft();
	if (m_strZoneInput.IsEmpty())
		m_strZoneInput = "0";

	m_lfZoneInput = atof(m_strZoneInput);		
}

void CGDAyDlg::OnSetfocusLatEastInput() 
{
	GetDlgItem(IDC_LAT_EAST)->SetWindowText(m_strLatEastInput);
	((CEdit*)GetDlgItem(IDC_LAT_EAST))->SetSel(0, -1, FALSE);
}

void CGDAyDlg::OnSetfocusLongNthInput() 
{
	GetDlgItem(IDC_LONG_NTH)->SetWindowText(m_strLongNorthInput);
	((CEdit*)GetDlgItem(IDC_LONG_NTH))->SetSel(0, -1, FALSE);
}

void CGDAyDlg::OnSetfocusZoneInput() 
{
	((CEdit*)GetDlgItem(IDC_ZONE))->SetSel(0, -1, FALSE);
}

void CGDAyDlg::OnSetfocusPtNumber() 
{
	((CEdit*)GetDlgItem(IDC_PT_NUMBER))->SetSel(0, -1, FALSE);	
}

void CGDAyDlg::OnFileTransDirectory() 
{
	CString msg;
	int user_feedback = IDYES;

	m_dPointsDirDlg.m_pfile_t = theUser;	
	while ((user_feedback == IDYES) && (m_dPointsDirDlg.DoModal () == IDOK))
	{
		msg.Format("%d file", m_dPointsDirDlg.m_intFilesSuccess);
		if (m_dPointsDirDlg.m_intFilesSuccess != 1)
			msg += "s were";
		else
			msg += " was";

		msg += " transformed. \nSee the log file for details.\n\n";
		msg += "Do you wish to transform another folder?    ";
		
		user_feedback = MessageBox(msg, "Transform another folder?",
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 );

		theUser.InputisHP = m_dPointsDirDlg.m_pfile_t.InputisHP;
		theUser.OutputisHP = m_dPointsDirDlg.m_pfile_t.OutputisHP;
		theUser.AddPFileHeader = m_dPointsDirDlg.m_pfile_t.AddPFileHeader;
	}
}

void CGDAyDlg::OnFileTransPointsfile() 
{
	int user_feedback = IDYES;
	CString msg, title;

	m_dPointsFileDlg.m_pfile_t = theUser;
	
	//Show the Points File Transformation dialog box
	while ((user_feedback == IDYES) && (m_dPointsFileDlg.DoModal () != IDCANCEL))
	{					
		if (m_dPointsFileDlg.m_bMultiFiles)
		{
			msg.Format("%d", m_dPointsFileDlg.m_intFilesTransformed);
			if (m_dPointsFileDlg.m_intFilesTransformed != 1)
				msg += " files were successfully transformed. ";
			else
				msg += " file was successfully transformed. ";
			msg += "\nSee the log file for details.\n\n";
			msg += "Do you wish to transform another points file?";
			title = "Transform another points file?";
		}
		else
		{
			if (m_dPointsFileDlg.m_lNumFail > 0 && m_dPointsFileDlg.m_lNumSuccess != 0)
				msg.Format("%d points were transformed. %d points were not.\nSee the log file for details.\n\n", 
					m_dPointsFileDlg.m_lNumSuccess, m_dPointsFileDlg.m_lNumFail);
			else if (m_dPointsFileDlg.m_lNumInterpolated != m_dPointsFileDlg.m_lNumSuccess)
				msg.Format("%d points were transformed. %d points were transformed  \nusing the transformation grid. See the log file for details.\n\n", 
					m_dPointsFileDlg.m_lNumSuccess, m_dPointsFileDlg.m_lNumInterpolated);
			else if (m_dPointsFileDlg.m_lNumSuccess < 1)
				msg.Format("%d points were transformed. \nSee the log file for details.\n\n", 
					m_dPointsFileDlg.m_lNumSuccess);
			else
				msg.Format("%d points were transformed.\n\n", m_dPointsFileDlg.m_lNumSuccess);

			// test for success of file transformation
			if (m_dPointsFileDlg.m_lNumSuccess < 1)
			{
				msg += "Do you wish to try a different points file?";
				title = "Try a different points file?";
			}
			else
			{
				msg += "Do you wish to transform another points file?";
				title = "Transform another points file?";
			}
		}		
		// On completion, prompt the user for another run
		user_feedback = MessageBox(msg,	title, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 );

		theUser.InputisHP = m_dPointsFileDlg.m_pfile_t.InputisHP;
		theUser.OutputisHP = m_dPointsFileDlg.m_pfile_t.OutputisHP;
		theUser.AddPFileHeader = m_dPointsFileDlg.m_pfile_t.AddPFileHeader;
	}
}

void CGDAyDlg::OnFileNewOutput() 
{
	CString strFolder, strTemp;
	char c_newfile[_MAX_PATH];
	
	CFileProc file;
	
	strFolder = file.GetFolder(theUser.OutputFile);
	strcpy_s(c_newfile, file.GetName(theUser.OutputFile, NULL));

	//Show the File Open dialog and Capture the result
	strTemp = file.FileOpenSave(FALSE, FALSE, strFolder, c_newfile, SELECT_OUTFILE, OUTPUT, OUT_FILTER, 0);
		
	//Update the dialog
	if (!strTemp.IsEmpty())
	{
		theUser.OutputFile = strTemp;
		PrintOutputHeader();
		OnSetStatusBarText(2, file.GetName(theUser.OutputFile, NULL));
	}
}

void CGDAyDlg::OnFileView() 
{	
	long hFile;
	CFileProc file;

	// create new log file if not already saved
	if (!file.Exists(theUser.OutputFile, &hFile))
		PrintOutputHeader();

	if (file.View(theUser.OutputFile, "notepad.exe") == 0)
		MessageBox("Could not load \"notepad.exe\"\n\nPlease check that the file has not been\nmoved or deleted.\n",
			"GDAy", MB_OK | MB_ICONWARNING);
}

void CGDAyDlg::PrintOutputHeader()
{
	CString strBuffer;
	
	// Print to interactive output file (use log file function)
	strBuffer = "///////////            GDAy Interactive Output File             ////////////////\n\n";
	strBuffer += "Created:  << Date >> \n\n";
	strBuffer += "Version 3.0\n";
	strBuffer += "////////////////////////////////////////////////////////////////////////////////\n\n";
	GCALC_PrintLogFile((char*)((LPCTSTR)theUser.OutputFile), "w", (char*)((LPCTSTR)strBuffer));

	// Print record (sub)header
	strBuffer = "\nPoint No, Input Lat/Easting, Long/Northing, Datum, Zone, Output, Lat/Easting, Long/Northing, Datum, Zone, Station Name, Accuracy (East), Accuracy (North)\n";
	GCALC_PrintLogFile((char*)((LPCTSTR)theUser.OutputFile), "a", (char*)((LPCTSTR)strBuffer));
}

void CGDAyDlg::OnFileExportToAscii() 
{
	OnFileExportGridFile(ASC);
}

void CGDAyDlg::OnFileExportToBinary() 
{
	OnFileExportGridFile(GSB);
}


void CGDAyDlg::OnFileGridProperties() 
{
	CGridProc grid;
	CErrMsg err;
	CGridPropertiesDlg dGridPropertiesDlg;
	theUser.GridFileStatus = grid.CheckGridStatus(theUser.GridFile);	
	switch (theUser.GridFileStatus)
	{
	case ERR_AUS_BINARY:
	case ERR_GRID_SUCCESS:
		dGridPropertiesDlg.m_strGridFile = theUser.GridFile;
		dGridPropertiesDlg.DoModal();
		break;
	default:
		err.ReportError(theUser.GridFileStatus, '\0');
		MessageBox(err.ErrorString, "Grid Properties", MB_OK | MB_ICONEXCLAMATION);
		break;
	}
}

// Relay events to the tooltip control	
BOOL CGDAyDlg::PreTranslateMessage(MSG* pMsg) 
{	
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

// run grid extractor
void CGDAyDlg::OnFileGridex() 
{
	char pszGridex[16] = {"GridEx.exe\0"};
	char pszAppPath[MAX_PATH+1] = {"GridEx.exe\0"};
	long hFile;
	CFileProc file;
	CGridProc grid;

	// Ensure file exists in current folder
	/*
	if (!file.Exists(pszGridex, &hFile))
	{
		if (!grid.GetGridExAppPath(pszAppPath))      // locate through registry
			return;
		strcat(pszAppPath, pszGridex);
	}
	*/

	// Use association to install on first use
	if (theUser.GridFile == "")
	{
		MessageBox("No Grid File selected. Unable to open Grid Extractor\n",
			"GridEx.exe", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		ShellExecute(NULL, "open", theUser.GridFile, NULL, NULL, SW_SHOWNORMAL);
	}

	/*
	if (file.View(theUser.OutputFile, theUser.GridFile) == 0)
		MessageBox("Could not load Grid Extractor.\n\nPlease check that the file has not been\nmoved or deleted.\n",
			"GridEx.exe", MB_OK | MB_ICONWARNING);
	*/
}

/////////////////////////////////////////////////////////////////////////////
// CGDAyDlg::OnEnterIdle
//      OnEnterIdle updates the status bar when there's nothing better to do.
//      This code is based on CFrameWnd::OnEnterIdle.
void CGDAyDlg::OnEnterIdle(UINT nWhy, CWnd* pWho)
{
	if (nWhy != MSGF_MENU || m_nIDTracking == m_nIDLastMessage)
		return;

	OnSetMessageString(m_nIDTracking);
	ASSERT(m_nIDTracking == m_nIDLastMessage);
}

void CGDAyDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	if (m_iMsgPaneWidth == -1 && m_bMenuSelected)
	{
		m_statusBar.GetPaneInfo(0, m_nMsgPaneID, m_nMsgPaneStyle, m_iMsgPaneWidth);
		CRect rc;
		m_statusBar.GetWindowRect(rc);
		m_statusBar.SetPaneInfo(0, m_nMsgPaneID, SBPS_NOBORDERS|SBPS_STRETCH, rc.Width());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGDAyDlg::OnMenuSelect
//      OnMenuSelect updates the status bar message, based on the state
//      of the dialog menu.
//      This code is based on CFrameWnd::OnMenuSelect.  We assume the
//      application does not support context sensitive help.
void CGDAyDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	if (nFlags == 0xFFFF && hSysMenu == 0 && m_iMsgPaneWidth != -1)
	{
		m_bMenuSelected = FALSE;
		m_statusBar.SetPaneInfo(0, m_nMsgPaneID, m_nMsgPaneStyle, m_iMsgPaneWidth);
		m_iMsgPaneWidth = -1;
	}
	else
		m_bMenuSelected = TRUE;

	// set the tracking state
	if (nFlags == 0xFFFF)
	{
		// cancel menu operation (go back to idle now)
		m_nIDTracking = IDS_TT_IDLEMESSAGE;
		OnSetMessageString(m_nIDTracking);  // set string now
		ASSERT(m_nIDTracking == m_nIDLastMessage);
	}
	else if (nItemID == 0 || nFlags & (MF_SEPARATOR|MF_POPUP|MF_MENUBREAK|MF_MENUBARBREAK))
	{
		// nothing should be displayed
		m_nIDTracking = 0;
		OnSetMessageString(IDS_TT_NULL);  // set string now
	}
	else if (nItemID >= 0xF000 && nItemID < 0xF1F0)
	{
		// special string table entries for system commands
		m_nIDTracking = ID_COMMAND_FROM_SC(nItemID);
		ASSERT(m_nIDTracking >= AFX_IDS_SCFIRST && m_nIDTracking < AFX_IDS_SCFIRST + 31);
	}
	else
	{
		// track on idle
		m_nIDTracking = nItemID;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGDAyDlg::OnSetMessageString
//      OnSetMessageString updates the status bar text.
//
//      This code is based on CFrameWnd::OnSetMessageString.  We assume
//      a string ID is always passed in wParam.
LRESULT CGDAyDlg::OnSetMessageString(WPARAM wParam, LPARAM lParam/*, int iPaneIndex*/)
{
	UINT    nIDMsg = (UINT)wParam;
	CString strMsg;

	if (nIDMsg)
	{
		if (strMsg.LoadString(nIDMsg) != 0)
			//m_statusBar.SetPaneText(iPaneIndex, strMsg, TRUE);
			m_statusBar.SetPaneText((int)lParam, strMsg, TRUE);
	}

	UINT nIDLast     = m_nIDLastMessage;
	m_nIDLastMessage = nIDMsg;
	m_nIDTracking    = nIDMsg;
	return nIDLast;
}

void CGDAyDlg::OnSetStatusBarText(int pane_index, CString strText)
{
	if (m_StatusBarCreated)
	{
		CString ToolTxt = strText;
		CRect rc;
		m_statusBar.GetItemRect(pane_index, rc);		
		CDC *pDC = m_statusBar.GetDC();
		CFont *pFont = GetFont();
		CFont *pOldFont = pDC->SelectObject(pFont);
		CSize size = pDC->GetTextExtent(strText);
		size.cx += 10;
		if (size.cx > rc.Width())
		{
			CString strEllipsis(_T("..."));
			CSize ellipsisSize = pDC->GetTextExtent(strEllipsis);
			int iMaxWidth = rc.Width() - ellipsisSize.cx - 3;
			while (size.cx > iMaxWidth)
			{
				strText = strText.Left(strText.GetLength()-1);
				size = pDC->GetTextExtent(strText);
				size.cx += 10;
			}
			// Tooltips will not be displayed if all the text can be seen in the specified pane
			// ...Make string longer than is necessary
			strText = ToolTxt.Mid(0, strText.GetLength()) + " " + strEllipsis + "         ";
			// set tooltip for long text
			m_statusBar.GetStatusBarCtrl().SetTipText(pane_index, ToolTxt);
		}		
		m_statusBar.ReleaseDC(pDC);
		pDC->SelectObject(pOldFont);
		m_statusBar.SetPaneText(pane_index, strText, TRUE);
	}
}

void CGDAyDlg::OnFileExportGridFile(CString strExt) 
{
	CString strFolder, msg;
	char c_newgrid[_MAX_PATH];
	int file_progress;

	ExportingGridFile = TRUE;	
	CFileProc file;	
	strFolder = file.GetFolder(theUser.GridFile);
	gridfile export_g;
		
	export_g.strGridFile = theUser.GridFile;

	if (strExt.CompareNoCase(GSB) == 0)
	{
		strcpy_s(c_newgrid, file.GetName(theUser.GridFile, GSB));
		export_g.strNewGrid = file.FileOpenSave(FALSE, FALSE, strFolder, c_newgrid, EXPORT_GRIDFILE, GSB, GSB_FILTER, 1);
	}
	else
	{
		strcpy_s(c_newgrid, file.GetName(theUser.GridFile, ASC));
		export_g.strNewGrid = file.FileOpenSave(FALSE, FALSE, strFolder, c_newgrid, EXPORT_GRIDFILE, ASC, ASC_FILTER, 1);
	}	

	if (export_g.strNewGrid != "")
	{
		// display the hourglass cursor
		BeginWaitCursor();

		if (strExt.CompareNoCase(GSB) == 0)
			AfxBeginThread(ExportBinaryGridFile, (void*)&export_g, THREAD_PRIORITY_NORMAL);
		else
			AfxBeginThread(ExportAsciiGridFile, (void*)&export_g, THREAD_PRIORITY_NORMAL);
		
		RECT rc;
		m_statusBar.GetItemRect(1, &rc);

		msg.Format("Exporting to:  ...\\%s", file.GetName(export_g.strNewGrid, NULL));
		OnSetStatusBarText(0, msg);

		CProgressCtrl *ExportProgress = new CProgressCtrl;
		
		msg.Format("Exporting  '%s'  to:  '%s'...\n", theUser.GridFile, export_g.strNewGrid);
		GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "a", (char*)((LPCTSTR)msg));

		if (ExportProgress->Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rc, &m_statusBar, 1))
		{
			ExportProgress->SetRange(0, 99);

			while (ExportingGridFile)
			{
				Sleep(500);
				if ((file_progress = GCALC_ReturnFileProgress()) > 0)
					ExportProgress->SetPos(file_progress);
				UpdateWindow();
			}

			delete ExportProgress;
		}

		msg.LoadString(IDS_TT_IDLEMESSAGE);
		OnSetStatusBarText(0, msg);

		// restore the default (arrow) cursor
		EndWaitCursor();

		if (iSuccess == ERR_GRID_SUCCESS)
		{
			msg.Format("%s\n\n was successfully exported to\n\n%s",
				theUser.GridFile, export_g.strNewGrid);
			MessageBox(msg, "Export success", MB_OK | MB_ICONINFORMATION);
			msg = "The grid file was successfully exported.\n\n";
		}
		else
		{
			CErrMsg err;

			err.ReportError(iSuccess, '\0');
			msg.Format("** ERROR: %sExport aborted.\n\n", err.ErrorString);
			MessageBox(msg, "Export failure", MB_OK | MB_ICONEXCLAMATION);
		}

		GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "a", (char*)((LPCTSTR)msg));
	}
}


void CGDAyDlg::UpdateDialogText()
{
	CString str_val;
	
	// Create tooltip control
	if (!m_ToolTipCreated)
	{
		if (m_ToolTip.Create(this))
		{
			// add the dialog's controls to the Tooltip
			m_ToolTip.AddTool(GetDlgItem(IDC_COMPUTE), IDC_COMPUTE);
			m_ToolTip.AddTool(GetDlgItem(IDC_PT_NUMBER), IDC_PT_NUMBER);

			// activate ToolTip control
			m_ToolTip.Activate(TRUE);
			m_ToolTipCreated = true;
		}
	}

	// Input Datum
	if (theUser.InputisGDA20)
		GetDlgItem(IDC_IN_DATUM_DESC)->SetWindowText(GDA20_STR);
	else
		GetDlgItem(IDC_IN_DATUM_DESC)->SetWindowText(GDA94_STR);

	// Input Zone
	GetDlgItem(IDC_INPUT_VAR3)->SetWindowText(_T("Zone: "));

	// Input Projection
	if (theUser.InputisProjected) 
	{
		if (theUser.InputisGDA20)
			GetDlgItem(IDC_IN_PROJ_DESC)->SetWindowText(MGA20_STR);
		else
			GetDlgItem(IDC_IN_PROJ_DESC)->SetWindowText(MGA94_STR);

		GetDlgItem(IDC_INPUT_VAR1)->SetWindowText(E);
		GetDlgItem(IDC_INPUT_VAR2)->SetWindowText(N);
		
		if (m_ToolTipCreated)
		{
			m_ToolTip.AddTool(GetDlgItem(IDC_LAT_EAST), IDC_EAST);
			m_ToolTip.AddTool(GetDlgItem(IDC_LONG_NTH), IDC_NORTH);
			m_ToolTip.AddTool(GetDlgItem(IDC_ZONE), IDC_ZONE);
		}
	}
	else 
	{
		if (theUser.InputDMS == DMS)
			str_val.LoadString(IDC_DMS);
		else if (theUser.InputDMS == DMIN)
			str_val.LoadString(IDC_DMIN);
		else
			str_val.LoadString(IDC_DDEG);
		
		str_val.Insert(0, GEO);

		GetDlgItem(IDC_IN_PROJ_DESC)->SetWindowText(str_val);
				
		GetDlgItem(IDC_INPUT_VAR1)->SetWindowText(LA);
		GetDlgItem(IDC_INPUT_VAR2)->SetWindowText(LO);

		if (m_ToolTipCreated)
		{
			m_ToolTip.AddTool(GetDlgItem(IDC_LAT_EAST), IDC_LAT);
			m_ToolTip.AddTool(GetDlgItem(IDC_LONG_NTH), IDC_LONG);
		}		
	}

	GetDlgItem(IDC_ZONE)->ShowWindow(theUser.InputisProjected);
	GetDlgItem(IDC_INPUT_VAR3)->ShowWindow(theUser.InputisProjected);	

	// Output Datum
	if (theUser.OutputisGDA)
		GetDlgItem(IDC_OUT_DATUM_DESC)->SetWindowText(GDA20_STR);
	else
		GetDlgItem(IDC_OUT_DATUM_DESC)->SetWindowText(GDA94_STR);

	// Output Zone
	GetDlgItem(IDC_OUTPUT_VAR3)->SetWindowText(_T("Zone: "));

	// Output Projection
	if (theUser.OutputisProjected) 
	{
		if (theUser.OutputisGDA)
			GetDlgItem(IDC_OUT_PROJ_DESC)->SetWindowText(MGA20_STR);
		else
			GetDlgItem(IDC_OUT_PROJ_DESC)->SetWindowText(MGA94_STR);

		GetDlgItem(IDC_OUTPUT_VAR1)->SetWindowText(E);
		GetDlgItem(IDC_OUTPUT_VAR2)->SetWindowText(N);

		if (m_ToolTipCreated)
		{
			m_ToolTip.AddTool(GetDlgItem(IDC_LAT_EAST2), IDC_EAST2);
			m_ToolTip.AddTool(GetDlgItem(IDC_LONG_NTH2), IDC_NORTH2);
			m_ToolTip.AddTool(GetDlgItem(IDC_ZONE2), IDC_ZONE2);
			m_ToolTip.AddTool(GetDlgItem(IDC_PREC_LATEAST), IDC_PREC_EAST);
			m_ToolTip.AddTool(GetDlgItem(IDC_PREC_LONGNORTH), IDC_PREC_NORTH);
		}
	}
	else 
	{
		if (theUser.OutputDMS == 0)
			str_val.LoadString(IDC_DMS);
		else if (theUser.OutputDMS == 1)
			str_val.LoadString(IDC_DMIN);
		else
			str_val.LoadString(IDC_DDEG);
		
		str_val.Insert(0, GEO);

		GetDlgItem(IDC_OUT_PROJ_DESC)->SetWindowText(str_val);
				
		GetDlgItem(IDC_OUTPUT_VAR1)->SetWindowText(LA);
		GetDlgItem(IDC_OUTPUT_VAR2)->SetWindowText(LO);

		if (m_ToolTipCreated)
		{
			m_ToolTip.AddTool(GetDlgItem(IDC_LAT_EAST2), IDC_LAT2);
			m_ToolTip.AddTool(GetDlgItem(IDC_LONG_NTH2), IDC_LONG2);
			m_ToolTip.AddTool(GetDlgItem(IDC_PREC_LATEAST), IDC_PREC_LAT);
			m_ToolTip.AddTool(GetDlgItem(IDC_PREC_LONGNORTH), IDC_PREC_LONG);
		}
	}

	GetDlgItem(IDC_ZONE2)->ShowWindow(theUser.OutputisProjected);
	GetDlgItem(IDC_OUTPUT_VAR3)->ShowWindow(theUser.OutputisProjected);

}

void CGDAyDlg::UpdateDialogOptions()
{
	CStrProc cs;

	// Show point numbering?
	GetDlgItem(IDC_PNUM_FRAME)->EnableWindow(theUser.ShowPointNumbers);
	GetDlgItem(IDC_PT_NUMBER)->EnableWindow(theUser.ShowPointNumbers);
	GetDlgItem(IDC_PNUM_VAR)->EnableWindow(theUser.ShowPointNumbers);
	
	if (GetDlgItem(IDC_LAT_EAST2)->GetWindowTextLength() > 0)	// is there any output data to update?
	{
		coordinate c;
		c.dNum1 = m_lfLatEastOutput;
		c.dNum2 = m_lfLongNorthOutput;
		c.dLatacc = m_lfLatacc;
		c.dLongacc = m_lfLongacc;
		OnDisplayOutput(&c);
	}

	// input edit box
	GetDlgItem(IDC_LONG_NTH)->SetWindowText(cs.AddSymbology(m_strLongNorthInput, theUser, 2));
	if (!theUser.ShowPointNumbers)
		GetDlgItem(IDC_PT_NUMBER)->SetWindowText(_T(""));
	if (theUser.ShowPointNumbers && !theUser.AutoIncrement)
	{
		GetDlgItem(IDC_LAT_EAST)->SetWindowText(cs.AddSymbology(m_strLatEastInput, theUser, 1));
		GotoDlgCtrl(GetDlgItem(IDC_PT_NUMBER));
	}
	else		
		GotoDlgCtrl(GetDlgItem(IDC_LAT_EAST));
}


BOOL CGDAyDlg::AddDlgStatusBar()
{
	CFileProc file;

	// Create status bar at the bottom of the dialog window
	if (m_statusBar.CreateEx(this, SBT_TOOLTIPS))
	{
		m_statusBar.SetIndicators(lpIDArray, sizeof(lpIDArray)/sizeof(UINT));
		
		// Make a sunken or recessed border around the first pane
		m_statusBar.SetPaneInfo(0, IDS_TT_IDLEMESSAGE, SBPS_STRETCH, NULL );
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
		m_StatusBarCreated = TRUE;
		return TRUE;
	}
	else
		return FALSE;
}

// update appropriate popup menu. if CMenu *GridPopupMenu is null, get main menu
void CGDAyDlg::UpdateMenuItems(CMenu *GridPopupMenu)
{
	CMenu *TmpMenu;
	if (GridPopupMenu == NULL)
	{
		TmpMenu = GetMenu();		// get main menu
		GetMenu()->GetSubMenu(1)->EnableMenuItem(IDM_TRANSFORM_GDA20TOGDA94, MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(IDM_TRANSFORM_GDA94TOGDA20, MF_GRAYED);
		if (!theUser.InputisGDA20 && theUser.OutputisGDA) 
			GetMenu()->GetSubMenu(1)->EnableMenuItem(IDM_TRANSFORM_GDA94TOGDA20, MF_ENABLED);
		else if (theUser.InputisGDA20 && !theUser.OutputisGDA) 
			GetMenu()->GetSubMenu(1)->EnableMenuItem(IDM_TRANSFORM_GDA20TOGDA94, MF_ENABLED);
	}	
	else
		TmpMenu = GridPopupMenu;

	// Grid properties
	if (theUser.GridFileStatus > ERR_GRID_SUCCESS)
		TmpMenu->GetSubMenu(0)->EnableMenuItem(IDM_FILE_GRIDPROP, MF_GRAYED);
	else
		TmpMenu->GetSubMenu(0)->EnableMenuItem(IDM_FILE_GRIDPROP, MF_ENABLED);

	// Export menu options...
	TmpMenu->GetSubMenu(0)->EnableMenuItem(IDM_FILE_EXPORTTOBINARY, MF_GRAYED);
	TmpMenu->GetSubMenu(0)->EnableMenuItem(IDM_FILE_EXPORTTOASCII, MF_GRAYED);
	
	if (theUser.GridType.CompareNoCase(GSB) == 0 && theUser.GridFileStatus == ERR_GRID_SUCCESS)
		TmpMenu->GetSubMenu(0)->EnableMenuItem(IDM_FILE_EXPORTTOASCII, MF_ENABLED);
	else if (theUser.GridType.CompareNoCase(ASC) == 0 && theUser.GridFileStatus == ERR_GRID_SUCCESS) 
		TmpMenu->GetSubMenu(0)->EnableMenuItem(IDM_FILE_EXPORTTOBINARY, MF_ENABLED);
}

// initialise grid file & transformation parameters before calling 
// a method that uses these values
void CGDAyDlg::OnInitGridFileSettings()
{
	m_dGridParametersDlg.m_intGridFileStatus = theUser.GridFileStatus;
	m_dGridParametersDlg.m_strGridFile = theUser.GridFile;
	m_dGridParametersDlg.m_strTransParam = theUser.TransParam;
}

void CGDAyDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu mnuPopup;
	int iInteractiveOutputPane = false;
	m_bMenuSelected = TRUE;
	if(pWnd->IsKindOf(RUNTIME_CLASS(CStatusBar)))
	{
		CRect client;
		pWnd->GetWindowRect(client);

		// test if user clicked in "grid file" pane or "interactive_output file" pane
		if (((int)(point.x - client.left)) > 458)		// leftmost position of outputfile pane
		{
			mnuPopup.LoadMenu(IDR_STATUS_OUTFILE_POPUP);
			iInteractiveOutputPane = true;
		}
		else if (((int)(point.x - client.left)) > 330)		// leftmost position of gridfile pane
			mnuPopup.LoadMenu(IDR_STATUS_GRID_POPUP);
		else
			mnuPopup.LoadMenu(IDR_POPUP);
	}
	else
		mnuPopup.LoadMenu(IDR_POPUP);
	if (!iInteractiveOutputPane)
		UpdateMenuItems(&mnuPopup);	
	mnuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this, NULL);
}

void CGDAyDlg::OnDisplayOutput(coordinate *cCoordVar)
{
	CString strBuf;
	int precision;
	CPointsFileZoneDlg dPointsFileZoneDlg;
	CStrProc cs;

	cCoordVar->iNum4 = theUser.OutputisGDA;
	cCoordVar->dNum3 = theUser.OutputisProjected;

	if (theUser.OutputisProjected)
	{
		// Initialise Qif Zone dialog variables
		if (theUser.OutputisGDA)
			dPointsFileZoneDlg.m_strPrompt = "Specify the MGA2020 output zone";
		else
			dPointsFileZoneDlg.m_strPrompt = "Specify the MGA94 output zone";

		// prompt for zone if necessary
		if (theUser.PromptforOutputZone)
		{
			if (dPointsFileZoneDlg.DoModal() == IDOK)
				cCoordVar->dNum3 = (double)dPointsFileZoneDlg.m_iZone;
		}
		
		GCALC_GeotoGrid(cCoordVar);
		strBuf.Format("%.0f", cCoordVar->dNum3);
		GetDlgItem(IDC_ZONE2)->SetWindowText(strBuf);
		precision = theUser.MetPrecision;
	}
	else
	{
		if (theUser.OutputDMS == DMS)			// convert to degrees minutes seconds
		{
			cCoordVar->dNum1 = GCALC_DegtoDms(cCoordVar->dNum1);
			cCoordVar->dNum2 = GCALC_DegtoDms(cCoordVar->dNum2);
		}
		else if (theUser.OutputDMS == DMIN)		// convert to degrees minutes
		{				
			cCoordVar->dNum1 = GCALC_DegtoDmin(cCoordVar->dNum1);
			cCoordVar->dNum2 = GCALC_DegtoDmin(cCoordVar->dNum2);
		}

		// set ellipsoidal height to zero
		cCoordVar->dNum3 = 0.0;

		precision = theUser.GeoPrecision + 4;
		if (theUser.OutputDMS == DMIN)
			precision -= 2;
	}

	// Update output Lat/East edit box 
	strBuf.Format("%.*f", precision, cCoordVar->dNum1);
	GetDlgItem(IDC_LAT_EAST2)->SetWindowText(cs.AddSymbology(strBuf, theUser, 3));		
	
	// Update output Long/North edit box
	strBuf.Format("%.*f", precision, cCoordVar->dNum2);
	GetDlgItem(IDC_LONG_NTH2)->SetWindowText(cs.AddSymbology(strBuf, theUser, 4));
	
	// update accuracy
	if (theUser.OutputisProjected)
	{
		// swap values
		if (cCoordVar->dLatacc == NULL_ACCURACY)
			GetDlgItem(IDC_PREC_LATEAST)->SetWindowText("  ----");
		else
		{
			strBuf.Format("%.2f", cCoordVar->dLongacc);
			GetDlgItem(IDC_PREC_LATEAST)->SetWindowText(strBuf);
		}
		if (cCoordVar->dLongacc == NULL_ACCURACY)
			GetDlgItem(IDC_PREC_LONGNORTH)->SetWindowText("  ----");
		else
		{
			strBuf.Format("%.2f", cCoordVar->dLatacc);
			GetDlgItem(IDC_PREC_LONGNORTH)->SetWindowText(strBuf);
		}
	}
	else
	{
		if (cCoordVar->dLatacc == NULL_ACCURACY)
			GetDlgItem(IDC_PREC_LATEAST)->SetWindowText("  ----");
		else
		{
			strBuf.Format("%.2f", cCoordVar->dLatacc);
			GetDlgItem(IDC_PREC_LATEAST)->SetWindowText(strBuf);
		}
		if (cCoordVar->dLongacc == NULL_ACCURACY)
			GetDlgItem(IDC_PREC_LONGNORTH)->SetWindowText("  ----");
		else
		{
			strBuf.Format("%.2f", cCoordVar->dLongacc);
			GetDlgItem(IDC_PREC_LONGNORTH)->SetWindowText(strBuf);
		}
	}
}

void CGDAyDlg::OnPrintInteractiveOutput(coordinate cCoordVar)
{
	CString str;
	CStrProc cs;

	str = cs.CoordinateOutput(m_lPointNumber, m_lfLatEastInput, m_lfLongNorthInput, (int)m_lfZoneInput, cCoordVar, theUser);	
	GCALC_PrintLogFile((char*)((LPCTSTR)theUser.OutputFile), "a", (char*)((LPCTSTR)str));
}


void CGDAyDlg::OnLogfileview() 
{
	long hFile;
	CFileProc file;

	// create new log file if not already saved
	if (!file.Exists(theUser.LogFile, &hFile))
		PrintOutputHeader();

	if (file.View(theUser.LogFile, "notepad.exe") == 0)
		MessageBox("Could not load \"notepad.exe\"\n\nPlease check that the file has not been\nmoved or deleted.\n",
			"GDAy", MB_OK | MB_ICONWARNING);
	
}

void CGDAyDlg::OnHelpContents() 
{
	HtmlHelp(0, HH_DISPLAY_TOC);
}

void CGDAyDlg::OnHelpIndex() 
{
	HtmlHelp(0, HH_DISPLAY_INDEX);
}

void CGDAyDlg::OnHelpSearch() 
{
	HH_FTS_QUERY q ;

	q.cbStruct         = sizeof(HH_FTS_QUERY);
	q.fUniCodeStrings  = FALSE;
	q.pszSearchQuery   = "";
	q.iProximity       = HH_FTS_DEFAULT_PROXIMITY;
	q.fStemmedSearch   = FALSE;
	q.fTitleOnly       = FALSE;
	q.fExecute         = FALSE;
	q.pszWindow        = NULL;

	HtmlHelp((DWORD)&q, HH_DISPLAY_SEARCH);
}

void CGDAyDlg::OnClearDialogData()
{
	m_strLatEastInput = m_strLongNorthInput = "0.";
	m_lfLatEastInput = m_lfLongNorthInput = 0.0;
	GetDlgItem(IDC_LAT_EAST)->SetWindowText(_T("0."));
	GetDlgItem(IDC_LONG_NTH)->SetWindowText(_T("0."));
	GetDlgItem(IDC_ZONE)->SetWindowText(_T("0."));

	GetDlgItem(IDC_LAT_EAST2)->SetWindowText(_T(""));
	GetDlgItem(IDC_LONG_NTH2)->SetWindowText(_T(""));
	GetDlgItem(IDC_ZONE2)->SetWindowText(_T(""));
	GetDlgItem(IDC_PREC_LATEAST)->SetWindowText(_T(""));
	GetDlgItem(IDC_PREC_LONGNORTH)->SetWindowText(_T(""));
}
