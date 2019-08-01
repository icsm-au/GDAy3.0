/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
		Filename:
			GCalc.cpp
		
		Required companion files:
			GCalc.h, GCalcext.h.
		
		Author:
			Roger Fraser. 

		Copyright
			Natural Resources and Mines (The state of Queensland, (C) 2003).

		Created:		23 June 1999.
		Last Modified:  21 August 2003.

		Build Version:		2.100
		Previous Version:	2.010

		Execution:
			To be used as an MFC statically linked (Regular) DLL. This DLL was written 
			intentionally for use by GDAy (C) 1999, which was compiled using Visual C++ 6.0,
			and has been recompiled using Visual C++ 7.0.
			This DLL exports the procedures mentioned in GCalcext.cpp, using implicit linkage to the export
			symbols contained within "GCalc.lib". "GCalcext.h" contains the declarations for all
			required typedef structs required for the use of the exported procedures. The following routines are
			used internally to support the external procedures.
			

		Procedures:
			ApplyDistortionGrid(coordinate *pThePoint);
			CGCalcApp();
			ClearGridFileMemory();
			DetermineFileType(char cType[]);
			FindSubGrid();
			GetSystemDate(char *cBuf, const char *format);
			Interpolate();
			Opengridfile(char *filename, char *filetype);
			PrintCsvFile(FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1, char *cPoint, char *cHeight, char *cDescription);
			PrintDatFile(FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1, char *cRecord);
			PrintFileHeader(FILE *f_out, pointsfile *ptrCFile1, char *gridfile);
			PrintManFile(FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1, char *pLat, char *cBuf);
			PrintResFile(FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1, char *cBuf);
			ProcessCsvFile(char *cBuf, FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1);
			ProcessDatFile(char *cBuf, FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1);
			ProcessManFile(char *cBuf, FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1);
			ProcessQifFile(char *cBuf, char *cCoord, FILE *f_out, pointsfile *ptrCFile1, qifindex *pFileExtent);
			ProcessResFile(char *cBuf, FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1);
			ReadAsciiShifts(coordinate *pNShifts[], int iNodeIndex, long lNode);
			ReadBinaryShifts(coordinate *pNShifts[], int iNodeIndex, long lNode);
			RoundAccuracy(double Accuracy);
			TransFilePoint(pointsfile *ptrCFile1, coordinate *pCVar1);
			~CGCalcApp();
				
		Acknowledgements:
			Steve Tarbit, Peter Todd, Matt Higgins, David Mitchell.
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////// GCalc.cpp : Defines the initialization routines for the DLL //////////////////////////////
#include "stdafx.h"
#include "GCalc.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef _AFXDLL
#define _AFXDLL
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CGCalcApp
BEGIN_MESSAGE_MAP(CGCalcApp, CWinApp)
END_MESSAGE_MAP()


// CGCalcApp construction
CGCalcApp::CGCalcApp()
{
	m_pGridfile = NULL;
	m_gfIndex = NULL;
	m_pGfileptr = NULL;
	m_dPercentComplete = 0.0;
	m_lBytesRead = 0;
	m_AddHeader = 0;
	m_Grid_Success = ERR_TRANS_SUCCESS;		// initialise status for all grid file operations
											// on successful exit from this function call, m_Grid_Success = 0.
}

// CGCalcApp deconstruction
CGCalcApp::~CGCalcApp()
{
	ClearGridFileMemory();
}

// The one and only CGCalcApp object
CGCalcApp theApp;

// CGCalcApp initialization
BOOL CGCalcApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// **  method definitions **

/////////////////////////////////////////////////////////////////////////
// ApplyDistortionGrid: Applies the distortion modelling technique (from the
//			  specified grid file) to a struct of geographic coordinates passed
//			  by pThePoint. This procedure calls Interpolate to retrieve the
//			  required shift values and apply the bi-linear interpolation technique.
/////////////////////////////////////////////////////////////////////////
// On Entry:  A pointer to the grid file is held by m_pGfileptr. The coordinates
//			  for the point to be transformed are passed by pThePoint. These values
//			  must be geographic coordinates in decimal degrees. The pointer to
//			  gridfile must point to a valid file before this procedure is called.
// On Exit:   The transformed point is returned by cThePoint.
/////////////////////////////////////////////////////////////////////////
int CGCalcApp::ApplyDistortionGrid(coordinate *pThePoint)
{
	CString msg;
	double dBuf1, dBuf2;			// retains original input values in seconds
	int i;
	int iResult;					// grid file array index # for most dense sub grid

	// Convert input lat and long to seconds
	dBuf1 = m_pGridfile->dLat = pThePoint->dNum1 * dDeg2Sec;
	dBuf2 = m_pGridfile->dLong = pThePoint->dNum2 * -dDeg2Sec;

	// Initialise success...
	pThePoint->IO_Status = ERR_TRANS_SUCCESS;
	
	// forward transformation occurs only once, reverse transformation occurs (i) times
	for(i=0; i<5; i++)
	{
		iResult = FindSubGrid();				// Find the most dense Sub Grid
		if (iResult == ERR_FINDSUBGRID_OUTSIDE)
		{
			// Point is outside limits of the grid
			pThePoint->IO_Status = ERR_PT_OUTSIDE_GRID;		            // return GDAy error status
			return pThePoint->IO_Status;
		}
		else if (iResult == ERR_FINDSUBGRID_GRIDERR)
		{	
			// There is an error in the grid
			pThePoint->IO_Status = ERR_GRIDFILE_ERROR;		            // return GDAy error status
			return pThePoint->IO_Status;
		}

		if ((pThePoint->IO_Status = Interpolate()) != ERR_TRANS_SUCCESS)		// an error occurred
			return pThePoint->IO_Status;
	
		if (pThePoint->iNum4 == GDA94)			// if it is a forward transformation (GDA94 to GDA2020)
			break;								// forward trans...end for loop
		else
		{
			// reverse transformation (GDA2020 to GDA94)...subtract shifts from original data
			m_pGridfile->dLat = dBuf1 - m_pGridfile->dLat;
			m_pGridfile->dLong = dBuf2 - m_pGridfile->dLong;
		}
	}

	if (pThePoint->iNum4 == GDA94)
	{
		// Input data is in GDA94...add to original data
		pThePoint->dNum1 = dBuf1 + m_pGridfile->dLat;	
		pThePoint->dNum2 = dBuf2 + m_pGridfile->dLong;			
	}
	else if (pThePoint->iNum4 == GDA2020)
	{
		// Input data is in GDA2020...copy values
		pThePoint->dNum1 = m_pGridfile->dLat;	
		pThePoint->dNum2 = m_pGridfile->dLong;			
	}

	// restore to decimal degrees
	pThePoint->dNum1 /= dDeg2Sec;
	pThePoint->dNum2 /= -dDeg2Sec;

	// compute lat & long shift accuracy
	if (m_pGridfile->dLatacc != NULL_ACCURACY)
	{
		pThePoint->dLatacc = m_pGridfile->dLatacc * pi * m_pGridfile->daf / (dDeg2Sec * 180.0);		// convert to metres
		RoundAccuracy(&pThePoint->dLatacc);			// round accuracy to nearest 50mm
	}
	else
		pThePoint->dLatacc = m_pGridfile->dLatacc;
	
	if (m_pGridfile->dLongacc != NULL_ACCURACY)
	{
		pThePoint->dLongacc = cos(pThePoint->dNum1 * pi / 180) * m_pGridfile->dLongacc * pi * m_pGridfile->daf / (dDeg2Sec * 180.0);	// convert to metres
		RoundAccuracy(&pThePoint->dLongacc);		// round accuracy to nearest 50mm
	}
	else
		pThePoint->dLongacc = m_pGridfile->dLongacc;

	return pThePoint->IO_Status;
}

