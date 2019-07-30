// PointsFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "GDAyDlg.h"
#include "PointsFileDlg.h"
#include <Gcalcext.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

T_Pointsfile *t_Pfile;
BOOL g_bTransformingPointsFile;
int pf_success;
BOOL g_CancelPressed;

UINT ProcessFile(LPVOID pParam)
{
	pf_success = GCALC_FileTran(t_Pfile->gridfile, t_Pfile->gridtype, t_Pfile->pCFile);
	g_bTransformingPointsFile = FALSE;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPointsFileDlg dialog

CPointsFileDlg::CPointsFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPointsFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPointsFileDlg)
	m_strInputFileName = _T("");
	m_strOutputFileName = _T("");
	m_strInputFileType = _T("");
	m_strOutputFileType = _T("");
	//}}AFX_DATA_INIT

	m_bMultiFiles = FALSE;
}


void CPointsFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointsFileDlg)
	DDX_Text(pDX, IDC_INPUT_FILENAME, m_strInputFileName);
	DDX_Text(pDX, IDC_OUTPUT_FILENAME, m_strOutputFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointsFileDlg, CDialog)
	//{{AFX_MSG_MAP(CPointsFileDlg)
	ON_BN_CLICKED(IDC_INPUT_BROWSE, OnInputBrowse)
	ON_BN_CLICKED(IDC_OUTPUT_BROWSE, OnOutputBrowse)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_LOGFILEVIEW, OnLogfileview)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const UINT lpIDArray[] =
{
	IDS_TT_READY,
	IDS_TT_NULL,
	IDS_TT_INDATUM,
	IDS_TT_OUTDATUM
};

/////////////////////////////////////////////////////////////////////////////
// CPointsFileDlg message handlers

BOOL CPointsFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_INPUT_SEP))->SetCheck(!m_pfile_t.InputisHP);
	((CButton*)GetDlgItem(IDC_INPUT_HP))->SetCheck(m_pfile_t.InputisHP);
	((CButton*)GetDlgItem(IDC_OUTPUT_SEP))->SetCheck(!m_pfile_t.OutputisHP);
	((CButton*)GetDlgItem(IDC_OUTPUT_HP))->SetCheck(m_pfile_t.OutputisHP);
	((CButton*)GetDlgItem(IDC_OUTPUT_HEADER))->SetCheck(m_pfile_t.AddPFileHeader);
	
	// Create tooltip control
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_FILENAME), IDC_INPUT_FILENAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_BROWSE), IDC_INPUT_BROWSE);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_FILENAME), IDC_OUTPUT_FILENAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_BROWSE), IDC_OUTPUT_BROWSE);
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_SEP), IDC_INPUT_SEP);
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_HP), IDC_INPUT_HP);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_SEP), IDC_OUTPUT_SEP);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_HP), IDC_OUTPUT_HP);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_HEADER), IDC_OUTPUT_HEADER);

		// activate ToolTip control
		m_ToolTip.Activate(TRUE);
	}
	
	if (AddDlgStatusBar())
	{
		m_StatusBarCreated = TRUE;
		ModifyDlgStatusBar();
	}
	
	((CEdit*)GetDlgItem(IDC_OUTPUT_FILENAME))->SetWindowText(m_strOutputFileName);
	((CEdit*)GetDlgItem(IDC_INPUT_FILENAME))->SetWindowText(m_strInputFileName);
	OnUpdateDialogControls();
	
	return TRUE;  
}


