/////////////////////////////////////////////
// Roger Fraser

#include "stdafx.h"
#include <winbase.h>
#include <windows.h>
#include <shlobj.h>
#include <ole2.h>
#include <io.h>
#include "Defines.h"
#include "FileProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lp, LPARAM pData) 
{
	TCHAR szDir[MAX_PATH];

	switch(uMsg) 
	{
	case BFFM_INITIALIZED: 
		if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir)) 
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
		break;
	case BFFM_SELCHANGED: 
		if (SHGetPathFromIDList((LPITEMIDLIST) lp, szDir)) 
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
		break;
	default:
		break;
	}
	return 0;
}

CFileProc::CFileProc()
{
	m_fIndex = NULL;
	m_intNumFiles = 1;
	m_lTotalBytes = 0;
}

CFileProc::~CFileProc()
{
	ClearFileArrayMemory();
}

void CFileProc::BuildFileArray(char *filedir, char *filetype)
{
	char chFilter[_MAX_PATH];
	struct _finddata_t _fFile;    
	long iNumFile;
	int success, FileCount;

	sprintf(chFilter,"%s\\*.%s", filedir, filetype);
	
	// how many are there
	FileCount = 0;
	success = iNumFile = _findfirst(chFilter, &_fFile );
	
	while (success > -1)
	{
		if (_fFile.attrib != _A_SUBDIR)
			FileCount++;
		
		if (success > -1)
			success = _findnext(iNumFile, &_fFile);
	}

	// If there are none
	if (FileCount == 0)
		return;

	if (m_fIndex == NULL)
		m_fIndex = new _fileindex[FileCount + 1];
	if (m_fIndex == NULL)
		return;		// could not allocate memory

	memset(m_fIndex, '\0', sizeof(_fileindex) * FileCount);

	// iterate a second time storing the names
	success = iNumFile = _findfirst(chFilter, &_fFile );
	while (success > -1)
	{
		if (_fFile.attrib != _A_SUBDIR)
		{
			strcpy_s(m_fIndex[m_intNumFiles].chfilename, _fFile.name);
			m_intNumFiles++;
			m_lTotalBytes += _fFile.size;
		}
		if (success > -1)
			success = _findnext (iNumFile, &_fFile);
	}
	
}

void CFileProc::EnterSingleFile(char *filename)
{
	// Only a single file to be used	
	m_fIndex = new _fileindex[1];
	memset(m_fIndex, '\0', sizeof(_fileindex));

	strcpy_s(m_fIndex[m_intNumFiles].chfilename, filename);
	m_intNumFiles = 1;
}

CString CFileProc::BrowseForDialog(CString msg, HWND hwndOwner)
{
	CString strPath = "";
	LPITEMIDLIST pidlSelected = NULL;
	BROWSEINFO bi = {0};
	LPMALLOC pMalloc = NULL;	
	SHGetMalloc(&pMalloc);
	
	OleInitialize(NULL);
	
	if (pMalloc != NULL)
	{
		ZeroMemory(&bi, sizeof(bi));
		bi.hwndOwner = hwndOwner;
		bi.pidlRoot = 0;
		bi.pszDisplayName = 0;
		bi.lpszTitle = msg;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_USENEWUI | BIF_VALIDATE;
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = 0;
		pidlSelected = SHBrowseForFolder(&bi);
		if(pidlSelected)
		{
			SHGetPathFromIDList(pidlSelected, m_strDirectory);
			strPath = m_strDirectory;
		}
		pMalloc->Free(pidlSelected);
		pMalloc->Release();
	}
	return strPath;
}