/////////////////////////////////////////////////////////////////////////
// ClearGridFileMemory: Closes an open grid file pointer and deletes memory 
//				created for sub grid header struct and array.
/////////////////////////////////////////////////////////////////////////
// On Entry:	A valid file pointer, and memory for the sub grid header
//				struct and array may or may not exist.
// On Exit:		An open grid file pointer is closed and any memory 
//				created for sub grid header struct and array is deleted.
/////////////////////////////////////////////////////////////////////////
void CGCalcApp::ClearGridFileMemory()
{
	if (m_pGfileptr != NULL)	// is file pointer to distortion grid file open?
	{
		fclose(m_pGfileptr);
		m_pGfileptr = NULL;
	}
	if (m_pGridfile != NULL)	// check for existence of allocated memory for gridfileindex struct
	{
		delete m_pGridfile;
		m_pGridfile = NULL;
	}
	if (m_gfIndex != NULL)		// check for existence of allocated memory for file_par struct
	{
		delete[] m_gfIndex;
		m_gfIndex = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////
// DetermineFileType: Determines the distortion grid's file type from a char array.
//
/////////////////////////////////////////////////////////////////////////
// On Entry:   The grid file type is passed by cType.
// On Exit:    The grid file type is compared against "asc" or "gsb".
//			   An integer is returned indicating the filetype.
/////////////////////////////////////////////////////////////////////////
int CGCalcApp::DetermineFileType(const char *cType)
{
	if (_stricmp(cType, ASC) == 0)				// asc "ASCII" file
		return TYPE_ASC;			
	else if (_stricmp(cType, GSB) == 0)			// gsb "Binary" file
		return TYPE_GSB;			
	else if ( (_stricmp(cType, TXT) == 0) ||	// dat/txt/prn file
			  (_stricmp(cType, DAT) == 0) ||
			  (_stricmp(cType, PRN) == 0) )
		return TYPE_DAT;
	else if (_stricmp(cType, CSV) == 0)			// csv file
		return TYPE_CSV;
	else if (_stricmp(cType, QIF) == 0)			// qif "Queensland Interchange Format" file
		return TYPE_QIF;
	else if (_stricmp(cType, MAN) == 0)			// man "SCDB" file
		return TYPE_MAN;
	else if (_stricmp(cType, "asd") == 0)		// asd "Topographic Mapping" file
		return 6;			
	else if (_stricmp(cType, RES) == 0)			// res "Newgan" file
		return TYPE_RES;			
	else
		return -1;								// Unsupported filetype
}

/////////////////////////////////////////////////////////////////////////
// FindSubGrid: Finds the most dense (sub) grid for interpolating
//				the lat and long shifts. This procedure succeeds for
//				multiple density grid files when the input point is contained
//				within the limits of the most dense sub grid. Tests are made to
//				ensure that only one sub grid can be selected. 
/////////////////////////////////////////////////////////////////////////
// On Entry:  The header information for the first sub grid file is held by
//			  m_pGridfile. The Input lat and long are stored in seconds.
// On Exit:   If the point is within the limits of the gridfile, the most dense grid
//			  is returned. -1 is returned if the point is ourside all (sub) grids.
/////////////////////////////////////////////////////////////////////////
int CGCalcApp::FindSubGrid()
{	
	double dPrec = 1.0e-5;					// buffer to trap points on or close to the limits
	double dBuf1, dBuf2, dBuf3, dBuf4;
	char cCurrentGrid[9];
	strcpy_s(cCurrentGrid, _countof(cCurrentGrid), "NONE    \0");		// initial parent grid set to NONE
	int i, iCurrentFlag, iPrevFlag;
	int iGridSuccess, iInSubGrid;
	int iPointInGrid;

	// initialise flags
	m_pGridfile->iTheGrid = ERR_FINDSUBGRID_OUTSIDE;
	m_pGridfile->iPointFlag = 0;
	iPointInGrid = 0;
	iGridSuccess = false;

	while (!iGridSuccess)
	{
		// initialise flags
		iPrevFlag = iInSubGrid = iCurrentFlag = 0;

		// loop through all subgrids in grid file array
		for (i=0; i<m_pGridfile->iNumsubgrids; i++)
		{
			if (strcmp(cCurrentGrid, m_pGridfile->ptrIndex[i].chParent) == 0)
			{
				// This grid has the same parent name.
				// Test whether point lies within the grid's limits
				// Use buffer precision of 1.0e-5 for lat & long to eliminate double & float precision mismatch
				dBuf1 = fabs(m_pGridfile->dLat - m_pGridfile->ptrIndex[i].dNlat);
				if (dBuf1 < dPrec)
					dBuf1 = 0.0;
				dBuf3 = fabs(m_pGridfile->dLat - m_pGridfile->ptrIndex[i].dSlat);
				if (dBuf3 < dPrec)
					dBuf3 = 0.0;
				dBuf2 = fabs(m_pGridfile->dLong - m_pGridfile->ptrIndex[i].dWlong);
				if (dBuf2 < dPrec)
					dBuf2 = 0.0;
				dBuf4 = fabs(m_pGridfile->dLong - m_pGridfile->ptrIndex[i].dElong);
				if (dBuf4 < dPrec)
					dBuf4 = 0.0;
					
				if ( ((m_pGridfile->dLat < m_pGridfile->ptrIndex[i].dNlat) || (dBuf1 == 0.0)) &&
					 ((m_pGridfile->dLat > m_pGridfile->ptrIndex[i].dSlat) || (dBuf3 == 0.0)) &&
					 ((m_pGridfile->dLong < m_pGridfile->ptrIndex[i].dWlong) || (dBuf2 == 0.0)) &&
					 ((m_pGridfile->dLong > m_pGridfile->ptrIndex[i].dElong) || (dBuf4 == 0.0)) )
				{
					// The point lies within the grid limits...test whether point lies on upper limits.					
					if (dBuf1 != 0.0 && dBuf2 != 0.0)
						iCurrentFlag = 1;			// The point is within the limits
					if (dBuf1 == 0.0 && dBuf2 != 0.0)
						iCurrentFlag = 2;			// The point is on the upper latitude limit (only)
					if (dBuf1 != 0.0 && dBuf2 == 0.0)
						iCurrentFlag = 3;			// The point is on the upper longitude limit (only)
					if (dBuf1 == 0.0 && dBuf2 == 0.0)
						iCurrentFlag = 4;			// The point is on both upper latitude and longitude limits
						
					if (iInSubGrid == 0)
					{
						// This is the first sub grid found
						iPrevFlag = iCurrentFlag;
						m_pGridfile->iTheGrid = i;
					}
					else if (iCurrentFlag < iPrevFlag)
					{
						// The 2nd or n'th grid found with the same parent that is adjacent to the previous sub grid.
						// Use flags to determine precedence.
						iPrevFlag = iCurrentFlag;
						m_pGridfile->iTheGrid = i;
					}
					else if (iCurrentFlag == iPrevFlag)
					{
						// ERROR...a point cannot be in the same location for two different sub grids
						m_pGridfile->iTheGrid = ERR_FINDSUBGRID_GRIDERR;
					}
					iInSubGrid = 1;
				}
			}
		}
		
		if (iInSubGrid == 0)
			iGridSuccess = true;	// No sub grids were found in the current parent grid name, or that contain the point
		else
		{
			// A sub grid was found to contain the point
			// cCurrentGrid becomes the sub file's SUB_NAME...continue loop
			strcpy_s(cCurrentGrid, _countof(cCurrentGrid), m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].chSubname);
			m_pGridfile->iPointFlag = iPrevFlag;
			iPointInGrid = 1;
		}
	}

	// The point was not in any grid
	if (iPointInGrid == 0)
		m_pGridfile->iTheGrid = ERR_FINDSUBGRID_OUTSIDE;

	// return the index # for the most dense (sub) grid in the grid file array
	return m_pGridfile->iTheGrid;
}

/////////////////////////////////////////////////////////////////////////
// GetSystemDate: Retrieves the system date based on am local time.
/////////////////////////////////////////////////////////////////////////
// On Entry:  cBuf is an empty char array. format is the format specifier for
//			  the date style.
// On Exit:   The system date/time is printed to cBuf.
/////////////////////////////////////////////////////////////////////////
void CGCalcApp::GetSystemDate(char *cBuf, const char *format)
{
	struct tm *today;
	time_t ltime;	
	time(&ltime);
	today = localtime(&ltime);
	strftime(cBuf, 100, format, today);
}

/////////////////////////////////////////////////////////////////////////
// Interpolate: Interpolates the latitude and longitude distortion shifts
//				from the specified grid file. 
/////////////////////////////////////////////////////////////////////////
// On Entry:  The information contained about the grid file and the coordinates to be
//			  interpolated are held by m_pGridfile. These values are in seconds.
// On Exit:   The lat and long shift values specific to the user input coordinates
//			  are interpolated and applied to the input coordinates. Interpolate
//			  returnes 0 on success.
/////////////////////////////////////////////////////////////////////////
int CGCalcApp::Interpolate()
{
	double dLatA, dLongA, dX, dY, da0, da1, da2, da3;
	double dPrec = 1.0e-5;
	long iA, iB, iC, iD, iRow_Index, iColumn_Index, iGrid_Points_Per_Row;
	int iFtype;
	CString msg;

	// declare a coordinate struct for the four values at each node
	coordinate cNodeShifts[5];
	coordinate *pNShifts[5];

	int i;

	// set up pointers
	for (i=0; i<4; i++)
		pNShifts[i] = &(cNodeShifts[i]);

	////////////  1. Retrieve Grid Shifts  (see Page 18 of the NTv2 Developer's Guide) ////////////
	// Compute Row_Index of Grid Node A...
	iRow_Index = (int)((m_pGridfile->dLat - m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dSlat + dPrec) / m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dLatinc);

	// Compute Column_Index of Grid Node A...
	iColumn_Index = (int)((m_pGridfile->dLong - m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dElong + dPrec) / m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dLonginc);

	// Compute Grid_Points_Per_Row (No of Columns)
	iGrid_Points_Per_Row = (int)((m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dWlong - m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dElong + dPrec) /
		m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dLonginc);
	iGrid_Points_Per_Row++;

	// Compute Record Index for nodes A, B, C and D.
	iA = (iRow_Index * iGrid_Points_Per_Row) + iColumn_Index + 1;			// Lower Right node
	iB = iA + 1;															// Lower Left node
	iC = iA + iGrid_Points_Per_Row;											// Upper Right node
	iD = iC + 1;															// Upper Left node 

	// Assign the correct grid node number depending on limit flag set in FindSubGrid
	// (see Page 19 & 20 of the NTv2 Developer's Guide)
	switch (m_pGridfile->iPointFlag)
	{
	case 1:
		// point is within the limits...no virtual cell is required
		break;
	case 2:
		// point is on the upper latitude limit
		iC = iA;				// Upper Right node becomes Lower Right node
		iD = iB;				// Upper Left node becomes Lower Left node
		break;
	case 3:
		// point is on the upper longitude limit
		iB = iA;				// Lower Left node becomes Lower Right node
		iD = iC;				// Upper Left node becomes Upper Right node
		break;
	case 4:
		// point is on both upper latitude and upper longitude limits
		iB = iA;				// Lower Left node becomes Lower Right node
		iC = iA;				// Upper Right node becomes Lower Right node
		iD = iA;				// Upper Left node becomes Lower Right node
		break;
	}	

	// determine grid file type...ASCII = 0; Binary = 1.
	iFtype = DetermineFileType(m_pGridfile->filetype);
	if (iFtype < 0 || iFtype > 1)
		return ERR_GRIDFILE_TYPE;

	// initialise pNShifts index
	i = 0;

	// Retrieve the values at the above four grid nodes.
	if (iFtype == 0)
	{	
		if (!(ReadAsciiShifts(pNShifts, i, iA)))			// Node A
			return ERR_READ_ASC_SHIFT;		// Could not retrieve shifts from Ascii file

		if (!(ReadAsciiShifts(pNShifts, i+1, iB)))			// Node B
			return ERR_READ_ASC_SHIFT;		// Could not retrieve shifts from Ascii file
	}
	else if (iFtype == 1)
	{
		if (!(ReadBinaryShifts(pNShifts, i, iA)))			// Node A
			return ERR_READ_BIN_SHIFT;		// Could not retrieve shifts from Binary file

		if (!(ReadBinaryShifts(pNShifts, i+1, iB)))			// Node B
			return ERR_READ_BIN_SHIFT;		// Could not retrieve shifts from Binary file
	}

	i += 2;

	if (iFtype == 0)
	{	
		if (!(ReadAsciiShifts(pNShifts, i, iC)))			// Node C
			return ERR_READ_ASC_SHIFT;		// Could not retrieve shifts from Ascii file

		if (!(ReadAsciiShifts(pNShifts, i+1, iD)))			// Node D
			return ERR_READ_ASC_SHIFT;		// Could not retrieve shifts from Ascii file
	}
	else if (iFtype == 1)
	{
		if (!(ReadBinaryShifts(pNShifts, i, iC)))			// Node C
			return ERR_READ_BIN_SHIFT;		// Could not retrieve shifts from Binary file

		if (!(ReadBinaryShifts(pNShifts, i+1, iD)))			// Node D
			return ERR_READ_BIN_SHIFT;		// Could not retrieve shifts from Binary file
	}
	
	////////////  2. Interpolate Grid Shifts  (see Page 21 of the NTv2 Developer's Guide) ////////////
	// Compute Coordinates of Grid Node A
	dLatA = m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dSlat + (iRow_Index * m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dLatinc);
	dLongA = m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dElong + (iColumn_Index * m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dLonginc);

	// Compute interpolation scale factors
	dY = (m_pGridfile->dLat - dLatA) / m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dLatinc;
	dX = (m_pGridfile->dLong - dLongA) / m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].dLonginc;

	///////////// latitude /////////////////////
	// Compute latitude interpolation parameters
	da0 = pNShifts[0]->dNum1;
	da1 = pNShifts[1]->dNum1 - pNShifts[0]->dNum1;
	da2 = pNShifts[2]->dNum1 - pNShifts[0]->dNum1;
	da3 = pNShifts[0]->dNum1 + pNShifts[3]->dNum1 - pNShifts[1]->dNum1 - pNShifts[2]->dNum1;

	// Interpolate final latitude value
	m_pGridfile->dLat = da0 + (da1 * dX) + (da2 * dY) + (da3 * dX * dY);

	// Compute latitude "accuracy" interpolation parameters
	// if any of the accuracy values are equal to -1, 
	// then it is not possible to interpolate the accuracy for the point
	if (pNShifts[0]->dLatacc != -1.0 &&
		pNShifts[1]->dLatacc != -1.0 &&
		pNShifts[2]->dLatacc != -1.0 &&
		pNShifts[3]->dLatacc != -1.0)
	{		
		da0 = pNShifts[0]->dLatacc;
		da1 = pNShifts[1]->dLatacc - pNShifts[0]->dLatacc;
		da2 = pNShifts[2]->dLatacc - pNShifts[0]->dLatacc;
		da3 = pNShifts[0]->dLatacc + pNShifts[3]->dLatacc - pNShifts[1]->dLatacc - pNShifts[2]->dLatacc;

		// Interpolate final latitude "accuracy" value
		m_pGridfile->dLatacc = da0 + (da1 * dX) + (da2 * dY) + (da3 * dX * dY);
	}
	else
		m_pGridfile->dLatacc = NULL_ACCURACY;
	
	///////////// longitude /////////////////////
	// Compute latitude interpolation parameters
	da0 = pNShifts[0]->dNum2;
	da1 = pNShifts[1]->dNum2 - pNShifts[0]->dNum2;
	da2 = pNShifts[2]->dNum2 - pNShifts[0]->dNum2;
	da3 = pNShifts[0]->dNum2 + pNShifts[3]->dNum2 - pNShifts[1]->dNum2 - pNShifts[2]->dNum2;

	// Interpolate final latitude value
	m_pGridfile->dLong = da0 + (da1 * dX) + (da2 * dY) + (da3 * dX * dY);

	// Compute latitude "accuracy" interpolation parameters
	// if any of the accuracy values are equal to -1, 
	// then it is not possible to interpolate the accuracy for the point
	if (pNShifts[0]->dLongacc != -1.0 &&
		pNShifts[1]->dLongacc != -1.0 &&
		pNShifts[2]->dLongacc != -1.0 &&
		pNShifts[3]->dLongacc != -1.0)
	{		
		da0 = pNShifts[0]->dLongacc;
		da1 = pNShifts[1]->dLongacc - pNShifts[0]->dLongacc;
		da2 = pNShifts[2]->dLongacc - pNShifts[0]->dLongacc;
		da3 = pNShifts[0]->dLongacc + pNShifts[3]->dLongacc - pNShifts[1]->dLongacc - pNShifts[2]->dLongacc;

		// Interpolate final latitude "accuracy" value
		m_pGridfile->dLongacc = da0 + (da1 * dX) + (da2 * dY) + (da3 * dX * dY);
	}
	else
		m_pGridfile->dLongacc = NULL_ACCURACY;

	// Return success
	return ERR_TRANS_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////
// OpenGridFile: Opens the specified distortion grid file and reads in the
//				 header information pertinent to the grid file format. An array
//				 of sub grid header information is built from the entire file.
//				 This procedure is transparent to all outside applications other
//				 than this dll.
/////////////////////////////////////////////////////////////////////////
// On Entry:  The grid file name and file type is held by filename and filetype.
// On Exit:   The grid file is opened and all header information is read 
//			  and returned by fGridFile. If the grid file does not meet the
//			  standard NTv2 grid file format, or contains values that are un-
//			  recognised, OpenGridFile returns fGridfile with a values
//			  corresponding to the nature of the error and its file location.
/////////////////////////////////////////////////////////////////////////
int CGCalcApp::Opengridfile(char *filename, char *filetype)
{
	#define IDENT_BUF 8
	#define HEADER_RECORD 100
	int i, gridtype, iLineLength;
	char access_mode[4], identifier[IDENT_BUF + 1], cBuf[HEADER_RECORD + 1];
	fpos_t fPos;

	m_pGridfile = new file_par;
	memset(m_pGridfile, 0, sizeof(file_par));
	strcpy_s(m_pGridfile->filename, _countof(m_pGridfile->filename), filename);
	strcpy_s(m_pGridfile->filetype, filetype);
	
	iLineLength = 4 * sizeof(float);
	m_Can_Format = true;			// assume file will be in Canadian format
	
	// Initialise Success
	gridtype = DetermineFileType(m_pGridfile->filetype);

	// test for type of grid file, then assign appropriate access mode...
	if (gridtype == TYPE_ASC)
		strcpy_s(access_mode, _countof(access_mode), "r");			// file type is ASCII
	else if (gridtype == TYPE_GSB)
		strcpy_s(access_mode, _countof(access_mode), "rb");			// file type is Binary
	else 
		return ERR_GRIDFILE_TYPE;

	// Open the distortion grid file
	//if ((m_pGfileptr = fopen(m_pGridfile->filename, access_mode)) == NULL)
	//	return ERR_GRIDFILE_READ;
	if (fopen_s(&m_pGfileptr, m_pGridfile->filename, access_mode) != 0)
		return ERR_GRIDFILE_READ;
	
	// read in all Overview Header information
	if (gridtype == 0)		// ascii
	{
		if ( (fscanf_s(m_pGfileptr, ENTRY, &identifier, &m_pGridfile->iH_info)) < 2)				// NUM_OREC
			return ERR_GRID_CORRUPT;
		if ( (fscanf_s(m_pGfileptr, ENTRY, &identifier, &m_pGridfile->iSubH_info)) < 2)			// NUM_SREC
			return ERR_GRID_CORRUPT;
		if ( (fscanf_s(m_pGfileptr, ENTRY, &identifier, &m_pGridfile->iNumsubgrids)) < 2)			// NUM_FILE
			return ERR_GRID_CORRUPT;
		
		fgets(cBuf, HEADER_RECORD, m_pGfileptr);												// GS_TYPE
			strncpy_s(m_pGridfile->chGs_type, _countof(m_pGridfile->chGs_type), cBuf + IDENT_BUF, IDENT_BUF);
		fgets(cBuf, HEADER_RECORD, m_pGfileptr);												// VERSION
			strncpy_s(m_pGridfile->chVersion, _countof(m_pGridfile->chVersion), cBuf + IDENT_BUF, IDENT_BUF);
		fgets(cBuf, HEADER_RECORD, m_pGfileptr);												// SYSTEM_F
			strncpy_s(m_pGridfile->chSystem_f, _countof(m_pGridfile->chSystem_f), cBuf + IDENT_BUF, IDENT_BUF);
		fgets(cBuf, HEADER_RECORD, m_pGfileptr);												// SYSTEM_T
			strncpy_s(m_pGridfile->chSystem_t, _countof(m_pGridfile->chSystem_t), cBuf + IDENT_BUF, IDENT_BUF);
			
		if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_pGridfile->daf)) < 2)				// MAJOR_F
			return ERR_GRID_CORRUPT;
		if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_pGridfile->dbf)) < 2)				// MINOR_F
			return ERR_GRID_CORRUPT;
		if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_pGridfile->dat)) < 2)				// MAJOR_T
			return ERR_GRID_CORRUPT;
		if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_pGridfile->dbt)) < 2)				// MINOR_T
			return ERR_GRID_CORRUPT;
	}
	else					// binary
	{
		fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
		if (fread(&m_pGridfile->iH_info, sizeof(int), 1, m_pGfileptr) != 1)						// NUM_OREC
			return ERR_GRID_CORRUPT;

		fread(&identifier, sizeof(char), 4, m_pGfileptr);										// "NUM_" or "    "

		// Test for Australian/Canadian Binary
		if (strncmp("NUM_", identifier, 4) == 0)
		{
			// Australian Binary format
			fread(&identifier, sizeof(char), 4, m_pGfileptr);
			if (fread(&m_pGridfile->iSubH_info, sizeof(int), 1, m_pGfileptr) != 1)				// NUM_SREC
				return ERR_GRID_CORRUPT;		
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_pGridfile->iNumsubgrids, sizeof(int), 1, m_pGfileptr) != 1)			// NUM_FILE
				return ERR_GRID_CORRUPT;
			m_Can_Format = false;
		}
		else
		{
			// Canadian Binary format
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_pGridfile->iSubH_info, sizeof(int), 1, m_pGfileptr) != 1)				// NUM_SREC
				return ERR_GRID_CORRUPT;		
			if (fseek(m_pGfileptr, 4, SEEK_CUR) != 0)											// Skip padding (4 bytes of null)
				return ERR_GRID_CORRUPT;		
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_pGridfile->iNumsubgrids, sizeof(int), 1, m_pGfileptr) != 1)			// NUM_FILE
				return ERR_GRID_CORRUPT;		
			if (fseek(m_pGfileptr, 4, SEEK_CUR) != 0)											// Skip padding (4 bytes of null)
				return ERR_GRID_CORRUPT;
		}

		fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
		if (fread(&m_pGridfile->chGs_type, sizeof(char), IDENT_BUF, m_pGfileptr) != IDENT_BUF)	// GS_TYPE
			return ERR_GRID_CORRUPT;
		fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
		if (fread(&m_pGridfile->chVersion, sizeof(char), IDENT_BUF, m_pGfileptr) != IDENT_BUF)	// VERSION
			return ERR_GRID_CORRUPT;
		fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
		if (fread(&m_pGridfile->chSystem_f, sizeof(char), IDENT_BUF, m_pGfileptr) != IDENT_BUF)	// SYSTEM_F
			return ERR_GRID_CORRUPT;
		fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
		if (fread(&m_pGridfile->chSystem_t, sizeof(char), IDENT_BUF, m_pGfileptr) != IDENT_BUF)	// SYSTEM_T
			return ERR_GRID_CORRUPT;
		fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
		if (fread(&m_pGridfile->daf, sizeof(double), 1, m_pGfileptr) != 1)						// MAJOR_F
			return ERR_GRID_CORRUPT;
		fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
		if (fread(&m_pGridfile->dbf, sizeof(double), 1, m_pGfileptr) != 1)						// MINOR_F
			return ERR_GRID_CORRUPT;
		fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
		if (fread(&m_pGridfile->dat, sizeof(double), 1, m_pGfileptr) != 1)						// MAJOR_T
			return ERR_GRID_CORRUPT;
		fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
		if (fread(&m_pGridfile->dbt, sizeof(double), 1, m_pGfileptr) != 1)						// MINOR_T
			return ERR_GRID_CORRUPT;
	}
	
	// create header file array
	m_gfIndex = new gridfileindex[m_pGridfile->iNumsubgrids];
	memset(m_gfIndex, 0, sizeof(gridfileindex)*m_pGridfile->iNumsubgrids);

	for (i=0; i<m_pGridfile->iNumsubgrids; i++)
	{	
		if (gridtype == 0)		// ascii
		{
			fgets(cBuf, HEADER_RECORD, m_pGfileptr);											// SUB_NAME
			strncpy_s(m_gfIndex[i].chSubname, _countof(m_gfIndex[i].chSubname), cBuf + IDENT_BUF, IDENT_BUF);
			fgets(cBuf, HEADER_RECORD, m_pGfileptr);											// PARENT
			strncpy_s(m_gfIndex[i].chParent, _countof(m_gfIndex[i].chParent), cBuf + IDENT_BUF, IDENT_BUF);
			fgets(cBuf, HEADER_RECORD, m_pGfileptr);											// UPDATED
			strncpy_s(m_gfIndex[i].chCreated, _countof(m_gfIndex[i].chCreated), cBuf + IDENT_BUF, IDENT_BUF);
			fgets(cBuf, HEADER_RECORD, m_pGfileptr);											// CREATED
			strncpy_s(m_gfIndex[i].chUpdated, _countof(m_gfIndex[i].chUpdated), cBuf + IDENT_BUF, IDENT_BUF);
			
			if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_gfIndex[i].dSlat)) < 2)		// S_LAT
				return ERR_GRID_CORRUPT;
			if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_gfIndex[i].dNlat)) < 2)		// N_LAT
				return ERR_GRID_CORRUPT;
			if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_gfIndex[i].dElong)) < 2)		// E_LONG
				return ERR_GRID_CORRUPT;
			if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_gfIndex[i].dWlong)) < 2)		// W_LONG
				return ERR_GRID_CORRUPT;
			if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_gfIndex[i].dLatinc)) < 2)		// LAT_INC
				return ERR_GRID_CORRUPT;
			if ( (fscanf_s(m_pGfileptr, ELLIPSOID, &identifier, &m_gfIndex[i].dLonginc)) < 2)		// LONG_INC
				return ERR_GRID_CORRUPT;
			if ( (fscanf_s(m_pGfileptr, GS_COUNT, &identifier, &m_gfIndex[i].lGscount)) < 2)		// GS_COUNT
				return ERR_GRID_CORRUPT;

			// Save ASCII position in grid file for first record of lat & long shifts 
			if (fgetpos(m_pGfileptr, &m_gfIndex[i].fpGridPos) != 0)
				return ERR_GRIDFILE_READ;
			
			// Set the file position after the GS_COUNT number...which is the start of the next sub file.
			fPos = m_gfIndex[i].fpGridPos + (m_gfIndex[i].lGscount * 42);
			
			if (fsetpos(m_pGfileptr, &fPos) != 0)			// End of file encountered first???
				return ERR_GRIDFILE_READ;		
		}
		else					// binary
		{
		
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].chSubname, sizeof(char), IDENT_BUF, m_pGfileptr) != IDENT_BUF)		// SUB_NAME
				return ERR_GRID_CORRUPT;
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].chParent, sizeof(char), IDENT_BUF, m_pGfileptr) != IDENT_BUF)		// PARENT
				return ERR_GRID_CORRUPT;
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].chCreated, sizeof(char), IDENT_BUF, m_pGfileptr) != IDENT_BUF)		// UPDATED
				return ERR_GRID_CORRUPT;
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].chUpdated, sizeof(char), IDENT_BUF, m_pGfileptr) != IDENT_BUF)		// CREATED
				return ERR_GRID_CORRUPT;	
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].dSlat, sizeof(double), 1, m_pGfileptr) != 1)						// S_LAT
				return ERR_GRID_CORRUPT;
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].dNlat, sizeof(double), 1, m_pGfileptr) != 1)						// N_LAT
				return ERR_GRID_CORRUPT;
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].dElong, sizeof(double), 1, m_pGfileptr) != 1)						// E_LONG
				return ERR_GRID_CORRUPT;
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].dWlong, sizeof(double), 1, m_pGfileptr) != 1)						// W_LONG
				return ERR_GRID_CORRUPT;
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].dLatinc, sizeof(double), 1, m_pGfileptr) != 1)						// LAT_INC
				return ERR_GRID_CORRUPT;
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].dLonginc, sizeof(double), 1, m_pGfileptr) != 1)						// LONG_INC
				return ERR_GRID_CORRUPT;
			fread(&identifier, sizeof(char), IDENT_BUF, m_pGfileptr);
			if (fread(&m_gfIndex[i].lGscount, sizeof(int), 1, m_pGfileptr) != 1)						// GS_COUNT
				return ERR_GRID_CORRUPT;

			if (m_Can_Format)			// Canadian binary format?
			{
				if (fseek(m_pGfileptr, 4, SEEK_CUR) != 0)												// Skip padding (4 bytes of null)
					return ERR_GRIDFILE_READ;		
			}
		
			// get Binary position in grid file for first record of lat & long shifts 
			m_gfIndex[i].lGridPos = ftell(m_pGfileptr);
	
			// End of file encountered first???
			if ( fseek(m_pGfileptr, (m_gfIndex[i].lGscount * iLineLength), SEEK_CUR) != 0)
				return ERR_GRIDFILE_READ;
		}

		if (strcmp(m_pGridfile->chGs_type, "RADIANS ") == 0)
		{
			// convert all radians values to seconds
			m_gfIndex[i].dSlat *= dRad2Sec;						// S_LAT
			m_gfIndex[i].dNlat *= dRad2Sec;						// N_LAT
			m_gfIndex[i].dElong *= dRad2Sec;					// E_LONG
			m_gfIndex[i].dWlong *= dRad2Sec;					// W_LONG
			m_gfIndex[i].dLatinc *= dRad2Sec;					// LAT_INC
			m_gfIndex[i].dLonginc *= dRad2Sec;					// LONG_INC
		}
	}

	m_pGridfile->ptrIndex = m_gfIndex;		// copy pointer to sub file array	
	return 0;
}

