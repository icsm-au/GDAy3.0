/////////////////////////////////////////////
// Roger Fraser

#include "stdafx.h"
#include "HelpProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CHlpProc::CHlpProc()
{

}

CHlpProc::~CHlpProc()
{

}

BOOL CHlpProc::OnHelpInfo(HELPINFO* pHelpInfo, LPCTSTR lpHelpPage)
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
		return HtmlHelp((HWND)pHelpInfo->hItemHandle, lpHelpPage, HH_DISPLAY_TOC, 0)!=NULL;
	return true;
}