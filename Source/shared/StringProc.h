/////////////////////////////////////////////
// Roger Fraser

#ifndef     __STRING_PROCEDURES__
#define     __STRING_PROCEDURES__

#include "RegProc.h"
#include "GCalcext.h"

class CStrProc
{
// Construction
public:
	CStrProc();
	~CStrProc();
	
	CString FormatString(CString strNumber);
	CString AddSymbology(CString strNumber, T_user theUser, int iFlag);
	CString CoordinateOutput(long lPointNum, double dLatEast, double dLonNorth, int iZone, coordinate c, T_user theUser);
	
	int CStrProc::GetFields(char *line, char delim, char *fmt, ...);
};


#endif