void CPointsFileDlg::OnUpdateDialogControls()
{
	if (m_pfile_t.InputisProjected || m_pfile_t.InputDMS == 2 ||
		m_strInputFileType == QIF ||
		m_strInputFileType == RES ||
		m_strInputFileType == MAN)
	{
		GetDlgItem(IDC_INPUT_SEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_INPUT_HP)->EnableWindow(FALSE);
		GetDlgItem(IDC_HP_IN)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_INPUT_SEP)->EnableWindow(TRUE);
		GetDlgItem(IDC_INPUT_HP)->EnableWindow(TRUE);
		GetDlgItem(IDC_HP_IN)->EnableWindow(TRUE);
	}

	
	if (m_pfile_t.OutputisProjected || m_pfile_t.OutputDMS == 2 ||
		m_strOutputFileType == QIF ||
		m_strOutputFileType == RES ||
		m_strOutputFileType == MAN)
	{
		GetDlgItem(IDC_OUTPUT_SEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_OUTPUT_HP)->EnableWindow(FALSE);
		GetDlgItem(IDC_HP_OUT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_OUTPUT_SEP)->EnableWindow(TRUE);
		GetDlgItem(IDC_OUTPUT_HP)->EnableWindow(TRUE);
		GetDlgItem(IDC_HP_OUT)->EnableWindow(TRUE);
	}

	GetDlgItem(IDC_OUTPUT_BROWSE)->EnableWindow(!m_bMultiFiles);
	((CEdit*)GetDlgItem(IDC_OUTPUT_FILENAME))->SetReadOnly(m_bMultiFiles);
	((CEdit*)GetDlgItem(IDC_INPUT_FILENAME))->SetReadOnly(m_bMultiFiles);

	if (m_strOutputFileType == QIF || m_strOutputFileType == RES || m_strOutputFileType == MAN)
		GetDlgItem(IDC_OUTPUT_HEADER)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_OUTPUT_HEADER)->EnableWindow(TRUE);


}


void CPointsFileDlg::OnInputBrowse() 
{	
	CString strFolder, strExt, strName;
	char c_newfile[_MAX_PATH];

	GetDlgItem(IDC_INPUT_FILENAME)->GetWindowText(strName);
	strFolder = file.GetFolder(strName);
	strcpy_s(c_newfile, file.GetName(strName, NULL));
	strExt = file.GetExt(strName);
	if (m_bMultiFiles)
		sprintf(c_newfile, "*.%s", strExt);
	strName = file.FileOpenSave(TRUE, TRUE, strFolder, c_newfile, OPEN_PTS_FILE, strExt, PTS_FILE_FILTER, file.FilterIndex(strExt));
	if (strName.IsEmpty())
		return;	
	if (file.m_intNumFiles > 1)
	{
		// multiple files have been selected
		GetDlgItem(IDC_OUTPUT_FILENAME)->SetWindowText(file.GetFolder(strName) + "\\ _*." + file.m_strFileExt);
		strName = file.GetFolder(strName) + "\\ *." + file.m_strFileExt;
		m_bMultiFiles = TRUE;
	}
	else
	{
		if (m_bMultiFiles)
				GetDlgItem(IDC_OUTPUT_FILENAME)->SetWindowText(_T(""));
		m_bMultiFiles = FALSE;
	}
	GetDlgItem(IDC_INPUT_FILENAME)->SetWindowText(strName);
	m_strInputFileType = file.GetExt(strName);
	OnUpdateDialogControls();
}

void CPointsFileDlg::OnOutputBrowse() 
{
	CString strFolder, strExt, strName;
	LPCTSTR strFilter;
	char c_newfile[_MAX_PATH];
	
	GetDlgItem(IDC_OUTPUT_FILENAME)->GetWindowText(strName);
	strcpy_s(c_newfile, file.GetName(strName, NULL));
	if (strName.IsEmpty())
		GetDlgItem(IDC_INPUT_FILENAME)->GetWindowText(strName);		
	strFolder = file.GetFolder(strName);
	strExt = file.GetExt(strName);
	
	if (strExt.CompareNoCase(CSV) == 0 ||
		strExt.CompareNoCase(DAT) == 0 ||
		strExt.CompareNoCase(TXT) == 0 ||
		strExt.CompareNoCase(PRN) == 0)
		strFilter = DATCSV_FILE_FILTER;
	else if (strExt.CompareNoCase(QIF) == 0)
		strFilter = QIF_FILE_FILTER;
	else if (strExt.CompareNoCase(RES) == 0)
		strFilter = RES_FILE_FILTER;
	else if (strExt.CompareNoCase(MAN) == 0)
		strFilter = MAN_FILE_FILTER;
	else
		strFilter = PTS_FILE_FILTER;

	strName = file.FileOpenSave(FALSE, FALSE, strFolder, c_newfile, SAVE_PTS_FILE, strExt, strFilter, file.FilterIndex(strExt));	
	if (strName.IsEmpty())
		return;	
	GetDlgItem(IDC_OUTPUT_FILENAME)->SetWindowText(strName);
	m_strOutputFileType = file.GetExt(strName);
	OnUpdateDialogControls();
}



