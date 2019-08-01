/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/*
		Filename:
			GCalc.h : main header file for the GCalc DLL
						: declaration of internal procedures used by GCalc		
		Author:
			Roger Fraser. 

		Copyright
			Department of Natural Resources (The state of Queensland, (C) 2000).

		Created: 23 June 1999.

		Build Version:		2.100
		Previous Version:	2.010

		Exported Procedures:
			GCALC_BiLinearTransformation(coordinate *pThePoint);
			GCALC_CarttoGeo(coordinate *pCartValue);
			GCALC_CreateGridIndex(char *filename, char *filetype, int *success);
			GCALC_DatumTran(coordinate *pDatumValue, double dParameters[]);
			GCALC_DegtoDms(double dNum);
			GCALC_DegtoDmin(double dNum);
			GCALC_DmstoDeg(double dNum);
			GCALC_DmintoDeg(double dNum);
			GCALC_ExportToAscii(char *gridfile, char *gridtype, char *OutputGrid, int *IO_Status);
			GCALC_ExportToBinary(char *gridfile, char *gridtype, char *OutputGrid, int *IO_Status);
			GCALC_FileTran(char *gridfile, char *gridtype, pointsfile *ptrCFile1);
			GCALC_FormatDMS_String(char *buf, double dVar1, int iType);
			GCALC_GeotoCart(coordinate *pGeoValue);
			GCALC_GeotoGrid(coordinate *pGeoValue);
			GCALC_GridtoGeo(coordinate *pGridValue);
			GCALC_MapGridTransformation(coordinate *pGridValue);
			GCALC_PointTran(char *gridfile, char *gridtype, coordinate *pGeoValue);
			GCALC_PrintLogFile(char *logfile, char *access_mode, char *logrecord);
			GCALC_ReportGridProperties(char *gridfile, char *gridtype, file_par *gridproperties, int *success);

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"					// main symbols
#include "Ellipsoid.h"					// R.Fraser 4.10.2007
#include "GCalcext.h"					// Exported function
#include "StringProc.h"
#include "ErrorMsg.h""
#include "Defines.h"

const double pi = 3.14159265359;				// pi
const double dDeg2Sec = 3600.0;					// Constant to convert DDEGREES to SECONDS
const double dRad2Sec = (180.0 / pi * 3600.0);	// Constant to convert RADIANS to SECONDS
const double dSec2Rad = (pi / 3600.0 / 180.0);	// Constant to convert SECONDS to RADIANS

// GDA20 parameters
const double GDA20a = 6378137.0;					// Semi major axis (a)
const double GDA20Inv_f = 298.257222101;			// Inverse flattening (1/f)

// GDA94 parameters
const double GDA94a = 6378137.0;					// Semi major axis (a)
const double GDA94Inv_f = 298.257222101;					// Inverse flattening (1/f)

// UTM parameters
const double FALSE_E = 500000.0;				// False Easting
const double FALSE_N = 10000000.0;				// False Northing
const double Ko = 0.9996;						// Central scale factor
const double Zw = 6.0;							// Zone width
const double LcmZ1 = -177.0;					// Longitude of central meridian of zone 1
const double LweZ0 = -186.0;					// Longitude of western edge of zone 0
const double LcmZ0 = -183.0;					// Longitude of central meridian of zone 0

const double CONST_1e15 = 1.0e-15;		// approximate machine epsilon for double precision
const double CONST_1e16 = 1.0e-16;		

///////////////////////////////////////////////////
// Declare typedef struct qifindex
typedef struct {
	char cFileRecord[151];				// Format of File Header Block "  "
	char cSubFileRec[151];				// Format of SubFile Extent Record "SX"
	long lHeaderPos;					// file position for whole file 
	long lSubHeaderPos;					// file position for sub file (SX record)
	double dHMaxEast;					// max easting for entire file
	double dHMaxNorth;					// max northing for entire file
	double dHMinEast;					// min easting for entire file
	double dHMinNorth;					// min northing for entire file
	double dSMaxEast;					// max easting for sub file
	double dSMaxNorth;					// max northing for sub file
	double dSMinEast;					// min easting for sub file
	double dSMinNorth;					// min northing for sub file
} qifindex;

///////////////////////////////////////////////////
// Declare fscanf format identifiers for distortion grid file
const char* const ENTRY = "%8s%d%*c";
const char* const SPECIAL = "%8s%8s";
const char* const ELLIPSOID = "%8s%lf%*c";
const char* const GRID_PARS = "%8s%lf%*c";
const char* const GS_COUNT = "%8s%d%*c";
const char* const SHIFTS = "%f%f%f%f";

///////////////////////////////////////////////////
// Declare fscanf format identifiers for input / output transformation files
//#define DAT_HP "%-11.11s%16.9f%16.9f%8.8s %-3.3s%-4.4s%-25.25s"
//#define DAT_ENZ "%-11.11s%16.3f%16.3f%8.8s %-3.3s%-4.4s%-25.25s"
//#define DAT_SEPDMS "%-11.11s%16s%16s%8.8s %-3.3s%-4.4s%-25.25s"
const char* const CSV_HP = "%-11s,%16.12f,%16.12f,%9s,%3.0f,%4s,%26s";
const char* const CSV_ENZ = "%-11s,%13.3f,%14.3f,%9s,%3.0f,%4s,%26s";
const char* const CSV_SEPDMS = "%-11s,%4d,%3d,%10.5lf,%4d,%3d,%10.5lf,%9s,%3.0f,%4s,%26s";
const char* const CSV_SEPDM = "%-11s,%4d,%13.9lf,%4d,%13.9lf,%9s,%3.0f,%4s,%26s";
const char* const DAT_SHIFT_ACC = "%7.2lf%7.2lf\n";
const char* const CSV_SHIFT_ACC = ",%7.2lf,%7.2lf\n";
const char* const DATE_SHORT = " %d-%b-%y %I:%M %p.";					// 24-Nov-99 11:19 AM.
const char* const DATE_LONG = " %A, %d %B %Y, %I:%M:%S %p.";			// Wednesday, 24 November 1999 11:19:35 AM.
const char* const DATE_QIF = "%d/%m/%Y";								// 24/11/1999

// CGCalcApp
// See GCalc.cpp for the implementation of this class
//

class CGCalcApp : public CWinApp
{
public:
	CGCalcApp();
	~CGCalcApp();

public:	
	void ClearGridFileMemory();										// Clears the gridfileindex struct built in memory
	void GetSystemDate(char *cBuf, const char *format);				// Prints the system date to a char array
	void PrintFileHeader(FILE *f_out,								// Prints a standard output file header to Text and CSV files
		pointsfile *ptrCFile1, char *gridfile);						
	void PrintDatFile(FILE *f_out,									// Prints output coordinates to a Text file (*.dat)
		pointsfile *ptrCFile1, coordinate *pCVar1, char *cRecord, int cRecordSize);	
	void PrintCsvFile(FILE *f_out, pointsfile *ptrCFile1,			// Prints output coordinates to a CSV file (*.csv)
		coordinate *pCVar1, LPCTSTR cPoint, LPCTSTR cHeight, 
		LPCTSTR cDescription);
	void PrintManFile(FILE *f_out, pointsfile *ptrCFile1,			// Prints output coordinates to a SCDB file (*.man)
		coordinate *pCVar1, char *pLat, char *cBuf);
	void PrintResFile(FILE *f_out,									// Prints output coordinates to a NEWGAN Res file (*.res)
		pointsfile *ptrCFile1, coordinate *pCVar1, char *cBuf, int cBufSize);
	void RoundAccuracy(double *Accuracy);							// Rounds an interpolation accuracy value to the nearest 50mm
	int ApplyDistortionGrid(coordinate *pThePoint);					// Applies the Bi-LinearInterpolation technique to a coordinate struct
	int DetermineFileType(const char *cType);						// Returns an integer value based on the file type
	int FindSubGrid();												// Finds the most dense (Sub) Grid that the point lies in
	int Interpolate();												// Interpolates the latitude and longitude distortion shifts
	int Opengridfile(char *filename, char *filetype);				// Opens the specified grid file and builds the gridfileindex struct
	int TransFilePoint(pointsfile *ptrCFile1, coordinate *pCVar1);	// Transforms a point from Text and CSV files using pointsfile struct
	BOOL ProcessCsvFile(char *cBuf, int cBufSize,					// Reads data from a CSV file (*.csv) and prints to the output file
		FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1);	
	BOOL ProcessDatFile(char *cBuf, int cBufSize,					// Reads data from a Text file (*.dat) and prints to the output file
		FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1);
	BOOL ProcessManFile(char *cBuf, int cBufSize,					// Reads data from a SCDB file (*.man) and prints to the output file
		FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1);
	BOOL ProcessQifFile(char *cBuf,	int cBufSize,					// Reads data from a QIF file (*.qif) and prints to the output file
		char *cCoord, FILE *f_out, pointsfile *ptrCFile1, 
		qifindex *pFileExtent);
	BOOL ProcessResFile(char *cBuf, int cBufSize,					// Reads data from a Res file (*.res) and prints to the output file
		FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1);
	BOOL ReadAsciiShifts(coordinate *pNShifts[],					// Reads the shift values from the specified Ascii file (*.asc)
		int iNodeIndex, long lNode);
	BOOL ReadBinaryShifts(coordinate *pNShifts[],					// Reads the shift values from the specified Binary file (*.gsb)
		int iNodeIndex, long lNode);


// Overrides
public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()

public:
	file_par		*m_pGridfile;				// Holds the parameters used for file transformations
	gridfileindex	*m_gfIndex;					// Holds the header information for the specified distortion grid
	FILE			*m_pGfileptr;				// Holds the address of the specified grid file
	bool			m_Can_Format;				// Format of Binary Grid File (true = Canadian; false = Australian)
	int				m_Grid_Success;				// Integer success value ( 0 = success; non zero = failure)
	int				m_AddHeader;				// file header switch
	double			m_dPercentComplete;			// percentage of bytes read from file
	long			m_lBytesRead;				// bytes read from file

	CEllipsoid		m_eGRS80;				// used for computations on the GRS80 ellipsoid
	CEllipsoid		m_eCustom;				// used for computations on a user defined ellipsoid
};
