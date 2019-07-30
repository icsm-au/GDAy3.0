// PointsDirDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "GDAyDlg.h"
#include "PointsDirDlg.h"
#include <Gcalcext.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern T_Pointsfile *t_Pfile;
extern BOOL g_bTransformingPointsFile;
extern int pf_success;
extern BOOL g_CancelPressed;

/////////////////////////////////////////////////////////////////////////////
// CPointsDirDlg dialog

CPointsDirDlg::CPointsDirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPointsDirDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPointsDirDlg)
	m_strOutputPath = _T("");
	m_strInputPath = _T("");
	m_Recurse = FALSE;
	m_intInputFormat = 0;
	m_intOutputFormat = 0;
	m_CreateDir = YES;
	//}}AFX_DATA_INIT
}

void CPointsDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointsDirDlg)
	DDX_Text(pDX, IDC_OUTPUT_DIR, m_strOutputPath);
	DDX_Text(pDX, IDC_INPUT_DIR, m_strInputPath);
	DDX_Check(pDX, IDC_RECURSE, m_Recurse);
	DDX_CBIndex(pDX, IDC_INPUT_FORMAT, m_intInputFormat);
	DDX_CBIndex(pDX, IDC_OUTPUT_FORMAT, m_intOutputFormat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointsDirDlg, CDialog)
	//{{AFX_MSG_MAP(CPointsDirDlg)
	ON_BN_CLICKED(IDC_DIR_LOOKIN, OnDirLookin)
	ON_BN_CLICKED(IDC_DIR_SAVEIN, OnDirSave)
	ON_CBN_SELCHANGE(IDC_INPUT_FORMAT, OnSelchangeInputFormat)
	ON_CBN_SELCHANGE(IDC_OUTPUT_FORMAT, OnSelchangeOutputFormat)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_LOGFILEVIEW, OnLogfileview)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const UINT lpIDArray[] =
{
	IDS_TT_READY,
	IDS_TT_INDATUM,
	IDS_TT_OUTDATUM
};

/////////////////////////////////////////////////////////////////////////////
// CPointsDirDlg message handlers

BOOL CPointsDirDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_INPUT_SEP))->SetCheck(!m_pfile_t.InputisHP);
	((CButton*)GetDlgItem(IDC_INPUT_HP))->SetCheck(m_pfile_t.InputisHP);
	((CButton*)GetDlgItem(IDC_OUTPUT_SEP))->SetCheck(!m_pfile_t.OutputisHP);
	((CButton*)GetDlgItem(IDC_OUTPUT_HP))->SetCheck(m_pfile_t.OutputisHP);
	((CButton*)GetDlgItem(IDC_OUTPUT_HEADER))->SetCheck(m_pfile_t.AddPFileHeader);
	((CButton*)GetDlgItem(IDC_RECURSE))->SetCheck(m_Recurse);
	
	// Create tooltip control
	if (m_ToolTip.Create(this))
	{
		// add the dialog's controls to the Tooltip
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_DIR), IDC_INPUT_DIR);
		m_ToolTip.AddTool(GetDlgItem(IDC_DIR_LOOKIN), IDC_DIR_LOOKIN);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_DIR), IDC_OUTPUT_DIR);
		m_ToolTip.AddTool(GetDlgItem(IDC_DIR_SAVEIN), IDC_DIR_SAVEIN);
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_FORMAT), IDC_INPUT_FORMAT);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_FORMAT), IDC_OUTPUT_FORMAT);
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_SEP), IDC_INPUT_SEP);
		m_ToolTip.AddTool(GetDlgItem(IDC_INPUT_HP), IDC_INPUT_HP);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_SEP), IDC_OUTPUT_SEP);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_HP), IDC_OUTPUT_HP);
		m_ToolTip.AddTool(GetDlgItem(IDC_OUTPUT_HEADER), IDC_OUTPUT_HEADER);
		m_ToolTip.AddTool(GetDlgItem(IDC_RECURSE), IDC_RECURSE);
		
		// activate ToolTip control
		m_ToolTip.Activate(TRUE);
	}
	
	AddDlgStatusBar();
	m_bSameOutputZone = IDNO;
	((CComboBox*)GetDlgItem(IDC_INPUT_FORMAT))->SetCurSel(m_intInputFormat);
	OnSelchangeInputFormat();
	
	return TRUE;  
}

