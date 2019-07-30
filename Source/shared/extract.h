/////////////////////////////////////////////////////////////////////////////////////
/*
		Filename:
			extract.h
		
		Author:
			Roger Fraser. 

		Copyright
			Department of Natural Resources and Mines (The state of Queensland, (C) 2001).

		Created: 16 March 2001.

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __GRID_EXTRACTION__
#define __GRID_EXTRACTION__

#include <math.h>
#include <afx.h>							// CString support
#include <Gcalcext.h>

#define pi 3.14159265359					// pi
#define dPrec 1.0e-5						// precision constant
#define dRad2Sec (180.0 / pi * 3600.0)		// Constant to convert RADIANS to SECONDS
#define NOPARENT "NONE    \0"

#define DATE_TIME_SHORT " %d-%b-%y %I:%M %p."				// 24-Nov-99 11:19 AM.
#define DATE_TIME_LONG " %A, %d %B %Y, %I:%M:%S %p."		// Wednesday, 24 November 1999 11:19:35 AM.
#define DATE "%d/%m/%Y"										// 24/11/1999
#define DATE_BRIEF "%m%y"									// 1199

class CGridEx
{
// construction
public:
	CGridEx();

	int		m_intGridType;
	int		m_intNewGridType;
	long	m_lTotByteCount;
	double	m_dPercentComplete;
	CString	m_strSubGrid;
	CString m_strLogFilePath;
	char	m_chError_code[19][101];
	FILE	*f_log;

	int Asc2Asc(FILE *f_in, FILE *f_out, file_par *fpGrid, file_par *fpNewGrid, int i, long iGWidth, long iGHeight);
	int Asc2Bin(FILE *f_in, FILE *f_out, file_par *fpGrid, file_par *fpNewGrid, int i, long iGWidth, long iGHeight);
	int Bin2Asc(FILE *f_in, FILE *f_out, file_par *fpGrid, file_par *fpNewGrid, int i, long iGWidth, long iGHeight);
	int Bin2Bin(FILE *f_in, FILE *f_out, file_par *fpGrid, file_par *fpNewGrid, int i, long iGWidth, long iGHeight);
	int CopyGridShifts(file_par *fpGrid, file_par *fpNewGrid);
	int DetermineFileType(char *cType);
	int ReadAscShifts(FILE *f_in, float *dLatShift, float *dLonShift, float *dLatAcc, float *dLonAcc);
	int ReadBinShifts(FILE *f_in, float *dLatShift, float *dLonShift, float *dLatAcc, float *dLonAcc);
	long ReturnExtractProgress();
	void BuildErrorTable();
	void BuildNewHeaderArray(double dELon, double dWLon, double dSLat, double dNLat, file_par *fpGrid, file_par *fpNewGrid);
	void CalcGridIndex(gridfileindex *ptrHeader, gridfileindex *ptrNewHeader, int i, long *n, long *m, long *g, long *a);
	void ExtractGrid(double dELon, double dWLon, double dSLat, double dNLat, file_par *fpGrid, file_par *fpNewGrid, int *success);
	void GenerateHeader(double dELon, double dWLon, double dSLat, double dNLat, file_par *fpGrid, file_par *fpNewGrid, int *success);
	void GetSystemDate(char *cBuf, const char *format);
	void OpenLogFileAndWriteHeader(char *cGridFilePath);
	void PrintAsciiHeader(FILE *f_out, file_par *fpNewGrid, char *indent);
	void PrintAscShifts(FILE *f_out, float dLatShift, float dLonShift, float dLatAcc, float dLonAcc);
	void PrintAsciiSubheader(FILE *f_out, file_par *fpNewGrid, int index);
	void PrintBinaryHeader(FILE *f_out, file_par *fpNewGrid);
	void PrintBinShifts(FILE *f_out, float dLatShift, float dLonShift, float dLatAcc, float dLonAcc);
	void PrintBinarySubheader(FILE *f_out, file_par *fpNewGrid, int index);
	void RemoveDuplicateSubGrids(file_par *fpNewGrid, int iNumSubGrids);
	void RoundLimits(double *dELon, double *dWLon, double *dSLat, double *dNLat, file_par *fpGrid, int iSGIndex);
	int ValidateFileFormats(file_par *fpGrid, file_par *fpNewGrid);
	CString ReturnSubGridName();

};

#endif