/////////////////////////////////////////////////////////////////////////
// PrintCsvFile: Prints the final coordinates to the specified output files.
//				 PrintCsvFile may be called by ProcessCsv/Dat/Man/ResFile, and is
//				 only called when the specified output file type is dat or csv.
/////////////////////////////////////////////////////////////////////////
// On Entry:  The transformed coordinates are held by pCVar1. f_out must point to a valid
//			  output file for writing. Output file information is held by ptrCFile1.
// On Exit:   If further transformation is required (ie to Map Grid), the coordinates
//			  are transformed. The point ID, coordinates, height etc are printed to the
//			  specified output file.
/////////////////////////////////////////////////////////////////////////
void CGCalcApp::PrintCsvFile(FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1, 
							LPCTSTR cPoint, LPCTSTR cHeight, LPCTSTR cDescription)
{
	int iLatDeg, iLatMin, iLonDeg, iLonMin;
	double dLat, dLon;
	const char* format_string;
	CString strDatum;

	coordinate cTemp;									// temporary struct for calculating output zone
	
	// get output datum
	if (ptrCFile1->OutputDatum == GDA94)
	{
		pCVar1->iNum4 = GDA94;
		strDatum = "G94 ";
	}
	else
	{
		pCVar1->iNum4 = GDA2020;
		strDatum = "G20 ";
	}

	// produce output zone. GeotoGrid will calculate output zone unless (47 < OutputProj < 58) has been specified
	cTemp.dNum1 = pCVar1->dNum1;
	cTemp.dNum2 = pCVar1->dNum2;
	cTemp.dNum3 = ptrCFile1->OutputProj;
	cTemp.iNum4 = pCVar1->iNum4;	
	GCALC_GeotoGrid(&cTemp);
	pCVar1->dNum3 = cTemp.dNum3;
	
	// if output coordinates are projected, copy temp values
	if (ptrCFile1->OutputProj > GEOGRAPHIC)
	{
		pCVar1->dNum1 = cTemp.dNum1;
		pCVar1->dNum2 = cTemp.dNum2;
	}
	else
	{
		// output is geographic...format appropriately for degrees minutes seconds / degrees minutes
		if (ptrCFile1->OutputDMS == DMS)
		{
			// convert to degrees minutes seconds
			pCVar1->dNum1 = GCALC_DegtoDms(pCVar1->dNum1);
			pCVar1->dNum2 = GCALC_DegtoDms(pCVar1->dNum2);
		}	
		else if (ptrCFile1->OutputDMS == DMIN)
		{
			// convert to degrees minutes
			pCVar1->dNum1 = GCALC_DegtoDmin(pCVar1->dNum1);
			pCVar1->dNum2 = GCALC_DegtoDmin(pCVar1->dNum2);
		}
	}

	// print to file
	if ((ptrCFile1->OutputDMS == DMS || ptrCFile1->OutputDMS == DMIN) &&
		ptrCFile1->OutputHP == !HP_NOTATION && ptrCFile1->OutputProj == GEOGRAPHIC)
		// Geographic degrees, minutes, seconds and degrees, minutes (separated DMS fields)
	{
		// convert latitude to (int)degrees, (int)minutes and (double)seconds
		dLat = fabs(pCVar1->dNum1);
		iLatDeg = (int)floor(dLat);
		iLatMin = (int)floor(((dLat - iLatDeg) * 100.0) + 0.0001);
		
		// convert longitude to (int)degrees, (int)minutes and (double)seconds
		dLon = fabs(pCVar1->dNum2);
		iLonDeg = (int)floor(dLon);
		iLonMin = (int)floor(((dLon - iLonDeg) * 100.0) + 0.0001);
		
		// print Degrees, minutes seconds (separated) to "csv" file
		if (ptrCFile1->OutputDMS == DMS)
		{
			// d,mm,ss.ssss,,
			fprintf(f_out, CSV_SEPDMS, cPoint,
				((pCVar1->dNum1 < 0.0) ? -iLatDeg : iLatDeg), iLatMin, ((((dLat - iLatDeg) * 100.0) - iLatMin) * 100.0), 
				((pCVar1->dNum2 < 0.0) ? -iLonDeg : iLonDeg), iLonMin, ((((dLon - iLonDeg) * 100.0) - iLonMin) * 100.0),
				cHeight, pCVar1->dNum3, strDatum, cDescription);
		}
		else
		{
			// d,m.mmmm,,,
			fprintf(f_out, CSV_SEPDM, cPoint, 
				((pCVar1->dNum1 < 0.0) ? -iLatDeg : iLatDeg), ((dLat - iLatDeg) * 100.0), 
				((pCVar1->dNum2 < 0.0) ? -iLonDeg : iLonDeg), ((dLon - iLonDeg) * 100.0),
				cHeight, pCVar1->dNum3, strDatum, cDescription);
		}
	}
	else
	{
		// print HP Notation / Decimal degrees or Map Grid to Output file
		if (ptrCFile1->OutputProj == GEOGRAPHIC)
			format_string = CSV_HP;
		else
			format_string = CSV_ENZ;
		fprintf(f_out, format_string, cPoint, pCVar1->dNum1, pCVar1->dNum2,
			cHeight, pCVar1->dNum3, strDatum, cDescription);
	}

	if (pCVar1->dLatacc == NULL_ACCURACY || pCVar1->dLongacc == NULL_ACCURACY)
		fprintf(f_out, ",    -- ,    --\n");
	else
	{
		// swap accuracy values if output is projected
		if (ptrCFile1->OutputProj > GEOGRAPHIC)
			fprintf(f_out, CSV_SHIFT_ACC, pCVar1->dLongacc, pCVar1->dLatacc);
		else
			fprintf(f_out, CSV_SHIFT_ACC, pCVar1->dLatacc, pCVar1->dLongacc);
	}
}

