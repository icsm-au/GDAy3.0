/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
		Filename:
			GCalcext.cpp
		
		Required companion files:
			GCalc.lib, GCalc.h, GCalcext.h.
		
		Author:
			Roger Fraser. 

		Copyright
			Natural Resources and Mines (The state of Queensland, (C) 2001).

		Created:		23 June 1999.
		Last Modified:  22 July 2002.

		Build Version:		2.100
		Previous Version:	2.010

		Execution:
			To be used as an MFC statically linked (Regular) DLL. This DLL was written 
			intentionally for use by GDAy (C) 1999, which was compiled using Visual C++ 6.0. 
			This DLL exports the procedures mentioned below, using implicit linkage to the export
			symbols contained within "GCalc.lib". "GCalcext.h" contains the declarations for all
			required typedef structs required for the use of the exported procedures.

		Procedures:
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
			GCALC_FileTran(char *gridfile, char *gridtype, pointsfile *ptrCFile1, BOOL ADD_HEADER);
			GCALC_FormatDMS_String(char *buf, int& buf_size, double dVar1, int iType);
			GCALC_GeotoCart(coordinate *pGeoValue);
			GCALC_GeotoGrid(coordinate *pGeoValue);
			GCALC_GridtoGeo(coordinate *pGridValue);
			GCALC_PointTran(char *gridfile, char *gridtype, coordinate *pGeoValue);
			GCALC_PrintLogFile(char *logfile, char *access_mode, char *logrecord);
			GCALC_ReportGridProperties(char *gridfile, char *gridtype, file_par *gridproperties, int *success);

*2.1*		GCALC_ReturnFileProgress();
*2.1*		GCALC_ReturnBytesRead();
*2.1*		GCALC_MapGridTransformation(coordinate *pGridValue);
*2.1*		GCALC_SetFileTranHeader();
*2.1*		GCALC_GetByteOffset();
*2.1*		GCALC_SetByteOffset();

		Acknowledgements:
			Steve Tarbit, Peter Todd, Matt Higgins, David Mitchell.
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <GCalc.h>
#include <GCalcext.h>

