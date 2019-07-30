/////////////////////////////////////////////
// Roger Fraser

#ifndef     __GRID_PROCEDURES__
#define     __GRID_PROCEDURES__

#include <afx.h>
#include "RegProc.h"

class CGridProc
{
public:
	CGridProc();

	int CheckGridStatus(CString strGridFile);
	void GetTransformationParameters(CString strParam, double *dTransParams);
	void PrintGridFiletoLog(T_user theUser);
	void PrintParamstoLog(T_user theUser);
	bool GetGridExAppPath(char *pszCmdLine);
};

#endif