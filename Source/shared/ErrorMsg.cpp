/////////////////////////////////////////////
// Roger Fraser

#include "stdafx.h"
#include <io.h>
#include "Defines.h"
#include "ErrorMsg.h"
#include "FileProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CErrMsg::CErrMsg()
{
	// construction
	ErrorString = "";
	strGridFile = "";
	strInputFile = "";
	strOutputFile = "";
}

void CErrMsg::ReportError(int iError, char* cBuf)
{
	CFileProc file;
	ErrorString.Empty();

	switch(iError)
	{
	case ERR_GRID_NOT_OPENED:	// -1
		ErrorString.Format("A default grid file has not been selected.\n");
		break;
	case ERR_GRIDFILE_READ:		// 1
		if (cBuf == NULL)
			ErrorString.Format("The default grid file could not be opened.\n");
		else
			ErrorString.Format("GDAy could not open the specified grid file:\n%s\n", strGridFile);
		break;
	case ERR_GRIDFILE_TYPE:		// 2
		ErrorString.Format("GDAy cannot read this type of grid file: %s\n\n", file.GetExt(strGridFile));
		break;
	case ERR_GRID_CORRUPT:		// 3
	case ERR_GRIDFILE_ERROR:	// 14
		if (cBuf == "NEW")
			ErrorString.Format("GDAy found an error in the selected grid file.\n");
		else if (cBuf == NULL)
			ErrorString.Format("GDAy found an error in the default grid file.\n");
		else
			ErrorString.Format("GDAy found an error in the default grid file:\n%s\n\n", strGridFile);
		break;
	case ERR_INFILE_READ:		// 4
		ErrorString.Format("GDAy could not read from the specified input file:\n%s\n\n", strInputFile);
		break;
	case ERR_OUTFILE_WRITE:		// 5
		ErrorString.Format("GDAy could not write to the specified output file:\n%s\n\n", strInputFile);
		break;
	case ERR_INFILE_TYPE:		// 6
		ErrorString.Format("GDAy cannot read this type of input file: %s\n\n", file.GetExt(strInputFile));
		break;
	case ERR_OUTFILE_TYPE:		// 7
		ErrorString.Format("GDAy cannot produce this type of output file: %s\n\n", file.GetExt(strInputFile));
		break;
	case ERR_NODATA_READ:		// 8
		ErrorString.Format("No data was contained within the last input record.\n");
		break;
	case ERR_LINE_TOO_SHORT:	// 9
		ErrorString.Format("The following record is too short to contain valid data:\n>>    %s\n", cBuf);
		break;
	case ERR_INVALID_INPUT:		// 10
		ErrorString.Format("The following record does not contain valid numeric input:\n>>    %s\n", cBuf);
		break;	
	case ERR_READING_DATA:		// 11
		ErrorString.Format("The required data could not be extracted from the following record:\n>>    %s\n", cBuf);
		break;
	case ERR_INVALID_ZONE:		// 12
		ErrorString.Format("The specified zone is invalid.\n");
		break;
	case ERR_PT_OUTSIDE_GRID:	// 13
		if (cBuf == NULL)
			ErrorString.Format("The point lies outside the extents of the distortion grid.\n\n");
		else
			ErrorString.Format("The point lies outside the extents of the distortion grid:\n>>    %s", cBuf);
		break;
	case ERR_READ_ASC_SHIFT:	// 15
		ErrorString.Format("The interpolation shifts could not be retrieved from the Ascii grid file:\n%s\n", strInputFile);
		break;
	case ERR_READ_BIN_SHIFT:	// 16
		ErrorString.Format("The interpolation shifts could not be retrieved from the Binary grid file:\n%s\n", strInputFile);
		break;
	case ERR_NUM_CSV_FIELDS:	// 17
		ErrorString.Format("The following csv record does not contain sufficient records for the\nselected input coordinate type.\n%s\n>>    ", cBuf);
		break;
	case ERR_QIF_COORD_MATCH:	// 18
		ErrorString.Format("The coordinate system parameters do not match the format of the specified QIF file.\n");
		break;
	case ERR_QIF_COORD_READ:	// 19
		ErrorString.Format("The coordinate system parameters could not be determined from the specified QIF file.\n");
		break;
	case ERR_QIF_NODATA_READ:	// 20
		ErrorString.Format("The DCDB extract \"%s\" contains no data. \nThe contents have been copied to the output file.\n", file.GetName(strInputFile, NULL));
		break;
	case ERR_RES_COORD_MATCH:	// 21
		ErrorString.Format("The specified coordinate system parameters do not match the following RES file cordinates:\n>>    %s\n", cBuf);
		break;
	case ERR_RES_COORD_TYPE:	// 22
		ErrorString.Format("GDAy cannot transform RES file coordinates of this type:\n>>    %s\n", cBuf);
		break;
	default:
		ErrorString = "";
		break;
	}
}
