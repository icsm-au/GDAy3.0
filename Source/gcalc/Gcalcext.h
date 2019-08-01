/////////////////////////////////////////////////////////////////////////////////////
/*
		Filename:
			GCalcext.h
		
		Author:
			Roger Fraser. 

		Copyright
			Natural Resources and Mines (The state of Queensland, (C) 2001).

		Created:		23 June 1999.
		Last Modified:  22 July 2002.

		Build Version:		2.100
		Previous Version:	2.010

		Procedures:
			GCALC_BiLinearTransformation(coordinate *pThePoint);
			GCALC_CarttoGeo(coordinate *pCartValue);
			GCALC_CreateGridIndex(char *filename, char *filetype, int *success);
			GCALC_DatumTran(coordinate *pDatumValue, double dParameters[]);
			GCALC_DatumTranV(coordinate *pDatumValue, double dParameters[], double dVelocities[]);
			GCALC_DegtoDms(double dNum);
			GCALC_DegtoDmin(double dNum);
			GCALC_DmstoDeg(double dNum);
			GCALC_DmintoDeg(double dNum);
			GCALC_ExportToAscii(char *gridfile, char *gridtype, char *OutputGrid, int *IO_Status);
			GCALC_ExportToBinary(char *gridfile, char *gridtype, char *OutputGrid, int *IO_Status);
			GCALC_FileTran(char *gridfile, char *gridtype, pointsfile *ptrCFile1, BOOL ADD_HEADER);
			GCALC_FormatDMS_String(char *buf, double dVar1, int iType);
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


*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CALC_H
#define CALC_H

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CALCAPI not used since the exported functions are defined in "GCalc.def".
// The exports are deliberately managed this way in order to retain the "original"
// entry points for the exported functions so as to support backward compatibility
// of the DLL.

//#ifndef _GCALC_
//	#define CALCAPI __declspec(dllexport) __stdcall
//#else
//	#define CALCAPI __declspec(dllexport) __stdcall
//#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////
// Define typedef struct coordinate
typedef struct {
	double dNum1;			// Latitude / Easting
	double dNum2;			// Longitude / Northing
	double dNum3;			// Zone...Height is passed as zero for 7 param trans
	double dLatacc;			// Latitude shift accuracy
	double dLongacc;		// Longitude shift accuracy
	int iNum4;				// On calling dll, is assigned 0 or 1 to indicate GDA94 or GDA2020
	int IO_Status;			// Holds status type for Point Input Output success\n
							/* -1:	The grid file has not been opened. Returned when a call is made to BiLinearTransformation without prviously calling CreateGridIndex.
							    0:	Success...Point read and written.
							    1:	Could not read from grid file...perhaps a wrong format or file is corrupted
							    2:	Unable to read this type of grid file
							    3:	The grid file is corrupt
							   12:	Invalid Zone...cannot transform to geographic from grid coords
							   13:	Point is outside limits of the grid
							   14:	There is an error in the grid
							   15:	Could not retrieve shifts from Ascii file
							   16:	Could not retrieve shifts from Binary file
							*/
} coordinate;

///////////////////////////////////////////////////
// Define typedef struct pointsfile
typedef struct {
	char chAppName[31];		// Brief name/description of the app that calls this dll. This name is printed within the header of all output files.
	char chFilein[601];		// Full file pathname for coordinate input file to be read
	char chFInputType[4];	// File type for input file to be read
	char chFileout[601];	// Full file pathname for coordinate output file to be written
	char chFOutputType[4];	// File type for output file to be written
	char chLogFile[601];	// Log filename
	char chAccessMode[3];	// Write / append access mode for fopen(...)
	int InputHP;			// HP Notation status of Input coordinates (0 = Not HP Notation; 1 = HP Notation) 
	int InputDMS;			// DMS status of Input coordinates (0 = DMS; 1 = Degrees Minutes; 2 = Decimal Degrees) 
	int InputDatum;			// Datum of Input Coordinates (0 = GDA94; 1 = GDA2020)
	int InputProj;			// Projection status of Input Coordinates (0 = None; 1 = Projected)
	int OutputHP;			// HP Notation status of Output coordinates (0 = Not HP Notation; 1 = HP Notation) 
	int OutputDMS;			// DMS status of Output coordinates (0 = DMS; 1 = Degrees Minutes; 2 = Decimal Degrees)
	int OutputDatum;		// Datum of Output Coordinates (0 = GDA94; 1 = GDA2020)
	int OutputProj;			// Projection status of Output Coordinates (0 = None; 1 or 47 < x > 58 = Projected)
	long lNumTrans[4];		// Number of success / fail transfromed points ([0] = success, [1] = total num read, [2] = total num interpolated )
	double dParameters[8];	// Holds 7 parameters. Used when a point lies outside the grid.
	int IO_Status;			// Holds status type for File Input Output success
							/* 0:	Success...Point read and written.
							   1:	Could not read from grid file...perhaps a wrong format or file is corrupted
							   2:	Unable to read this type of grid file
							   3:	The grid file is corrupt
							   4:	Could not read from input file
							   5:	Could not write to output file
							   6:	Unsupported input file
							   7:	Unsupported output file
							   8:	No data contained within line just read
							   9:	Line is too short to contain valid coordinates
							  10:	Line does not contain valid numeric input
							  11:	Could not read required data...possibly wrong format
							  12:	Invalid Zone...cannot transform to geographic from grid coords
							  13:	Point is outside limits of the grid
							  14:	There is an error in the grid
							  15:	Could not retrieve shifts from Ascii file
							  16:	Could not retrieve shifts from Binary file
							  17:	Insufficient fields for a valid csv record
							  18:	Input coordinate system parameters do not match the specified qif file
							  19:	Unable to determine coordinate parameters from the Input QIF file.
							  20:   An Empty QIF file
							  21:	Input coordinate system parameters do not match the specified res file
							  22:	Unable to transform res file coordinates of this type
							*/
} pointsfile;