/////////////////////////////////////////////////////////////////////////
// PrintDatFile: Prints the final coordinates to the specified output files.
//				 PrintDatFile may be called by ProcessCsv/Dat/Man/ResFile, and is
//				 only called when the specified output file type is dat or csv.
/////////////////////////////////////////////////////////////////////////
// On Entry:  The transformed coordinates are held by pCVar1. f_out must point to a valid
//			  output file for writing. Output file information is held by ptrCFile1.
// On Exit:   If further transformation is required (ie to Map Grid), the coordinates
//			  are transformed. The point ID, coordinates, height etc are printed to the
//			  specified output file.
/////////////////////////////////////////////////////////////////////////
void CGCalcApp::PrintDatFile(FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1, char *cRecord, int cRecordSize)
{
	CString strLat, strLon;
	int i, precision;

	coordinate cTemp;									// temporary struct for calculating output zone
	
	// get output datum
	if (ptrCFile1->OutputDatum == GDA94)
	{
		pCVar1->iNum4 = GDA94;
		memcpy(cRecord+55, " G94", 4);
	}
	else
	{
		pCVar1->iNum4 = GDA2020;
		memcpy(cRecord+55, " G20", 4);
	}

	// produce output zone. GeotoGrid will calculate output zone unless (47 < OutputProj < 58) has been specified
	cTemp = *pCVar1;
	cTemp.dNum3 = ptrCFile1->OutputProj;
	GCALC_GeotoGrid(&cTemp);

	// print zone
	pCVar1->dNum3 = cTemp.dNum3;
	_snprintf_s(cRecord+52, cRecordSize, 3, "%3.0f ", pCVar1->dNum3);
	
	// test whether output coordinates are projected
	if (ptrCFile1->OutputProj > GEOGRAPHIC)
		*pCVar1 = cTemp;	// copy temp values
	else
	{
		// output is geographic...test whether in degrees minutes seconds or degrees minutes
		if (ptrCFile1->OutputDMS == DMS)
		{
			// convert to degrees minutes seconds
			pCVar1->dNum1 = GCALC_DegtoDms(pCVar1->dNum1);
			pCVar1->dNum2 = GCALC_DegtoDms(pCVar1->dNum2);
		}	
		else if (ptrCFile1->OutputDMS == DMIN)
		{
			// convert to degrees minutes
			pCVar1->dNum1 = GCALC_DegtoDmin(pCVar1->dNum1);
			pCVar1->dNum2 = GCALC_DegtoDmin(pCVar1->dNum2);
		}
	}

	if ((ptrCFile1->OutputDMS == DMS || ptrCFile1->OutputDMS == DMIN) &&
		ptrCFile1->OutputHP == !HP_NOTATION && ptrCFile1->OutputProj == GEOGRAPHIC)
	{
		// convert latitude to separated degrees minutes and seconds CString
		strLat.Format("%.9f", pCVar1->dNum1);
		i = strLat.Find('.');
		strLat.Replace('.', ' ');
		if (ptrCFile1->OutputDMS == DMS)												// d mm ss.ssss
			strLat = strLat.Mid(0, i+3) + " " + strLat.Mid(i+3, 2) + "." + strLat.Mid(i+5);
		else																			// d mm.mmmmmm
			strLat = strLat.Mid(0, i+3) + "." + strLat.Mid(i+3);

		// convert longitude to separated degrees minutes and seconds CString
		strLon.Format("%.9f", pCVar1->dNum2);
		i = strLon.Find('.');
		strLon.Replace('.', ' ');
		if (ptrCFile1->OutputDMS == DMS)												// d mm ss.ssss
			strLon = strLon.Mid(0, i+3) + " " + strLon.Mid(i+3, 2) + "." + strLon.Mid(i+5);
		else																			// d mm.mmmmmm
			strLon = strLon.Mid(0, i+3) + "." + strLon.Mid(i+3);

		_snprintf_s(cRecord+11, cRecordSize, 32, "%16s%16s", strLat, strLon);
	}
	else
	{
		// print HP Notation, Map Grid, or Decimal degrees to Output file
		if (ptrCFile1->OutputProj == GEOGRAPHIC)
			precision = 9;
		else
			precision = 3;

		_snprintf_s(cRecord+11, cRecordSize, 32, "%16.*f%16.*f", precision, pCVar1->dNum1, precision, pCVar1->dNum2);
	}

	if (pCVar1->dLatacc == NULL_ACCURACY || pCVar1->dLongacc == NULL_ACCURACY)
		sprintf_s(cRecord+85, cRecordSize, "    --     --");
	else
	{
		// swap accuracy values if output is projected (ie. so Latacc is equivalent to East
		if (ptrCFile1->OutputProj > GEOGRAPHIC)
			sprintf_s(cRecord+85, cRecordSize, "%7.2lf%7.2lf", pCVar1->dLongacc, pCVar1->dLatacc);
		else
			sprintf_s(cRecord+85, cRecordSize, "%7.2lf%7.2lf", pCVar1->dLatacc, pCVar1->dLongacc);
	}

	fprintf(f_out, "%s\n", cRecord);
}

/////////////////////////////////////////////////////////////////////////
// PrintFileHeader: Prints a file header at the beginning of dat and csv files.
//					The header lists the system date, grid file, and input and output
//					files.
/////////////////////////////////////////////////////////////////////////
// On Entry:  The grid file name is held by gridfile. f_out must point to a valid
//			  output file for writing. Input and output files are held by ptrCFile1.
// On Exit:   The header is printed.
/////////////////////////////////////////////////////////////////////////
void CGCalcApp::PrintFileHeader(FILE *f_out, pointsfile *ptrCFile1, char *gridfile)
{
	char cHeader[96], cBuf[111];
	CString strBuf, strCoords;
	int i;		
	
	// initialise input coordinate string
	if (ptrCFile1->InputDatum == GDA94)
	{
		if (ptrCFile1->InputProj == GEOGRAPHIC)
			strCoords = "Geographic coordinates (GDA94)";
		else
			strCoords = MGA94_STR;
	}
	else
	{
		if (ptrCFile1->InputProj == GEOGRAPHIC)
			strCoords = "Geographic coordinates (GDA2020)";
		else
			strCoords = MGA20_STR;
	}
	
	memset(cHeader, '/', sizeof(cHeader));
	sprintf_s(cHeader+93, _countof(cHeader), "\n\n");

	if (ptrCFile1->chAppName[0] == '\0' || ptrCFile1->chAppName[1] == '\0' || ptrCFile1->chAppName[2] == '\0')
		strcpy_s(ptrCFile1->chAppName, _countof(ptrCFile1->chAppName), "GCalc.dll File Output");

	strBuf = ptrCFile1->chAppName;
	strBuf.TrimLeft();
	strBuf.TrimRight();
	strBuf = "        " + strBuf + "        ";
	i = (int)((93 - strlen(strBuf))/2);
	_snprintf_s(cHeader+i, _countof(cHeader), (int)strlen(strBuf), "%s", strBuf);
	
	GetSystemDate(cBuf,  DATE_LONG);	// get system date and time

	strBuf.Format("%s\tCreated:  %s\n\n\tInput File:     %s\n\tCoordinates:    %s\n\n\tOutput File:    %s\n\tCoordinates:    ",
		cHeader, cBuf, ptrCFile1->chFilein, strCoords, ptrCFile1->chFileout);		
	
	// initialise output coordinate string
	if (ptrCFile1->OutputDatum == GDA94)
	{
		if (ptrCFile1->OutputProj == GEOGRAPHIC)
			strCoords = "Geographic coordinates (GDA94)";
		else
			strCoords = "Map Grid of Australia (MGA94)";
	}
	else
	{
		if (ptrCFile1->OutputProj == GEOGRAPHIC)
			strCoords = "Geographic coordinates (GDA2020)";
		else
			strCoords = "Map Grid of Australia (MGA2020)";
	}
	
	strBuf += strCoords + "\n\n\tGrid File:      " + gridfile + "\n\n\tGCalc.dll    Version 3.00";
	strBuf += "\n//////////////////////////////////////////////////////////////////////////////////////////\n\n\n";

	// print header info
	fprintf(f_out, "%s", strBuf);
}

/////////////////////////////////////////////////////////////////////////
// PrintManFile: Prints the final coordinates to the specified man file.
//				 PrintFilePoint is only called by ProcessManFile, and is
//				 only called when the specified output file type is man.
/////////////////////////////////////////////////////////////////////////
// On Entry:  The transformed coordinates are held by pCVar1. f_out must point to a valid
//			  output file for writing. Output file information is held by ptrCFile1.
// On Exit:   The Output coordinates are formatted, and map grid coordinates are calculated.
//			  The point ID, coordinates, height etc are printed to the
//			  specified output file.
/////////////////////////////////////////////////////////////////////////
void CGCalcApp::PrintManFile(FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1, char *pLat, char *cBuf)
{
	double dDMS, dDeg, dMin, dSec;
	char *pComma;
	int i;
	CString strLat, strLon, strBuf;
	
	// convert latitude decimal degrees to degrees minutes seconds
	dDMS = -1 * GCALC_DegtoDms(pCVar1->dNum1);		// compute a negative value (assumes man is +ve south lat)
	dDeg = floor(dDMS);
	dMin = floor(((dDMS - dDeg) * 100) + 0.0001);
	dSec = (((dDMS - dDeg) * 100) - dMin) * 100;	
	strLat.Format("%.0f %.0f %.4f", dDeg, dMin, dSec);
	
	// append spaces for single digit numbers (ie. # < 10)
	if (dDeg < 10.)
		strLat += ' ';
	if (dMin < 10.)
		strLat += ' ';
	if (dSec < 9.99995)
		strLat += ' ';	
	
	// convert longitude
	dDMS = GCALC_DegtoDms(pCVar1->dNum2);	
	dDeg = floor(dDMS);
	dMin = floor(((dDMS - dDeg) * 100) + 0.0001);
	dSec = (((dDMS - dDeg) * 100) - dMin) * 100;
	strLon.Format("%.0f %.0f %.4f", dDeg, dMin, dSec);
	
	// append spaces for single digit numbers (ie. # < 10)
	if (dMin < 10.)
		strLon += ' ';
	if (dSec < 9.99995)
		strLon += ' ';	
	
	pComma = pLat;

	for (i=0; i<6; i++)
		pComma = strchr(pComma, ',') + 1;

	pCVar1->iNum4 = ptrCFile1->OutputDatum;		// get output datum
	pCVar1->dNum3 = 1;							// initialise zone (force zone to be calculated)
	GCALC_GeotoGrid(pCVar1);					// convert to projection coordinates

	// copy projection coordinates to buffer
	strBuf.Format("%s%s,%s,%11.3f,%11.3f,%6.0f,%s               ,%s               ,%s",
		cBuf, strLat, strLon, pCVar1->dNum1, pCVar1->dNum2, pCVar1->dNum3,
		((ptrCFile1->OutputDatum == GDA2020) ? "GDA2020" : "GDA94"), pComma);
	
	fprintf(f_out, "%s", strBuf);
}


