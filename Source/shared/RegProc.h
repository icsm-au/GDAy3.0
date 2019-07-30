/////////////////////////////////////////////
// Roger Fraser

#ifndef     __REGISTRY_PROCEDURES__
#define     __REGISTRY_PROCEDURES__

typedef struct {
	int AddPFileHeader;
	int AddPrefix;
	int AddStationName;
	int AddSymbols;
	int AutoIncrement;
	int AutoNameAllStations;
	CString AutoStationName;
	int DoNotPromptforGridFile;
	int GeoPrecision;
	CString GridFile;
	int GridFileStatus;
	CString GridType;
	int InputDMS;
	int InputisGDA20;
	int InputisHP;
	int InputisProjected;
	int LatisPosSouth;
	CString LogFile;
	CString LogHeader;
	int MetPrecision;
	int OnStartNewOutput;
	int OutputDMS;
	CString OutputFile;
	int OutputisGDA;
	int OutputisHP;
	int OutputisProjected;
	int PromptforOutputZone;
	CString PromptStationName;
	int ShowPointNumbers;
	int StartNumber;
	CString TransParam;
} T_user;


class CRegProc
{
// Construction
public:
	CRegProc();
	
	bool GetGlobalSettings(T_user *theUser, HKEY HKEY_InstallKey);
	bool SaveGlobalSettings(T_user *theUser, HKEY HKEY_InstallKey);

	bool SaveDword(HKEY hMainKey, LPSTR pszKey, LPSTR pszName, int i_value);
	bool SaveString(HKEY hMainKey, LPSTR pszKey, LPSTR pszName, LPSTR pszValue);
    
	CString GetString(HKEY hMainKey, LPSTR pszKey, LPSTR pszName);
	int GetDword(HKEY hMainKey, LPSTR pszKey, LPSTR pszName);

	long GetKeyHandle(LPCTSTR RegistryKey, LPCTSTR AppName, HKEY *hKey);
	long GetKeyHandleReadOnly(LPCTSTR RegistryKey, LPCTSTR AppName, HKEY *hKey);

	CString GetPreferencesFolder();
	bool PreferencesFileExists();
	bool CreatePreferencesFile();
	bool PreferencesFolderExists();
	bool CreatePreferencesFolder(char*);
	void InitialiseSettings(T_user*);
	bool GetTemporaryFileSettings(T_user*);
	void SaveTemporaryFileSettings(T_user*);

	void SaveTFDword(FILE*, int);
	int GetTFDword(FILE*);
	void SaveTFString(FILE*, char*, int);
	CString GetTFString(FILE*, int);
};

#endif