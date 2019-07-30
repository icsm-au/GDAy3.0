/////////////////////////////////////////////
// Roger Fraser

#include "stdafx.h"
#include <io.h>
#include "Defines.h"
#include "RegProc.h"
#include "FileProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRegProc::CRegProc()
{
}


// return either HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER - whichever works
long CRegProc::GetKeyHandle(LPCTSTR RegistryKey, LPCTSTR AppName, HKEY *hKey)
{
	// CString msg;
	long rs = NULL;

	// msg.Format("ERROR: Unable to access the system registry.\n\nEither you do not have sufficient registry privileges or you have not correctly installed\n%s on your computer. Contact your local system administrator for assistance.", AppName);
	if ((rs = RegOpenKeyEx(HKEY_LOCAL_MACHINE, RegistryKey, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, hKey)) == ERROR_SUCCESS)
		return rs;
	else if ((rs = RegOpenKeyEx(HKEY_CURRENT_USER, RegistryKey, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, hKey)) == ERROR_SUCCESS)
		return rs;	
	return rs;
}

// return either HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER - whichever works
long CRegProc::GetKeyHandleReadOnly(LPCTSTR RegistryKey, LPCTSTR AppName, HKEY *hKey)
{
	// CString msg;
	long rs = NULL;
	
	// msg.Format("ERROR: Unable to access the system registry.\n\nEither you do not have sufficient registry privileges or you have not correctly installed\n%s on your computer. Contact your local system administrator for assistance.", AppName);
	if ((rs = RegOpenKeyEx(HKEY_LOCAL_MACHINE, RegistryKey, 0, KEY_QUERY_VALUE, hKey)) == ERROR_SUCCESS)
		return rs;
	else if ((rs = RegOpenKeyEx(HKEY_CURRENT_USER, RegistryKey, 0, KEY_QUERY_VALUE, hKey)) == ERROR_SUCCESS)
		return rs;	
	return rs;
}

int CRegProc::GetDword(HKEY hMainKey, LPSTR pszKey, LPSTR pszName)
{
	HKEY hSubKey;
	DWORD len = 4;
	long rs;
	int i_val = -1;
	CString strbuf;

	if ((rs = RegOpenKeyEx(hMainKey, pszKey, 0, KEY_QUERY_VALUE, &hSubKey)) == ERROR_SUCCESS)
		rs = RegQueryValueEx(hSubKey, pszName, 0, NULL, (LPBYTE)&i_val, &len);		// on success
	RegCloseKey(hSubKey);
	if (rs != ERROR_SUCCESS)
	{
		strbuf.Format("Warning: Unable to retrieve information from system registry\n\nDWORD Key name: %s", pszKey);
		AfxMessageBox(strbuf);
	}
	return i_val;
}

CString CRegProc::GetString(HKEY hMainKey, LPSTR pszKey, LPSTR pszName)
{
	HKEY hSubKey;
	char cbuf[_MAX_PATH];
	DWORD bufsize = sizeof(cbuf);
	CString strbuf;
	long rs;

	memset(cbuf, '\0', bufsize);
	if ((rs = RegOpenKeyEx(hMainKey, pszKey, 0, KEY_QUERY_VALUE, &hSubKey)) == ERROR_SUCCESS)
		rs = RegQueryValueEx(hSubKey, pszName, 0, NULL, (BYTE*)cbuf, &bufsize);		// on success
	RegCloseKey(hSubKey);	
	if (rs != ERROR_SUCCESS)
	{
		strbuf.Format("Warning: Unable to retrieve information from system registry\n\nString Name: %s", pszKey);
		AfxMessageBox(strbuf);
	}
	return (CString)cbuf;
}

bool CRegProc::SaveString(HKEY hMainKey, LPSTR pszKey, LPSTR pszName, LPSTR pszValue)
{
	long rs;
	HKEY hKey;
	CString strbuf;
	
	if ((rs = RegOpenKeyEx(hMainKey, pszKey, 0, KEY_SET_VALUE, &hKey)) == ERROR_SUCCESS)
		rs = RegSetValueEx(hKey, pszName, 0, REG_SZ, (BYTE*)pszValue, (strlen(pszValue)+1)*sizeof(TCHAR));
	RegCloseKey(hKey);
	if (rs != ERROR_SUCCESS)
	{
		strbuf.Format("Unable to save information to system registry\n\nKey name: %s  \nString Name: %s", pszKey, pszName);
		AfxMessageBox(strbuf);
	}
	if (rs == 0)
		return true;
	else
		return false;
}