void CPointsFileDlg::OnOK() 
{
	UpdateData(TRUE);	

	BOOL bSameOutputZone = IDNO;
	BOOL StatusBarProgressCreated = FALSE;
	CString msg, strLogMsg, strBuffer;
	char cInputExt[4] = {'\0'};
	char cOutputExt[4] = {"\0"};
	int count, file_progress, iProgress;
	RECT rc;
	MSG message;
	
	pointsfile pCoordFile;
	pointsfile *ptrCoordFile = &pCoordFile;
	
	ptrCoordFile->InputHP = m_pfile_t.InputisHP = ((CButton*)GetDlgItem(IDC_INPUT_HP))->GetCheck();
	ptrCoordFile->OutputHP = m_pfile_t.OutputisHP = ((CButton*)GetDlgItem(IDC_OUTPUT_HP))->GetCheck();	
	
	// validate whether input and ouptut files and coordinate system settings
	// are valid for a successful transformation
	if (!OnValidateTransformationFiles(cInputExt, cOutputExt))
		return;
	
	CRegProc cr;
	CGridProc grid;
	CErrMsg err;
	
	T_Pointsfile m_thPfile;
	CProgressCtrl *FileProgress = NULL;
	g_CancelPressed = FALSE;
	m_intFilesTransformed = 0;

	// copy user parameters, files and relevant switches
	OnInitTransformationData(ptrCoordFile, cInputExt, cOutputExt);
	grid.GetTransformationParameters(m_pfile_t.TransParam, ptrCoordFile->dParameters);
		
	// Initialise Qif Zone dialog variables
	CPointsFileZoneDlg dPointsFileZoneDlg;
	if (ptrCoordFile->OutputDatum)
		dPointsFileZoneDlg.m_strPrompt = "Specify the MGA2020 zone";
	else
		dPointsFileZoneDlg.m_strPrompt = "Specify the MGA zone";

	g_bTransformingPointsFile = TRUE;
	err.strGridFile = m_pfile_t.GridFile;

	t_Pfile = &m_thPfile;
	strcpy_s(m_thPfile.gridfile, m_pfile_t.GridFile);
	strcpy_s(m_thPfile.gridtype, m_pfile_t.GridType);

	if (m_bMultiFiles)
	{
		// dislay modeless progress dialog
		m_dDirProgDlg = new CPointsDirProgressDlg(this);
		if(m_dDirProgDlg != NULL)
		{
			if (m_dDirProgDlg->Create() == TRUE)
			{
				m_dDirProgDlg->SetWindowText("Transforming multiple files...");
				strBuffer.Format("From '%s'", file.GetFolder(m_strInputFileName));
				m_dDirProgDlg->GetDlgItem(IDC_DIR_TO_DIR)->SetWindowText(strBuffer);
				m_dDirProgDlg->CenterWindow(this);
				m_dDirProgDlg->m_DirProgress.SetRange(1, (int)(file.m_intNumFiles*100.0));
				m_dDirProgDlg->m_intProgressPos = 0;
				m_dDirProgDlg->UpdateWindow();
			}
		}		
		m_statusBar.SetPaneText(0, "Transforming...", TRUE);
	}
	else
	{
		// A single file...display progress indicator in status bar
		ModifyDlgStatusBar();
		m_statusBar.GetItemRect(1, &rc);
		FileProgress = new CProgressCtrl;
		if (FileProgress->Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rc, &m_statusBar, 0))
		{
			StatusBarProgressCreated = TRUE;
			FileProgress->SetRange(0, 100);
		}
	}

	OnEnableButtons(FALSE);
	for (count=0; count<file.m_intNumFiles; count++)
	{
		if (g_CancelPressed)
		{
			CString str = "Transformation of multiple files cancelled.\n\n";
			GCALC_PrintLogFile((char*)((LPCTSTR)m_pfile_t.LogFile), "a", (char*)((LPCTSTR)str));
			break;
		}			

		strBuffer = file.GetName(file.m_fIndex[count].chfilename, NULL);
		if (m_bMultiFiles)
		{
			m_dDirProgDlg->GetDlgItem(IDC_FILENAME)->SetWindowText(strBuffer);
			sprintf(ptrCoordFile->chFilein, file.m_fIndex[count].chfilename);
			sprintf(ptrCoordFile->chFileout, "%s\\_%s", file.GetFolder(file.m_fIndex[count].chfilename), file.GetName(file.m_fIndex[count].chfilename, NULL));
			err.strInputFile = file.m_fIndex[count].chfilename;
		}
		else
		{
			m_statusBar.SetPaneText(0, file.GetName(file.m_fIndex[count].chfilename, NULL), TRUE);
			strcpy_s(ptrCoordFile->chFilein, m_strInputFileName);
			strcpy_s(ptrCoordFile->chFileout, m_strOutputFileName);
			err.strInputFile = m_strInputFileName;
		}

		// if output is projection and user wants prompt or if output is QIF regardless...get output zone
		if (
			((m_strOutputFileType.CompareNoCase(QIF) == 0 && m_pfile_t.OutputisProjected > 0) ||
			(m_pfile_t.OutputisProjected > 0 && m_pfile_t.PromptforOutputZone)) && bSameOutputZone == IDNO)
		{
			// Show the PointsFileZone Dialog and capture zone
			if (dPointsFileZoneDlg.DoModal() == IDOK)
			{
				// The user clicked ok...get zone
				ptrCoordFile->OutputProj = dPointsFileZoneDlg.m_iZone;
				if (file.m_intNumFiles - count > 1)
				{
					strBuffer.Format("Use Zone %d for all remaining output files?\n", dPointsFileZoneDlg.m_iZone);
					bSameOutputZone = MessageBox(strBuffer, "Output Zone", MB_YESNO | MB_ICONQUESTION);
				}
			}
			else
			{
				ptrCoordFile->OutputProj = 1;		// set 1 to force GCalc to compute zone
			}
		}

		// write to log file
		strBuffer.Format("Beginning of points file transformation...\nInput file:      %s\nOutput file:     %s\n\n",
			ptrCoordFile->chFilein, ptrCoordFile->chFileout);
		GCALC_PrintLogFile(ptrCoordFile->chLogFile, ptrCoordFile->chAccessMode, (char*)((LPCTSTR)strBuffer));
				
		UpdateWindow();		
		m_thPfile.pCFile = ptrCoordFile;
		
		g_bTransformingPointsFile = TRUE;
		AfxBeginThread(ProcessFile, (void*)this, THREAD_PRIORITY_NORMAL);

		if (StatusBarProgressCreated)
		{
			BeginWaitCursor();
			while (g_bTransformingPointsFile)
			{
				if ((file_progress = GCALC_ReturnFileProgress()) > 0)
					FileProgress->SetPos(file_progress);
				UpdateWindow();
				Sleep(150);
			}
			ModifyDlgStatusBar();
			EndWaitCursor();
		}
		else if (m_dDirProgDlg != NULL)
		{
			while (g_bTransformingPointsFile)
			{
				iProgress = GCALC_ReturnFileProgress() + (int)(count * 100.0);
				m_dDirProgDlg->m_DirProgress.SetPos(iProgress);
				UpdateWindow();
				if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&message);
					::DispatchMessage(&message);
				}
				if (g_CancelPressed)
					BeginWaitCursor();
				Sleep(150);
			}
		}

		// copy count of points successfully transformed
		m_lNumSuccess = ptrCoordFile->lNumTrans[0];
		m_lNumInterpolated = ptrCoordFile->lNumTrans[2];
		m_lNumFail = ptrCoordFile->lNumTrans[1] - ptrCoordFile->lNumTrans[0];

		if (!pf_success)
		{
			if (!m_bMultiFiles)
			{
				err.ReportError(ptrCoordFile->IO_Status, '\0');
				MessageBox(err.ErrorString, "File Transformation Error...", MB_OK | MB_ICONEXCLAMATION);
			}
		}
		else
			m_intFilesTransformed ++;
		
		// write to log file
		strLogMsg.Format("\n>>  %d points were transformed in total. ", m_lNumSuccess);
		if (ptrCoordFile->lNumTrans[0] != ptrCoordFile->lNumTrans[2])
			strBuffer.Format("%s\n>>  %d points were transformed using the distortion grid.\n", strLogMsg, ptrCoordFile->lNumTrans[2]);
		else
			strBuffer.Format("%s\n", strLogMsg, m_lNumSuccess);
		strLogMsg.Format("%s\nEnd of points file transformation.\n\n", strBuffer, m_lNumSuccess);
		GCALC_PrintLogFile(ptrCoordFile->chLogFile, ptrCoordFile->chAccessMode, (char*)((LPCTSTR)strLogMsg));
	}
	OnEnableButtons();
	g_CancelPressed = FALSE;	
	if (FileProgress != NULL)
		delete FileProgress;
	CDialog::OnOK();
}

