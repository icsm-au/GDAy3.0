// GridEx.h : main header file for the GRIDEX application
//

#if !defined(AFX_GRIDEX_H__1F22E604_13CA_11D5_A1BE_00C04F88C574__INCLUDED_)
#define AFX_GRIDEX_H__1F22E604_13CA_11D5_A1BE_00C04F88C574__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "shared\Defines.h"
//#include "shared\regproc.h"
#include "shared\gridproc.h"
#include "shared\stringproc.h"
#include "shared\fileproc.h"
#include "shared\extract.h"


/////////////////////////////////////////////////////////////////////////////
// CGridExApp:
// See GridEx.cpp for the implementation of this class
//

class CGridExApp : public CWinApp
{
public:
	CGridExApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridExApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGridExApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDEX_H__1F22E604_13CA_11D5_A1BE_00C04F88C574__INCLUDED_)
