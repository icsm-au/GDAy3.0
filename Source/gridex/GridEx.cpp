// GridEx.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GridEx.h"
#include "GridExDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridExApp

BEGIN_MESSAGE_MAP(CGridExApp, CWinApp)
	//{{AFX_MSG_MAP(CGridExApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridExApp construction

CGridExApp::CGridExApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGridExApp object

CGridExApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGridExApp initialization

BOOL CGridExApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CGridExDlg dlg;

	if (!cmdInfo.m_strFileName.IsEmpty())
		dlg.m_strGridFilename = cmdInfo.m_strFileName;

	
	int nRet(0);
	nRet = dlg.DoModal();
	
	if (dlg.fpGridHeaderArray != NULL)
		delete dlg.fpGridHeaderArray;	
	if (dlg.fpNewGridHeaderArray != NULL)
		delete dlg.fpNewGridHeaderArray;	
	
	return FALSE;
}