extern CGCalcApp theApp;
/////////////////////////////////////////////////////////////////////////////
// External Procedures
extern "C" {


/////////////////////////////////////////////////////////////////////////
// BiLinearTransformation: External wrapper for ApplyDistortionGrid
/////////////////////////////////////////////////////////////////////////
// On Entry:  A pointer to the grid file is held by m_pGfileptr. The coordinates
//			  for the point to be transformed are passed by pThePoint. These values
//			  must be geographic coordinates in decimal degrees. The pointer to
//			  gridfile must point to a valid file before this procedure is called.
// On Exit:   The transformed point is returned by cThePoint.
/////////////////////////////////////////////////////////////////////////
void GCALC_BiLinearTransformation(coordinate *pThePoint)
{	
	// initialise success
	pThePoint->IO_Status = ERR_TRANS_SUCCESS;
	if (theApp.m_pGridfile != NULL)		// has gridfile been opened?
		theApp.ApplyDistortionGrid(pThePoint);
	else
		pThePoint->IO_Status = ERR_GRID_UNOPENED;
}

/////////////////////////////////////////////////////////////////////////
// CarttoGeo: Computes Geodetic Latitude and Longitude from
//			  Cartesian coordinates of X, Y and Z. 
//			  See "GCalc.h" for datum parameters.
/////////////////////////////////////////////////////////////////////////
// On Entry: X, Y and Z is passed by cCartValue 
// On Exit:  Latitude and Longitude in decimal degrees is returned by
//			 cCartValue
/////////////////////////////////////////////////////////////////////////
void GCALC_CarttoGeo(coordinate *pCartValue)
{
	double a, Inv_f, f, e2, Nu, p;
	double lat1, lat2, long1, ht;
	int i;

	if (pCartValue->iNum4 == GDA94)
	{
		a = GDA94a;
		Inv_f = GDA94Inv_f;
	}
	else
	{
		a = GDA20a;
		Inv_f = GDA20Inv_f;
	}

	f = 1.0 / Inv_f;
	e2 = (2.0 * f) - (f * f);
	p = pow(((pow(pCartValue->dNum1, 2.0)) + (pow(pCartValue->dNum2, 2.0))), 0.5);

	// "Cartesian to geographic" conversion problem resides in the 
	// equation to compute latitude (phi)...latitude is required on both
	// sides of the equation. A simple iterative method is used to determine 
	// the latitude, whereby the starting latitude is computed from 
	// tan(phi) = Z / p

	// Compute starting lat value from second eccentricity squared
	lat1 = atan(pCartValue->dNum3 / p);
	lat2 = atan((pCartValue->dNum3 + (e2 * a * sin(lat1)))/ p);

	Nu = a / pow( (1 - (e2 * pow(sin(lat2), 2.0))), 0.5);

	for (i=0; i < 10; i++)
	{
		lat1 = atan((pCartValue->dNum3 + (e2 * Nu * sin(lat2)))/ p);
		Nu = a / pow( (1 - (e2 * pow(sin(lat1), 2.0))), 0.5);
		lat2 = atan((pCartValue->dNum3 + (e2 * Nu * sin(lat1)))/ p);

		if (fabs(lat1 - lat2) < CONST_1e16)
			break;
	}
		
	// Compute longitude
	long1 = atan(pCartValue->dNum2 / pCartValue->dNum1);
	pCartValue->dNum3 = (long1 / pi * 180.0);

	// determine correct quadrant and apply negative long accordingly
	if (pCartValue->dNum1 < 0.0 && pCartValue->dNum2 > 0.0)
		long1 += pi;
	else if (pCartValue->dNum1 < 0.0 && pCartValue->dNum2 < 0.0)
		long1 = -(pi - long1);
	
	// Compute height
	ht = (p / cos(lat2)) - Nu;

	pCartValue->dNum1 = (lat2 / pi * 180.0);
	pCartValue->dNum2 = (long1 / pi * 180.0);
	pCartValue->dNum3 = ht;
}


/////////////////////////////////////////////////////////////////////////
// CreateGridIndex: External wrapper for Opengridfile
/////////////////////////////////////////////////////////////////////////
// On Entry:  The grid file name and file type is held by filename and filetype.
// On Exit:   The grid file is opened and all header information is read 
//			  and returned by fGridFile. If the grid file does not meet the
//			  standard NTv2 grid file format, or contains values that are un-
//			  recognised, OpenGridFile returns fGridfile with a values
//			  corresponding to the nature of the error and its file location.
/////////////////////////////////////////////////////////////////////////
void GCALC_CreateGridIndex(char *filename, char *filetype, int *success)
{
	*success = ERR_TRANS_SUCCESS;

	if (theApp.m_pGridfile == NULL)		// grid file not opened?
		theApp.m_Grid_Success = theApp.Opengridfile(filename, filetype);

	if (_stricmp(theApp.m_pGridfile->filename, filename) != 0)	// a new filename or filetype?
	{
		theApp.ClearGridFileMemory();
		theApp.m_Grid_Success = theApp.Opengridfile(filename, filetype);
	}

	// Australian binary file encountered (on success)
	if (theApp.m_Can_Format == false && theApp.m_Grid_Success < ERR_GRIDFILE_READ)
		theApp.m_Grid_Success = ERR_AUS_BINARY;		// Australian binary file encountered

	if (theApp.m_Grid_Success > ERR_TRANS_SUCCESS)	// an error occurred?
		theApp.ClearGridFileMemory();

	*success = theApp.m_Grid_Success;
}

/////////////////////////////////////////////////////////////////////////
// DatumTran: Computes cartesian coordinates of X, Y and Z from one
//			  datum to another. The direction is specidied in "cDatumValue.iNum4" 
//			  See "GCalc.h" for datum parameters.
/////////////////////////////////////////////////////////////////////////
// On Entry: X, Y and Z (datum1) is passed by cDatumValue 
// On Exit:  X, Y and Z (datum2) is returned by cDatumValue
/////////////////////////////////////////////////////////////////////////
void GCALC_DatumTran(coordinate *pDatumValue, double dParameters[])
{
	double dDatum1[3][1], dDatum2[3][1], dRotation[3][3];
	double dx, dy, dz, rx, ry, rz, sc;
	int i, j;

	dx = dParameters[0];
	dy = dParameters[1];
	dz = dParameters[2];
	rx = dParameters[3];
	ry = dParameters[4];
	rz = dParameters[5];
	sc = dParameters[6];

	// if cDatumValue is based on the GDA2020...reverse transformation reqd.
	if (pDatumValue->iNum4 == GDA2020)
	{
		dx *= -1.0;
		dy *= -1.0;
		dz *= -1.0;
		rx *= -1.0;
		ry *= -1.0;
		rz *= -1.0;
		sc *= -1.0;
	}
	
	// Convert to radians
	rx *= dSec2Rad;
	ry *= dSec2Rad;
	rz *= dSec2Rad;
	sc = 1.0 + (sc / 1000000.0);
	
	// Initialise 'from datum' matrix
	dDatum1[0][0] = pDatumValue->dNum1;
	dDatum1[1][0] = pDatumValue->dNum2;
	dDatum1[2][0] = pDatumValue->dNum3;

	// Initialise 'rotation' matrix
	dRotation[0][0] = cos(ry) * cos(rz);
	dRotation[0][1] = cos(ry) * sin(rz);
	dRotation[0][2] = -1.0 * sin(ry);
	dRotation[1][0] = (sin(rx) * sin(ry) * cos(rz)) - (cos(rx) * sin(rz));
	dRotation[1][1] = (sin(rx) * sin(ry) * sin(rz)) + (cos(rx) * cos(rz));
	dRotation[1][2] = sin(rx) * cos(ry);
	dRotation[2][0] = (cos(rx) * sin(ry) * cos(rz)) + (sin(rx) * sin(rz));
	dRotation[2][1] = (cos(rx) * sin(ry) * sin(rz)) - (sin(rx) * cos(rz));
	dRotation[2][2] = cos(rx) * cos(ry);
	

	// 3x3 * 3x1 Matrix multiplication
	for (i=0; i<3; i++)			// For each row
	{
		// Initialise 'to datum' matrix
		dDatum2[i][0] = 0.0;

		for (j=0; j<3; j++)		// For each column in the row
			dDatum2[i][0] += dRotation[i][j] * dDatum1[j][0];	// Sum partial products

		// Apply scale change
		dDatum2[i][0] *= sc;		
	}

	// Add translations and save in struct
	pDatumValue->dNum1 = dDatum2[0][0] + dx;
	pDatumValue->dNum2 = dDatum2[1][0] + dy;
	pDatumValue->dNum3 = dDatum2[2][0] + dz;
}


/////////////////////////////////////////////////////////////////////////
// DatumTranV: Computes cartesian coordinates of X, Y and Z from one
//			  datum to another and incorporates velocities. The direction is specidied in "cDatumValue.iNum4" 
//			  See "GCalc.h" for datum parameters.
/////////////////////////////////////////////////////////////////////////
// On Entry: X, Y and Z (datum1) is passed by cDatumValue 
// On Exit:  X, Y and Z (datum2) is returned by cDatumValue
/////////////////////////////////////////////////////////////////////////
void GCALC_DatumTranV(coordinate *pDatumValue, double dParameters[], double dVelocities[])
{
	double dDatum1[3][1], dDatum2[3][1], dRotation[3][3];
	double dx, dy, dz, rx, ry, rz, sc;
	int i, j;

	// dVelocities[7] holds the length of time by which to scale dVelocities[0..6]
	dx = dParameters[0] + dVelocities[7] * dVelocities[0];
	dy = dParameters[1] + dVelocities[7] * dVelocities[1];
	dz = dParameters[2] + dVelocities[7] * dVelocities[2];
	rx = dParameters[3] + dVelocities[7] * dVelocities[3];
	ry = dParameters[4] + dVelocities[7] * dVelocities[4];
	rz = dParameters[5] + dVelocities[7] * dVelocities[5];
	sc = dParameters[6] + dVelocities[7] * dVelocities[6];

	// if cDatumValue is based on the GDA2020...reverse transformation reqd.
	if (pDatumValue->iNum4 == GDA2020)
	{
		dx *= -1.0;
		dy *= -1.0;
		dz *= -1.0;
		rx *= -1.0;
		ry *= -1.0;
		rz *= -1.0;
		sc *= -1.0;
	}
	
	// Convert to radians
	rx *= dSec2Rad;
	ry *= dSec2Rad;
	rz *= dSec2Rad;
	sc = 1.0 + (sc / 1000000.0);
	
	// Initialise 'from datum' matrix
	dDatum1[0][0] = pDatumValue->dNum1;
	dDatum1[1][0] = pDatumValue->dNum2;
	dDatum1[2][0] = pDatumValue->dNum3;

	// Initialise 'rotation' matrix
	dRotation[0][0] = cos(ry) * cos(rz);
	dRotation[0][1] = cos(ry) * sin(rz);
	dRotation[0][2] = -1.0 * sin(ry);
	dRotation[1][0] = (sin(rx) * sin(ry) * cos(rz)) - (cos(rx) * sin(rz));
	dRotation[1][1] = (sin(rx) * sin(ry) * sin(rz)) + (cos(rx) * cos(rz));
	dRotation[1][2] = sin(rx) * cos(ry);
	dRotation[2][0] = (cos(rx) * sin(ry) * cos(rz)) + (sin(rx) * sin(rz));
	dRotation[2][1] = (cos(rx) * sin(ry) * sin(rz)) - (sin(rx) * cos(rz));
	dRotation[2][2] = cos(rx) * cos(ry);
	

	// 3x3 * 3x1 Matrix multiplication
	for (i=0; i<3; i++)			// For each row
	{
		// Initialise 'to datum' matrix
		dDatum2[i][0] = 0.0;

		for (j=0; j<3; j++)		// For each column in the row
			dDatum2[i][0] += dRotation[i][j] * dDatum1[j][0];	// Sum partial products

		// Apply scale change
		dDatum2[i][0] *= sc;		
	}

	// Add translations and save in struct
	pDatumValue->dNum1 = dDatum2[0][0] + dx;
	pDatumValue->dNum2 = dDatum2[1][0] + dy;
	pDatumValue->dNum3 = dDatum2[2][0] + dz;
}

/////////////////////////////////////////////////////////////////////////
// DegtoDms:  Computes angular value of degrees minutes and seconds from
//			  decimal degrees.
/////////////////////////////////////////////////////////////////////////
// On Entry: value in decimal degrees 
// On Exit:  value in degrees minutes and seconds
/////////////////////////////////////////////////////////////////////////
double GCALC_DegtoDms(double dNum)
{
	double dh, d, m, s;
	
	dh = fabs(dNum);	// retain original value
	d = floor(dh);
	m = floor(((dh - d) * 60.0));
	s = (dh - d - (m/60.0)) * 3600.0;
	if (fabs(s - 60.0) < 0.000000001)
	{
		s = 0.0;
		m += 1.0;
	}
	dh = d + (m/100.0) + (s/10000.0);	
	if (dNum < 0.0)
		dh *= -1;
	return dh;
}

/////////////////////////////////////////////////////////////////////////
// DegtoDmin:  Computes angular value of degrees and decimal minutes from
//			  decimal degrees.
/////////////////////////////////////////////////////////////////////////
// On Entry: value in decimal degrees 
// On Exit:  value in degrees and decimal minutes
/////////////////////////////////////////////////////////////////////////
double GCALC_DegtoDmin(double dNum)
{
	double dh, d;
	
	dh = fabs(dNum);	// retain original value
	d = floor(dh);
	dh = d + ((dh - d) * 0.6);
	
	if (dNum < 0.0)
		dh *= -1;
	return dh;
}

/////////////////////////////////////////////////////////////////////////
// DmstoDeg:  Computes angular value of decimal degrees from
//			  degrees minutes and seconds.
/////////////////////////////////////////////////////////////////////////
// On Entry:  value in degrees minutes and seconds 
// On Exit:   value in decimal degrees
/////////////////////////////////////////////////////////////////////////
double GCALC_DmstoDeg(double dNum)
{
	double dh, dd, dm, ds;

	dh = fabs(dNum);	// retain original value
	dd = floor(dh);
	dm = floor(((dh - dd) * 100.0) + 0.0001);
	ds = (((dh - dd) * 100.0) - dm) * 100.0;
	dd += ((dm / 60.0) + (ds / 3600.0));
	if (dNum < 0.0)
		dd *= -1;
	return dd;	
}

/////////////////////////////////////////////////////////////////////////
// DmintoDeg:  Computes angular value of decimal degrees from
//			  degrees and decimal minutes.
/////////////////////////////////////////////////////////////////////////
// On Entry:  value in degrees and decimal minutes
// On Exit:   value in decimal degrees
/////////////////////////////////////////////////////////////////////////
double GCALC_DmintoDeg(double dNum)
{
	double dh, dd;

	dh = fabs(dNum);	// retain original value
	dd = floor(dh);
	dd += (dh - dd) * 100.0 / 60.0;
	if (dNum < 0.0)
		dd *= -1;
	return dd;	
}

/////////////////////////////////////////////////////////////////////////
// ExportToAscii:  Exports a Binary NTv2 distortion grid to the specified
//			  output file. Any errors that occur are captured and an error
//            value is saved to IO_Status.
/////////////////////////////////////////////////////////////////////////
// On Entry:  File path for Binary distortion grid file 
// On Exit:   An ASCII grid file is created and saved to the specified file path
/////////////////////////////////////////////////////////////////////////
void GCALC_ExportToAscii(char *gridfile, char *gridtype, char *OutputGrid, int *IO_Status)
{
	int i, j;
	long lFileLen;
	float fTemp;
	FILE *f_out;

	*IO_Status = ERR_TRANS_SUCCESS;

	theApp.m_dPercentComplete = 0.0;

	// Open grid file
	if (theApp.m_pGridfile == NULL)
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);

	if (_stricmp(theApp.m_pGridfile->filename, gridfile) != 0)
	{
		theApp.ClearGridFileMemory();
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);
	}
	
	*IO_Status = theApp.m_Grid_Success;

	// Test for successful opening of grid file
	if (theApp.m_Grid_Success > ERR_TRANS_SUCCESS)
	{
		theApp.ClearGridFileMemory();
		return;
	}
	
	if (fopen_s(&f_out, OutputGrid, "w") != 0)
	{
		*IO_Status = ERR_OUTFILE_WRITE;
		return;
	}

	lFileLen = _filelength(_fileno(theApp.m_pGfileptr));

	// print header block information
	fprintf(f_out, "%8s%3d\n", "NUM_OREC", theApp.m_pGridfile->iH_info);		// Number of header identifiers (NUM_OREC)
	fprintf(f_out, "%8s%3d\n", "NUM_SREC", theApp.m_pGridfile->iSubH_info);		// Number of sub-header idents (NUM_SREC)
	fprintf(f_out, "%8s%3d\n", "NUM_FILE", theApp.m_pGridfile->iNumsubgrids);	// number of subgrids in file (NUM_FILE)
	fprintf(f_out, "%8s%-8s\n", "GS_TYPE ", theApp.m_pGridfile->chGs_type);		// grid shift type (GS_TYPE)
	fprintf(f_out, "%8s%-8s\n", "VERSION ", theApp.m_pGridfile->chVersion);		// grid file version (VERSION)
	fprintf(f_out, "%8s%-8s\n", "SYSTEM_F", theApp.m_pGridfile->chSystem_f);	// reference system (SYSTEM_F)
	fprintf(f_out, "%8s%-8s\n", "SYSTEM_T", theApp.m_pGridfile->chSystem_t);	// reference system (SYSTEM_T)
	fprintf(f_out, "%8s%12.3f\n", "MAJOR_F ", theApp.m_pGridfile->daf);			// semi major of from system (MAJOR_F)
	fprintf(f_out, "%8s%12.3f\n", "MINOR_F ", theApp.m_pGridfile->dbf);			// semi minor of from system (MINOR_F)
	fprintf(f_out, "%8s%12.3f\n", "MAJOR_T ", theApp.m_pGridfile->dat);			// semi major of to system (MAJOR_T)
	fprintf(f_out, "%8s%12.3f\n", "MINOR_T ", theApp.m_pGridfile->dbt);			// semi minor of to system (MINOR_T)
	
	// loop through number of sub grids
	for (i=0; i<theApp.m_pGridfile->iNumsubgrids; i++)
	{
		fprintf(f_out, "%8s%-8s\n", "SUB_NAME", theApp.m_pGridfile->ptrIndex[i].chSubname);	// grid shift type (SUB_NAME)
		fprintf(f_out, "%8s%-8s\n", "PARENT  ", theApp.m_pGridfile->ptrIndex[i].chParent);		// grid file version (PARENT)
		fprintf(f_out, "%8s%-8s\n", "CREATED ", theApp.m_pGridfile->ptrIndex[i].chCreated);		// reference system (CREATED)
		fprintf(f_out, "%8s%-8s\n", "UPDATED ", theApp.m_pGridfile->ptrIndex[i].chUpdated);		// reference system (UPDATED)
		
		if (strcmp(theApp.m_pGridfile->chGs_type, "RADIANS ") == 0)
		{
			// convert all seconds values to radians
			fprintf(f_out, "%8s%15.6f\n", "S_LAT   ", theApp.m_pGridfile->ptrIndex[i].dSlat / dRad2Sec);		// reference system (S_LAT)
			fprintf(f_out, "%8s%15.6f\n", "N_LAT   ", theApp.m_pGridfile->ptrIndex[i].dNlat / dRad2Sec);		// reference system (N_LAT)
			fprintf(f_out, "%8s%15.6f\n", "E_LONG  ", theApp.m_pGridfile->ptrIndex[i].dElong / dRad2Sec);		// reference system (E_LONG)
			fprintf(f_out, "%8s%15.6f\n", "W_LONG  ", theApp.m_pGridfile->ptrIndex[i].dWlong / dRad2Sec);		// reference system (W_LONG)
			fprintf(f_out, "%8s%15.6f\n", "LAT_INC ", theApp.m_pGridfile->ptrIndex[i].dLatinc / dRad2Sec);		// reference system (LAT_INC)
			fprintf(f_out, "%8s%15.6f\n", "LONG_INC", theApp.m_pGridfile->ptrIndex[i].dLonginc / dRad2Sec);		// reference system (LONG_INC)
		}
		else
		{
			fprintf(f_out, "%8s%15.6f\n", "S_LAT   ", theApp.m_pGridfile->ptrIndex[i].dSlat);		// reference system (S_LAT)
			fprintf(f_out, "%8s%15.6f\n", "N_LAT   ", theApp.m_pGridfile->ptrIndex[i].dNlat);		// reference system (N_LAT)
			fprintf(f_out, "%8s%15.6f\n", "E_LONG  ", theApp.m_pGridfile->ptrIndex[i].dElong);		// reference system (E_LONG)
			fprintf(f_out, "%8s%15.6f\n", "W_LONG  ", theApp.m_pGridfile->ptrIndex[i].dWlong);		// reference system (W_LONG)
			fprintf(f_out, "%8s%15.6f\n", "LAT_INC ", theApp.m_pGridfile->ptrIndex[i].dLatinc);		// reference system (LAT_INC)
			fprintf(f_out, "%8s%15.6f\n", "LONG_INC", theApp.m_pGridfile->ptrIndex[i].dLonginc);	// reference system (LONG_INC)
		}

		fprintf(f_out, "%8s%6ld\n", "GS_COUNT", theApp.m_pGridfile->ptrIndex[i].lGscount);		// reference system (GS_COUNT)

		// set file pointer to file position held by ptrIndex[i].lGridPos
		if (fseek(theApp.m_pGfileptr, theApp.m_pGridfile->ptrIndex[i].lGridPos, SEEK_SET) != 0)
		{
			*IO_Status = ERR_INFILE_READ;		// Could not seek file position from grid file
			fclose(f_out);		
			return;
		}			

		// read / write all coordinate shifts in this sub file
		for (j=0; j<theApp.m_pGridfile->ptrIndex[i].lGscount; j++)
		{
			// Latitude shift
			if (fread(&fTemp, sizeof(float), 1, theApp.m_pGfileptr) < 1)
				*IO_Status = ERR_READ_BIN_SHIFT;		// Could not retrieve shifts from Binary file
			else
				fprintf(f_out, "%10.6f", fTemp);
			
			// Longitude shift
			if (fread(&fTemp, sizeof(float), 1, theApp.m_pGfileptr) < 1)
				*IO_Status = ERR_READ_BIN_SHIFT;		// Could not retrieve shifts from Binary file
			else
				fprintf(f_out, "%10.6f", fTemp);
			
			// Latitude accuracy
			if (fread(&fTemp, sizeof(float), 1, theApp.m_pGfileptr) < 1)
				*IO_Status = ERR_READ_BIN_SHIFT;		// Could not retrieve shifts from Binary file
			else
				fprintf(f_out, "%10.6f", fTemp);
			
			// Longitude accuracy
			if (fread(&fTemp, sizeof(float), 1, theApp.m_pGfileptr) < 1)
				*IO_Status = ERR_READ_BIN_SHIFT;		// Could not retrieve shifts from Binary file
			else
				fprintf(f_out, "%10.6f\n", fTemp);

			theApp.m_dPercentComplete = (double)(ftell(theApp.m_pGfileptr)) / (double)lFileLen * 100.0;
			
			if (*IO_Status != ERR_TRANS_SUCCESS)
			{
				fclose(f_out);		
				return;
			}	
		}
	}

	theApp.m_dPercentComplete = 0.0;

	// print end of file record
	fprintf(f_out, "%-8s%10.2e\n", "END", 3.33E+32);
	fclose(f_out);
}


