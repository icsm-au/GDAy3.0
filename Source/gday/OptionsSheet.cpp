// OptionsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "GDAy.h"
#include "OptionsSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptions

IMPLEMENT_DYNAMIC(COptions, CPropertySheet)

COptions::COptions(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	//{{AFX_DATA_INIT(CPropertySheet)
	m_iLastPageIndex = 0;
	//}}AFX_DATA_INIT

	AddPage(&m_dView);
	AddPage(&m_dInteractiveOutput);
	AddPage(&m_dLogFile);
}

BEGIN_MESSAGE_MAP(COptions, CPropertySheet)
	//{{AFX_MSG_MAP(COptions)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void COptions::OnHelp()
{
	LPCTSTR HelpID;

	switch (GetActiveIndex())
	{
	case 0:
		HelpID = "gday.chm::/html/UI_options.htm";
		break;
	case 1:
		HelpID = "gday.chm::/html/IO_options.htm";
		break;
	case 2:
		HelpID = "gday.chm::/html/LF_options.htm";
		break;
	default:
		HelpID = "gday.chm";
		break;
	}

	::HtmlHelp(NULL, HelpID, HH_DISPLAY_TOC, 0);
}
