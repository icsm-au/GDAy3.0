/////////////////////////////////////////////
// Roger Fraser

#include "stdafx.h"
#include <winbase.h>
#include <windows.h>
#include <shlobj.h>
#include <io.h>
#include <math.h>

#include "FileProc.h"
#include "StringProc.h"
#include "RegProc.h"
#include "Defines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CStrProc::CStrProc()
{

}

CStrProc::~CStrProc()
{

}

CString CStrProc::AddSymbology(CString strNumber, T_user theUser, int iFlag)
{
	int precision, decimal, isProjected, isDMS;
	CString strBuf;

	strBuf = strNumber;

	decimal = strNumber.Find('.');

	//iFlag: (1) Lat/East Input, (2) Long/North Input, (3) Lat/East Output, (4) Long/North Output

	if (iFlag == 1 || iFlag == 2)
	{
		isProjected = theUser.InputisProjected;
		isDMS = theUser.InputDMS;
		precision = strBuf.GetLength() - decimal - 1;
	}
	else
	{
		isProjected = theUser.OutputisProjected;
		isDMS = theUser.OutputDMS;
		if (theUser.OutputisProjected)
			precision = theUser.MetPrecision;
		else
		{
			if (theUser.OutputDMS == DMS)
				precision = theUser.GeoPrecision + 4;
			else if (theUser.OutputDMS == DMIN)
				precision = theUser.GeoPrecision + 2;
			// SF 19/12/17
			// Not handling DDEG, so precision was uninitialised
			else
				precision = theUser.GeoPrecision;
		}
	}

	if (theUser.AddSymbols && !isProjected && isDMS < DDEG)
	{
		// Add symbols for degrees minutes and seconds
		if (decimal > -1)
		{
			// found a decimal point!
			if (decimal == 0)				// decimal point at start, ie >>   .0123
			{
				strBuf.Insert(decimal, '0');
				decimal++;
			}

			strBuf.SetAt(decimal, ' '); 
			strBuf.Insert(decimal, '°');
						
			if (isDMS == DMS)		// ddd.mmssss
			{
				// add zero after "tens" minute or "tens" second value
				if ((iFlag == 1 || iFlag == 2) && (precision == 1 || precision == 3) )
				{
					strBuf += "0";
					precision++;
				}
											
				if (precision > 1)						// add minute symbol
					strBuf.Insert((decimal+4), "\' ");
											
				if (precision > 3)
					strBuf.Insert((decimal+8), '"');
					
				if (precision > 4)
				{
					strBuf.SetAt((decimal+8), '.');
					strBuf += "\"";
				}
			}
			else		// ddd.mmmmmm
			{
				// add zero after "tens" minute
				if ((iFlag == 1 || iFlag == 3) && precision == 1 )
				{
					strBuf += "0";
					precision++;
				}

				if (precision > 1)					// add minute symbol
					strBuf = strBuf.Mid(0, decimal+4) + "'" + strBuf.Mid(decimal+4);

				if (precision > 2)
				{
					strBuf.SetAt((decimal+4), '.');
					strBuf += "\'";
				}
			}
		}	
		else
			strBuf += "°";		// couldn't find a decimal point
	}
	else
	{
		// must be decimal degrees or projection format...simply truncate value at required precision
		if (decimal == 0)
		{
			strBuf.Insert(decimal, '0');
			decimal++;
		}
		
		strBuf = strBuf.Mid(0, decimal + precision + 1);
	}
	
	if (theUser.AddPrefix && 
		((!theUser.InputisProjected && iFlag < 3) ||
		(!theUser.OutputisProjected && iFlag > 2))
		)
	{
		// Show North and South notation
		if (strNumber[0] == '-' || strNumber[0] == 's' || strNumber[0] == 'S' || 
			strNumber[0] == 'w' || strNumber[0] == 'W')
		{
			if (iFlag == 1 || iFlag == 3)	// input/output latitude
				strBuf.SetAt(0, 'S');
			else							// input/output longitude
				strBuf.SetAt(0, 'W');
		}
		else if (theUser.LatisPosSouth && iFlag == 1)			
			strBuf = "S" + strBuf;			// input/output latitude
		else
		{
			if (iFlag == 1 || iFlag == 3)
				strBuf = "N" + strBuf;
			else
				strBuf = "E" + strBuf;
		}
		
		strBuf = strBuf.Mid(0, 1) + " " + strBuf.Mid(1);
	}

	return strBuf;
}