/////////////////////////////////////////////////////////////////////////
// ExportToBinary:  Exports an ASCII NTv2 distortion grid to the specified
//			  output file. Any errors that occur are captured and an error
//            value is saved to IO_Status.
/////////////////////////////////////////////////////////////////////////
// On Entry:  File path for ASCII distortion grid file 
// On Exit:   A Binary grid file is created and saved to the specified file path
/////////////////////////////////////////////////////////////////////////
void GCALC_ExportToBinary(char *gridfile, char *gridtype, char *OutputGrid, int *IO_Status)
{
	#define NULL_PAD 4
	int i, j;
	long lFileLen;
	const int OVERVIEW_RECS = 8;
	const char *format;
	char cBuf[100], cPad[NULL_PAD + 1];
	float fGSRecord[5];
	double dTemp;
	FILE *f_out;

	*IO_Status = ERR_TRANS_SUCCESS;

	theApp.m_dPercentComplete = 0.0;

	// Open grid file
	if (theApp.m_pGridfile == NULL)
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);

	if (_stricmp(theApp.m_pGridfile->filename, gridfile) != 0)
	{
		theApp.ClearGridFileMemory();
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);
	}
	
	*IO_Status = theApp.m_Grid_Success;

	// Test for successful opening of grid file
	if (theApp.m_Grid_Success > ERR_TRANS_SUCCESS)
	{
		theApp.ClearGridFileMemory();
		return;
	}
	
	if (fopen_s(&f_out, OutputGrid, "wb") != 0)
	{
		*IO_Status = ERR_OUTFILE_WRITE;
		return;
	}

	i = 0;
	memset(cPad, '\0', NULL_PAD);

	lFileLen = _filelength(_fileno(theApp.m_pGfileptr));

	// print header block information
	fwrite("NUM_OREC", OVERVIEW_RECS, 1, f_out);						// Number of header identifiers (NUM_OREC)
	fwrite(&theApp.m_pGridfile->iH_info, sizeof(int), 1, f_out);
	fwrite(cPad, sizeof(char), NULL_PAD, f_out);						// Pad out extra 4 bytes of null space
	
	fwrite("NUM_SREC", OVERVIEW_RECS, 1, f_out);						// Number of sub-header idents (NUM_SREC)
	fwrite(&theApp.m_pGridfile->iSubH_info, sizeof(int), 1, f_out);
	fwrite(cPad, sizeof(char), NULL_PAD, f_out);						// Pad out extra 4 bytes of null space
	
	fwrite("NUM_FILE", OVERVIEW_RECS, 1, f_out);						// number of subgrids in file (NUM_FILE)
	fwrite(&theApp.m_pGridfile->iNumsubgrids, sizeof(int), 1, f_out);
	fwrite(cPad, sizeof(char), NULL_PAD, f_out);						// Pad out extra 4 bytes of null space
		
	fwrite("GS_TYPE ", OVERVIEW_RECS, 1, f_out);						// grid shift type (GS_TYPE)
	fwrite(theApp.m_pGridfile->chGs_type, OVERVIEW_RECS, 1, f_out);
	fwrite("VERSION ", OVERVIEW_RECS, 1, f_out);						// grid file version (VERSION)
	fwrite(theApp.m_pGridfile->chVersion, OVERVIEW_RECS, 1, f_out);
	fwrite("SYSTEM_F", OVERVIEW_RECS, 1, f_out);						// reference system (SYSTEM_F)
	fwrite(theApp.m_pGridfile->chSystem_f, OVERVIEW_RECS, 1, f_out);
	fwrite("SYSTEM_T", OVERVIEW_RECS, 1, f_out);						// reference system (SYSTEM_T)
	fwrite(theApp.m_pGridfile->chSystem_t, OVERVIEW_RECS, 1, f_out);
	fwrite("MAJOR_F ", OVERVIEW_RECS, 1, f_out);						// semi major of from system (MAJOR_F)
	fwrite(&theApp.m_pGridfile->daf, sizeof(double), 1, f_out);
	fwrite("MINOR_F ", OVERVIEW_RECS, 1, f_out);						// semi minor of from system (MINOR_F)
	fwrite(&theApp.m_pGridfile->dbf, sizeof(double), 1, f_out);
	fwrite("MAJOR_T ", OVERVIEW_RECS, 1, f_out);						// semi major of to system (MAJOR_T)
	fwrite(&theApp.m_pGridfile->dat, sizeof(double), 1, f_out);
	fwrite("MINOR_T ", OVERVIEW_RECS, 1, f_out);						// semi minor of to system (MINOR_T)
	fwrite(&theApp.m_pGridfile->dbt, sizeof(double), 1, f_out);

	// loop through number of sub grids and read / write coordinate shifts
	for (i=0; i<theApp.m_pGridfile->iNumsubgrids; i++)
	{
		fwrite("SUB_NAME", OVERVIEW_RECS, 1, f_out);								// grid shift type (SUB_NAME)
		fwrite(&theApp.m_pGridfile->ptrIndex[i].chSubname, OVERVIEW_RECS, 1, f_out);
		fwrite("PARENT  ", OVERVIEW_RECS, 1, f_out);								// grid file version (PARENT)
		fwrite(&theApp.m_pGridfile->ptrIndex[i].chParent, OVERVIEW_RECS, 1, f_out);
		fwrite("CREATED ", OVERVIEW_RECS, 1, f_out);								// reference system (CREATED)
		fwrite(&theApp.m_pGridfile->ptrIndex[i].chCreated, OVERVIEW_RECS, 1, f_out);
		fwrite("UPDATED ", OVERVIEW_RECS, 1, f_out);								// reference system (UPDATED)
		fwrite(&theApp.m_pGridfile->ptrIndex[i].chUpdated, OVERVIEW_RECS, 1, f_out);
		
		if (strcmp(theApp.m_pGridfile->chGs_type, "RADIANS ") == 0)
		{
			// convert all seconds values to radians
			fwrite("S_LAT   ", OVERVIEW_RECS, 1, f_out);								// reference system (S_LAT)
			dTemp = theApp.m_pGridfile->ptrIndex[i].dSlat / dRad2Sec;
			fwrite(&dTemp, sizeof(double), 1, f_out);
			
			fwrite("N_LAT   ", OVERVIEW_RECS, 1, f_out);								// reference system (N_LAT)
			dTemp = theApp.m_pGridfile->ptrIndex[i].dNlat / dRad2Sec;
			fwrite(&dTemp, sizeof(double), 1, f_out);
			
			fwrite("E_LONG  ", OVERVIEW_RECS, 1, f_out);								// reference system (E_LONG)
			dTemp = theApp.m_pGridfile->ptrIndex[i].dElong / dRad2Sec;
			fwrite(&dTemp, sizeof(double), 1, f_out);
			
			fwrite("W_LONG  ", OVERVIEW_RECS, 1, f_out);								// reference system (W_LONG)
			dTemp = theApp.m_pGridfile->ptrIndex[i].dWlong / dRad2Sec;
			fwrite(&dTemp, sizeof(double), 1, f_out);
			
			fwrite("LAT_INC ", OVERVIEW_RECS, 1, f_out);								// reference system (LAT_INC)
			dTemp = theApp.m_pGridfile->ptrIndex[i].dLatinc / dRad2Sec;
			fwrite(&dTemp, sizeof(double), 1, f_out);
			
			fwrite("LONG_INC", OVERVIEW_RECS, 1, f_out);								// reference system (LONG_INC)
			dTemp = theApp.m_pGridfile->ptrIndex[i].dLonginc / dRad2Sec;
			fwrite(&dTemp, sizeof(double), 1, f_out);
		}
		else
		{
			fwrite("S_LAT   ", OVERVIEW_RECS, 1, f_out);								// reference system (S_LAT)
			fwrite(&theApp.m_pGridfile->ptrIndex[i].dSlat, sizeof(double), 1, f_out);
			fwrite("N_LAT   ", OVERVIEW_RECS, 1, f_out);								// reference system (N_LAT)
			fwrite(&theApp.m_pGridfile->ptrIndex[i].dNlat, sizeof(double), 1, f_out);
			fwrite("E_LONG  ", OVERVIEW_RECS, 1, f_out);								// reference system (E_LONG)
			fwrite(&theApp.m_pGridfile->ptrIndex[i].dElong, sizeof(double), 1, f_out);
			fwrite("W_LONG  ", OVERVIEW_RECS, 1, f_out);								// reference system (W_LONG)
			fwrite(&theApp.m_pGridfile->ptrIndex[i].dWlong, sizeof(double), 1, f_out);
			fwrite("LAT_INC ", OVERVIEW_RECS, 1, f_out);								// reference system (LAT_INC)
			fwrite(&theApp.m_pGridfile->ptrIndex[i].dLatinc, sizeof(double), 1, f_out);
			fwrite("LONG_INC", OVERVIEW_RECS, 1, f_out);								// reference system (LONG_INC)
			fwrite(&theApp.m_pGridfile->ptrIndex[i].dLonginc, sizeof(double), 1, f_out);
		}

		fwrite("GS_COUNT", OVERVIEW_RECS, 1, f_out);								// reference system (GS_COUNT)
		fwrite(&theApp.m_pGridfile->ptrIndex[i].lGscount, sizeof(int), 1, f_out);
		fwrite(cPad, sizeof(char), NULL_PAD, f_out);								// Pad out extra 4 bytes of null space
					
		// set file pointer to file position held by ptrIndex[i].lGridPos
		if (fsetpos(theApp.m_pGfileptr, &theApp.m_pGridfile->ptrIndex[i].fpGridPos) != 0)
		{
			// could not set file position
			*IO_Status = ERR_INFILE_READ;
			fclose(f_out);
			return;			
		}

		format = SHIFTS;

		// read / write all coordinate shifts in this sub file
		for (j=0; j<theApp.m_pGridfile->ptrIndex[i].lGscount; j++)
		{
			// Retrieve the whole record
			fgets(cBuf, 100, theApp.m_pGfileptr);
			if (sscanf_s(cBuf, format, &fGSRecord[0], &fGSRecord[1], &fGSRecord[2], &fGSRecord[3]) < 4)
			{
				// Could not retrieve shifts from Ascii file
				*IO_Status = ERR_READ_ASC_SHIFT;
				fclose(f_out);
				return;			
			}
			else
				fwrite(fGSRecord, sizeof(float), 4, f_out);		// print 2 shifts and 2 accuracy values
			theApp.m_dPercentComplete = (double)(ftell(theApp.m_pGfileptr)) / (double)lFileLen * 100.0;
		}
	}

	theApp.m_dPercentComplete = 0.0;

	// print end of file record
	fwrite("END     ", OVERVIEW_RECS, 1, f_out);
	dTemp = (double) 3.33E+32;
	fwrite(&dTemp, sizeof(double), 1, f_out);
	fclose(f_out);
}