bool CRegProc::SaveDword(HKEY hMainKey, LPSTR pszKey, LPSTR pszName, int i_value)
{
	long rs;
	HKEY hKey;
	CString strbuf;
	
	if ((rs = RegOpenKeyEx(hMainKey, pszKey, 0, KEY_SET_VALUE, &hKey)) == ERROR_SUCCESS)
		rs = RegSetValueEx(hKey, pszName, 0, REG_DWORD, (CONST BYTE *)&i_value, sizeof(DWORD));
	RegCloseKey(hKey);
	if (rs != ERROR_SUCCESS)
	{
		strbuf.Format("Unable to save information to system registry\n\nDWORD Key name: %s  ", pszKey);
		AfxMessageBox(strbuf);
	}
	if (rs == 0)
		return true;
	else
		return false;
}

bool CRegProc::GetGlobalSettings(T_user *theUser, HKEY HKEY_InstallKey)
{
	int i_val;
	CString str_val;

	// Input Datum
	i_val = GetDword(HKEY_InstallKey, CRD_SYSTEM, INPUT_DATUM);
	if (i_val < 0)
		return false;
	else
		theUser->InputisGDA20 = i_val;

	// Input Coordinates projected?
	i_val = GetDword(HKEY_InstallKey, CRD_SYSTEM, INPUT_PROJ);
	if (i_val < 0)
		return false;
	else
		theUser->InputisProjected = i_val;

	// degrees minutes seconds switch
	i_val = GetDword(HKEY_InstallKey, CRD_SYSTEM, INPUT_DMS);
	if (i_val < 0)
		return false;
	else
		theUser->InputDMS = i_val;

	// Input HP
	i_val = GetDword(HKEY_InstallKey, CRD_SYSTEM, INPUT_HP);
	if (i_val < 0)
		return false;
	else
		theUser->InputisHP = i_val;

	// Output Datum
	i_val = GetDword(HKEY_InstallKey, CRD_SYSTEM, OUTPUT_DATUM);
	if (i_val < 0)
		return false;
	else
		theUser->OutputisGDA = i_val;

	// Output Coordinates projected?
	i_val = GetDword(HKEY_InstallKey, CRD_SYSTEM, OUTPUT_PROJ);
	if (i_val < 0)
		return false;
	else
		theUser->OutputisProjected = i_val;

	// Prompt for Output Zone?
	i_val = GetDword(HKEY_InstallKey, CUSTOM, OUTPUT_ZONE_PROMPT);
	if (i_val < 0)
		return false;
	else
		theUser->PromptforOutputZone = i_val;

	// Prompt for Grid File?
	i_val = GetDword(HKEY_InstallKey, CUSTOM, GRIF_FILE_PROMPT);
	if (i_val < 0)
		return false;
	else
		theUser->DoNotPromptforGridFile = !(i_val);

	// degrees minutes seconds switch
	i_val = GetDword(HKEY_InstallKey, CRD_SYSTEM, OUTPUT_DMS);
	if (i_val < 0)
		return false;
	else
		theUser->OutputDMS = i_val;

	// Output HP
	i_val = GetDword(HKEY_InstallKey, CRD_SYSTEM, OUTPUT_HP);
	if (i_val < 0)
		return false;
	else
		theUser->OutputisHP = i_val;

	// Positive south latitude
	i_val = GetDword(HKEY_InstallKey, CUSTOM, POS_SOUTH);
	if (i_val < 0)
		return false;
	else
		theUser->LatisPosSouth = i_val;

	// Add Points File Header
	i_val = GetDword(HKEY_InstallKey, CUSTOM, ADD_PF_HEADER);
	if (i_val < 0)
		return false;
	else
		theUser->AddPFileHeader = i_val;

	// Add Prefix
	i_val = GetDword(HKEY_InstallKey, CUSTOM, ADD_PREFIX);
	if (i_val < 0)
		return false;
	else
		theUser->AddPrefix = i_val;

	// Add Symbols
	i_val = GetDword(HKEY_InstallKey, CUSTOM, ADD_SYMBOLS);
	if (i_val < 0)
		return false;
	else
		theUser->AddSymbols = i_val;

	// Add Station Name
	i_val = GetDword(HKEY_InstallKey, CUSTOM, ADD_STATION_NAME);
	if (i_val < 0)
		return false;
	else
		theUser->AddStationName = i_val;

	// Auto Name All Stations
	i_val = GetDword(HKEY_InstallKey, CUSTOM, AUTO_NAME_ALL_STATIONS);
	if (i_val < 0)
		return false;
	else
		theUser->AutoNameAllStations = i_val;

	// Auto Increment
	i_val = GetDword(HKEY_InstallKey, CUSTOM, AUTO_INCR);
	if (i_val < 0)
		return false;
	else
		theUser->AutoIncrement = i_val;

	// Output precision for geographic coordinates
	i_val = GetDword(HKEY_InstallKey, CUSTOM, GEO_PREC);
	if (i_val < 0)
		return false;
	else
		theUser->GeoPrecision = i_val;

	// Output precision for projection coordinates
	i_val = GetDword(HKEY_InstallKey, CUSTOM, MET_PREC);
	if (i_val < 0)
		return false;
	else
		theUser->MetPrecision = i_val;

	// Prompt for New Output file on start up
	i_val = GetDword(HKEY_InstallKey, CUSTOM, START_OUTPUT);
	if (i_val < 0)
		return false;
	else
		theUser->OnStartNewOutput = i_val;

	// Show Point Numbers
	i_val = GetDword(HKEY_InstallKey, CUSTOM, SHOW_PT_NUM);
	if (i_val < 0)
		return false;
	else
		theUser->ShowPointNumbers = i_val;

	// Start Number
	i_val = GetDword(HKEY_InstallKey, CUSTOM, START_NUM);
	if (i_val < 0)
		return false;
	else
		theUser->StartNumber = i_val;

	// Add Station Name
	str_val = GetString(HKEY_InstallKey, CUSTOM, AUTO_NAME);
	if (str_val.IsEmpty())
		return false;
	else
		theUser->AutoStationName = str_val;

	// Distortion grid type
	str_val = GetString(HKEY_InstallKey, CUSTOM, GRIDTYPE);
	if (str_val.IsEmpty())
		return false;
	else
		theUser->GridType = str_val;

	// Grid file
	str_val = GetString(HKEY_InstallKey, FILE_LOCATIONS, DISTORTION_GRID);
	if (str_val.IsEmpty())
		return false;
	else
		theUser->GridFile = str_val;
	
	// Interactive Output file
	str_val = GetString(HKEY_InstallKey, FILE_LOCATIONS, INTERACTIVE_OUTPUT);
	if (str_val.IsEmpty())
		return false;
	else
		theUser->OutputFile = str_val;
	
	// Log file
	str_val = GetString(HKEY_InstallKey, FILE_LOCATIONS, LOG_FILE);
	if (str_val.IsEmpty())
		return false;
	else
		theUser->LogFile = str_val;

	// Log Header...can be null!
	theUser->LogHeader = GetString(HKEY_InstallKey, CUSTOM, HEADER);
	
	// Transformation parameters
	str_val = GetString(HKEY_InstallKey, TRANS_PARAMS, TRANS_PARAM);
	if (str_val.IsEmpty())
		return false;
	else
		theUser->TransParam = str_val;
	
	return true;
}

