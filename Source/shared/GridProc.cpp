/////////////////////////////////////////////
// Roger Fraser

#include "stdafx.h"
#include <io.h>
#include "Defines.h"
#include "GridProc.h"
#include "FileProc.h"
#include "ErrorMsg.h"
#include <GCalcExt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGridProc::CGridProc()
{

}
	

int CGridProc::CheckGridStatus(CString strGridFile)
{
	CFileProc File;
	int success;

	if (_stricmp(strGridFile, NO_GRID_SPECIFIED) == 0)
		return ERR_GRID_NOT_OPENED;

	GCALC_CreateGridIndex((char*)((LPCTSTR)strGridFile), (char*)((LPCTSTR)(File.GetExt(strGridFile))), &success);
	
	return success;
}
	

void CGridProc::GetTransformationParameters(CString strParam, double *dTransParams)
{
	// SF
	// The registry access code should really go, as it is no longer
	// needed (there is only one set of params for all Australia.
	// I've left it here simply to avoid any knock-on effects.
	
	LPSTR tKey;
	HKEY HKEY_InstallKey;
	CString strValue;
	
	CRegProc registry;
	
	// initialise tKey
	tKey = KEYROOT_PARAM_NAT94;

	// retrieve transformation parameters
	if (registry.GetKeyHandleReadOnly(REGKEY_GDAY30, THE_APP, &HKEY_InstallKey) == ERROR_SUCCESS)
	{	
		strValue = registry.GetString(HKEY_InstallKey, tKey, "DX");
		if (strValue.IsEmpty() == TRUE)
			return;
		dTransParams[0] = atof(strValue);

		strValue = registry.GetString(HKEY_InstallKey, tKey, "DY");
		if (strValue.IsEmpty() == TRUE)
			return;
		dTransParams[1] = atof(strValue);

		strValue = registry.GetString(HKEY_InstallKey, tKey, "DZ");
		if (strValue.IsEmpty() == TRUE)
			return;
		dTransParams[2] = atof(strValue);

		strValue = registry.GetString(HKEY_InstallKey, tKey, "RX");
		if (strValue.IsEmpty() == TRUE)
			return;
		dTransParams[3] = atof(strValue);

		strValue = registry.GetString(HKEY_InstallKey, tKey, "RY");
		if (strValue.IsEmpty() == TRUE)
			return;
		dTransParams[4] = atof(strValue);

		strValue = registry.GetString(HKEY_InstallKey, tKey, "RZ");
		if (strValue.IsEmpty() == TRUE)
			return;
		dTransParams[5] = atof(strValue);

		strValue = registry.GetString(HKEY_InstallKey, tKey, "SC");
		if (strValue.IsEmpty() == TRUE)
			return;
		dTransParams[6] = atof(strValue);

		RegCloseKey(HKEY_InstallKey);
	}
	else
	{
		// SF 19/12/17
		// Default GDA94 -> GDA2020 params (see ICMS GDA2020 Interim Release Note 1
		dTransParams[0] = 0.06155;
		dTransParams[1] = -0.01087;
		dTransParams[2] = -0.04019;
		dTransParams[3] = -0.0394924;
		dTransParams[4] = -0.0327221;
		dTransParams[5] = -0.0328979;
		dTransParams[6] = -0.009994;
		
		// SF 19/12/17
		// There is no need for this warning, there is only one set of params.
		/*strValue = "ERROR: GDAy could not read the required transformation parameters from the system registry.\n\n";
		strValue += "Any transformations performed between GDA94 and GDA2020 requiring a 7-Parameter Transformation \n";
		strValue += "will be performed using the National GDA94 - GDA2020 parameters.";
		
		AfxMessageBox(strValue);
		*/
	}
}
	

// Print grid filename to log file
void CGridProc::PrintGridFiletoLog(T_user theUser)
{
	CString strBuffer, strGridtype;
	CErrMsg err;

	switch (theUser.GridFileStatus)
	{
	case ERR_AUS_BINARY:
		strGridtype = "(Australian NTv2 Binary)";
		break;
	case ERR_GRID_SUCCESS:
		if (theUser.GridType.CompareNoCase(ASC) == ERR_GRID_SUCCESS)
			strGridtype = "(NTv2 ASCII)";
		else
			strGridtype = "(Canadian NTv2 Binary)";
		break;
	case ERR_GRID_NOT_OPENED:		
		err.ReportError(theUser.GridFileStatus, '\0');
		strGridtype.Format("\nNOTE: %s", err.ErrorString);
		break;
	default:
		err.ReportError(theUser.GridFileStatus, '\0');
		strGridtype.Format("\nERROR: %s", err.ErrorString);
		break;
	}
	strBuffer.Format("Distortion Grid:    %s %s\n", theUser.GridFile, strGridtype);	
	GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "a", (char*)((LPCTSTR)strBuffer));
}
	

// Print transformation parameters to log file
void CGridProc::PrintParamstoLog(T_user theUser)
{
	CString strBuffer;
	strBuffer.Format("Transformation Parameters:    %s\n\n", theUser.TransParam);	
	GCALC_PrintLogFile((char*)((LPCTSTR)theUser.LogFile), "a", (char*)((LPCTSTR)strBuffer));
}
	

bool CGridProc::GetGridExAppPath(char *pszCmdLine)
{
	HKEY HKEY_InstallKey;
	CString str_val;
	CRegProc reg;
	if (reg.GetKeyHandleReadOnly(REGKEY_GRIDEX, THE_APP, &HKEY_InstallKey) == ERROR_SUCCESS)
	{
		str_val = reg.GetString(HKEY_InstallKey, NULL, APP_PATH);
		RegCloseKey(HKEY_InstallKey);
	}
	if (str_val.IsEmpty())
		return false;
	else
		strcpy_s(pszCmdLine, MAX_PATH+1, str_val);
	return true;
}