// sets global switch for adding header information to formatted text and csv output files
void GCALC_SetFileTranHeader(int nAddHeader)
{
	theApp.m_AddHeader = nAddHeader;
}

/////////////////////////////////////////////////////////////////////////
// FileTran:  Computes new geographic coordinates for all points in a file,
//			  by applying lat and long correctional shift values from a 
//			  distortion grid. The grid file contains all necessary
//			  parameters for the conversion. If a line/record within the specified
//			  input file does not contain the required format, or does not have any 
//			  numbers the line is duplicated to the output file and the log file.
//			  If the point lies outside the grid the point is transformed using a
//			  seven parameter trasnsformation only and is noted within the log file.
// See "GCalc.h" for datum parameters.
/////////////////////////////////////////////////////////////////////////
// On Entry: The input file, output file, their file types, and user set values
//			 for HP Notation, DMS and Projection is passed by pCFile1.
//			 The location of the grid file is passed by gridfile.
//			 Ellipsoidal height is not used.
// On Exit:  New Latitude and Longitude is returned by cGeoValue. If there
//			 are any errors encountered in the file or in processing, the
//			 function displays a message and returns cGeoValue.iNum4 with
//			 a numeric status of the error.
/////////////////////////////////////////////////////////////////////////
int GCALC_FileTran(char *gridfile, char *gridtype, pointsfile *ptrCFile1)
{
	#define MAX_LENGTH 600

	FILE *f_in, *f_out;
	char *pHRec, cBuf[MAX_LENGTH + 1], *cResult;
	char cManBuffer[1002], cDate[12];
	int i, iF_inType, iF_outType;
	long lFileLen;
	CString strInDatum, strOutDatum, strInCoord, strOutCoord, strBuf;
	
	theApp.m_dPercentComplete = 0.0;
	GCALC_SetByteOffset();
	// initialise file record counters
	for (i=0; i<3; i++)
		ptrCFile1->lNumTrans[i] = 0;
	
	// error message instance
	CErrMsg err;
	err.strGridFile = gridfile;
	err.strInputFile = ptrCFile1->chFilein;
	err.strOutputFile = ptrCFile1->chFileout;

	coordinate cCVar1;
	memset(cBuf, '\0', sizeof(cBuf));
	ptrCFile1->IO_Status = ERR_TRANS_SUCCESS;
	
	// get pointer to input file
	if (fopen_s(&f_in, ptrCFile1->chFilein, "r") != 0)
	{
		ptrCFile1->IO_Status = ERR_INFILE_READ;
		err.ReportError(ptrCFile1->IO_Status, cBuf);
		GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
		return false;
	}
	lFileLen = _filelength(_fileno(f_in));

	// get pointer to output file
	if (fopen_s(&f_out, ptrCFile1->chFileout, "w") != 0)
	{
		ptrCFile1->IO_Status = ERR_OUTFILE_WRITE;
		fclose(f_in);
		err.ReportError(ptrCFile1->IO_Status, cBuf);
		GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
		return false;
	}

	// Open the specified grid file and read in all header info
	if (theApp.m_pGridfile == NULL)
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);

	// check for change of filename
	if (strcmp(theApp.m_pGridfile->filename, gridfile) != 0)
	{
		theApp.ClearGridFileMemory();
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);
	}

	ptrCFile1->IO_Status = theApp.m_Grid_Success;

	// test for failure
	if (ptrCFile1->IO_Status > ERR_TRANS_SUCCESS)			
	{
		err.ReportError(ptrCFile1->IO_Status, cBuf);
		GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
		theApp.ClearGridFileMemory();
		return false;
	}

	// test for "dat...etc", "csv", "qif", "man", and  "asd" file types, then read data appropriately
	iF_inType = theApp.DetermineFileType(ptrCFile1->chFInputType);
	iF_outType = theApp.DetermineFileType(ptrCFile1->chFOutputType);
	
	if (iF_inType < 0)
	{
		// unsupported input file type
		ptrCFile1->IO_Status = ERR_INFILE_TYPE;
		fclose(f_in);
		fclose(f_out);
		err.ReportError(ptrCFile1->IO_Status, cBuf);
		GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
		return false;
	}
	if (iF_outType < 0)
	{
		// unsupported output file type
		ptrCFile1->IO_Status = ERR_OUTFILE_TYPE;
		fclose(f_in);
		fclose(f_out);
		err.ReportError(ptrCFile1->IO_Status, cBuf);
		GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
		return false;
	}
	
	// Print output file header information (ONLY for CSV and DAT files!!! and if requested)
	if (iF_outType == TYPE_DAT || iF_outType == TYPE_CSV)
	{
		switch (theApp.m_AddHeader)
		{
		case 1:
			theApp.PrintFileHeader(f_out, ptrCFile1, gridfile);
			break;
		case 2:
			fputc('\n', f_out);
			break;
		}
	}	

	cResult = NULL;
	
	switch (iF_inType)
	{
	case TYPE_DAT:		// DAT		
		cResult = fgets(cBuf, sizeof(cBuf), f_in);
		while (cResult != NULL)			// while EOF not found
		{
			theApp.m_lBytesRead = ftell(f_in);
			theApp.m_dPercentComplete = (double)theApp.m_lBytesRead / (double)lFileLen * 100.0;
			cCVar1.iNum4 = ptrCFile1->InputDatum;
			// transform the record and print
			if (!theApp.ProcessDatFile(cBuf, _countof(cBuf), f_out, ptrCFile1, &cCVar1))
			{
				err.ReportError(ptrCFile1->IO_Status, cBuf);
				if (strcmp(cBuf, "\n") != 0)
					GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
				if (ptrCFile1->IO_Status == ERR_PT_OUTSIDE_GRID)
					GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, "NOTE: A 7-Parameter transformation has been performed.\n\n");
			}
			cResult = fgets(cBuf, 150, f_in);			
		}
		break;
	case TYPE_CSV:		// CSV
		cResult = fgets(cBuf, sizeof(cBuf), f_in);
		while (cResult != NULL)			// while EOF not found
		{
			theApp.m_lBytesRead = ftell(f_in);
			theApp.m_dPercentComplete = (double)theApp.m_lBytesRead / (double)lFileLen * 100.0;
			cCVar1.iNum4 = ptrCFile1->InputDatum;
			// transform the record and print
			if (!theApp.ProcessCsvFile(cBuf, _countof(cBuf), f_out, ptrCFile1, &cCVar1))
			{
				err.ReportError(ptrCFile1->IO_Status, cBuf);
				if (strcmp(cBuf, "\n") != 0)
					GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
				if (ptrCFile1->IO_Status == ERR_PT_OUTSIDE_GRID)
					GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, "NOTE: A 7-Parameter transformation has been performed.\n\n");			
			}	
			cResult = fgets(cBuf, 150, f_in);
		}			
		break;
	case TYPE_QIF:		// QIF
		cResult = fgets(cBuf, sizeof(cBuf), f_in);
		if (ptrCFile1->OutputDatum == GDA94)
		{
			strOutDatum = "GDA94";
			if (ptrCFile1->OutputProj == GEOGRAPHIC)
				strOutCoord = "GDA94";
			else
				strOutCoord = "MGA94";
		}
		else
		{
			strOutDatum = "GDA2020";
			if (ptrCFile1->OutputProj == GEOGRAPHIC)
				strOutCoord = "GDA2020";
			else
				strOutCoord = "MGA2020";
		}
		if (ptrCFile1->InputDatum == GDA94)
		{
			if (ptrCFile1->InputProj == GEOGRAPHIC)
				strInCoord = "GDA94";
			else
				strInCoord = "MGA94";
		}
		else
		{
			if (ptrCFile1->InputProj == GEOGRAPHIC)
				strInCoord = "GDA2020";
			else
				strInCoord = "MGA2020";
		}
		
		// write & read next 8 records of qif file (ie. the header)
		for (i=0; (i<8 && cResult != NULL); i++)
		{
			fprintf(f_out, "%s", cBuf);
			cResult = fgets(cBuf, sizeof(cBuf), f_in);
		}

		// replace "DATA IN ##### ON #####         "
		_snprintf_s(cBuf, _countof(cBuf), 30, "  DATA IN %.3s ON %s        ", strOutCoord, strOutDatum);
		fprintf(f_out,  "%s", cBuf);
					
		// create qif file index for max & min file & sub-file extents
		qifindex qf;
		qf.lHeaderPos = ftell(f_out);	// file pos at start of the File Header Block record
		
		cResult = fgets(cBuf, 150, f_in);

		// check for an empty file (...first character of record is '*')
		if (cBuf[0] == '*')
		{
			ptrCFile1->IO_Status = ERR_QIF_NODATA_READ;			// empty qif file.
			err.ReportError(ptrCFile1->IO_Status, cBuf);
			GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
			// copy remainder of file contents and return
			while (cResult != NULL)			// while EOF not found
			{
				fprintf(f_out, "%s", cBuf);
				cResult = fgets(cBuf, 150, f_in);
			}
			theApp.GetSystemDate(cDate, DATE_QIF);
			fprintf(f_out, "* Converted by GCalc Ver 3.00 on %s from %s to %s\n", cDate, strInCoord, strOutCoord);
			fclose(f_in);
			fclose(f_out);
			return false;
		}

		// check coordinate system for entire file
		strBuf = cBuf;
		strInDatum = strBuf.Mid(3, 5);
		if (cBuf[8] == 'A' || (strInDatum.CompareNoCase("MGA94") == 0))
		{
			// MGA94 projection
			if (ptrCFile1->InputDatum != GDA94 || ptrCFile1->InputProj == GEOGRAPHIC) //st
				ptrCFile1->IO_Status = ERR_QIF_COORD_MATCH;
		}
		else if (strcmp(strInDatum, "MGA2020") == 0)
		{
			// MGA2020 projection
			if (ptrCFile1->InputDatum != GDA2020 || ptrCFile1->InputProj == GEOGRAPHIC) //st
				ptrCFile1->IO_Status = ERR_QIF_COORD_MATCH;
		}
		else if (cBuf[8] == 'G' || (strInDatum.CompareNoCase("GDA94") == 0))
		{
			// GDA94 geographic
			if (ptrCFile1->InputDatum != GDA94 || ptrCFile1->InputProj != GEOGRAPHIC)
				ptrCFile1->IO_Status = ERR_QIF_COORD_MATCH;
		}
		else if (strcmp(strInDatum, "GDA2020") == 0)
		{
			// GDA2020 geographic
			if (ptrCFile1->InputDatum != GDA2020 || ptrCFile1->InputProj != GEOGRAPHIC)
				ptrCFile1->IO_Status = ERR_QIF_COORD_MATCH;
		}
		else
			ptrCFile1->IO_Status = ERR_QIF_COORD_READ;			// unable to determine coordinate parameters from file.

		if (ptrCFile1->IO_Status != ERR_TRANS_SUCCESS)
		{
			// An error occurred...halt transformation and report failure to log file
			err.ReportError(ptrCFile1->IO_Status, cBuf);
			GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));			
			fclose(f_in);
			fclose(f_out);				
			return false;
		}

		// initialise minimum & maximum file & sub-file extents
		if (ptrCFile1->OutputProj == GEOGRAPHIC)
		{
			qf.dHMaxEast = qf.dSMaxEast = 0.0;
			qf.dHMinEast = qf.dSMinEast = 90.0;
			qf.dHMaxNorth = qf.dSMaxNorth = 0.0;
			qf.dHMinNorth = qf.dSMinNorth = 180.0;
		}
		else
		{
			qf.dHMaxEast = qf.dSMaxEast = 2.225e-308;			// a very small number!
			qf.dHMinEast = qf.dSMinEast = 1.797e308;			// a very large number!
			qf.dHMaxNorth = qf.dSMaxNorth = 2.225e-308;
			qf.dHMinNorth = qf.dSMinNorth = 1.797e308;
		}
		
		// store 0 to indicate that no sub-file record (SX) has been found yet
		qf.lSubHeaderPos = 0;

		while (cResult != NULL)			// while EOF not found
		{
			theApp.m_lBytesRead = ftell(f_in);
			theApp.m_dPercentComplete = (double)theApp.m_lBytesRead / (double)lFileLen * 100.0;
			// transform the record and print
			if (!theApp.ProcessQifFile(cBuf, _countof(cBuf), (char *)((LPCTSTR)strOutCoord), f_out, ptrCFile1, &qf))
			{
				// An error occurred...halt transformation and report failure to log file
				err.ReportError(ptrCFile1->IO_Status, cBuf);
				GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));			
				fclose(f_in);
				fclose(f_out);
				return false;
			}						
			cResult = fgets(cBuf, 150, f_in);			
		}

		// Print maximum and minimums
		pHRec = qf.cFileRecord + 18;

		if (ptrCFile1->OutputProj != GEOGRAPHIC)		// projection
		{
			// print max & min sub-file extents to output file
			sprintf_s(pHRec, _countof(qf.cFileRecord)-18, "%15.3f%15.3f", qf.dHMinEast, qf.dHMinNorth);
			pHRec = qf.cFileRecord + 48;
			sprintf_s(pHRec, _countof(qf.cFileRecord)-18, "%15.3f%15.3f", qf.dHMaxEast, qf.dHMaxNorth);
		}
		else								// geographic
		{
			// print max & min sub-file extents to output file
			// NOTE: latitude max & min east are reversed to cater for neg south
			sprintf_s(pHRec, _countof(qf.cFileRecord)-18, "%15.10f%15.10f", qf.dHMinNorth, qf.dHMaxEast);
			pHRec = qf.cFileRecord + 48;
			sprintf_s(pHRec, _countof(qf.cFileRecord)-18, "%15.10f%15.10f", qf.dHMaxNorth, qf.dHMinEast);
		}
	
		// print extents and conversion metadata
		fseek(f_out, qf.lHeaderPos, SEEK_SET);
		fprintf(f_out, "%s", qf.cFileRecord);
		theApp.GetSystemDate(cDate, DATE_QIF);
		fseek(f_out, 0, SEEK_END);
		fprintf(f_out, "* Converted by GCalc Ver 3.00 on %s from %s to %s using the\n* following grid:\n* %s\n",
			cDate, strInCoord, strOutCoord, gridfile);
		break;
	case TYPE_MAN:		// MAN
		cResult = fgets(cManBuffer, 1000, f_in);
		while (cResult != NULL)
		{
			theApp.m_lBytesRead = ftell(f_in);
			theApp.m_dPercentComplete = (double)theApp.m_lBytesRead / (double)lFileLen * 100.0;
			cCVar1.iNum4 = ptrCFile1->InputDatum;
			// transform the record and print
			if (!theApp.ProcessManFile(cManBuffer, _countof(cManBuffer), f_out, ptrCFile1, &cCVar1))
			{
				// Use value held by ptrCFile1->IO_Status to report failure to log file
				//strBuf = cManBuffer;
				//strcpy(cBuf, strBuf.Mid(0, 6) + " ... " + strBuf.Mid(547, 28));
				err.ReportError(ptrCFile1->IO_Status, cManBuffer);
				GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
			}
			cResult = fgets(cManBuffer, 1000, f_in);
		}
		break;
	case 6:
		// Transformation of AS2482 mapping files (*.ASD) handled by Abakos Digital Images.
		// See Laurie Glass for details.
		break;
	case TYPE_RES:		// RES
		cResult = fgets(cBuf, 150, f_in);
		while (cResult != NULL)
		{
			theApp.m_lBytesRead = ftell(f_in);
			theApp.m_dPercentComplete = (double)theApp.m_lBytesRead / (double)lFileLen * 100.0;
			// transform the record and print
			if (!theApp.ProcessResFile(cBuf, _countof(cBuf), f_out, ptrCFile1, &cCVar1))
			{
				err.ReportError(ptrCFile1->IO_Status, cBuf);
				GCALC_PrintLogFile(ptrCFile1->chLogFile, ptrCFile1->chAccessMode, (char*)((LPCTSTR)err.ErrorString));
			}
			cResult = fgets(cBuf, 150, f_in);
		}
		break;
	default:
		// unsupported input file type
		ptrCFile1->IO_Status = ERR_INFILE_TYPE;
		fclose(f_in);
		fclose(f_out);
		return false;
	}

	theApp.m_dPercentComplete = 0.0;

	cResult = NULL;
	fclose(f_in);
	fclose(f_out);
	return true;
}

