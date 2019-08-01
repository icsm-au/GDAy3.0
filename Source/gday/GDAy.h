// GDAy.h : main header file for the GDAY application
//

#if !defined(AFX_GDAY_H__9145B9A7_6765_11D3_AD1C_0008C725A4CD__INCLUDED_)
#define AFX_GDAY_H__9145B9A7_6765_11D3_AD1C_0008C725A4CD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Defines.h"
#include "ErrorMsg.h"
#include "FileProc.h"
#include "StringProc.h"
#include "GridProc.h"
#include "RegProc.h"

#include <GCalcext.h>

/////////////////////////////////////////////////////////////////////////////
// CGDAyApp:
// See GDAy.cpp for the implementation of this class
//
class CGDAyApp : public CWinApp
{
public:
	CGDAyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGDAyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGDAyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GDAY_H__9145B9A7_6765_11D3_AD1C_0008C725A4CD__INCLUDED_)