void CPointsFileDlg::OnHelp() 
{
	::HtmlHelp(NULL, "gday.chm::/html/TF_point_file.htm", HH_DISPLAY_TOC, 0);
}

BOOL CPointsFileDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CPointsFileDlg::AddDlgStatusBar()
{
	// Create status bar at the bottom of the dialog window
	if (m_statusBar.Create(this))
	{
		m_statusBar.SetIndicators(lpIDArray, sizeof(lpIDArray)/sizeof(UINT));
				
		// figure out how big the control bars are.
		CRect rcClientStart, rcClientNow;
		GetClientRect(rcClientStart);
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNow);

		// move all controls so they are in the same relative position within client with no control bars.
		CPoint ptOffset(rcClientNow.left - rcClientStart.left, rcClientNow.top - rcClientStart.top);

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

void CPointsFileDlg::ModifyDlgStatusBar()
{
	// Create status bar at the bottom of the dialog window
	if (m_StatusBarCreated)
	{
		if (!g_bTransformingPointsFile)
			m_statusBar.SetPaneText(0, "Ready", TRUE);

		// when a single point is being transformed...
		if (g_bTransformingPointsFile && !m_bMultiFiles)
		{
			m_statusBar.SetPaneInfo(0, IDS_TT_READY, SBPS_STRETCH, NULL);
			m_statusBar.SetPaneInfo(1, IDS_TT_NULL, SBPS_NORMAL, 58);
			m_statusBar.SetPaneText(1, "", TRUE);
			m_statusBar.SetPaneText(2, (m_pfile_t.InputisGDA20 ? (m_pfile_t.InputisProjected ? "MGA2020" : "GDA2020") : (m_pfile_t.InputisProjected ? "MGA94" : "GDA94")), TRUE);
			m_statusBar.SetPaneText(3, (m_pfile_t.OutputisGDA ? (m_pfile_t.OutputisProjected ? "MGA2020" : "GDA2020") : (m_pfile_t.OutputisProjected ? "MGA94" : "GDA94")), TRUE);
			
		}
		else	// on idle
		{
			CRect rc;
			m_statusBar.GetWindowRect(&rc);
			//m_statusBar.SetPaneInfo(0, IDS_TT_READY, SBPS_STRETCH, (rc.Width() - 66));
			m_statusBar.SetPaneInfo(0, IDS_TT_READY, SBPS_STRETCH, (rc.Width() / 4));
			m_statusBar.SetPaneInfo(1, IDS_TT_INDATUM, SBPS_NORMAL, (rc.Width() / 4));
			m_statusBar.SetPaneInfo(2, IDS_TT_OUTDATUM, SBPS_NORMAL, (rc.Width() / 4));
			m_statusBar.SetPaneText(1, (m_pfile_t.InputisGDA20 ? (m_pfile_t.InputisProjected ? "MGA2020" : "GDA2020") : (m_pfile_t.InputisProjected ? "MGA94" : "GDA94")), TRUE);
			m_statusBar.SetPaneText(2, (m_pfile_t.OutputisGDA ? (m_pfile_t.OutputisProjected ? "MGA2020" : "GDA2020") : (m_pfile_t.OutputisProjected ? "MGA94" : "GDA94")), TRUE);
		}
	}
}


void CPointsFileDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu mnuPopup;	
	mnuPopup.LoadMenu(IDR_VIEWLOG);		
	mnuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this, NULL);
}