/////////////////////////////////////////////////////////////////////////
// FormatDMS_String:  Takes a number and prints it to a char array with
//			  degrees minutes (and seconds) notation.
/////////////////////////////////////////////////////////////////////////
// On Entry:  Number as a double
// On Exit:   The formatted number is printed to the char array with width 17.
/////////////////////////////////////////////////////////////////////////
void GCALC_FormatDMS_String(char *buf, int buf_size, double dVar1, int iType)
{
	int i;
	CString strValue;

	strValue.Format("%.9f", dVar1);
	i = strValue.Find('.');

	if (iType == 0)
		strValue = strValue.Mid(0, i) + " " + strValue.Mid(i+1, 2) + " " + strValue.Mid(i+3, 2) + "." + strValue.Mid(i+5);
	else
		strValue = strValue.Mid(0, i) + " " + strValue.Mid(i+1, 2) + "." + strValue.Mid(i+3);

	sprintf_s(buf, buf_size, "%17s", strValue);
}


/////////////////////////////////////////////////////////////////////////
// GeotoCart: Computes Cartesian coordinates of X, Y and Z from 
//			  Geodetic Latitude and Longitude. 
//			  See "GCalc.h" for datum parameters.
/////////////////////////////////////////////////////////////////////////
// On Entry: Latitude and Longitude in decimal degrees is passed by
//			 cGeoValue 
// On Exit:  X, Y and Zone returned by cGeoValue
/////////////////////////////////////////////////////////////////////////
void GCALC_GeotoCart(coordinate *pGeoValue)
{
	double f, e2, Nu, a, Inv_f, Var1;

	if (pGeoValue->iNum4 == GDA94)
	{
		a = GDA94a;
		Inv_f = GDA94Inv_f;
	}
	else
	{
		a = GDA20a;
		Inv_f = GDA20Inv_f;
	}

	Var1 = pGeoValue->dNum1 * pi / 180;
	pGeoValue->dNum2 = pGeoValue->dNum2 * pi / 180;

	f = 1.0 / Inv_f;
	e2 = (2.0 * f) - (f * f);
	Nu = a / pow((1.0 - (e2 * pow(sin(Var1), 2.0))), 0.5);
			
	pGeoValue->dNum1 = (Nu + pGeoValue->dNum3) * cos(Var1) * cos(pGeoValue->dNum2);
	pGeoValue->dNum2 = (Nu + pGeoValue->dNum3) * cos(Var1) * sin(pGeoValue->dNum2);
	pGeoValue->dNum3 = (((1 - e2) * Nu) + pGeoValue->dNum3) * sin(Var1);
}