bool CRegProc::SaveGlobalSettings(T_user *theUser, HKEY HKEY_InstallKey)
{
	// Input Datum
	if (!SaveDword(HKEY_InstallKey, CRD_SYSTEM, INPUT_DATUM, theUser->InputisGDA20))
		return false;

	// Input Coordinates projected?
	if (!SaveDword(HKEY_InstallKey, CRD_SYSTEM, INPUT_PROJ, theUser->InputisProjected))
		return false;

	// degrees minutes seconds switch
	if (!SaveDword(HKEY_InstallKey, CRD_SYSTEM, INPUT_DMS, theUser->InputDMS))
		return false;

	// Input HP
	if (!SaveDword(HKEY_InstallKey, CRD_SYSTEM, INPUT_HP, theUser->InputisHP))
		return false;

	// Output Datum
	if (!SaveDword(HKEY_InstallKey, CRD_SYSTEM, OUTPUT_DATUM, theUser->OutputisGDA))
		return false;

	// Output Coordinates projected?
	if (!SaveDword(HKEY_InstallKey, CRD_SYSTEM, OUTPUT_PROJ, theUser->OutputisProjected))
		return false;

	// Prompt for Output Zone?
	if (!SaveDword(HKEY_InstallKey, CUSTOM, OUTPUT_ZONE_PROMPT, theUser->PromptforOutputZone))
		return false;

	// Prompt for Grid File?
	if (!SaveDword(HKEY_InstallKey, CUSTOM, GRIF_FILE_PROMPT, !(theUser->DoNotPromptforGridFile)))
		return false;

	// degrees minutes seconds switch
	if (!SaveDword(HKEY_InstallKey, CRD_SYSTEM, OUTPUT_DMS, theUser->OutputDMS))
		return false;

	// Output HP
	if (!SaveDword(HKEY_InstallKey, CRD_SYSTEM, OUTPUT_HP, theUser->OutputisHP))
		return false;
	
	// Positive south latitude
	if (!SaveDword(HKEY_InstallKey, CUSTOM, POS_SOUTH, theUser->LatisPosSouth))
		return false;

	// Add Points File Header
	if (!SaveDword(HKEY_InstallKey, CUSTOM, ADD_PF_HEADER, theUser->AddPFileHeader))
		return false;

	// Add Prefix
	if (!SaveDword(HKEY_InstallKey, CUSTOM, ADD_PREFIX, theUser->AddPrefix))
		return false;

	// Add Station Name
	if (!SaveDword(HKEY_InstallKey, CUSTOM, ADD_STATION_NAME, theUser->AddStationName))
		return false;

	// Auto Name All Stations
	if (!SaveDword(HKEY_InstallKey, CUSTOM, AUTO_NAME_ALL_STATIONS, theUser->AutoNameAllStations))
		return false;

	// Add Symbols
	if (!SaveDword(HKEY_InstallKey, CUSTOM, ADD_SYMBOLS, theUser->AddSymbols))
		return false;

	// Auto Increment
	if (!SaveDword(HKEY_InstallKey, CUSTOM, AUTO_INCR, theUser->AutoIncrement))
		return false;

	// Output precision for geographic coordinates
	if (!SaveDword(HKEY_InstallKey, CUSTOM, GEO_PREC, theUser->GeoPrecision))
		return false;

	// Output precision for projection coordinates
	if (!SaveDword(HKEY_InstallKey, CUSTOM, MET_PREC, theUser->MetPrecision))
		return false;

	// Prompt for New Output file on start up
	if (!SaveDword(HKEY_InstallKey, CUSTOM, START_OUTPUT, theUser->OnStartNewOutput))
		return false;

	// Show Point Numbers
	if (!SaveDword(HKEY_InstallKey, CUSTOM, SHOW_PT_NUM, theUser->ShowPointNumbers))
		return false;

	// Start Number
	if (!SaveDword(HKEY_InstallKey, CUSTOM, START_NUM, theUser->StartNumber))
		return false;

	// Add Station Name
	if (!SaveString(HKEY_InstallKey, CUSTOM, AUTO_NAME, (char*)((LPCTSTR)theUser->AutoStationName)))
		return false;

	// Distortion grid type
	if (!SaveString(HKEY_InstallKey, CUSTOM, GRIDTYPE, (char*)((LPCTSTR)theUser->GridType)))
		return false;

	// Grid file
	if (!SaveString(HKEY_InstallKey, FILE_LOCATIONS, DISTORTION_GRID, (char*)((LPCTSTR)theUser->GridFile)))
		return false;

	// Interactive Output file
	if (!SaveString(HKEY_InstallKey, FILE_LOCATIONS, INTERACTIVE_OUTPUT, (char*)((LPCTSTR)theUser->OutputFile)))
		return false;

	// Log file
	if (!SaveString(HKEY_InstallKey, FILE_LOCATIONS, LOG_FILE, (char*)((LPCTSTR)theUser->LogFile)))
		return false;

	// Log Header
	if (!SaveString(HKEY_InstallKey, CUSTOM, HEADER, (char*)((LPCTSTR)theUser->LogHeader)))
		return false;

	// Transformation parameters
	if (!SaveString(HKEY_InstallKey, TRANS_PARAMS, TRANS_PARAM, (char*)((LPCTSTR)theUser->TransParam)))
		return false;

	return true;
}
	

