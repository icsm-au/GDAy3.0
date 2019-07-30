/////////////////////////////////////////////
// Roger Fraser

#pragma once

#ifndef     __ERROR_MESSAGES__
#define     __ERROR_MESSAGES__

#include <afx.h>

#define ERR_AUS_BINARY      -2
#define ERR_FINDSUBGRID_GRIDERR     -2
#define ERR_GRID_UNOPENED           -1
#define ERR_FINDSUBGRID_OUTSIDE     -1
#define ERR_GRID_NOT_OPENED 		-1
#define ERR_TRANS_SUCCESS    		0
#define ERR_GRID_SUCCESS     		0
#define ERR_GRIDFILE_READ    		1
#define ERR_GRIDFILE_TYPE    		2
#define ERR_GRID_CORRUPT     		3
#define ERR_INFILE_READ      		4
#define ERR_OUTFILE_WRITE    		5
#define ERR_INFILE_TYPE      		6
#define ERR_OUTFILE_TYPE     		7
#define ERR_NODATA_READ      		8
#define ERR_LINE_TOO_SHORT   		9
#define ERR_INVALID_INPUT    		10
#define ERR_READING_DATA     		11
#define ERR_INVALID_ZONE     		12
#define ERR_PT_OUTSIDE_GRID  		13
#define ERR_GRIDFILE_ERROR   		14
#define ERR_READ_ASC_SHIFT   		15
#define ERR_READ_BIN_SHIFT   		16
#define ERR_NUM_CSV_FIELDS   		17
#define ERR_QIF_COORD_MATCH  		18
#define ERR_QIF_COORD_READ   		19
#define ERR_QIF_NODATA_READ  		20
#define ERR_RES_COORD_MATCH  		21
#define ERR_RES_COORD_TYPE   		22


class CErrMsg
{
// Construction
public:
	CErrMsg();
	
	void ReportError(int iError, char* cBuf);

	CString ErrorString;
	CString strGridFile;
	CString strInputFile;
	CString strOutputFile;
};

#endif