/////////////////////////////////////////////////////////////////////////
// GeotoGrid: Computes Universal Transverse Mercator Projection 
//			  Coordinates of Easting, Northing and Zone from Geodetic Latitude and
//			  Longitude. 
//			  See "GCalc.h" for standard UTM and GDA2020 parameters.
/////////////////////////////////////////////////////////////////////////
// On Entry: Latitude and Longitude in decimal degrees is passed by pGeoValue
//			 Datum is held by pGridValue->iNum4. 
// On Exit:  Easting, Northing and Zone stored in pGridValue
/////////////////////////////////////////////////////////////////////////
void GCALC_GeotoGrid(coordinate *pGeoValue)
{
	double Inv_f, f, a, b, e2, m, omega, lat; 
	double A0, A2, A4, A6;
	double Rho, Nu, CMeridian, Num1;
	double Term1, Term2, Term3, Term4;

	if (pGeoValue->iNum4 == GDA94)
	{
		a = GDA94a;
		Inv_f = GDA94Inv_f;
	}
	else
	{
		a = GDA20a;
		Inv_f = GDA20Inv_f;
	}

	// Compute Zone if not previously specified
	if(pGeoValue->dNum3 < 47.0 || pGeoValue->dNum3 > 58.0)
		pGeoValue->dNum3 = floor(((pGeoValue->dNum2) - LweZ0) / Zw);

	// Compute Geodetic Longitude of the Central Meridian of pGeoValue->dNum3 (the UTM Zone)
	CMeridian = (pGeoValue->dNum3 * Zw) + LcmZ0;

	// Compute diff in Longitude between CMeridian and Longitude
	omega = pGeoValue->dNum2 - CMeridian;

	// Convert CMeridian, omega & Latitude to radians
	lat = pGeoValue->dNum1 * pi / 180.0;
	CMeridian = CMeridian * pi / 180.0;
	omega = omega * pi / 180.0;

	f = 1 / Inv_f;
	b = a * (1 - f);
	e2 = (2.0 * f) - (f * f);

	Rho = a * (1.0 - e2) / pow( (1.0 - (e2 * pow(sin(lat), 2.0))), 1.5);
	Nu = a / pow( (1.0 - (e2 * pow(sin(lat), 2.0))), 0.5);

	A0 = 1.0 - (e2 / 4.0) - (3.0 * pow(e2, 2.0) / 64.0) - (5.0 * pow(e2, 3.0) / 256.0);
	A2 = 3.0 / 8.0 * (e2 + (pow(e2, 2.0) / 4.0) + (15.0 * pow(e2, 3.0) / 128.0));
	A4 = 15.0 / 256.0 * (pow(e2, 2.0) + (3.0 * pow(e2, 3.0) / 4.0));
	A6 = 35.0 * pow(e2, 3.0) / 3072.0;

	m = (a * (
		(A0 * lat) - 
		(A2 * sin(2.0 * lat)) +
		(A4 * sin(4.0 * lat)) -
		(A6 * sin(6.0 * lat))
		));

	Num1 = (Ko * Nu * omega * cos(lat));

	// Compute Easting
	Term1 = (
		(pow(omega, 2.0) / 6.0) * 
		(pow(cos(lat), 2.0)) *
		((Nu / Rho) - pow(tan(lat), 2.0))
		);
	Term2 = (
		(pow(omega, 4.0) / 120) *
		(pow(cos(lat), 4.0)) *
		( ((4.0 * pow((Nu / Rho), 3.0)) * (1.0 - (6.0 * pow(tan(lat), 2.0)))) +
		((pow((Nu / Rho), 2.0)) * (1.0 + (8.0 * pow(tan(lat), 2.0)))) -
		(Nu / Rho * 2.0 * pow(tan(lat), 2.0)) + 
		(pow(tan(lat), 4.0)) )
		);
	Term3 = (
		(pow(omega, 6.0) / 5040) *
		(pow(cos(lat), 6.0)) *
		(61.0 - (479.0 * pow(tan(lat), 2.0)) +
			  (179.0 * pow(tan(lat), 4.0)) -
			  (pow(tan(lat), 4.0)))
		);


	pGeoValue->dNum1 = (Num1 * (1.0 + Term1 + Term2 + Term3)) + FALSE_E;

	// Compute Northing
	Term1 = (pow(omega, 2.0) / 2.0 * Nu * sin(lat) * cos(lat));
	Term2 = (pow(omega, 4.0) / 24.0 * Nu * sin(lat) * pow(cos(lat), 3.0));
	Term2 *= (
		(4.0 * pow((Nu / Rho), 2.0)) + (Nu / Rho) - 
		(pow(tan(lat), 2.0))
		);
	Term3 = (
		pow(omega, 6.0) / 720.0 * Nu *
		sin(lat) *
		pow(cos(lat), 5.0)
		);
	Term3 *= ( 
		(8.0 * pow((Nu / Rho), 4.0) * (11.0 - (24.0 * pow(tan(lat), 2.0)))) - 
		(28.0 * pow((Nu / Rho), 3.0) * (1.0 - (6.0 * pow(tan(lat), 2.0)))) +
		(pow((Nu / Rho), 2.0) * (1.0 - (32.0 * pow(tan(lat), 2.0)))) -
		(Nu / Rho * 2.0 * pow((tan(lat)), 2.0))+
		(pow(tan(lat), 4.0))
		);
	Term4 = (
		pow(omega, 8.0) / 40320 * Nu *
		sin(lat) *
		pow(cos(lat), 7.0)
		);
	Term4 *= (1385.0 -
		(3111.0 * pow(tan(lat), 2.0)) +
		(543.0 * pow(tan(lat), 4.0)) -
		(pow(tan(lat), 6.0))
		);

	pGeoValue->dNum2 = (Ko * (m + Term1 + Term2 + Term3 + Term4)) + FALSE_N;
}