void CPointsDirDlg::OnUpdateDialogControls()
{
	int iInputFormat, iOutputFormat;
	char szOutputFormat[6];

	iInputFormat = ((CComboBox*)GetDlgItem(IDC_INPUT_FORMAT))->GetCurSel();
	iOutputFormat = ((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->GetLBText(iOutputFormat, szOutputFormat);

	if (m_pfile_t.InputisProjected || m_pfile_t.InputDMS == 2 ||
		iInputFormat == _qif ||
		iInputFormat == _man ||
		iInputFormat == _res)
	{
		GetDlgItem(IDC_INPUT_SEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_INPUT_HP)->EnableWindow(FALSE);
		GetDlgItem(IDC_IN_GEO_BOX)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_INPUT_SEP)->EnableWindow(TRUE);
		GetDlgItem(IDC_INPUT_HP)->EnableWindow(TRUE);
		GetDlgItem(IDC_IN_GEO_BOX)->EnableWindow(TRUE);
	}
	
	if (m_pfile_t.OutputisProjected || m_pfile_t.OutputDMS == 2 ||
		(strcmp(szOutputFormat, QIF) == 0) ||
		(strcmp(szOutputFormat, MAN) == 0) ||
		(strcmp(szOutputFormat, RES) == 0))
	{
		GetDlgItem(IDC_OUTPUT_SEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_OUTPUT_HP)->EnableWindow(FALSE);
		GetDlgItem(IDC_OUT_GEO_BOX)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_OUTPUT_SEP)->EnableWindow(TRUE);
		GetDlgItem(IDC_OUTPUT_HP)->EnableWindow(TRUE);
		GetDlgItem(IDC_OUT_GEO_BOX)->EnableWindow(TRUE);
	}

	if ((strcmp(szOutputFormat, QIF) == 0) ||
		(strcmp(szOutputFormat, MAN) == 0) ||
		(strcmp(szOutputFormat, RES) == 0))
		GetDlgItem(IDC_OUTPUT_HEADER)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_OUTPUT_HEADER)->EnableWindow(TRUE);
}

void CPointsDirDlg::OnDirLookin() 
{
	CString strPath;
	GetDlgItem(IDC_INPUT_DIR)->GetWindowText(strPath);
	if (!strPath.IsEmpty())
		SetCurrentDirectory(strPath);
	strPath = file.BrowseForDialog("Choose the folder containing the input coordinate files", GetSafeHwnd());
	if (!strPath.IsEmpty())
	{
		GetDlgItem(IDC_INPUT_DIR)->SetWindowText(strPath);
		SetCurrentDirectory(strPath);
	}
	GotoDlgCtrl(GetDlgItem(IDC_DIR_SAVEIN));
}

void CPointsDirDlg::OnDirSave() 
{	
	CString strPath;
	GetDlgItem(IDC_OUTPUT_DIR)->GetWindowText(strPath);
	if (!strPath.IsEmpty())
		SetCurrentDirectory(strPath);
	strPath = file.BrowseForDialog("Choose the folder to save the transformed output coordinate files in", GetSafeHwnd());
	if (!strPath.IsEmpty())
		GetDlgItem(IDC_OUTPUT_DIR)->SetWindowText(strPath);
	GotoDlgCtrl(GetDlgItem(IDOK));
}

