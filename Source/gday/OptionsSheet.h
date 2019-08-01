#if !defined(AFX_OPTIONSSHEET_H__4138FFA7_6777_11D3_AD1C_0008C725A4CD__INCLUDED_)
#define AFX_OPTIONSSHEET_H__4138FFA7_6777_11D3_AD1C_0008C725A4CD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OptionsSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptions
#include "Page1.h"
#include "Page2.h"
#include "Page3.h"

class COptions : public CPropertySheet
{
	DECLARE_DYNAMIC(COptions)

// Construction
public:
	COptions(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
// Attributes
public:
	CPage1	m_dView;
	CPage2	m_dInteractiveOutput;
	CPage3	m_dLogFile;
	int		m_iLastPageIndex;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptions)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(COptions)
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSHEET_H__4138FFA7_6777_11D3_AD1C_0008C725A4CD__INCLUDED_)