/////////////////////////////////////////////////////////////////////////
// GridtoGeo: Computes Geodetic Latitude and Longitude from
//			  Universal Transverse Mercator Projection Coordinates of
//			  Easting, Northing and Zone.
//			  See "GCalc.h" for standard UTM and GDA2020 parameters.
/////////////////////////////////////////////////////////////////////////
// On Entry: Easting, Northing and Zone is passed by pGridValue
//			 Datum is held by pGridValue->iNum4. 
// On Exit:  Latitude and Longitude in decimal degrees is stored in 
//			 pGridValue
/////////////////////////////////////////////////////////////////////////
void GCALC_GridtoGeo(coordinate *pGridValue)
{
	double EPrime, NPrime, x, m, f, Inv_f, a, b, e2, e;
	double Seconde2, Seconde, n, n2, n3, n4, G;
	double sigma, LatPrime, Rho, Nu, CMeridian;
	double Num1;	// Num1 = tan(LatPrime) / (Ko * Rho)
	double Term1, Term2, Term3, Term4;

	if (pGridValue->iNum4 == GDA94)
	{
		a = GDA94a;
		Inv_f = GDA94Inv_f;
	}
	else
	{
		a = GDA20a;
		Inv_f = GDA20Inv_f;
	}

	f = 1 / Inv_f;
	b = a * (1 - f);
	e2 = (2 * f) - (f * f);
	e = sqrt(e2);
	Seconde2 = e2 / (1 - e2);
	Seconde = sqrt(Seconde2);
	n = (a - b) / (a + b);
	n2 = pow(n, 2.0);
	n3 = pow(n, 3.0);
	n4 = pow(n, 4.0);
	G = a * (1 - n) * (1 - n2);
	G *= (1 + (9 * n2 / 4) + (225 * n4 / 64));
	G *= (pi / 180);

	EPrime = pGridValue->dNum1 - FALSE_E;
	NPrime = pGridValue->dNum2 - FALSE_N;
	m = NPrime / Ko;
	sigma = (m * pi) / (180 * G);
	
	LatPrime = sigma + (( (3 * n / 2) - (27 * n3 / 32) ) * sin(2 * sigma));
	LatPrime += ( (21 * n2 / 16) - (55 * n4 / 32) ) * sin(4 * sigma);
	LatPrime += (151 * n3 / 96) * sin(6 * sigma);
	LatPrime += (1097 * n4 / 512) * sin(8 * sigma);

	Rho = a * (1 - e2) / pow( (1 - (e2 * pow(sin(LatPrime), 2.0))), 1.5);
	Nu = a / pow( (1 - (e2 * pow(sin(LatPrime), 2.0))), 0.5);
	Num1 = tan(LatPrime) / (Ko * Rho);
	x = EPrime / (Ko * Nu);
	
	Term1 = Num1 * x * EPrime / 2;
	Term2 = Num1 * EPrime * pow(x, 3.0) / 24;
	Term2 *= ( (-4 * pow((Nu / Rho), 2.0)) + (9 * Nu / Rho * (1 - pow((tan(LatPrime)), 2.0))) + (12 * pow((tan(LatPrime)), 2.0))  );
	Term3 = Num1 * EPrime * pow(x, 5.0) / 720;
	Term3 *= ( 
		(8 * pow((Nu / Rho), 4.0) * (11 - (24 * pow((tan(LatPrime)), 2.0)))) - 
		(12 * pow((Nu / Rho), 3.0) * (21 - (71 * pow((tan(LatPrime)), 2.0)))) +
		(15 * pow((Nu / Rho), 2.0) * (15 - (98 * pow((tan(LatPrime)), 2.0)) + (15 * pow((tan(LatPrime)), 4.0)))) +
		(180 * (Nu / Rho) * ((5 * pow((tan(LatPrime)), 2.0))-(3 * pow((tan(LatPrime)), 4.0))))+
		(360 * pow((tan(LatPrime)), 4.0))
		);
	Term4 = Num1 * EPrime * pow(x, 7.0) / 40320;
	Term4 *= (1385 +
		(3633 * pow((tan(LatPrime)), 2.0)) +
		(4095 * pow((tan(LatPrime)), 4.0)) +
		(1575 * pow((tan(LatPrime)), 6.0))
		);

	// Store Decimal degrees value of Geodetic Latitude
	pGridValue->dNum1 = ((LatPrime - Term1 + Term2 - Term3 + Term4) / pi * 180);

	// Compute Geodetic Longitude of the Central Meridian of pGridValue->dNum3 (the UTM Zone) in radians
	CMeridian = ((pGridValue->dNum3 * Zw) + LcmZ1 - Zw) * pi / 180;
	
	Num1 = 1 / (cos(LatPrime));

	Term1 = x * Num1;
	Term2 = ( pow(x, 3.0) / 6 * Num1 * ((Nu / Rho) + (2 * pow((tan(LatPrime)), 2.0))) );
	Term3 = ( pow(x, 5.0) / 120 * Num1 * (
		(-4 * pow((Nu / Rho), 3.0) * (1 - (6 * pow((tan(LatPrime)), 2.0)))) +
		(pow((Nu / Rho), 2.0) * (9 - (68 * pow((tan(LatPrime)), 2.0)))) +
		(72 * (Nu / Rho) * (pow((tan(LatPrime)), 2.0))) +
		(24 * (pow((tan(LatPrime)), 4.0)))
		));
	Term4 = ( pow(x, 7.0) / 5040 * Num1 * (
		61 + (662 * (pow((tan(LatPrime)), 2.0))) +
		(1320 * (pow((tan(LatPrime)), 4.0))) +
		(720 * (pow((tan(LatPrime)), 6.0)))
		));

	// Store Decimal degrees value of Geodetic Longitude
	pGridValue->dNum2 = ((CMeridian + Term1 - Term2 + Term3 - Term4) / pi * 180);

}