CString CFileProc::FileOpenSave(BOOL OPEN_DLG, BOOL MULTI_FILE, LPCTSTR lpszDir, char *cNewFile, LPCTSTR lpszTitle, LPCTSTR lpszExt, LPCTSTR strFilter, int iFileType)
{
	char szFile[MAX_FILEPATH] = "";
	DWORD Flags;
	int i;
	
	typedef struct tagOFNA { // ofn
		DWORD         nFilterIndex; 
		LPTSTR        lpstrFile; 
		DWORD         nMaxFile; 
		LPCTSTR       lpstrInitialDir;
		LPCTSTR       lpstrTitle; 
		DWORD         Flags; 
		LPCTSTR       lpstrDefExt;
	} OPENFILENAME;

	memset(szFile, '\0', sizeof(szFile));

	if (OPEN_DLG)			// Open File
	{
		if (MULTI_FILE)
			Flags = OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;	// multi files
		else				
			Flags = OFN_HIDEREADONLY;							// single file
	}
	else					// Save File
		Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
		
	CFileDialog m_ldFile(OPEN_DLG, lpszExt, cNewFile, Flags, strFilter, NULL);
	
	if (MULTI_FILE)
	{
		m_ldFile.m_ofn.lpstrFile = szFile;
		strcpy_s(szFile, cNewFile);
		m_ldFile.m_ofn.nMaxFile = sizeof(szFile);	
	}
	else
	{
		m_ldFile.m_ofn.lpstrFile = cNewFile;
	}

	//Initialise the starting variables for the open / save as file dialog box
	m_ldFile.m_ofn.nFilterIndex = iFileType;
	m_ldFile.m_ofn.lpstrInitialDir = lpszDir;
	m_ldFile.m_ofn.lpstrTitle = lpszTitle;
	m_ldFile.m_ofn.lpstrDefExt = lpszExt;
	
	//Show the File Open dialog and return the result
	if (m_ldFile.DoModal() == IDOK)
	{
		m_strFileExt = m_ldFile.GetFileExt();
		
		m_intNumFiles = 1;
		if (MULTI_FILE)
		{
			m_intNumFiles = 0;
			POSITION pos = m_ldFile.GetStartPosition();

			while (pos != NULL)
			{
				 m_ldFile.GetNextPathName(pos);
				 m_intNumFiles++;
			}

			if (m_intNumFiles == 0)
				return "";

			ClearFileArrayMemory();
			m_fIndex = new _fileindex[m_intNumFiles + 1];
			
			if (m_fIndex == NULL)
				return "";		// could not allocate memory

			memset(m_fIndex, '\0', sizeof(_fileindex) * (m_intNumFiles + 1));

			// iterate a second time storing the names
			pos = m_ldFile.GetStartPosition();
			i = 0;
			while (pos != NULL)
			{
				 strcpy_s(m_fIndex[i].chfilename, m_ldFile.GetNextPathName(pos));
				 i ++;
			}
			
			m_strFileExt = GetExt(m_fIndex[0].chfilename);
			return m_fIndex[0].chfilename;
		}
		else
			return m_ldFile.GetPathName();
	}
	else
		return "";
}



void CFileProc::ClearFileArrayMemory()
{
	// check for existence of allocated memory for _fileindex struct
	if (m_fIndex != NULL)
	{
		// delete and NULL pointer to sruct
		delete[] m_fIndex;
		m_fIndex = NULL;
	}	
}


CString CFileProc::GetFolder(CString strFilepath)
{
	int i;
	
	i = strFilepath.ReverseFind('\\');
	
	if (i == -1)
		return "";
	else
		return strFilepath.Mid(0, i);
}

CString CFileProc::GetExt(CString strFilepath)
{
	int i;
	
	i = strFilepath.ReverseFind('.');
	
	if (i == -1)
		return "";
	else
		return strFilepath.Mid(i + 1);
}

CString CFileProc::GetName(CString strFilepath, LPCTSTR lpszExt)
{
	int i;

	i = strFilepath.ReverseFind('\\');
	
	if (i != -1)
		strFilepath = strFilepath.Mid(i+1);
	else
		return "";

	if (lpszExt == NULL)
		return strFilepath;

	i = strFilepath.ReverseFind('.');

	if (i == -1)
		strFilepath += '.' + lpszExt;
	else
		strFilepath = strFilepath.Mid(0, i+1) + lpszExt;

	return strFilepath;
}

int CFileProc::Exists(CString strFilepath, long *hFile)
{
	struct _finddata_t file_t;

	if (hFile == NULL)		// do not need handle
		return _findfirst((char*)((LPCTSTR)strFilepath), &file_t);
	if ((*hFile = _findfirst((char*)((LPCTSTR)strFilepath), &file_t)) != -1L)
		return 1;
	else
		return 0;
}

// use for points files only
int CFileProc::FilterIndex(CString strExt)
{
	if (!strExt.CompareNoCase(CSV))
		return 2;
	else if (!strExt.CompareNoCase(QIF))
		return 3;
	else if (!strExt.CompareNoCase(MAN))
		return 4;
	else if (!strExt.CompareNoCase(RES))
		return 5;
	else
		return 1;		// txt, dat, prn
}


int CFileProc::View(CString strFilepath, CString f_viewer)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	long hFile;

	if(!Exists(strFilepath, &hFile))
		return -1;

	strFilepath.Insert(0, " \"");
	strFilepath.Insert(0, f_viewer);
	strFilepath +="\"";
	
	si.cb = sizeof(si);
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW;

	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpReserved2 = NULL;
	si.cbReserved2 = 0;
	si.lpTitle = (char*)((LPCTSTR)strFilepath); 
	si.lpDesktop = (LPTSTR)NULL;
	si.dwFlags = STARTF_FORCEONFEEDBACK;

	// run process
	if (!CreateProcess(NULL, (char*)((LPCTSTR)strFilepath), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS,
						NULL, (LPTSTR)NULL, &si, &pi))		
		return 0;

	// close process and thread handles
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return 1;
}

void CFileProc::SetReadWriteAttribute(CString strFilePath, int iReadOnly)
{
	CFileStatus status;
	if (CFile::GetStatus(strFilePath, status))
	{
		if (iReadOnly)
			status.m_attribute = status.m_attribute & _A_ARCH | !_A_NORMAL;
		else
			status.m_attribute = status.m_attribute & _A_ARCH | _A_NORMAL;
		CFile::SetStatus(strFilePath, status);
	}

}