bool CRegProc::CreatePreferencesFile()
{
	TCHAR szPath[MAX_PATH];
	
	// Get path for each computer, non-user specific and non-roaming data.
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
	{
		TCHAR szTempFileName[MAX_PATH];

		if (!PreferencesFolderExists())
			CreatePreferencesFolder(szPath);

		// Generate a temporary file name within the application folder (product-specific path).
		sprintf_s(szTempFileName, "%s\\GDAy\\Preferences.dat", szPath);
		HANDLE hFile = NULL;
		
		// Open the file.
		if ((hFile = CreateFile(szTempFileName, 
									GENERIC_WRITE, 
									0, 
									NULL, 
									CREATE_ALWAYS, 
									FILE_ATTRIBUTE_NORMAL, 
									NULL)) != INVALID_HANDLE_VALUE)
		{
			// Write temporary data (code omitted).
			CloseHandle(hFile);
			return true;
		}
	}
	return false;
}
	

bool CRegProc::CreatePreferencesFolder(char* szPath)
{
	TCHAR szTempFileName[MAX_PATH];

	sprintf_s(szTempFileName, "%s\\GDAy\\", szPath);
	if (!CreateDirectory(szTempFileName, NULL)) 
		return false;
	
	/*
	sprintf_s(szTempFileName, "%s\\NRW\\GDAy\\", szPath);
	if (!CreateDirectory(szTempFileName, NULL)) 
		return false;

	sprintf_s(szTempFileName, "%s\\NRW\\GDAy\\2.1", szPath);
	if (!CreateDirectory(szTempFileName, NULL)) 
		return false;
	*/

	return true;
}
	