void CPointsFileDlg::OnLogfileview()
{
	long hFile;
	CFileProc file;

	// create new log file if not already saved
	if (!file.Exists(m_pfile_t.LogFile, &hFile))
	{
		CGDAyDlg dlg;
		dlg.PrintOutputHeader();
	}

	if (file.View(m_pfile_t.LogFile, "notepad.exe") == 0)
		MessageBox("Could not load \"notepad.exe\"\n\nPlease check that the file has not been\nmoved or deleted.\n",
			"GDAy", MB_OK | MB_ICONWARNING);
}

BOOL CPointsFileDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/TF_point_file.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}

int CPointsFileDlg::OnValidateTransformationFiles(char *cInputExt, char *cOutputExt)
{
	CString strMsg;

	// check contents of edit boxes
	if (m_strInputFileName.IsEmpty())
		return false;
	if (m_strOutputFileName.IsEmpty())
		return false;
	
	if (m_strOutputFileName == m_strInputFileName)
	{
		// input & output files are the same
		MessageBox("The input and output file names can not be the same.", 
			"Points File Transformation", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	strcpy_s(cInputExt, 4, file.GetExt(m_strInputFileName));
	if (*cInputExt == '\0')
	{
		MessageBox("Please specify an input file type.",
			"Points File Transformation", MB_OK | MB_ICONEXCLAMATION);
		GotoDlgCtrl(GetDlgItem(IDC_INPUT_FILENAME));
		((CEdit*)GetDlgItem(IDC_INPUT_FILENAME))->SetSel(0, -1, FALSE);
		return false;
	}
	strcpy_s(cOutputExt, 4, file.GetExt(m_strOutputFileName));
	if (*cOutputExt == '\0')
	{
		MessageBox("Please specify an output file type.",
			"Points File Transformation",
			MB_OK | MB_ICONEXCLAMATION);
		GotoDlgCtrl(GetDlgItem(IDC_OUTPUT_FILENAME));
		((CEdit*)GetDlgItem(IDC_OUTPUT_FILENAME))->SetSel(0, -1, FALSE);
		return false;
	}
	if ( (_stricmp(cInputExt, CSV) == 0 ||
		  _stricmp(cInputExt, DAT) == 0 ||
		  _stricmp(cInputExt, TXT) == 0 ||
		  _stricmp(cInputExt, PRN) == 0)
		  &&
		 (_stricmp(cOutputExt, QIF) == 0 ||
		  _stricmp(cOutputExt, MAN) == 0 ||
		  _stricmp(cOutputExt, RES) == 0) )
	{
		// insufficient data to output to these types of files
		strMsg.Format("The input file type does not contain sufficient data to create a valid .%s file.\n", cOutputExt);
		MessageBox(strMsg, "Points File Transformation",
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if ((_stricmp(cInputExt, QIF) == 0 && _stricmp(cOutputExt, QIF) != 0) ||	// cannot export qif to any other file format
		(_stricmp(cOutputExt, QIF) == 0 && _stricmp(cInputExt, QIF) != 0) ||	// cannot convert any other file format to qif
		(_stricmp(cInputExt, MAN) == 0 && _stricmp(cOutputExt, RES) == 0) ||	// cannot export man to res
		(_stricmp(cInputExt, RES) == 0 && _stricmp(cOutputExt, MAN) == 0)		// cannot export res to man
		)
	{
		// insufficient data to output to these types of files
		strMsg.Format("A .%s file can not be created from a .%s file.\n", cOutputExt, cInputExt);
		MessageBox(strMsg, "Points File Transformation",
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	if ((m_pfile_t.InputisGDA20 == m_pfile_t.OutputisGDA &&				// Datums are the same
		m_pfile_t.InputisProjected == m_pfile_t.OutputisProjected &&	// Projections are the same
		m_pfile_t.InputDMS == m_pfile_t.OutputDMS &&					// DMS / DM / DDeg are the same
		m_pfile_t.InputisHP == m_pfile_t.OutputisHP &&					// HP Not / DMS Separated are the same
		!m_pfile_t.InputisProjected)									// Geographic
		||
		(m_pfile_t.InputisGDA20 == m_pfile_t.OutputisGDA &&				// Datums are the same
		m_pfile_t.InputisProjected == m_pfile_t.OutputisProjected &&	// Projections are the same
		m_pfile_t.InputisProjected))									// Geographic
	{
		// input and output coordinates are the same
		MessageBox("The input and output coordinate parameters are the same.",
			"Points File Transformation", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	return true;
}

void CPointsFileDlg::OnInitTransformationData(pointsfile *ptrCoordFile, char *cInputExt, char *cOutputExt)
{
	strcpy_s(ptrCoordFile->chFInputType, cInputExt);
	strcpy_s(ptrCoordFile->chFOutputType, cOutputExt);
	ptrCoordFile->InputDatum = m_pfile_t.InputisGDA20;			// Input Datum
	ptrCoordFile->InputProj = m_pfile_t.InputisProjected;		// Input projection
	ptrCoordFile->InputDMS = m_pfile_t.InputDMS;				// Input data is in Degrees, minutes & seconds
	ptrCoordFile->OutputDatum = m_pfile_t.OutputisGDA;			// Input Datum
	ptrCoordFile->OutputProj = m_pfile_t.OutputisProjected;		// Input projection
	ptrCoordFile->OutputDMS = m_pfile_t.OutputDMS;				// Input data is in Degrees, minutes & seconds		
	strcpy_s(ptrCoordFile->chLogFile, m_pfile_t.LogFile);
	strcpy_s(ptrCoordFile->chAccessMode, "a");	
	strcpy_s(ptrCoordFile->chAppName, "GDAy Datum Transformations");	
	m_pfile_t.AddPFileHeader = ((CButton*)GetDlgItem(IDC_OUTPUT_HEADER))->GetCheck();
	GCALC_SetFileTranHeader(m_pfile_t.AddPFileHeader);
}

void CPointsFileDlg::OnEnableButtons(BOOL bEnable)
{
	(GetDlgItem(IDC_INPUT_BROWSE))->EnableWindow(bEnable);
	(GetDlgItem(IDC_OUTPUT_BROWSE))->EnableWindow(bEnable);
	(GetDlgItem(IDOK))->EnableWindow(bEnable);
	(GetDlgItem(IDCANCEL))->EnableWindow(bEnable);
}
