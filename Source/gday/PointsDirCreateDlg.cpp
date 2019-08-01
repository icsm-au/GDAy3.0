// PointsDirCreateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "PointsDirCreateDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointsDirCreateDlg dialog


CPointsDirCreateDlg::CPointsDirCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPointsDirCreateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPointsDirCreateDlg)
	m_strDirPath = _T("");
	//}}AFX_DATA_INIT
}


void CPointsDirCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointsDirCreateDlg)
	DDX_Control(pDX, IDC_INCONQUESTION, m_IconArea);
	DDX_Text(pDX, IDC_DIR_PROMPT, m_strDirPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointsDirCreateDlg, CDialog)
	//{{AFX_MSG_MAP(CPointsDirCreateDlg)
	ON_BN_CLICKED(ID_NO, OnNo)
	ON_BN_CLICKED(ID_NOALL, OnNoall)
	ON_BN_CLICKED(ID_YES, OnYes)
	ON_BN_CLICKED(ID_YESALL, OnYesall)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointsDirCreateDlg message handlers

BOOL CPointsDirCreateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	((CStatic*)GetDlgItem(IDC_ICONQUESTION))->SetIcon(LoadIcon(NULL, IDI_QUESTION));
	m_strDirPath = "The folder  \"" + m_strDirPath + "\"  could not be found.\n\nWould you like to create it?";
	UpdateData(FALSE);
	OnInitDialogSize();	
	RepositionControls();	
	return TRUE;
}

void CPointsDirCreateDlg::OnInitDialogSize()
{	
	this->SetWindowPos(NULL, DLG_LEFT, DLG_TOP, (DLG_RIGHT - DLG_LEFT), (DLG_BOT - DLG_TOP), SWP_NOMOVE | SWP_NOZORDER);
	GetDlgItem(IDC_DIR_PROMPT)->SetWindowPos(NULL, TXT_LEFT, TXT_TOP, (TXT_RIGHT - TXT_LEFT), (TXT_BOT - TXT_TOP), SWP_NOMOVE | SWP_NOZORDER);
}

void CPointsDirCreateDlg::OnResizeDialog(int iDiffWidth, int iDiffHeight)
{
	CRect rc, bannerline;	
	this->GetWindowRect(rc);
	rc.right = rc.right + iDiffWidth;
	rc.bottom = rc.bottom + iDiffHeight;
	this->SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
	// move 6 buttons (keep central)
	OnMoveDlgCtrl(ID_YES, (iDiffWidth/2), iDiffHeight);
	OnMoveDlgCtrl(ID_YESALL, (iDiffWidth/2), iDiffHeight);
	OnMoveDlgCtrl(ID_NO, (iDiffWidth/2), iDiffHeight);
	OnMoveDlgCtrl(ID_NOALL, (iDiffWidth/2), iDiffHeight);
	OnMoveDlgCtrl(IDCANCEL, (iDiffWidth/2), iDiffHeight);
	OnMoveDlgCtrl(ID_HELP, (iDiffWidth/2), iDiffHeight);
	GetDlgItem(IDC_SEPARATOR)->GetWindowRect(bannerline);
	ScreenToClient(bannerline);
	GetDlgItem(IDC_SEPARATOR)->MoveWindow(
		bannerline.left, (bannerline.top+iDiffHeight), (bannerline.Width()+iDiffWidth), bannerline.Height());
}


void CPointsDirCreateDlg::OnMoveDlgCtrl(UINT nButtonID, int iHzOffset, int iVOffset)
{
	CRect rcButton;	
	GetDlgItem(nButtonID)->GetWindowRect(rcButton);
	ScreenToClient(rcButton);
	GetDlgItem(nButtonID)->MoveWindow(rcButton.left + iHzOffset, rcButton.top + iVOffset, rcButton.Width(), rcButton.Height());
}


void CPointsDirCreateDlg::OnCancel() 
{
	EndDialog(CANCEL);
	return;
}

void CPointsDirCreateDlg::OnNo() 
{
	EndDialog(NO);
	return;
}

void CPointsDirCreateDlg::OnNoall() 
{
	EndDialog(NOALL);
	return;
}

void CPointsDirCreateDlg::OnYes() 
{
	EndDialog(YES);
	return;
}

void CPointsDirCreateDlg::OnYesall() 
{
	EndDialog(YESALL);
	return;
}

BOOL CPointsDirCreateDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return ::HtmlHelp((HWND)pHelpInfo->hItemHandle, "gday.chm::/html/TF_dir_create.htm", HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}

void CPointsDirCreateDlg::OnHelp() 
{
	::HtmlHelp(NULL, "gday.chm::/html/TF_dir_create.htm", HH_DISPLAY_TOC, 0);
}


void CPointsDirCreateDlg::RepositionControls()
{
	CWnd *textWnd = GetDlgItem(IDC_DIR_PROMPT);
	CRect rc;
	textWnd->GetWindowRect(rc);
    ScreenToClient(rc);
	CDC *pDC = textWnd->GetDC();
	CFont *pFont = GetFont();
    CFont *pOldFont = pDC->SelectObject(pFont);
	int nDiffWdth = 0;
	int nDiffHt = 0;
	int dirs = 0;
	CString str, dir, s;

	str = m_strDirPath;
	CSize sizeText = pDC->GetTextExtent(m_strDirPath);
	sizeText.cy = pDC->DrawText(m_strDirPath, -1, rc, DT_CALCRECT | DT_WORDBREAK | DT_EXPANDTABS);
	// Does the full filename (and prompting) have a string of text between spaces
	// so long that it would go beyond 2 times the width of the parent dialog...
	// such as a filename that is greater than normal (256)???
	// If so the user needs to put their computer back in its box and take up stamp collecting.
	if (rc.Width() > (m_nParentWidth*2))
	{
		CStringArray aStr;
		while (str.GetLength() > 0)
		{
			s = str.SpanExcluding(_T(" "));
			str = str.Mid(s.GetLength());
			dir = str.SpanIncluding(_T(" "));
			str = str.Mid(dir.GetLength());
			dirs++;
			aStr.Add((LPCTSTR)s);
		}
		m_strDirPath.Empty();
		for (int i=0; i<dirs; i++)
		{
			s = aStr.GetAt(i);
			if (pDC->GetTextExtent(_T(aStr.GetAt(i))).cx > (m_nParentWidth*2))
				s.Insert((s.GetLength() / 2), ' ');			// force wordbreak
			m_strDirPath += s;
			m_strDirPath += " ";
		}
		// update text box with "broken" long strings
		UpdateData(FALSE);
		sizeText = pDC->GetTextExtent(m_strDirPath);
		sizeText.cy = pDC->DrawText(m_strDirPath, -1, rc, DT_CALCRECT | DT_WORDBREAK | DT_EXPANDTABS);	
	}

	textWnd->ReleaseDC(pDC);
	if (rc.Height() > (TXT_BOT - TXT_TOP))
		nDiffHt = (rc.bottom - rc.top) - (TXT_BOT - TXT_TOP);
	if (rc.Width() > (TXT_RIGHT - TXT_LEFT))
		nDiffWdth = (rc.right - rc.left) - (TXT_RIGHT - TXT_LEFT);
	if (nDiffWdth > 0 || nDiffHt > 0)
	{
		OnResizeDialog(nDiffWdth, nDiffHt);
		textWnd->MoveWindow(&rc, FALSE);
		textWnd->UpdateWindow();
		CenterWindow();
	}
	pDC->SelectObject(pOldFont);
}