BOOL CPointsDirDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Relay events to the tooltip control	
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CPointsDirDlg::AddDlgStatusBar()
{	
	// Create status bar at the bottom of the dialog window
	if (m_statusBar.Create(this))
	{
		m_statusBar.SetIndicators(lpIDArray, sizeof(lpIDArray)/sizeof(UINT));
		
		// Make a sunken or recessed border around the first pane
		m_statusBar.SetPaneInfo(0, IDS_TT_READY, SBPS_STRETCH, NULL);
		m_statusBar.SetPaneText(1,
			(m_pfile_t.InputisGDA20 ? (m_pfile_t.InputisProjected ? "MGA2020" : "GDA2020") : (m_pfile_t.InputisProjected ? "MGA94" : "GDA94")), TRUE);
		m_statusBar.SetPaneText(2,
			(m_pfile_t.OutputisGDA ? (m_pfile_t.OutputisProjected ? "MGA2020" : "GDA2020") : (m_pfile_t.OutputisProjected ? "MGA94" : "GDA94")), TRUE);
		
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


int CPointsDirDlg::OnValidateDirectories()
{
	CString str;
	if (m_strOutputPath.IsEmpty() || m_strInputPath.IsEmpty())
		return false;
	if (file.Exists(m_strInputPath, NULL) < 0)
	{
		/*
	attr = GetFileAttributes(m_strInputPath);
	if (attr != FILE_ATTRIBUTE_DIRECTORY)
	{
	*/	MessageBox("Invalid input folder, or the folder does not exist", "Folder Transformation",
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	OnCheckDirectoryExists();
	if (m_CreateDir == NO || m_CreateDir == NOALL || m_CreateDir == CANCEL)
		return false;
	return true;
}


void CPointsDirDlg::OnOK() 
{	
	// check gridfilestatus
	if (m_pfile_t.GridFileStatus > ERR_GRID_SUCCESS ||
		m_pfile_t.GridFileStatus == ERR_GRID_NOT_OPENED)
	{
		CErrMsg err;
		err.ReportError(m_pfile_t.GridFileStatus, '\0');
		MessageBox(err.ErrorString, "Folder Transformation", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CString strBuffer;
	char cRecursePath[601];
	int n;
	
	memset(cRecursePath, '\0', sizeof(cRecursePath));
	m_pSubDir = cRecursePath;

	UpdateData(TRUE);
	m_CreateDir = YES;
	m_ExitDirectory = FALSE;
	g_CancelPressed = FALSE;
	
	strBuffer.Format("Beginning folder transformation...\nInput folder:   %s\n", m_strInputPath);
	GCALC_PrintLogFile((char*)((LPCTSTR)m_pfile_t.LogFile), "a", (char*)((LPCTSTR)strBuffer));

	m_strInputFileType = ReturnFileTypeSelection(m_intInputFormat);
	n = ((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->GetLBTextLen(m_intOutputFormat);
	((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->GetLBText(
		m_intOutputFormat, m_strOutputFileType.GetBuffer(n));
	
	if (m_strInputFileType.IsEmpty())
	{
		MessageBox("Unknown file type", "Folder Transformation", MB_OK | MB_ICONWARNING);
		return;
	}
	
	if (!OnValidateDirectories())
		return;
	
	m_pfile_t.InputisHP = ((CButton*)GetDlgItem(IDC_INPUT_HP))->GetCheck();
	m_pfile_t.OutputisHP = ((CButton*)GetDlgItem(IDC_OUTPUT_HP))->GetCheck();
	m_pfile_t.AddPFileHeader = ((CButton*)GetDlgItem(IDC_OUTPUT_HEADER))->GetCheck();
	
	t_Pfile = new T_Pointsfile;
	t_Pfile->pCFile = new pointsfile;

	OnInitialisePFileVars();
	m_NumFiles = 0;
	m_lTotalBytes = 0;
	
	strcpy_s(cRecursePath, m_strInputPath);
	m_pSubDir += m_strInputPath.GetLength();

	m_dDirProgDlg = new CPointsDirProgressDlg(this);
	if(m_dDirProgDlg != NULL)
	{
		if (m_dDirProgDlg->Create() == TRUE)
		{
			m_dDirProgDlg->CenterWindow(this);
			m_dDirProgDlg->GetDlgItem(IDC_DIR_TO_DIR)->SetWindowText("");			
			m_dDirProgDlg->m_DirProgress.SetRange(1, 100);
			m_dDirProgDlg->m_intProgressPos = 0;
			m_dDirProgDlg->UpdateWindow();
		}
	}
	
	m_statusBar.SetPaneText(0, "Transforming...", TRUE);
	
	// determine how many files and bytes for total progress
	RecurseFilesinDir(cRecursePath);
	m_nFileProgress = 0;
	
	// Initialise Qif Zone dialog variables
	m_dPointsFileZoneDlg.m_strPrompt = (m_pfile_t.OutputisGDA) ? "Specify the MGA2020 zone" : "Specify the MGA94 zone";

	// recursively transform folder
	strcpy_s(cRecursePath, m_strInputPath);
	TransformDirectory(cRecursePath);

	if (m_CreateDir == CANCEL || g_CancelPressed)
	{
		CString str = "Folder transformation cancelled.\n\n";
		GCALC_PrintLogFile((char*)((LPCTSTR)m_pfile_t.LogFile), "a", (char*)((LPCTSTR)str));
	}

	strBuffer.Format("Total number of files successfully transformed: %d.   %d file(s) had errors.\n\n",
		m_intFilesSuccess, m_intFilesTransformed - m_intFilesSuccess);
	GCALC_PrintLogFile((char*)((LPCTSTR)m_pfile_t.LogFile), "a", (char*)((LPCTSTR)strBuffer));
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	g_CancelPressed = FALSE;

	delete t_Pfile->pCFile;
	delete t_Pfile;
	m_dDirProgDlg = NULL;		// don't delete m_dDirProgDlg ... the dialog's own code will delete it.
	m_pSubDir = NULL;

	CDialog::OnOK();
}

void CPointsDirDlg::RecurseFilesinDir(char *strfiledir)
{
	GetFilesinDir(strfiledir);
	m_NumFiles += file.m_intNumFiles;
	m_lTotalBytes += file.m_lTotalBytes;
	
	CFileFind finder;
	CString strWildcard(strfiledir);
	strWildcard += _T("\\*.*");
	BOOL bWorking = finder.FindFile(strWildcard);
	
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots())		// skip . and .. files to prevent infinite recursion
			continue;
		if (finder.IsDirectory())	// if a directory, recursively search it
		{
			strcpy_s(strfiledir, 601, finder.GetFilePath());
			if (m_Recurse)
				RecurseFilesinDir(strfiledir);
		}
	}
	finder.Close();
	return;
}


void CPointsDirDlg::TransformDirectory(char *strfiledir)
{
	TransformFiles(strfiledir);
	
	if (m_ExitDirectory)
	{
		m_ExitDirectory = FALSE;
		return;
	}
	else if (m_CreateDir == CANCEL || g_CancelPressed)
		return;

	CFileFind finder;
	CString strWildcard(strfiledir);
	strWildcard += _T("\\*.*");
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots())		// skip . and .. files to prevent infinite recursion
			continue;
		if (finder.IsDirectory())	// if a folder, recursively search it
		{
			strcpy_s(strfiledir, 601, finder.GetFilePath());
			if (m_Recurse)
			{
				PrintDirtoLog(strfiledir);
				TransformDirectory(strfiledir);
			}
		}
	}
	finder.Close();
	return;
}


int CPointsDirDlg::TransformFiles(LPCTSTR strfiledir)
{
	CString strBuffer, strLogMsg;
	int count, nPos, i;
	MSG message;

	CErrMsg err;
	err.strGridFile = m_pfile_t.GridFile;

	strBuffer.Format("From '%s'", strfiledir);
	m_dDirProgDlg->GetDlgItem(IDC_DIR_TO_DIR)->SetWindowText(strBuffer);

	OnCheckDirectoryExists();
	if (m_ExitDirectory || m_CreateDir == CANCEL)
		return false;	

	GetFilesinDir(strfiledir);	
	nPos = i = 0;

	EnableWindow(FALSE);
	for (count=0; count<file.m_intNumFiles; count++)
	{
		if (g_CancelPressed)
			return false;
		
		sprintf(t_Pfile->pCFile->chFilein, "%s\\%s", strfiledir, file.m_fIndex[count].chfilename);
		// if input and output directory are the same, prefix output files with an underscore
		if (m_strOutputPath.Compare(m_strInputPath) == 0)
		{
			if (m_Recurse)
				sprintf(t_Pfile->pCFile->chFileout, "%s%s\\_%s", m_strOutputPath, m_pSubDir, file.m_fIndex[count].chfilename);
			else
				sprintf(t_Pfile->pCFile->chFileout, "%s\\_%s", m_strOutputPath, file.m_fIndex[count].chfilename);
		}
		else
		{
			if (m_Recurse)
				sprintf(t_Pfile->pCFile->chFileout, "%s%s\\%s", m_strOutputPath, m_pSubDir, file.m_fIndex[count].chfilename);
			else
				sprintf(t_Pfile->pCFile->chFileout, "%s\\%s", m_strOutputPath, file.m_fIndex[count].chfilename);
		}

		err.strInputFile = t_Pfile->pCFile->chFilein;
		m_dDirProgDlg->GetDlgItem(IDC_FILENAME)->SetWindowText(file.m_fIndex[count].chfilename);
		
		// print file to log
		strBuffer.Format("\n%-16s..\\%s\n", "File:", file.m_fIndex[count].chfilename);
		GCALC_PrintLogFile((char*)((LPCTSTR)m_pfile_t.LogFile), "a", (char*)((LPCTSTR)strBuffer));

		// if output is projection...get output zone
		if (t_Pfile->pCFile->OutputProj > 0 && m_bSameOutputZone == IDNO)
		{
			// Show the PointsFileZone Dialog and capture zone
			if (m_dPointsFileZoneDlg.DoModal() == IDOK)
			{
				// The user clicked ok...get zone
				t_Pfile->pCFile->OutputProj = m_dPointsFileZoneDlg.m_iZone;
				if (m_NumFiles - m_intFilesTransformed > 1)
				{
					strBuffer.Format("Use Zone %d for all remaining output files?\n", m_dPointsFileZoneDlg.m_iZone);
					m_bSameOutputZone = MessageBox(strBuffer, "Output Zone", MB_YESNO | MB_ICONQUESTION);
				}
			}
			else
				t_Pfile->pCFile->OutputProj = 1;		// set 1 to force GCalc to compute zone
		}

		GCALC_SetByteOffset();
		g_bTransformingPointsFile = TRUE;
		AfxBeginThread(ProcessFile, NULL, THREAD_PRIORITY_NORMAL);
		while (g_bTransformingPointsFile)
		{
			m_dDirProgDlg->m_DirProgress.SetPos(m_nFileProgress + (100 * GCALC_GetByteOffset() / m_lTotalBytes));
			if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&message);
				::DispatchMessage(&message);
			}
			if (g_CancelPressed)
				BeginWaitCursor();
			UpdateWindow();
			Sleep(150);
		}
		
		m_nFileProgress+=(int)(ceil(100.0 * GCALC_GetByteOffset() / m_lTotalBytes));
		m_dDirProgDlg->m_DirProgress.SetPos(m_nFileProgress);
		
		// copy count of points successfully transformed
		m_lNumSuccess = t_Pfile->pCFile->lNumTrans[0];
		m_lNumFail = t_Pfile->pCFile->lNumTrans[1] - t_Pfile->pCFile->lNumTrans[0];

		// write to log file
		strLogMsg.Format(">>  %d points were transformed in total. \n", m_lNumSuccess);
		if (t_Pfile->pCFile->lNumTrans[0] != t_Pfile->pCFile->lNumTrans[2])
			strBuffer.Format("%s>>  %d points were transformed using the distortion grid.\n", strLogMsg, t_Pfile->pCFile->lNumTrans[2]);
		else
			strBuffer.Format("%s\n", strLogMsg, m_lNumSuccess);
		
		GCALC_PrintLogFile(t_Pfile->pCFile->chLogFile, t_Pfile->pCFile->chAccessMode, (char*)((LPCTSTR)strBuffer));

		if (m_lNumSuccess > 0 && m_lNumFail == 0)
			m_intFilesSuccess ++;			// successful transformations with no errors
		
		m_intFilesTransformed ++;
	}

	EnableWindow(TRUE);

	return true;
}


void CPointsDirDlg::PrintDirtoLog(CString Path)
{
	CString str;
	
	str = "\nSub folder:     ..";
	str += m_pSubDir;
	str += "\n";
	GCALC_PrintLogFile((char*)((LPCTSTR)m_pfile_t.LogFile), "a", (char*)((LPCTSTR)str));
}

void CPointsDirDlg::GetFilesinDir(LPCTSTR strfiledir)
{
	file.ClearFileArrayMemory();
	file.m_intNumFiles = 0;
	file.m_lTotalBytes = 0;
	
	file.BuildFileArray((char*)((LPCTSTR)strfiledir), (char*)((LPCTSTR)m_strInputFileType));
}

CString CPointsDirDlg::ReturnFileTypeSelection(int iComboPos)
{
	switch (iComboPos)
	{
	case 0:
		return DAT;
		break;
	case 1:
		return TXT;
		break;
	case 2:
		return PRN;
		break;
	case 3:
		return CSV;
		break;
	case 4:
		return QIF;
		break;
	case 5:
		return MAN;
		break;
	case 6:
		return RES;
		break;
	default:
		return "";
		break;
	}
}

void CPointsDirDlg::OnInitialisePFileVars()
{
	CGridProc grid;

	m_intFilesTransformed = m_intFilesSuccess = 0;
	strcpy_s(t_Pfile->gridfile, m_pfile_t.GridFile);
	strcpy_s(t_Pfile->gridtype, m_pfile_t.GridType);	
	strcpy_s(t_Pfile->pCFile->chLogFile, m_pfile_t.LogFile);
	strcpy_s(t_Pfile->pCFile->chAccessMode, "a");	
	strcpy_s(t_Pfile->pCFile->chAppName, "GDAy Datum Transformations");
	strcpy_s(t_Pfile->pCFile->chFInputType, m_strInputFileType);
	strcpy_s(t_Pfile->pCFile->chFOutputType, m_strOutputFileType);
	t_Pfile->pCFile->InputHP = m_pfile_t.InputisHP;
	t_Pfile->pCFile->InputDMS = m_pfile_t.InputDMS;
	t_Pfile->pCFile->InputDatum = m_pfile_t.InputisGDA20;
	t_Pfile->pCFile->InputProj = m_pfile_t.InputisProjected;
	t_Pfile->pCFile->OutputHP = m_pfile_t.OutputisHP;
	t_Pfile->pCFile->OutputDMS = m_pfile_t.OutputDMS;
	t_Pfile->pCFile->OutputDatum = m_pfile_t.OutputisGDA;
	t_Pfile->pCFile->OutputProj = m_pfile_t.OutputisProjected;
	
	grid.GetTransformationParameters(m_pfile_t.TransParam, t_Pfile->pCFile->dParameters);
}

void CPointsDirDlg::OnCheckDirectoryExists()
{
	CString strPath;
	strPath.Format("%s%s", m_strOutputPath, m_pSubDir);
	CPointsDirCreateDlg dCreateDirDlg;		// modal
	CRect rcWindow;
	GetWindowRect(rcWindow);
	dCreateDirDlg.m_nParentWidth = rcWindow.Width();
	if (file.Exists(strPath, NULL) < 0)
	{
		switch (m_CreateDir)
		{
		case YES:
		case NO:
			dCreateDirDlg.m_strDirPath = strPath;
			m_CreateDir = dCreateDirDlg.DoModal();
			break;
		}
		switch (m_CreateDir)
		{
		case YES:
		case YESALL:
			if (!CreateDirectory(strPath, NULL))
			{
				m_ExitDirectory = TRUE;		// exit this folder
				MessageBox("Unable to create the new folder.", "Folder Transformation", MB_OK | MB_ICONSTOP);
			}
			else
				m_ExitDirectory = FALSE;
			break;
		case NO:
		case NOALL:
			m_ExitDirectory = TRUE;			// exit this folder - preventing potential for files to be overwritten.
			break;
		}
	}
}

void CPointsDirDlg::OnCancel() 
{
	if (g_bTransformingPointsFile)
		m_CreateDir = CANCEL;
	else
		CDialog::OnCancel();
}

void CPointsDirDlg::OnHelp() 
{
	::HtmlHelp(NULL, "gday.chm::/html/TF_directories.htm", HH_DISPLAY_TOC, 0);	
}

void CPointsDirDlg::OnSelchangeInputFormat() 
{
	int m_intInputFormat = ((CComboBox*)GetDlgItem(IDC_INPUT_FORMAT))->GetCurSel();
	switch (m_intInputFormat)
	{
	case _dat:
	case _txt:
	case _prn:
	case _csv:
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->ResetContent();
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(DAT);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(TXT);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(PRN);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(CSV);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->SetCurSel(m_intInputFormat);
		break;
	case _qif:
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->ResetContent();
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(QIF);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->SetCurSel(0);
		break;
	case _man:
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->ResetContent();
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(DAT);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(TXT);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(PRN);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(CSV);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(MAN);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->SetCurSel(4);
		break;
	case _res:
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->ResetContent();
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(DAT);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(TXT);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(PRN);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(CSV);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->AddString(RES);
		((CComboBox*)GetDlgItem(IDC_OUTPUT_FORMAT))->SetCurSel(4);
		break;
	}
	
	OnUpdateDialogControls();
}

void CPointsDirDlg::OnSelchangeOutputFormat() 
{
	OnUpdateDialogControls();	
}

void CPointsDirDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu mnuPopup;	
	mnuPopup.LoadMenu(IDR_VIEWLOG);		
	mnuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this, NULL);
	
}


void CPointsDirDlg::OnLogfileview() 
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

BOOL CPointsDirDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/TF_directories.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}

void CPointsDirDlg::OnEnableButtons(BOOL bEnable)
{
	(GetDlgItem(IDC_DIR_LOOKIN))->EnableWindow(bEnable);
	(GetDlgItem(IDC_DIR_SAVEIN))->EnableWindow(bEnable);
	(GetDlgItem(IDOK))->EnableWindow(bEnable);
	(GetDlgItem(IDCANCEL))->EnableWindow(bEnable);
}