bool CRegProc::PreferencesFileExists()
{
	TCHAR szPath[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
		return false;

	TCHAR szTempFileName[MAX_PATH];

	// Generate a temporary file name within the application folder (product-specific path).
	sprintf_s(szTempFileName, "%s\\GDAy\\Preferences.dat", szPath);

	long hFile;
	CFileProc fp;

	if (fp.Exists(szTempFileName, &hFile))
		return true;
	else
		return false;
}
	

bool CRegProc::PreferencesFolderExists()
{
	TCHAR szPath[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
		return false;

	TCHAR szTempFileName[MAX_PATH];

	// Generate a temporary file name within the application folder (product-specific path).
	sprintf_s(szTempFileName, "%s\\GDAy\\", szPath);

	HANDLE hFile = NULL;
		
	// Open the folder.
	if ((hFile = CreateFile(
					szTempFileName, 
					GENERIC_READ, 
					FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 
					NULL, 
					OPEN_EXISTING, 
					FILE_FLAG_BACKUP_SEMANTICS, 
					NULL)) != INVALID_HANDLE_VALUE)
	{
		// Write temporary data (code omitted).
		CloseHandle(hFile);
		return true;
	}
	return false;
}
	

CString CRegProc::GetPreferencesFolder()
{
	TCHAR szPath[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
		return "";
	return (CString)szPath;
}
	

bool CRegProc::GetTemporaryFileSettings(T_user* theUser)
{
	TCHAR szPath[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
		return false;

	CString strTmp;
	long hfile;
	CFileProc fp;
	
	// Open the preferences file from the application folder (product-specific path).
	FILE* fin;
	TCHAR szTempFileName[MAX_PATH];			
	
	sprintf_s(szTempFileName, "%s\\GDAy\\Preferences.dat", szPath);	
	if (fopen_s(&fin, szTempFileName, "rb") != 0)
		return false;

	// Input Datum
	theUser->InputisGDA20 = GetTFDword(fin);

	// Input Coordinates projected?
	theUser->InputisProjected = GetTFDword(fin);

	// degrees minutes seconds switch
	theUser->InputDMS = GetTFDword(fin);

	// Input HP
	theUser->InputisHP = GetTFDword(fin);

	// Output Datum
	theUser->OutputisGDA = GetTFDword(fin);

	// Output Coordinates projected?
	theUser->OutputisProjected = GetTFDword(fin);

	// Prompt for Output Zone?
	theUser->PromptforOutputZone = GetTFDword(fin);

	// Prompt for Grid File?
	theUser->DoNotPromptforGridFile = !GetTFDword(fin);

	// degrees minutes seconds switch
	theUser->OutputDMS = GetTFDword(fin);

	// Output HP
	theUser->OutputisHP = GetTFDword(fin);
	
	// Positive south latitude
	theUser->LatisPosSouth = GetTFDword(fin);

	// Add Points File Header
	theUser->AddPFileHeader = GetTFDword(fin);

	// Add Prefix
	theUser->AddPrefix = GetTFDword(fin);

	// Add Station Name
	theUser->AddStationName = GetTFDword(fin);

	// Auto Name All Stations
	theUser->AutoNameAllStations = GetTFDword(fin);

	// Add Symbols
	theUser->AddSymbols = GetTFDword(fin);

	// Auto Increment
	theUser->AutoIncrement = GetTFDword(fin);

	// Output precision for geographic coordinates
	theUser->GeoPrecision = GetTFDword(fin);

	// Output precision for projection coordinates
	theUser->MetPrecision = GetTFDword(fin);

	// Prompt for New Output file on start up
	theUser->OnStartNewOutput = GetTFDword(fin);

	// Show Point Numbers
	theUser->ShowPointNumbers = GetTFDword(fin);

	// Start Number
	theUser->StartNumber = GetTFDword(fin);

	// Add Station Name
	theUser->AutoStationName = GetTFString(fin, 100);

	// Distortion grid type
	theUser->GridType = GetTFString(fin, 4);

	// ---------------------------------------------------------
	// Grid file path	
	// create default
	theUser->GridFile = "no_grid_specified";
	
	// get file path and save if file exists
	strTmp = GetTFString(fin, MAX_PATH);
	if (fp.Exists(strTmp, &hfile))
		theUser->GridFile = strTmp;
	// ---------------------------------------------------------

	// ---------------------------------------------------------
	// Interactive Output file path
	// create default
	theUser->OutputFile.Format("%s\\GDAy\\GDAy.out", szPath);

	// get file path and save if file exists
	strTmp = GetTFString(fin, MAX_PATH);
	if (fp.Exists(strTmp, &hfile))
		theUser->OutputFile = strTmp;
	// ---------------------------------------------------------

	// ---------------------------------------------------------
	// Log file path
	// create default
	theUser->LogFile.Format("%s\\GDAy\\GDAy.log", szPath);

	// get file path and save if file exists
	strTmp = GetTFString(fin, MAX_PATH);
	if (fp.Exists(strTmp, &hfile))
		theUser->LogFile = strTmp;
	// ---------------------------------------------------------

	// Log Header
	theUser->LogHeader = GetTFString(fin, 1000);

	// Transformation parameters
	theUser->TransParam = GetTFString(fin, 32);

	fclose(fin);
	return true;
}
	

void CRegProc::InitialiseSettings(T_user *theUser)
{
	// Input Datum
	theUser->InputisGDA20 = 0;

	// Input Coordinates projected?
	theUser->InputisProjected = 0;

	// degrees minutes seconds switch
	theUser->InputDMS = 0;

	// Input HP
	theUser->InputisHP = 0;

	// Output Datum
	theUser->OutputisGDA = 1;

	// Output Coordinates projected?
	theUser->OutputisProjected = 0;

	// Prompt for Output Zone?
	theUser->PromptforOutputZone = 0;

	// Prompt for Grid File?
	theUser->DoNotPromptforGridFile = 0;

	// degrees minutes seconds switch
	theUser->OutputDMS = 0;

	// Output HP
	theUser->OutputisHP = 0;
	
	// Positive south latitude
	theUser->LatisPosSouth = 1;

	// Add Points File Header
	theUser->AddPFileHeader = 0;

	// Add Prefix
	theUser->AddPrefix = 1;

	// Add Station Name
	theUser->AddStationName = 1;

	// Auto Name All Stations
	theUser->AutoNameAllStations = 1;

	// Add Symbols
	theUser->AddSymbols = 1;

	// Auto Increment
	theUser->AutoIncrement = 0;

	// Output precision for geographic coordinates
	theUser->GeoPrecision = 7;

	// Output precision for projection coordinates
	theUser->MetPrecision = 3;

	// Prompt for New Output file on start up
	theUser->OnStartNewOutput = 0;

	// Show Point Numbers
	theUser->ShowPointNumbers = 0;

	// Start Number
	theUser->StartNumber = 0;

	// Add Station Name
	theUser->AutoStationName = "First Order, Class A GPS";

	// Distortion grid type
	theUser->GridType = "gsb";

	// Grid file path	
	theUser->GridFile = "no_grid_specified";
	
	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
	{	
		// Interactive Output file path
		theUser->OutputFile.Format("%s\\GDAy\\GDAy.out", szPath);

		// Log file path
		theUser->LogFile.Format("%s\\GDAy\\GDAy.log", szPath);
	}
	else
	{
		// Interactive Output file path
		theUser->OutputFile = "";

		// Log file path
		theUser->LogFile = "";
	}

	// Log Header
	theUser->LogHeader = "";

	// Transformation parameters
	theUser->TransParam = "GDA94 - GDA2020 (National)";

	SaveTemporaryFileSettings(theUser);
}
	

void CRegProc::SaveTemporaryFileSettings(T_user *theUser)
{
	TCHAR szPath[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
		return;

	FILE* fout;
	TCHAR szTempFileName[MAX_PATH];

	// Generate a temporary file name within the application folder (product-specific path).
	sprintf_s(szTempFileName, "%s\\GDAy\\Preferences.dat", szPath);

	fopen_s(&fout, szTempFileName, "wb");

	// Input Datum
	SaveTFDword(fout, theUser->InputisGDA20);

	// Input Coordinates projected?
	SaveTFDword(fout, theUser->InputisProjected);

	// degrees minutes seconds switch
	SaveTFDword(fout, theUser->InputDMS);

	// Input HP
	SaveTFDword(fout, theUser->InputisHP);

	// Output Datum
	SaveTFDword(fout, theUser->OutputisGDA);

	// Output Coordinates projected?
	SaveTFDword(fout, theUser->OutputisProjected);

	// Prompt for Output Zone?
	SaveTFDword(fout, theUser->PromptforOutputZone);

	// Prompt for Grid File?
	SaveTFDword(fout, (!(theUser->DoNotPromptforGridFile)));

	// degrees minutes seconds switch
	SaveTFDword(fout, theUser->OutputDMS);

	// Output HP
	SaveTFDword(fout, theUser->OutputisHP);
	
	// Positive south latitude
	SaveTFDword(fout, theUser->LatisPosSouth);

	// Add Points File Header
	SaveTFDword(fout, theUser->AddPFileHeader);

	// Add Prefix
	SaveTFDword(fout, theUser->AddPrefix);

	// Add Station Name
	SaveTFDword(fout, theUser->AddStationName);

	// Auto Name All Stations
	SaveTFDword(fout, theUser->AutoNameAllStations);

	// Add Symbols
	SaveTFDword(fout, theUser->AddSymbols);

	// Auto Increment
	SaveTFDword(fout, theUser->AutoIncrement);

	// Output precision for geographic coordinates
	SaveTFDword(fout, theUser->GeoPrecision);

	// Output precision for projection coordinates
	SaveTFDword(fout, theUser->MetPrecision);

	// Prompt for New Output file on start up
	SaveTFDword(fout, theUser->OnStartNewOutput);

	// Show Point Numbers
	SaveTFDword(fout, theUser->ShowPointNumbers);

	// Start Number
	SaveTFDword(fout, theUser->StartNumber);

	// Add Station Name
	SaveTFString(fout, (char*)((LPCTSTR)theUser->AutoStationName), 100);

	// Distortion grid type
	SaveTFString(fout, (char*)((LPCTSTR)theUser->GridType), 4);

	// Grid file
	SaveTFString(fout, (char*)((LPCTSTR)theUser->GridFile), MAX_PATH);

	// Interactive Output file
	SaveTFString(fout, (char*)((LPCTSTR)theUser->OutputFile), MAX_PATH);

	// Log file
	SaveTFString(fout, (char*)((LPCTSTR)theUser->LogFile), MAX_PATH);

	// Log Header
	SaveTFString(fout, (char*)((LPCTSTR)theUser->LogHeader), 1000);

	// Transformation parameters
	SaveTFString(fout, (char*)((LPCTSTR)theUser->TransParam), 32);

	fclose(fout);
}


void CRegProc::SaveTFDword(FILE* fout, int i)
{
	fwrite(&i, sizeof(int), 1, fout);
}

int CRegProc::GetTFDword(FILE* fin)
{
	int i;
	fread(&i, sizeof(int), 1, fin);
	return i;
}

void CRegProc::SaveTFString(FILE* fout, char* s, int iLen)
{
	fwrite(s, sizeof(char), iLen, fout);	
}

CString CRegProc::GetTFString(FILE* fin, int iLen)
{
	char* str = new char[iLen+1];
	fread(&(*str), sizeof(char), iLen, fin);
	CString s = str;
	delete []str;
	return s;
}