///////////////////////////////////////////////////
// Declare typedef struct gridfileindex
struct gridfileindex {
	char chSubname[9];  		// name of subgrid (SUB_NAME)
	char chParent[9];			// name of parent grid (PARENT)
	char chCreated[9];			// date of creation (CREATED)
	char chUpdated[9];			// date of last file update (UPDATED)
	double dSlat;				// lower latitude (S_LAT)
	double dNlat;				// upper latitude (N_LAT)
	double dElong;				// lower longitude (E_LONG)
	double dWlong;				// upper longitude (W_LONG)
	double dLatinc;				// latitude interval (LAT_INC)
	double dLonginc;			// longitude interval (LONG_INC)
	long lGscount;				// number of nodes (GS_COUNT)
	long lGridPos;				// position of first record of grid shifts in Binary File
	fpos_t fpGridPos;			// position of first record of grid shifts in ASCII File
}; 

///////////////////////////////////////////////////
// Declare typedef struct filepar
typedef struct {
	gridfileindex *ptrIndex;	// pointer to an index (an array) of grid sub files
	char filename[601];			// Distortion grid file name
	char filetype[4];			// Distortion grid file type (ascii: "asc" = 0; binary: "gsb" = 1)
	char chGs_type[9];			// grid shift type (GS_TYPE)
	char chVersion[9];			// grid file version (VERSION)
	char chSystem_f[9];			// reference system (SYSTEM_F)
	char chSystem_t[9];			// reference system (SYSTEM_T)
	double daf;					// semi major of from system (MAJOR_F)
	double dat;					// semi major of to system (MAJOR_T)
	double dbf;					// semi minor of from system (MINOR_F)
	double dbt;					// semi minor of to system (MINOR_T)
	double dLat;				// user input latitude
	double dLatacc;				// interpolated latitude accuracy
	double dLong;				// user input longitude
	double dLongacc;			// interpolated longitude accuracy
	int iH_info;				// Number of header identifiers (NUM_OREC)
	int iSubH_info;				// Number of sub-header idents (NUM_SREC)
	int iNumsubgrids;			// number of subgrids in file (NUM_FILE)
	int iPointFlag;				// a four way flag indicating the location of the point within subgrid
	int iTheGrid;				// The array index of the most dense sub grid
} file_par;

///////////////////////////////////////////////////
// Declare External Procedures
extern "C"{
	// Transforms geographic coordinates using bi linear interpolation 
	void GCALC_BiLinearTransformation(coordinate *pThePoint);

	// Transforms cartesian coordinates to geographic coordinates
	void GCALC_CarttoGeo(coordinate *pCartValue);

	// Reads the gridfile header information into memory
	void GCALC_CreateGridIndex(char *filename, char *filetype, int *success);

	// Performs a 7 parameter datum transformation
	void GCALC_DatumTran(coordinate *pDatumValue, double dParameters[]);

	// Converts decimal degrees to deg.minsec
	double GCALC_DegtoDms(double dNum);

	// Converts decimal degrees to deg.min
	double GCALC_DegtoDmin(double dNum);

	// Converts deg.minsec to decimal degrees
	double GCALC_DmstoDeg(double dNum);

	// Converts deg.min to decimal degrees
	double GCALC_DmintoDeg(double dNum);

	// Exports an Binary grid file to a Ascii format
	void GCALC_ExportToAscii(char *gridfile, char *gridtype, char *OutputGrid, int *IO_Status);

	// Exports an Ascii grid file to a Binary format
	void GCALC_ExportToBinary(char *gridfile, char *gridtype, char *OutputGrid, int *IO_Status);

	// Set Global to add Header to output files
	void GCALC_SetFileTranHeader(int nAddHeader);
	
	// Transforms a file of coordinates
	int GCALC_FileTran(char *gridfile, char *gridtype, pointsfile *ptrCFile1);

	// Prints geographic coordinate value to a char array in separated fields
	void GCALC_FormatDMS_String(char *buf, int buf_size, double dVar1, int iType);

	// Transforms geographic coordinates to cartesian coordinates
	void GCALC_GeotoCart(coordinate *pGeoValue);

	// Transforms geographic coordinates to projection coordinates
	void GCALC_GeotoGrid(coordinate *pGeoValue);

	// Transforms projection coordinates to geographic coordinates
	void GCALC_GridtoGeo(coordinate *pGridValue);
	
	// Performs bi linear transfromation. Reads the gridfile into memory if not already opened.
	void GCALC_PointTran(char *gridfile, char *gridtype, coordinate *pGeoValue);

	// Prints the contents of a char array to the specified log file
	int GCALC_PrintLogFile(char *logfile, char *access_mode, char *logrecord);

	// Transforms projection coordinates using bi linear interpolation 
	void GCALC_MapGridTransformation(coordinate *pGridValue);

	// Prints the gridfile header information to a file_par struct
	void GCALC_ReportGridProperties(char *gridfile, char *gridtype, file_par *gridproperties, int *success);

	// Returns the file progress
	long GCALC_ReturnFileProgress();

	// Returns the byte offset
	long GCALC_GetByteOffset();

	// Sets the byte offset
	void GCALC_SetByteOffset();


}

#endif