CString CStrProc::CoordinateOutput(long lPointNum, double dLatEast, double dLonNorth, int iZone, coordinate c, T_user theUser)
{
	CString str, buf;
	char cNum1[51], cNum2[51];

	// point number
	if (theUser.ShowPointNumbers)
		//str.Format("%-9d", lPointNum);
		str.Format("%d,", lPointNum);
	else
		//str.Format("%9s", " ");
		str.Format("%s,", " ");

	// input coordinates
	if (theUser.InputisProjected)
		//buf.Format("%17.3f%17.3f", dLatEast, dLonNorth);
		buf.Format("%.3f,%.3f,", dLatEast, dLonNorth);
	else
	{
		switch (theUser.InputDMS)
		{
		case DMS:
		case DMIN:
			GCALC_FormatDMS_String(cNum1, 50, dLatEast, theUser.InputDMS);
			GCALC_FormatDMS_String(cNum2, 50, dLonNorth, theUser.InputDMS);
			//buf.Format("%17s%17s", cNum1, cNum2);
			buf.Format("%s,%s,", cNum1, cNum2);
			break;
		default:		
			//buf.Format("%17.9f%17.9f", dLatEast, dLonNorth);
			buf.Format("%.9f,%.9f,", dLatEast, dLonNorth);
			break;
		}
	}

	str += buf;

	// datum and zone
	if (theUser.InputisProjected)
		//buf.Format("  %-7s%d  ", (theUser.InputisGDA20 ? "GDA2020" : "GDA94"), iZone);
		buf.Format("%s,%d,", (theUser.InputisGDA20 ? "GDA2020" : "GDA94"), iZone);
	else
		//buf.Format("  %-7s    ", (theUser.InputisGDA20 ? "GDA2020" : "GDA94"));
		buf.Format("%s,,", (theUser.InputisGDA20 ? "GDA2020" : "GDA94"));
	str += buf;
	
	// output coordinates
	if (theUser.OutputisProjected)
		//buf.Format("%17.3f%17.3f", c.dNum1, c.dNum2, c.dNum3);
		buf.Format("%.3f,%.3f,", c.dNum1, c.dNum2, c.dNum3);
	else
	{
		switch (theUser.OutputDMS)
		{
		case DMS:
		case DMIN:
			GCALC_FormatDMS_String(cNum1, 50, c.dNum1, theUser.OutputDMS);
			GCALC_FormatDMS_String(cNum2, 50, c.dNum2, theUser.OutputDMS);
			//buf.Format("%17s%17s", cNum1, cNum2);
			buf.Format("%s,%s,", cNum1, cNum2);
			break;
		default:		
			//buf.Format("%17.9f%17.9f", c.dNum1, c.dNum2);
			buf.Format("%.9f,%.9f,", c.dNum1, c.dNum2);
			break;
		}
	}
	
	str += buf;
	
	// datum and zone
	if (theUser.OutputisProjected)
		//buf.Format("  %-7s%2.0f   ", (theUser.OutputisGDA ? "GDA2020" : "GDA94"), c.dNum3);
		buf.Format("%s,%.0f,", (theUser.OutputisGDA ? "GDA2020" : "GDA94"), c.dNum3);
	else
		//buf.Format("  %-7s     ", (theUser.OutputisGDA ? "GDA2020" : "GDA94"));
		buf.Format("%s,,", (theUser.OutputisGDA ? "GDA2020" : "GDA94"));

	str += buf;
	
	if (theUser.AddStationName)
		//buf.Format("%-25s   ", (theUser.AutoNameAllStations ? theUser.AutoStationName : theUser.PromptStationName));
		buf.Format("\"%s\",", (theUser.AutoNameAllStations ? theUser.AutoStationName : theUser.PromptStationName));
	else
		//buf.Format("%25s   ", " ");
		buf.Format("\"%s\",", " ");

	str += buf;

	if (c.dLatacc == NULL_ACCURACY)
		//buf.Format("%s", "---- ");
		buf.Format("%s,", "----");
	else
		//buf.Format("%.2f   ", c.dLatacc);
		buf.Format("%.2f,", c.dLatacc);

	str += buf;

	if (c.dLongacc == NULL_ACCURACY)
		//buf.Format("%s", "  ----\n");
		buf.Format("%s\n", "----");
	else
		//buf.Format("%.2f \n", c.dLatacc);
		buf.Format("%.2f\n", c.dLatacc);

	str += buf;

	return str;
}

int CStrProc::GetFields(char *line, char delim, char *fmt, ...)
{
	va_list ap;
	int field[50], i, j;
	bool in_quote = false; // Used to ignore delimters in quotes

	i = j = 0;
	field[j++] = i;

	while(line[i] != '\n' && line[i] != NULL)
	{
		// if there is a quote, toggle the in_quote var
		if (line[i] == '"')
		{
			in_quote = !in_quote;
		}
		
		// replace all delimiters with NULL if not inside quotes
		if((line[i] == delim)&&(!in_quote))
		{
			line[i] = '\0';
			field[j++] = i+1;
		}
		i++;
	}
	line[i] = '\0';
	i = 0;
	va_start(ap, fmt);
	while(i < j && *fmt != '\0') {

		switch(*fmt) {
		case 'd' :
			if (&line[field[i]] != "")
				*(va_arg(ap, int *)) = atoi(&line[field[i]]);
			break;
		case 'f' :
			if (&line[field[i]] != "")
				*(va_arg(ap, double *)) = atof(&line[field[i]]);
			break;
		case 's' :
			strcpy((va_arg(ap, char *)), &line[field[i]]);
			break;
		default :
			return(0);
			break;
		}
		i++;
		fmt++;
	}
	va_end(ap);

	return(i);		// return number of fields
}

CString CStrProc::FormatString(CString strValue)
{
	int i, decimal, length;
	CString strInput;

	strInput = strValue;

	decimal = strInput.Find('.');
	length = strInput.GetLength();

	// if a decimal point is found
	if (decimal != -1)
	{
		// for all missing digits after decimal
		for (i = length; i<(decimal + 5); i++)
			strInput += "0";

		// if the decimal point was found at the start
		if (decimal == 0)
		{
			strInput.Insert(decimal, "0");
			decimal++;
		}

		strInput.SetAt(decimal, '°');
		strInput.Insert(decimal + 1, " ");
		strInput.Insert(decimal + 4, "' ");
		length = strInput.GetLength();
		if (length - decimal > 8)
			strInput.Insert(decimal + 8, ".");
		strInput += "\"";
	}
	else
		strInput += "° 00' 00\"";			// couldn't find a decimal point

	return strInput;
}
