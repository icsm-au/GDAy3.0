// GDAy.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GDAy.h"
#include "GDAyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGDAyApp

BEGIN_MESSAGE_MAP(CGDAyApp, CWinApp)
	//{{AFX_MSG_MAP(CGDAyApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGDAyApp construction

CGDAyApp::CGDAyApp()
{
	EnableHtmlHelp();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGDAyApp object

CGDAyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGDAyApp initialization

BOOL CGDAyApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	CRegProc Registry;

	CGDAyDlg dlg;
	HKEY HKEY_InstallKey = NULL;
	
	bool bRegistryAccessFailed = true;

	// HKEY_XXXXXXXX\SOFTWARE\Department of Natural Resources\GDAy 2.0\2.1

	// try to get registry settings with read/write access
	if (Registry.GetKeyHandle(REGKEY_GDAY30, THE_APP, &HKEY_InstallKey) == ERROR_SUCCESS)
	{
		if (!Registry.GetGlobalSettings(&dlg.theUser, HKEY_InstallKey))
			return FALSE;
		bRegistryAccessFailed = false;
		RegCloseKey(HKEY_InstallKey);
	}
	// Registry read-write access not allowed --- try reading from preferences file
	else if (Registry.PreferencesFileExists())
	{
		Registry.GetTemporaryFileSettings(&dlg.theUser);
	}
	// this is commented out, since it is more sensible to load what has been saved to the Preferences file
	// try to get registry settings in read-only mode
	//else if (Registry.GetKeyHandleReadOnly(REGKEY_GDAY21, THE_APP, &HKEY_InstallKey) == ERROR_SUCCESS)
	//{
	//	if (Registry.GetGlobalSettings(&dlg.theUser, HKEY_InstallKey))
	//	{
	//		bRegistryAccessFailed = false;
	//		RegCloseKey(HKEY_InstallKey);

	//		// save to preferences file
	//		Registry.SaveTemporaryFileSettings(&dlg.theUser);
	//	}
	//}
	// No registry access allowed, try to create a preferences file and save defaults
	else if (Registry.CreatePreferencesFile())
	{
		Registry.InitialiseSettings(&dlg.theUser);
	}
	else
	{
		CString msg;
		msg.Format("Warning: Unable to load user settings.\n\n");
		msg += "You may not have sufficient privileges to read from the system registry, or do not have write access to\n" + Registry.GetPreferencesFolder() + "\n";
		msg += "or you have not correctly installed " + (CString)THE_APP + " on your computer.\n\nContact your local system administrator for assistance.";	
		AfxMessageBox(msg);
	}

	dlg.DoModal();
	
	dlg.theUser.OutputFile = dlg.m_strDefaultOutputFile;	// save default file

	if (bRegistryAccessFailed)
		Registry.SaveTemporaryFileSettings(&dlg.theUser);
	else
	{
		if (Registry.GetKeyHandle(REGKEY_GDAY30, THE_APP, &HKEY_InstallKey) == ERROR_SUCCESS)
		{
			Registry.SaveGlobalSettings(&dlg.theUser, HKEY_InstallKey);
			RegCloseKey(HKEY_InstallKey);
		}
	}
	return FALSE;
}