/////////////////////////////////////////////////////////////////////////
// MapGridTransformation: Computes new projection coordinates for a single
//			              point by wrapping the functions GridtoGeo,
//			 BiLinearTransformation, and GeotoGrid. See "GCalc.h" for datum
//           parameters.
/////////////////////////////////////////////////////////////////////////
// On Entry: Easting, Northing and Zone is passed by pGridValue
//			 Datum is held by pGridValue->iNum4.
// On Exit:  Easting, Northing and Zone stored in pGridValue. If there
//			 are any errors encountered in the file or in processing, the
//			 function returns cGridValue.iNum4 with a non-zero numeric 
//			 status of the error.
/////////////////////////////////////////////////////////////////////////
void GCALC_MapGridTransformation(coordinate *pGridValue)
{
	int input_datum = pGridValue->iNum4;
	pGridValue->IO_Status = ERR_TRANS_SUCCESS;		// initialise success
	if (theApp.m_pGridfile != NULL)					// Has gridfile index been created?
	{
		GCALC_GridtoGeo(pGridValue);				// convert to geographic
		if (theApp.ApplyDistortionGrid(pGridValue) == ERR_TRANS_SUCCESS)
		{
			pGridValue->iNum4 = !pGridValue->iNum4;	// change to opposite datum
			GCALC_GeotoGrid(pGridValue);			// convert to projection
			pGridValue->iNum4 = input_datum;		// restore original input datum
		}
	}
	else
		pGridValue->IO_Status = ERR_GRID_UNOPENED;	// gridfile index has not been created
}

/////////////////////////////////////////////////////////////////////////
// PointTran: Computes new geographic coordinates for a single point, by
//			  applying lat and long correctional shift values from a 
//			  distortion grid. The grid file contains all necessary
//			  parameters for the conversion.
// See "GCalc.h" for datum parameters.
/////////////////////////////////////////////////////////////////////////
// On Entry: Latitude and Longitude in decimal degrees is passed by
//			 cGeoValue. The location of the grid file is passed by gridfile.
//			 Ellipsoidal height is not used.
// On Exit:  New Latitude and Longitude is returned by cGeoValue. If there
//			 are any errors encountered in the file or in processing, the
//			 function displays a message and returns cGeoValue.iNum4 with
//			 a numeric status of the error.
/////////////////////////////////////////////////////////////////////////
void GCALC_PointTran(char *gridfile, char *gridtype, coordinate *pGeoValue)
{
	// initialise success
	pGeoValue->IO_Status = ERR_TRANS_SUCCESS;

	if (theApp.m_pGridfile == NULL)		// Open grid file
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);
	if (_stricmp(theApp.m_pGridfile->filename, gridfile) != 0)
	{
		theApp.ClearGridFileMemory();
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);
	}
	if (theApp.m_Grid_Success > ERR_TRANS_SUCCESS)	// Test for failure
	{
		pGeoValue->IO_Status = theApp.m_Grid_Success;
		theApp.ClearGridFileMemory();
		return;
	}
	pGeoValue->IO_Status = theApp.ApplyDistortionGrid(pGeoValue);
}



/////////////////////////////////////////////////////////////////////////
// PrintLogFile: Prints the contents of a char array to a file.
/////////////////////////////////////////////////////////////////////////
// On Entry: File path, access mode and char array are passed. 
// On Exit:  The file is opened, printed to and closed. If the file does not exist,
//			 false is returned.
/////////////////////////////////////////////////////////////////////////
int GCALC_PrintLogFile(char *logfile, char *access_mode, char *logrecord)
{
	CString strHeader;
	char cBuf[50];
	char date[] = "<< Date >>";
	char *pdest = NULL;
	FILE *f_log = NULL;

	if (strcmp(access_mode, "a") != 0 && strcmp(access_mode, "w") != 0)
		return false;
	if (fopen_s(&f_log, logfile, access_mode) != 0)
		return false;

	strHeader = logrecord;
	pdest = strstr(logrecord, date);	// find occurrence of << Date >>

	if (pdest != NULL)
	{
		// get system date
		theApp.GetSystemDate(cBuf, DATE_LONG);
		strHeader = strHeader.Mid(0, (int)(pdest - logrecord)) + cBuf + (pdest + strlen(date));
	}
	fprintf(f_log, "%s", strHeader);
	fclose(f_log);
	return true;
}


/////////////////////////////////////////////////////////////////////////
// ReportGridProperties: Retrieves the fundamental parameters from a grid file.
/////////////////////////////////////////////////////////////////////////
// On Entry: The full file path of the grid file and its type is passed with a 
//			 pointer to a file_par struct.
// On Exit:  Checks if the grid file has been read or not. If so, copies the
//			 grid file parameters from memory (theApp.m_pGridfile). Otherwise the
//			 file is opened and stored in memory. success is modified to depict
//			 a numeric status of the error.
/////////////////////////////////////////////////////////////////////////
void GCALC_ReportGridProperties(char *gridfile, char *gridtype, file_par *gridproperties, int *success)
{
	*success = ERR_TRANS_SUCCESS;

	if (theApp.m_pGridfile == NULL)
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);
	if (_stricmp(theApp.m_pGridfile->filename, gridfile) != 0)		// a new filename or filetype
	{
		theApp.ClearGridFileMemory();
		theApp.m_Grid_Success = theApp.Opengridfile(gridfile, gridtype);
	}
	
	// get grid properties on success
	if (theApp.m_Grid_Success < ERR_GRIDFILE_READ)
		memcpy(gridproperties, theApp.m_pGridfile, sizeof(*theApp.m_pGridfile));

	// Australian binary file encountered (on success)
	if (theApp.m_Can_Format == false && theApp.m_Grid_Success < ERR_GRIDFILE_READ)
		theApp.m_Grid_Success = ERR_AUS_BINARY;

	if (theApp.m_Grid_Success > ERR_TRANS_SUCCESS)
		theApp.ClearGridFileMemory();

	*success = theApp.m_Grid_Success;
}


/////////////////////////////////////////////////////////////////////////
// ReturnFileProgress: Returns the current progress of the file read/write process.
/////////////////////////////////////////////////////////////////////////
// On Entry: Nil.
// On Exit:  The value held by m_dPercentComplete.
/////////////////////////////////////////////////////////////////////////
long GCALC_ReturnFileProgress()
{
	return (long)theApp.m_dPercentComplete;
}


/////////////////////////////////////////////////////////////////////////
// GetByteOffset: Returns the byte offset of the file currently being read.
/////////////////////////////////////////////////////////////////////////
// On Entry: Nil.
// On Exit:  The value held by m_lBytesRead.
/////////////////////////////////////////////////////////////////////////
long GCALC_GetByteOffset()
{
	return theApp.m_lBytesRead;
}


/////////////////////////////////////////////////////////////////////////
// GetByteOffset: Sets the byte offset to 0.
/////////////////////////////////////////////////////////////////////////
// On Entry: Nil.
// On Exit:  theApp.m_lBytesRead = 0
/////////////////////////////////////////////////////////////////////////
void GCALC_SetByteOffset()
{
	theApp.m_lBytesRead = 0;
}
	

// extern "C"
}