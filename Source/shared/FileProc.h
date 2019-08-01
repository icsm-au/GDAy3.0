/////////////////////////////////////////////
// Roger Fraser

#ifndef     __FILE_PROCEDURES__
#define     __FILE_PROCEDURES__

#include <afx.h>

#define MAX_FILEPATH 2048
struct _fileindex {
	char chfilename[_MAX_PATH];		// the filename array	
};

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

class CFileProc
{
// Construction
public:
	CFileProc();
	~CFileProc();

	int FilterIndex(CString strExt);
	int Exists(CString strFilepath, long *hFile);
	int View(CString strFilepath, CString f_viewer);
	void SetReadWriteAttribute(CString strFilepath, int iReadOnly);
	CString GetExt(CString strFilepath);
	CString GetFolder(CString strFilepath);
	CString GetName(CString strFilepath, LPCTSTR lpszExt);
	CString BrowseForDialog(CString strMsg, HWND hwndOwner);

	void ClearFileArrayMemory();
	void BuildFileArray(char *filedir, char *filetype);
	void EnterSingleFile(char *filename);

	CString FileOpenSave(BOOL bOpen_Save, BOOL bMulti_Files, LPCTSTR lpszDir, char *cNewFile, LPCTSTR lpszTitle, LPCTSTR lpszExt, LPCTSTR strFilter, int iFileType);
	
	char m_strDirectory[MAX_PATH];
	
	int m_intNumFiles;				// number of files to be transformed
	long m_lTotalBytes;
	CString m_strFileExt;
	_fileindex *m_fIndex;
	
	
};

#endif