/////////////////////////////////////////////////////////////////////////
// PrintResFile: Prints a final coordinates to the specified output file.
//				 PrintResFile may be called by ProcessResFile, and is
//				 only called when the specified output file type is res.
/////////////////////////////////////////////////////////////////////////
// On Entry:  The transformed coordinates are held by pCVar1. f_out must point to a valid
//			  output file for writing. Output file information is held by ptrCFile1.
// On Exit:   If further transformation is required (ie to Map Grid), the coordinates
//			  are transformed. The point ID, coordinates, height etc are printed to the
//			  specified output file.
/////////////////////////////////////////////////////////////////////////
void CGCalcApp::PrintResFile(FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1, char *cBuf, int cBufSize)
{
	double dDMS, dDeg, dMin, dSec;
	
	if (ptrCFile1->OutputProj > GEOGRAPHIC)
	{
		// convert geographic to projection
		pCVar1->dNum3 = ptrCFile1->OutputProj;
		pCVar1->iNum4 = ptrCFile1->OutputDatum;
		GCALC_GeotoGrid(pCVar1);
		_snprintf_s(cBuf+39, cBufSize, 31, "%s%2.0f  %13.3f%13.3f",
			((ptrCFile1->OutputDatum == GDA94) ? "U" : "A"),
			pCVar1->dNum3, pCVar1->dNum1, pCVar1->dNum2);
	}
	else
	{
		// convert latitude decimal degrees to degrees minutes seconds
		dDMS = -1 * GCALC_DegtoDms(pCVar1->dNum1);		// compute a negative value (assumes man is +ve south lat)
		dDeg = floor(dDMS);
		dMin = floor(((dDMS - dDeg) * 100) + 0.0001);
		dSec = (((dDMS - dDeg) * 100) - dMin) * 100;

		_snprintf_s(cBuf+39, cBufSize, 16, "%s%2.0f  %13.3f%13.3f",
			((pCVar1->dNum1 < 0) ? "S" : "N"),
			pCVar1->dNum3, pCVar1->dNum1, pCVar1->dNum2);
	
		// convert longitude
		dDMS = GCALC_DegtoDms(pCVar1->dNum2);	
		dDeg = floor(dDMS);
		dMin = floor(((dDMS - dDeg) * 100) + 0.0001);
		dSec = (((dDMS - dDeg) * 100) - dMin) * 100;

		// copy longitude to buffer
		_snprintf_s(cBuf + 45, cBufSize, 15, "%.0f %2.0f%9.6f", dDeg, dMin, dSec);
	}

	// test for additional data after lat and long fields
	if ((int)strlen(cBuf) < 71)
		memcpy(cBuf + 70, "\n\0", 2);

	fprintf(f_out, "%s", cBuf);
}


/////////////////////////////////////////////////////////////////////////
// ProcessCsvFile: Extracts the required data from a single record of a csv file.
//				 The input coordinates are transformed via TransFilePoint, and
//				 then the new record is printed to the output file via PrintFilePoint.
/////////////////////////////////////////////////////////////////////////
// On Entry:  A single record from a csv file is held by cBuf. f_out must point to a
//			  valid output file for writing. Output file information is held by ptrCFile1.
// On Exit:   The record is read and the input coordinates are stored in pCVar1.
//			  The coordinates are transformed and printed to the specified output file.
/////////////////////////////////////////////////////////////////////////
BOOL CGCalcApp::ProcessCsvFile(char *cBuf, int cBufSize, FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1)
{
	char cRecord[151], cPoint[16], cHeight[11], cDatum[8], cDescription[101];
	int iDeg1, iMin1, iDeg2, iMin2, count, success, iFiletype;
	double dMin1, dMin2, dSec1, dSec2, dZone;

	CStrProc str;

	strcpy_s(cRecord, _countof(cRecord), cBuf);

	ptrCFile1->IO_Status = ERR_TRANS_SUCCESS;

	memset(cPoint, '\0', sizeof(cPoint));
	memset(cDatum, '\0', sizeof(cDatum));
	memset(cHeight, '\0', sizeof(cHeight));
	memset(cDescription, '\0', sizeof(cDescription));

	iDeg1 = iMin1 = iDeg2 = iMin2 = -999;
	dMin1 = dMin2 = dSec1 = dSec2 = -999.0;
	pCVar1->dNum1 = pCVar1->dNum1 = 0.0;
	pCVar1->dNum3 = dZone = -1;
	
	// dZone is used only as a temporary variable and is not used
	// the output zone is determined from ptrCFile1->OutputProj

	if (ptrCFile1->InputProj == GEOGRAPHIC && ptrCFile1->InputHP == !HP_NOTATION && ptrCFile1->InputDMS == DMS)
	{
		// Geographic, d° mm ss.ss", separated fields
		count = str.GetFields(cRecord, ',', "sddfddfsfss", cPoint,
			&iDeg1, &iMin1, &dSec1, &iDeg2, &iMin2, &dSec2, cHeight, &dZone, cDatum, cDescription);

		if (count < 7 ||
			iDeg1 == -999 || iMin1 == -999 || iDeg2 == -999 || iMin2 == -999 || dSec1 == -999.0 || dSec2 == -999.0)
		{
			// insufficient fields for a valid csv record
			ptrCFile1->IO_Status = ERR_NUM_CSV_FIELDS;
			return false;
		}
		else if (iDeg1 == 0 && iMin1 == 0 && iDeg2 == 0 && iMin2 == 0 && dSec1 == 0.0 && dSec2 == 0.0)
		{
			// No data contained within line just read
			ptrCFile1->IO_Status = ERR_NODATA_READ;
			return false;
		}

		if (iDeg1 > 0)
			pCVar1->dNum1 = iDeg1 + (iMin1/100.0) + (dSec1/10000.0);
		else
			pCVar1->dNum1 = iDeg1 - (iMin1/100.0) - (dSec1/10000.0);

		if (iDeg2 > 0)
			pCVar1->dNum2 = iDeg2 + (iMin2/100.0) + (dSec2/10000.0);
		else
			pCVar1->dNum2 = iDeg2 - (iMin2/100.0) - (dSec2/10000.0);
	}
	else if (ptrCFile1->InputProj == GEOGRAPHIC && ptrCFile1->InputHP == !HP_NOTATION && ptrCFile1->InputDMS == DMIN)
	{
		// Geographic, d° mm.mm', separated fields
		count = str.GetFields(cBuf, ',', "sdfdfsfss", cPoint,
			&iDeg1, &dMin1, &iDeg2, &dMin2, cHeight, &dZone, cDatum, cDescription);
		
		if (count < 5 ||
			iDeg1 == -999 || dMin1 == -999 || iDeg2 == -999 || dMin2 == -999)
		{
			// insufficient fields for a valid csv record
			ptrCFile1->IO_Status = ERR_NUM_CSV_FIELDS;
			return false;
		}
		else if (iDeg1 == 0 && dMin1 == 0 && iDeg2 == 0 && dMin2 == 0)
		{
			// No data contained within line just read
			ptrCFile1->IO_Status = ERR_NODATA_READ;
			return false;
		}
	
		if (iDeg1 > 0)
			pCVar1->dNum1 = iDeg1 + (dMin1/100.0);
		else
			pCVar1->dNum1 = iDeg1 - (dMin1/100.0);

		if (iDeg2 > 0)
			pCVar1->dNum2 = iDeg2 + (dMin2/100.0);
		else
			pCVar1->dNum2 = iDeg2 - (dMin2/100.0);
	}
	else
	{
		// geographic (d.dddd° or d.mmssss) or projection
		count = str.GetFields(cBuf, ',', "sffsfss", cPoint,
			&pCVar1->dNum1, &pCVar1->dNum2, cHeight, &dZone, cDatum, cDescription);
		
		if (count < 3 || pCVar1->dNum1 == -999 || pCVar1->dNum2 == -999 ||
			(ptrCFile1->InputProj != GEOGRAPHIC && dZone == -1.0))
		{
			// insufficient fields for a valid csv record
			ptrCFile1->IO_Status = ERR_NUM_CSV_FIELDS;
			return false;
		}
		else if (pCVar1->dNum1 == 0.0 && pCVar1->dNum2 == 0.0)
		{
			// No data contained within line just read
			ptrCFile1->IO_Status = ERR_NODATA_READ;
			return false;
		}

		if (ptrCFile1->InputProj != GEOGRAPHIC)
			pCVar1->dNum3 = dZone;
	}

	//  send captured point data to TransFilePoint
	success = TransFilePoint(ptrCFile1, pCVar1);

	if (success == ERR_TRANS_SUCCESS || success == ERR_PT_OUTSIDE_GRID)
	{
		iFiletype = DetermineFileType(ptrCFile1->chFOutputType);

		// Print to file
		if (iFiletype == TYPE_DAT)									// dat, txt, prn
		{			
			sprintf_s(cRecord, _countof(cRecord), "%-11s%*s%9s%*s%26s", cPoint, 32, " ", cHeight, 7, " ", cDescription);
			PrintDatFile(f_out, ptrCFile1, pCVar1, cRecord, _countof(cRecord));
		}
		else if (iFiletype == TYPE_CSV)							// csv
			PrintCsvFile(f_out, ptrCFile1, pCVar1, cPoint, cHeight, cDescription);
		
		// increment number of "successfully" transformed coordinates.
		ptrCFile1->lNumTrans[0] ++;
		// increment number of coordinates transformed using distortion grid
		if (success == ERR_TRANS_SUCCESS)
			ptrCFile1->lNumTrans[2] ++;
	}

	if (success == ERR_TRANS_SUCCESS)
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////
// ProcessDatFile: Extracts the required data from a single record of a dat file.
//				 The input coordinates are transformed via TransFilePoint, and
//				 then the new record is printed to the output file via PrintFilePoint.
/////////////////////////////////////////////////////////////////////////
// On Entry:  A single record from a dat file is held by cBuf. f_out must point to a
//			  valid output file for writing. Output file information is held by ptrCFile1.
// On Exit:   The record is read and the input coordinates are stored in pCVar1.
//			  The coordinates are transformed and printed to the specified output file.
/////////////////////////////////////////////////////////////////////////
BOOL CGCalcApp::ProcessDatFile(char *cBuf, int cBufSize, FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1)
{
	int success, iFiletype, iLength;
	char cRecord[151], cValues[40];
	CString strRec;
	double dDeg1, dDeg2, dMin1, dMin2, dSec1, dSec2;
		
	ptrCFile1->IO_Status = ERR_TRANS_SUCCESS;
	iLength = (int)strlen(cBuf);

	if (iLength < 11)
	{
		// No data contained within line just read
		ptrCFile1->IO_Status = ERR_NODATA_READ;
		return false;
	} 
	else if (iLength < 30)
	{
		// Line is too short to contain valid coordinates
		ptrCFile1->IO_Status = ERR_LINE_TOO_SHORT;
		return false;
	}

	sprintf_s(cRecord, _countof(cRecord), "%*s", 120, " ");
	memcpy(cRecord, cBuf, iLength-1);
	strRec = cRecord;
	
	// Extract Lat/East & Long/North values into char array & check for valid input
	sprintf_s(cValues, _countof(cValues), "%s %s", (LPCTSTR)strRec.Mid(11, 16), (LPCTSTR)strRec.Mid(27, 16));
	for (int i=0; cValues[i] != '\0'; i++)
	{
		if ((cValues[i] > 0 && cValues[i] < 32) ||
			(cValues[i] > 32 && cValues[i] < 45) || 
			cValues[i] == 47 || cValues[i] > 57 )
		{
			// Line does not contain valid numeric input
			ptrCFile1->IO_Status = ERR_INVALID_INPUT;
			return false;
		}
	}
	
	// Only read zone if input is projected. Output zone is calculated unless specified by ptrCFile1->OutputProj.
	if (ptrCFile1->InputProj == PROJECTED)
	{
		if ((pCVar1->dNum3 = atof(strRec.Mid(52, 3))) == 0.0)
		{
			// Invalid Zone...cannot transform to geographic from grid coords
			ptrCFile1->IO_Status = ERR_INVALID_ZONE;
			return false;
		}
	}

	// check for non valid/numeric input	
	if (ptrCFile1->InputDMS == DMS && ptrCFile1->InputHP == !HP_NOTATION && ptrCFile1->InputProj == GEOGRAPHIC)
	{
		// ddd° mm' ss.ssss"
		if (sscanf_s(cValues, "%lf%lf%lf%lf%lf%lf", &dDeg1, &dMin1, &dSec1, &dDeg2, &dMin2, &dSec2) != 6)
		{
			// Could not read required data...possibly wrong format
			ptrCFile1->IO_Status = ERR_READING_DATA;
			return false;
		}

		dMin1 /= 100.0;
		dSec1 /= 10000.0;
		dMin2 /= 100.0;
		dSec2 /= 10000.0;
				
		// convert separated fields to single field values (ie. into HP notation)
		if (dDeg1 < 0)
			pCVar1->dNum1 = dDeg1 - dMin1 - dSec1;
		else
			pCVar1->dNum1 = dDeg1 + dMin1 + dSec1;		
		if (dDeg2 < 0)
			pCVar1->dNum2 = dDeg2 - dMin2 - dSec2;
		else
			pCVar1->dNum2 = dDeg2 + dMin2 + dSec2;

	}
	else if (ptrCFile1->InputDMS == DMIN && ptrCFile1->InputHP == !HP_NOTATION && ptrCFile1->InputProj == GEOGRAPHIC)
	{
		// ddd° mm.mmmm'
		if (sscanf_s(cValues, "%lf%lf%lf%lf", &dDeg1, &dMin1, &dDeg2, &dMin2) != 4)
		{
			// Could not read required data...possibly wrong format
			ptrCFile1->IO_Status = ERR_READING_DATA;
			return false;
		}

		dMin1 /= 100.0;
		dMin2 /= 100.0;
				
		// convert separated fields to single field values (ie. into HP notation)
		if (dDeg1 < 0)
			pCVar1->dNum1 = dDeg1 - dMin1;
		else
			pCVar1->dNum1 = dDeg1 + dMin1;		
		if (dDeg2 < 0)
			pCVar1->dNum2 = dDeg2 - dMin2;
		else
			pCVar1->dNum2 = dDeg2 + dMin2;

	}
	else
	{
		// ddd.mmssss, ddd.dddddd, or ENENENEN.ENE
		if (sscanf_s(cValues, "%lf%lf", &pCVar1->dNum1, &pCVar1->dNum2) != 2)
		{
			// Could not read required data...possibly wrong format
			ptrCFile1->IO_Status = ERR_READING_DATA;
			return false;
		}
	}

	//  send captured point data to TransFilePoint
	success = TransFilePoint(ptrCFile1, pCVar1);

	if (success == ERR_TRANS_SUCCESS || success == ERR_PT_OUTSIDE_GRID)
	{
		iFiletype = DetermineFileType(ptrCFile1->chFOutputType);
		
		// Print to file
		if (iFiletype == TYPE_DAT)										// dat, txt, prn
			PrintDatFile(f_out, ptrCFile1, pCVar1, cRecord, _countof(cRecord));
		else if (iFiletype == TYPE_CSV)									// csv
			PrintCsvFile(f_out, ptrCFile1, pCVar1, (LPCTSTR)strRec.Mid(0, 11), (LPCTSTR)strRec.Mid(43, 9), (LPCTSTR)strRec.Mid(59, 26));
		
		// increment number of "successfully" transformed coordinates.
		ptrCFile1->lNumTrans[0] ++;
		// increment number of coordinates transformed using distortion grid
		if (success == ERR_TRANS_SUCCESS)
			ptrCFile1->lNumTrans[2] ++;
	}

	if (success == ERR_TRANS_SUCCESS)
		return true;
	else
		return false;
}


////////////////////////////////////`/////////////////////////////////////
// ProcessManFile: Extracts the required data from a single record of a man file.
//				 The input coordinates are transformed, and then the new record is 
//				 printed to the output file via PrintFilePoint/PrintManFile.
/////////////////////////////////////////////////////////////////////////
// On Entry:  A single record from a man file is held by cBuf. f_out must point to a
//			  valid output file for writing. Output file information is held by ptrCFile1.
// On Exit:   The record is read and the input coordinates are stored in pCVar1.
//			  The coordinates are transformed and printed to the specified output file.
/////////////////////////////////////////////////////////////////////////
BOOL CGCalcApp::ProcessManFile(char *cBuf, int cBufSize, FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1)
{
	double dDeg1, dMin1, dSec1;			// latitude
	double dDeg2, dMin2, dSec2;			// longitude
	int i, iF_outType;
	int  ch = ',';
	char *pLat, *pLon, *pHt, *pDesc, cValues[100], cRecord[121];
	char cPoint[12], cHeight[10], cDescription[27]; 
	CString strRecord, strValues;

	ptrCFile1->IO_Status = ERR_TRANS_SUCCESS;
	pLat = cBuf;
	strRecord = cBuf;
	if (strRecord.GetLength() < 40)
	{
		// No data contained within line just read
		ptrCFile1->IO_Status = ERR_NODATA_READ;
		return false;
	}	
	
	for (i=0; i<36; i++)
	{
		pLat = strchr(pLat, ch) + 1;
		if (i == 11)		// locality description
			pDesc = pLat;
		if (i == 22)		// height field
			pHt = pLat;
	}

	pLon = strchr(pLat, ch) + 1;
	pLon = strchr(pLon, ch);
	
	memset(cValues, '\0', sizeof(cValues));

	// copy lat and long fields
	memcpy(cValues, pLat, pLon - pLat);

	// Replace commas with spaces
	for (i=0; i<(int)strlen(cValues); i++)
	{
		if (cValues[i] == ',')
			cValues[i] = ' ';
	}

	if (sscanf_s(cValues, "%lf%lf%lf%lf%lf%lf", &dDeg1, &dMin1, &dSec1, &dDeg2, &dMin2, &dSec2) != 6)
	{
		// Could not read required data...possibly wrong format
		ptrCFile1->IO_Status = ERR_READING_DATA;
		return false;
	}

	// At this point the record is successfully read and contains valid numeric input data.
	// ...increment lNumTrans[1]
	ptrCFile1->lNumTrans[1] ++;

	dMin1 /= 100.0;
	dSec1 /= 10000.0;
	dMin2 /= 100.0;
	dSec2 /= 10000.0;
				
	// convert separated fields to decimal degrees
	pCVar1->dNum1 = -1 * GCALC_DmstoDeg(dDeg1 + dMin1 + dSec1);		// compute a negative value (assumes man is +ve south lat)
	pCVar1->dNum2 = GCALC_DmstoDeg(dDeg2 + dMin2 + dSec2);	

	if (ptrCFile1->InputDatum != ptrCFile1->OutputDatum)
		ptrCFile1->IO_Status = ApplyDistortionGrid(pCVar1);			// Send data to ApplyDistortionGrid

	if (ptrCFile1->IO_Status != ERR_TRANS_SUCCESS)
		return false;

	// print to output file
	if (ptrCFile1->IO_Status == ERR_TRANS_SUCCESS)
	{
		iF_outType = DetermineFileType(ptrCFile1->chFOutputType);

		if (iF_outType == TYPE_DAT)				// dat, txt, prn file
		{
			memset(cRecord, ' ', sizeof(cRecord));

			// copy registered number
			memcpy(cRecord, strRecord.Mid(0, 6), 6);
			memcpy(cRecord+44, pHt, 8);			// copy height
			memcpy(cRecord+59, pDesc, 26);		// copy description
			PrintDatFile(f_out, ptrCFile1, pCVar1, cRecord, _countof(cRecord));
		}
		else if (iF_outType == TYPE_CSV)		// csv file
		{
			memset(cRecord, ' ', sizeof(cRecord));
			sprintf_s(cPoint, _countof(cPoint), "%-11s", strRecord.Mid(0, 6));	// copy registered number
			memcpy(cHeight, pHt, 9);			// copy height
			cHeight[9] = '\0';
			memcpy(cDescription, pDesc, 26);	// copy description
			cDescription[26] = '\0';
			PrintCsvFile(f_out, ptrCFile1, pCVar1, cPoint, cHeight, cDescription);
		}
		else if (iF_outType == TYPE_MAN)		// man file
			PrintManFile(f_out, ptrCFile1, pCVar1, pLat, cBuf);
		
		// increment number of "successfully" transformed coordinates.
		ptrCFile1->lNumTrans[0] ++;
		ptrCFile1->lNumTrans[2] ++;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////
// ProcessQifFile: Extracts the required data from a single record of a qif file.
//				 The input coordinates are transformed, and then the new record is 
//				 printed to the output file.
/////////////////////////////////////////////////////////////////////////
// On Entry:  A single record from a qif file is held by cBuf. f_out must point to a
//			  valid output file for writing. Output file information is held by ptrCFile1.
// On Exit:   The record is read and the input coordinates are stored in pCVar1.
//			  The coordinates are transformed and printed to the specified output file.
//			  The maximum and minimum values are determined and stored in pFileExtent.
/////////////////////////////////////////////////////////////////////////
BOOL CGCalcApp::ProcessQifFile(char *cBuf, int cBufSize, char *cCoord, FILE *f_out, pointsfile *ptrCFile1, qifindex *pFileExtent)
{
	char cData[80], cNewRecord[151], cTemp[151], *pRec, *pSFRec;
	CString strBuf, strFormat, strXMBuf;
	double dVar1, dVar2, dVar3;
	int iLen;
	long lfpos;
	BOOL bTran1, bTran2;

	// declare coordinate struct and pointer to struct
	coordinate cCVar1;
	coordinate *pCVar1;
	pCVar1 = &cCVar1;

	strBuf = cBuf;
	strFormat = strBuf.Mid(0, 2);
	
	strcpy_s(cNewRecord, _countof(cNewRecord), cBuf);
	pRec = cNewRecord + 18;
	
	pSFRec = pFileExtent->cSubFileRec + 18;
	
	iLen = (int)strlen(cBuf);

	bTran1 = bTran2 = true;			// each record has 2 sets of coordinates to transform

	if (strcmp(strFormat, "  ") == 0 || strcmp(strFormat, "SX") == 0)
	{
		// "6.1 File Header Record" or "6.3 Subfile Extent Record"
		if (ptrCFile1->InputProj == PROJECTED)
		{
			// Projection
			if (sscanf_s((cBuf+9), "%d%lf%lf%lf%lf",
				&ptrCFile1->InputProj,		// Zone for entire file
				&pCVar1->dNum1,				// Min easting
				&pCVar1->dNum2,				// Min northing
				&dVar1,						// Max easting
				&dVar2						// Max northing
				) != 5)
			{
				// Could not read required data...possibly wrong format
				ptrCFile1->IO_Status = ERR_READING_DATA;
				return false;
			}
		}
		else
		{
			// Geographic
			if (sscanf_s((cBuf+18), "%lf%lf%lf%lf",
				&pCVar1->dNum1,			// Min longitude
				&pCVar1->dNum2,			// Min latitude
				&dVar1,					// Max longitude
				&dVar2					// Max latitude
				) != 4)
			{
				// Could not read required data...possibly wrong format
				ptrCFile1->IO_Status = ERR_READING_DATA;
				return false;
			}
		}
		
		// copy subfile extent record
		if (strcmp(strFormat, "SX") == 0)
			strcpy_s(pFileExtent->cSubFileRec, _countof(pFileExtent->cSubFileRec), cBuf);
	}
	else if (
		strcmp(strFormat, "P ") == 0 ||		// "6.10 Parcel Record"
		strcmp(strFormat, "SH") == 0 ||		// "6.2  Subfile Header Record"
		strcmp(strFormat, "OP") == 0)		// "6.4  Before Image Parcel Record"
	{
		memcpy(cData, cBuf+18, 30);
		cData[30] = '\0';

		if (sscanf_s(cData, "%lf%lf",
			&pCVar1->dNum1,			// Easting / longitude
			&pCVar1->dNum2			// Northing / latitude
			) != 2)
		{
			// Could not read required data...possibly wrong format
			ptrCFile1->IO_Status = ERR_READING_DATA;
			return false;
		}
		bTran2 = false;			// Record has only 1 set of coordinates to transform
	}
	else if (
		strcmp(strFormat, "OC") == 0 ||		// "6.5  Before Image Parcel Corners Record"
		strcmp(strFormat, "PC") == 0 ||		// "6.11 Parcel Corners Record"
		strcmp(strFormat, "XM") == 0)		// "6.16 Point Movement Record"
	{
		memcpy(cData, cBuf + 8, 30);
		cData[30] = '\0';
		strXMBuf = cData;
		strXMBuf.TrimRight();

		if (strcmp(strXMBuf, "") == 0 && strcmp(strFormat, "XM") == 0)
			bTran1 = false;				// XM record with no data for first set of coordinates
		else if (sscanf_s((cData), "%lf%lf",
			&pCVar1->dNum1,					// Easting / longitude
			&pCVar1->dNum2					// Northing / latitude
			) != 2)
		{
			// Could not read required data...possibly wrong format
			ptrCFile1->IO_Status = ERR_READING_DATA;
			return false;
		}

		pRec = cNewRecord + 8;

		if (iLen > 77)
		{
			strXMBuf = cBuf + 49;
			strXMBuf.TrimRight();

			if (strcmp(strXMBuf, "") == 0 && strcmp(strFormat, "XM") == 0)
				bTran2 = false;				// XM record with no data for second set of coordinates
			else if (sscanf_s((cBuf + 49), "%lf%lf",
				&dVar1,						// Easting / longitude
				&dVar2						// Northing / latitude
				) != 2)
			{
				// Could not read required data...possibly wrong format
				ptrCFile1->IO_Status = ERR_READING_DATA;
				return false;
			}
		}
		else
			bTran2 = false;			// Record has only 1 set of coordinates to transform
	}
	else if (strcmp(strFormat, "*E") == 0)		// End of File
	{
		// No coordinates in record to transform
		fprintf(f_out, "%s", cBuf);

		// print last SX record
		if (ptrCFile1->OutputProj != GEOGRAPHIC)		// projection
		{
			// print max & min sub-file extents to output file
			sprintf_s(pSFRec, _countof(pFileExtent->cSubFileRec), "%15.3f%15.3f", pFileExtent->dSMinEast, pFileExtent->dSMinNorth);
			pSFRec = pFileExtent->cSubFileRec + 48;
			sprintf_s(pSFRec, _countof(pFileExtent->cSubFileRec), "%15.3f%15.3f", pFileExtent->dSMaxEast, pFileExtent->dSMaxNorth);
		}
		else								// geographic
		{
			// print max & min sub-file extents to output file
			// NOTE: latitude max & min east are reversed to cater for neg south
			sprintf_s(pSFRec, _countof(pFileExtent->cSubFileRec), "%15.10f%15.10f", pFileExtent->dSMinNorth, pFileExtent->dSMaxEast);
			pSFRec = pFileExtent->cSubFileRec + 48;
			sprintf_s(pSFRec, _countof(pFileExtent->cSubFileRec), "%15.10f%15.10f", pFileExtent->dSMaxNorth, pFileExtent->dSMinEast);
		}
		
		// move the file pointer to last Subfile Extent record
		fseek(f_out, pFileExtent->lSubHeaderPos, SEEK_SET);

		// Print new record to output file
		fprintf(f_out, "%s", pFileExtent->cSubFileRec);

		// check max & mins with whole file extents
		if (pFileExtent->dSMaxEast > pFileExtent->dHMaxEast)
			pFileExtent->dHMaxEast = pFileExtent->dSMaxEast;		// copy maximum easting / latitude
		if (pFileExtent->dSMinEast < pFileExtent->dHMinEast)
			pFileExtent->dHMinEast = pFileExtent->dSMinEast;		// copy minimum easting / latitude
		if (pFileExtent->dSMaxNorth > pFileExtent->dHMaxNorth)
			pFileExtent->dHMaxNorth = pFileExtent->dSMaxNorth;		// copy maximum northing / longitude
		if (pFileExtent->dSMinNorth < pFileExtent->dHMinNorth)
			pFileExtent->dHMinNorth = pFileExtent->dSMinNorth;		// copy minimum northing / longitude

		// move the file pointer to end of file
		fseek(f_out, 0, SEEK_END);
		
		return true;
	}
	else
	{
		// No coordinates in record to transform
		fprintf(f_out, "%s", cBuf);
		return true;
	}

	// data exists for first set of coordinates to be transformed
	if (bTran1)
	{
		// increment number of "successfully" read numeric input data.
		ptrCFile1->lNumTrans[1] ++;
		
		// initialise input datum
		pCVar1->iNum4 = ptrCFile1->InputDatum;

		if (ptrCFile1->InputProj != GEOGRAPHIC)
		{
			pCVar1->dNum3 = ptrCFile1->InputProj;
			GCALC_GridtoGeo(pCVar1);		// send to gridtogeo
		}
		else
		{
			// Swap variables
			dVar3 = pCVar1->dNum1;				// Longitude
			pCVar1->dNum1 = pCVar1->dNum2;		// Latitude
			pCVar1->dNum2 = dVar3;				// Longitude
			pCVar1->dNum1 *= -1;				// change to neg south lat
		}
	
		if (ptrCFile1->InputDatum != ptrCFile1->OutputDatum)
			ptrCFile1->IO_Status = ApplyDistortionGrid(pCVar1);	// Transform data
	
		if (ptrCFile1->IO_Status != ERR_TRANS_SUCCESS)
			return false;

		// increment number of "successfully" transformed coordinates.
		ptrCFile1->lNumTrans[0] ++;
		ptrCFile1->lNumTrans[2] ++;
	}

	if (strcmp(strFormat, "  ") == 0)		// "6.1 Format of File Header Block"
	{
		if (ptrCFile1->OutputProj != GEOGRAPHIC)
			sprintf_s(cTemp, _countof(cTemp), "%s %d", cCoord, ptrCFile1->OutputProj);	// Coordinate System and Zone
		else
			sprintf_s(cTemp, _countof(cTemp), "%s   ", cCoord);						// Coordinate System

		memcpy((cNewRecord + 3), cTemp, strlen(cTemp)); // st

		// copy File Header Block record
		memcpy(pFileExtent->cFileRecord, cNewRecord, 100);

	}

	// data exists for first set of coordinates to be transformed
	if (bTran1)
	{
		if (ptrCFile1->OutputProj != GEOGRAPHIC)
		{
			// convert to projection coords
			pCVar1->dNum3 = ptrCFile1->OutputProj;
			pCVar1->iNum4 = ptrCFile1->OutputDatum;
			GCALC_GeotoGrid(pCVar1);
			sprintf_s(cTemp, _countof(cTemp), "%15.3f%15.3f", pCVar1->dNum1, pCVar1->dNum2); //st...East North
			memcpy(pRec, cTemp, 30); //st
		}
		else
		{
			// change back to pos south lat 
			pCVar1->dNum1 *= -1;			
			sprintf_s(cTemp, _countof(cTemp), "%15.10f%15.10f", pCVar1->dNum2, pCVar1->dNum1); //st...Long Lat
			memcpy(pRec, cTemp, 30); //st
		}
	
		// get maximum & minimums for sub record
		if (
			strcmp(strFormat, "OC") == 0 ||		// "6.5  Before Image Parcel Corners Record"
			strcmp(strFormat, "PC") == 0 )		// "6.11 Parcel Corners Record"
		{
			if (pCVar1->dNum1 > pFileExtent->dSMaxEast)
				pFileExtent->dSMaxEast = pCVar1->dNum1;		// copy maximum easting / latitude
			if (pCVar1->dNum1 < pFileExtent->dSMinEast)
				pFileExtent->dSMinEast = pCVar1->dNum1;		// copy minimum easting / latitude
			if (pCVar1->dNum2 > pFileExtent->dSMaxNorth)
				pFileExtent->dSMaxNorth = pCVar1->dNum2;		// copy maximum northing / longitude
			if (pCVar1->dNum2 < pFileExtent->dSMinNorth)
				pFileExtent->dSMinNorth = pCVar1->dNum2;		// copy minimum northing / longitude
	
		}

		// get maximum & minimums for whole file
		if (strcmp(strFormat, "SX") == 0 && pFileExtent->lSubHeaderPos != 0)
		{	
			// check max & mins with whole file extents
			if (pFileExtent->dSMaxEast > pFileExtent->dHMaxEast)
				pFileExtent->dHMaxEast = pFileExtent->dSMaxEast;		// copy maximum easting / latitude
			if (pFileExtent->dSMinEast < pFileExtent->dHMinEast)
				pFileExtent->dHMinEast = pFileExtent->dSMinEast;		// copy minimum easting / latitude
			if (pFileExtent->dSMaxNorth > pFileExtent->dHMaxNorth)
				pFileExtent->dHMaxNorth = pFileExtent->dSMaxNorth;		// copy maximum northing / longitude
			if (pFileExtent->dSMinNorth < pFileExtent->dHMinNorth)
				pFileExtent->dHMinNorth = pFileExtent->dSMinNorth;		// copy minimum northing / longitude
		}
	}

	// set pointer to start of second set of coordinates
	pRec = cNewRecord + 48;	
	
	// data exists for second set of coordinates to be transformed
	if (bTran2)
	{
		// initialise input datum
		pCVar1->iNum4 = ptrCFile1->InputDatum;

		// copy e & n
		pCVar1->dNum1 = dVar1;			// Easting
		pCVar1->dNum2 = dVar2;			// Northing
	
		if (ptrCFile1->InputProj != GEOGRAPHIC) //st
		{ //st
			pCVar1->dNum3 = ptrCFile1->InputProj;  //st
			GCALC_GridtoGeo(pCVar1);			// Convert to geographic
		} //st
		else
		{
			// swap variables
			pCVar1->dNum1 = dVar2;			// Latitude
			pCVar1->dNum2 = dVar1;			// Longitude
			pCVar1->dNum1 *= -1;			// change to neg south lat 
		}

		// increment number of "successfully" read numeric input data.
		ptrCFile1->lNumTrans[1] ++;

		if (ptrCFile1->InputDatum != ptrCFile1->OutputDatum) //st
			ptrCFile1->IO_Status = ApplyDistortionGrid(pCVar1);	// Transform data

		if (ptrCFile1->IO_Status != ERR_TRANS_SUCCESS)
			return false;

		// increment number of "successfully" transformed coordinates.
		ptrCFile1->lNumTrans[0] ++;
		ptrCFile1->lNumTrans[2] ++;

		if (ptrCFile1->OutputProj != GEOGRAPHIC) //st
		{
			// convert to projection coords
			pCVar1->dNum3 = ptrCFile1->OutputProj;  //st
			pCVar1->iNum4 = ptrCFile1->OutputDatum; //st
			GCALC_GeotoGrid(pCVar1);
			sprintf_s(cTemp, _countof(cTemp), "%15.3f%15.3f", pCVar1->dNum1, pCVar1->dNum2); //st
			memcpy(pRec, cTemp, 30); //st
		}
		else
		{
			// change back to pos south lat 
			pCVar1->dNum1 *= -1;			
			// print to file
			sprintf_s(cTemp, _countof(cTemp), "%15.10f%15.10f", pCVar1->dNum2, pCVar1->dNum1); //st
			memcpy(pRec, cTemp, 30); //st
		}

		// get maximum & minimums for sub record
		if (
			strcmp(strFormat, "OC") == 0 ||		// "6.5  Before Image Parcel Corners Record"
			strcmp(strFormat, "PC") == 0 )		// "6.11 Parcel Corners Record"
		{
			if (pCVar1->dNum1 > pFileExtent->dSMaxEast)
				pFileExtent->dSMaxEast = pCVar1->dNum1;		// copy maximum easting / latitude
			if (pCVar1->dNum1 < pFileExtent->dSMinEast)
				pFileExtent->dSMinEast = pCVar1->dNum1;		// copy minimum easting / latitude
			if (pCVar1->dNum2 > pFileExtent->dSMaxNorth)
				pFileExtent->dSMaxNorth = pCVar1->dNum2;		// copy maximum northing / longitude
			if (pCVar1->dNum2 < pFileExtent->dSMinNorth)
				pFileExtent->dSMinNorth = pCVar1->dNum2;		// copy minimum northing / longitude
		}
	}

	// Print new record to output file
	fprintf(f_out, "%s", cNewRecord);
	
	if (strcmp(strFormat, "SX") == 0 && pFileExtent->lSubHeaderPos != 0)
	{
		// check max & mins with whole file extents
		if (pFileExtent->dSMaxEast > pFileExtent->dHMaxEast)
			pFileExtent->dHMaxEast = pFileExtent->dSMaxEast;		// copy maximum easting / latitude
		if (pFileExtent->dSMinEast < pFileExtent->dHMinEast)
			pFileExtent->dHMinEast = pFileExtent->dSMinEast;		// copy minimum easting / latitude
		if (pFileExtent->dSMaxNorth > pFileExtent->dHMaxNorth)
			pFileExtent->dHMaxNorth = pFileExtent->dSMaxNorth;		// copy maximum northing / longitude
		if (pFileExtent->dSMinNorth < pFileExtent->dHMinNorth)
			pFileExtent->dHMinNorth = pFileExtent->dSMinNorth;		// copy minimum northing / longitude

		if (ptrCFile1->OutputProj != GEOGRAPHIC)				// projection
		{
			// print max & min sub-file extents to output file
			sprintf_s(pSFRec, _countof(pFileExtent->cSubFileRec), "%15.3f%15.3f", pFileExtent->dSMinEast, pFileExtent->dSMinNorth);
			pSFRec = pFileExtent->cSubFileRec + 48;
			sprintf_s(pSFRec, _countof(pFileExtent->cSubFileRec), "%15.3f%15.3f", pFileExtent->dSMaxEast, pFileExtent->dSMaxNorth);
			
			// initialise sub-file max values
			pFileExtent->dSMaxEast = 0.0;
			pFileExtent->dSMinEast = 10000000.0;
			pFileExtent->dSMaxNorth = 0.0;
			pFileExtent->dSMinNorth = 10000000.0;
		}
		else										// geographic
		{
			// print max & min sub-file extents to output file
			// NOTE: latitude max & min east are reversed to cater for neg south
			sprintf_s(pSFRec, _countof(pFileExtent->cSubFileRec), "%15.10f%15.10f", pFileExtent->dSMinNorth, pFileExtent->dSMaxEast);
			pSFRec = pFileExtent->cSubFileRec + 48;
			sprintf_s(pSFRec, _countof(pFileExtent->cSubFileRec), "%15.10f%15.10f", pFileExtent->dSMaxNorth, pFileExtent->dSMinEast);
			
			// initialise sub-file max values
			pFileExtent->dSMaxEast = 0.0;
			pFileExtent->dSMinEast = 90.0;
			pFileExtent->dSMaxNorth = 0.0;
			pFileExtent->dSMinNorth = 180.0;
		}
		
		// get current file position
		lfpos = ftell(f_out);

		// move the file pointer to last Subfile Extent record
		fseek(f_out, pFileExtent->lSubHeaderPos, SEEK_SET);

		// Print new record to output file
		fprintf(f_out, "%s", pFileExtent->cSubFileRec);

		// move the file pointer back to the current record
		fseek(f_out, lfpos, SEEK_SET);

	}

	// get the output-file position at start of the subfile record
	if (strcmp(strFormat, "SX") == 0)
	{
		pFileExtent->lSubHeaderPos = ftell(f_out);
		pFileExtent->lSubHeaderPos -= (iLen + 1);
	}

	return true;
}



/////////////////////////////////////////////////////////////////////////
// ProcessResFile: Extracts the required data from a single record of a res file.
//				 The input coordinates are transformed, and then the new record is 
//				 printed to the output file via PrintResFile.
/////////////////////////////////////////////////////////////////////////
// On Entry:  A single record from a res file is held by cBuf. f_out must point to a
//			  valid output file for writing. Output file information is held by ptrCFile1.
// On Exit:   The record is read and the input coordinates are stored in pCVar1.
//			  The coordinates are transformed and printed to the specified output file.
/////////////////////////////////////////////////////////////////////////
BOOL CGCalcApp::ProcessResFile(char *cBuf, int cBufSize, FILE *f_out, pointsfile *ptrCFile1, coordinate *pCVar1)
{
	char cValues[51], cRecord[121];
	double dDeg1, dDeg2, dMin1, dMin2, dSec1, dSec2;
	int iF_outType;
	CString strRec;

	ptrCFile1->IO_Status = ERR_TRANS_SUCCESS;
	strRec = cBuf;	
	if (strRec.GetLength() < 40)
	{
		// No data contained within line just read
		ptrCFile1->IO_Status = ERR_NODATA_READ;
		return false;
	}	

	// check coordinate system parameters against record contents
	switch (cBuf[39])
	{
	case 'S':
	case ' ':
		// Southern
		if (ptrCFile1->InputProj != GEOGRAPHIC)
			ptrCFile1->IO_Status = ERR_RES_COORD_MATCH;
		break;
	case 'N':
		// Northern
		if (ptrCFile1->InputProj != GEOGRAPHIC)
			ptrCFile1->IO_Status = ERR_RES_COORD_MATCH;
		break;
	case 'A':
		// Southern AMG
		if (ptrCFile1->InputProj == GEOGRAPHIC)
			ptrCFile1->IO_Status = ERR_RES_COORD_MATCH;
		break;
	case 'U':
		// Southern UTM (generic...assume MGA)
		if (ptrCFile1->InputProj == GEOGRAPHIC)
			ptrCFile1->IO_Status = ERR_RES_COORD_MATCH;
		break;
	default:
		ptrCFile1->IO_Status = ERR_RES_COORD_TYPE;
	}

	if (ptrCFile1->IO_Status != ERR_TRANS_SUCCESS)
		return false;

	pCVar1->iNum4 = ptrCFile1->InputDatum;
	
	if (cBuf[39] == 'U' || cBuf[39] == 'A')		// Projection
	{		
		strncpy_s(cValues, _countof(cValues), cBuf + 40, 30);
		if (sscanf_s(cValues, "%lf%lf%lf", &pCVar1->dNum3, &pCVar1->dNum1, &pCVar1->dNum2) != 3)
		{
			// Could not read required data...possibly wrong format
			ptrCFile1->IO_Status = ERR_READING_DATA;
			return false;
		}
		GCALC_GridtoGeo(pCVar1);		
	}
	else		// Geographic
	{
		sprintf_s(cValues, _countof(cValues), "%-50s", strRec.Mid(40, 5));
		memcpy(cValues + 6, cBuf + 45, 16);
		memcpy(cValues + 23, cBuf + 61, 9);
		cValues[15] = ' ';

		if (sscanf_s(cValues, "%lf%lf%lf%lf%lf%lf", &dDeg1, &dMin1, &dSec1, &dDeg2, &dMin2, &dSec2) != 6)
		{	
			// Could not read required data...possibly wrong format
			ptrCFile1->IO_Status = ERR_READING_DATA;
			return false;
		}

		// convert separated fields to decimal degrees
		dMin1 /= 100.0;
		dSec1 /= 10000.0;
		dMin2 /= 100.0;
		dSec2 /= 10000.0;
		pCVar1->dNum1 = GCALC_DmstoDeg(dDeg1 + dMin1 + dSec1);
		pCVar1->dNum2 = GCALC_DmstoDeg(dDeg2 + dMin2 + dSec2);

		// test for negative value
		if (cBuf[39] == 'S' || cBuf[39] == ' ')
			pCVar1->dNum1 *= -1;
	}

	// At this point the record is successfully read and contains valid numeric input data.
	// ...increment lNumTrans[1]
	ptrCFile1->lNumTrans[1] ++;

	if (ptrCFile1->InputDatum != ptrCFile1->OutputDatum)
		ptrCFile1->IO_Status = ApplyDistortionGrid(pCVar1);			// Bilinear transformation

	if (ptrCFile1->IO_Status != ERR_TRANS_SUCCESS)
		return false;
	
	iF_outType = DetermineFileType(ptrCFile1->chFOutputType);

	if (iF_outType == TYPE_DAT)				// dat file
	{
		sprintf_s(cRecord, _countof(cRecord), "%11s%32s%9s%7s%26s", strRec.Mid(6, 10), " ", strRec.Mid(70, 9), " ", strRec.Mid(16, 23)); 
		PrintDatFile(f_out, ptrCFile1, pCVar1, cRecord, _countof(cRecord));		// print to dat file
	}
	else if (iF_outType == TYPE_CSV)		// csv file
		PrintCsvFile(f_out, ptrCFile1, pCVar1, (LPCTSTR)strRec.Mid(6, 10), (LPCTSTR)strRec.Mid(70, 9), (LPCTSTR)strRec.Mid(16, 23));		// print to csv file
	else if (iF_outType == TYPE_RES)		// res file
		PrintResFile(f_out, ptrCFile1, pCVar1, cBuf, cBufSize);
	
	// increment number of "successfully" transformed coordinates.
	ptrCFile1->lNumTrans[0] ++;
	ptrCFile1->lNumTrans[2] ++;
	return true;
}


/////////////////////////////////////////////////////////////////////////
// ReadAsciiShifts:	Reads the Latitude and Longitude shifts & accuracies from an
//					an ascii grid file.
/////////////////////////////////////////////////////////////////////////
// On Entry:  A char array is read (being one node record) from the grid file
//			  This array should contain (as text) 1 Latitude shift, 
//			  1 Longitude shift, 1 Latitude accuracy and 1 Longitude accuracy. A
//			  valid file pointer must exist.
// On Exit:   Four floats are read from the char array. The success of the sscanf
//			  from the array is assigned 1 or 0. The floats are then stored as 
//			  doubles and returned via the "crdNode" struct of type "coordinate".
/////////////////////////////////////////////////////////////////////////
BOOL CGCalcApp::ReadAsciiShifts(coordinate *pNShifts[], int iNodeIndex, long lNode)
{
	float fNum1, fNum2, fNum3, fNum4;
	char cBuf[100];
	const char *format;
	fpos_t fPos;

	format = SHIFTS;

	fPos = m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].fpGridPos + ((lNode-1) * 42);
	if ( fsetpos(m_pGfileptr, &fPos) != 0 )  
		return false;			// Could not get position
		
	// Retrieve the whole line for node A
	fgets(cBuf, 100, m_pGfileptr);
	if (sscanf_s(cBuf, format, &fNum1, &fNum2, &fNum3, &fNum4) < 4)
		return false;			// Could not read from grid file
	
	pNShifts[iNodeIndex]->dNum1 = double(fNum1);
	pNShifts[iNodeIndex]->dNum2 = double(fNum2);
	pNShifts[iNodeIndex]->dLatacc = double(fNum3);
	pNShifts[iNodeIndex]->dLongacc = double(fNum4);

	if (strcmp(m_pGridfile->chGs_type, "RADIANS ") == 0)
	{
		// convert radians values to seconds
		pNShifts[iNodeIndex]->dNum1 *= dRad2Sec;
		pNShifts[iNodeIndex]->dNum2 *= dRad2Sec;
		pNShifts[iNodeIndex]->dLatacc *= dRad2Sec;
		pNShifts[iNodeIndex]->dLongacc *= dRad2Sec;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////
// ReadBinaryShifts: Reads the Latitude and Longitude shifts & accuracies from an
//					 a binary grid file.
/////////////////////////////////////////////////////////////////////////
// On Entry:  A char array is read (being one node record) from the grid file
//			  This array should contain (as text) 1 Latitude shift, 
//			  1 Longitude shift, 1 Latitude accuracy and 1 Longitude accuracy. A
//			  valid file pointer must exist.
// On Exit:   Four floats are read from the char array. The success of the sscanf
//			  from the array is assigned 1 or 0. The floats are then stored as 
//			  doubles and returned via the "crdNode" struct of type "coordinate".
/////////////////////////////////////////////////////////////////////////
BOOL CGCalcApp::ReadBinaryShifts(coordinate *pNShifts[], int iNodeIndex, long lNode)
{
	float fNum;
	long lPos;
	int iLineLength;

	iLineLength = 4 * sizeof(float);

	lPos = m_pGridfile->ptrIndex[m_pGridfile->iTheGrid].lGridPos + ((lNode-1) * iLineLength);
	if (fseek(m_pGfileptr, lPos, SEEK_SET) != 0)
		return false;			// Could not seek file position from grid file	
	if ( (fread(&fNum, sizeof(float), 1, m_pGfileptr)) < 1 )
		return false;			// Could not read from grid file
	pNShifts[iNodeIndex]->dNum1 = double(fNum);	
	if ( (fread(&fNum, sizeof(float), 1, m_pGfileptr)) < 1 )
		return false;
	pNShifts[iNodeIndex]->dNum2 = double(fNum);	
	if ( (fread(&fNum, sizeof(float), 1, m_pGfileptr)) < 1 )
		return false;
	pNShifts[iNodeIndex]->dLatacc = double(fNum);	
	if ( (fread(&fNum, sizeof(float), 1, m_pGfileptr)) < 1 )
		return false;
	pNShifts[iNodeIndex]->dLongacc = double(fNum);

	if (strcmp(m_pGridfile->chGs_type, "RADIANS ") == 0)
	{
		// convert radians values to seconds
		pNShifts[iNodeIndex]->dNum1 *= dRad2Sec;
		pNShifts[iNodeIndex]->dNum2 *= dRad2Sec;
		pNShifts[iNodeIndex]->dLatacc *= dRad2Sec;
		pNShifts[iNodeIndex]->dLongacc *= dRad2Sec;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////
// RoundAccuracy: Rounds the interpolated accuracy to nearest 50mm above set value.
//
/////////////////////////////////////////////////////////////////////////
// On Entry:   The accuracy held by Accuracy.
// On Exit:    Accuracy is rounded to the nearest 50mm.
/////////////////////////////////////////////////////////////////////////
void CGCalcApp::RoundAccuracy(double *Accuracy)
{
	double dRoundFigure;
	double dIncrement;

	dRoundFigure = 0.05;
	if (Accuracy < 0)
		dRoundFigure *= -1.0;
	dIncrement = dRoundFigure;
	while (dRoundFigure < *Accuracy)
		dRoundFigure += dIncrement;
	*Accuracy = dRoundFigure;
}


/////////////////////////////////////////////////////////////////////////
// TransFilePoint: Extracts the input coordinates from a char array passed by
//				 ProcessCsv/DatFile. The input coordinates are transformed via
//				 ApplyDistortionGrid.
/////////////////////////////////////////////////////////////////////////
// On Entry:  f_out must point to a valid output file for writing.
//			  Output file information is held by ptrCFile1.
// On Exit:   The coordinates are transformed and stored in pCVar1.
/////////////////////////////////////////////////////////////////////////
int CGCalcApp::TransFilePoint(pointsfile *ptrCFile1, coordinate *pCVar1)
{
	double dZone;

	// Input coords are projected...convert to geographic
	if (ptrCFile1->InputProj == PROJECTED)
		GCALC_GridtoGeo(pCVar1);
	else
	{
		// geographic...
		if (ptrCFile1->InputDMS == DMS)		// degrees minutes seconds
		{
			// Convert to Decimal degrees
			pCVar1->dNum1 = GCALC_DmstoDeg(pCVar1->dNum1);	// Lat
			pCVar1->dNum2 = GCALC_DmstoDeg(pCVar1->dNum2);	// Long
		}
		else if (ptrCFile1->InputDMS == DMIN)		// degrees minutes
		{
			// Convert to Decimal degrees
			pCVar1->dNum1 = GCALC_DmintoDeg(pCVar1->dNum1);	// Lat
			pCVar1->dNum2 = GCALC_DmintoDeg(pCVar1->dNum2);	// Long
		}
	}

	// check for valid input
	if (pCVar1->dNum1 < -90.0 || pCVar1->dNum1 > 90.0)
	{
		// Line does not contain valid latitude input
		ptrCFile1->IO_Status = ERR_INVALID_INPUT;
		return ptrCFile1->IO_Status;
	}

	if (pCVar1->dNum2 < -100.0 || pCVar1->dNum2 > 170.0)
	{
		// Line does not contain valid longitude input
		ptrCFile1->IO_Status = ERR_INVALID_INPUT;
		return ptrCFile1->IO_Status;
	}

	// At this point the record is successfully read and contains valid numeric input data.
	// ...increment lNumTrans[1]
	ptrCFile1->lNumTrans[1] ++;
	
	if (ptrCFile1->InputDatum != ptrCFile1->OutputDatum)
		ptrCFile1->IO_Status = ApplyDistortionGrid(pCVar1);
	else
		pCVar1->dLatacc = pCVar1->dLongacc = NULL_ACCURACY;

	// point is outside the grid file...continue with 7 parameter transformation
	if (ptrCFile1->InputDatum != ptrCFile1->OutputDatum && ptrCFile1->IO_Status == ERR_PT_OUTSIDE_GRID)
	{
		pCVar1->iNum4 = ptrCFile1->InputDatum;			// initialise ellipsoid type
		dZone = pCVar1->dNum3;
		pCVar1->dNum3 = 0.0;							// initialise ellipsoidal height
		GCALC_GeotoCart(pCVar1);

		GCALC_DatumTran(pCVar1, ptrCFile1->dParameters);
		
		pCVar1->iNum4 = ptrCFile1->OutputDatum;			// initialise ellipsoid type
		GCALC_CarttoGeo(pCVar1);

		pCVar1->dLatacc = NULL_ACCURACY;
		pCVar1->dLongacc = NULL_ACCURACY;
		pCVar1->dNum3 = dZone;							// restore zone
	}
	
	return ptrCFile1->IO_Status;
}


