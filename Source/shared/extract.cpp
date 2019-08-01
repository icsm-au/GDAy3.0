/////////////////////////////////////////////////////////////////////////////////////
/*
		Filename:
			extract.cpp
		
		Author:
			Roger Fraser. 

		Copyright
			Department of Natural Resources and Mines (The state of Queensland, (C) 2001).

		Created: 16 March 2001.

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "extract.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGridEx::CGridEx()
{
	// build error table from OpenGridFile success
	BuildErrorTable();

	m_dPercentComplete = 0.0;
	m_strSubGrid = " ";
	m_strLogFilePath = "";
}

void CGridEx::ExtractGrid(
				double dELon,
				double dWLon,
				double dSLat,
				double dNLat,
				file_par *fpGrid, 
				file_par *fpNewGrid,
				int *success)
{
	// initialise success
	*success = 0;

	// create log file and write header
	OpenLogFileAndWriteHeader(fpNewGrid->filename);	

	// create header file array of same size as the original grid
	fpNewGrid->ptrIndex = new gridfileindex[fpGrid->iNumsubgrids];
	memset(fpNewGrid->ptrIndex, 0, sizeof(gridfileindex)*fpGrid->iNumsubgrids);

	if ((*success = ValidateFileFormats(fpGrid, fpNewGrid)) == 0)
	{
		// determine new subgrid limits and copy header info to new sub grid array
		GenerateHeader(dELon, dWLon, dSLat, dNLat, fpGrid, fpNewGrid, success);
	}

	// print extraction limits
	fprintf(f_log, "Extraction Limits:\n\n  N Latitude: %18.7f\n  S Latitude: %18.7f\n  E Longitude: %17.7f\n  W Longitude: %17.7f\n\n",
		dNLat, dSLat, -dELon, -dWLon);

	if (*success == 0)
	{
		fprintf(f_log, "Successfully created sub grid header blocks.\n\n");
		fprintf(f_log, "Extracting sub grids...\n\n");
		*success = CopyGridShifts(fpGrid, fpNewGrid);	// print to new file
	}

	delete[] fpNewGrid->ptrIndex;

	// print success / failure
	if (*success != 0)
	{
		fprintf(f_log, "%s\n", m_chError_code[*success]);
		fprintf(f_log, "Grid extraction aborted.\n");
	}
	else
	{
		fprintf(f_log, "Extracted %d sub grid(s).\n\n", fpNewGrid->iNumsubgrids);
		fprintf(f_log, "%s\n\n", m_chError_code[0]);
	}

	fclose(f_log);
}

void CGridEx::GenerateHeader(
					double dELon,
					double dWLon,
					double dSLat,
					double dNLat,
					file_par *fpGrid, 
					file_par *fpNewGrid,
					int *success)
{
	char cBuf[51];
	
	// copy standard header information
	memcpy(fpNewGrid->chGs_type, fpGrid->chGs_type, sizeof(fpGrid->chGs_type));
	memcpy(fpNewGrid->chVersion, fpGrid->chVersion, sizeof(fpGrid->chVersion));
	memcpy(fpNewGrid->chSystem_f, fpGrid->chSystem_f, sizeof(fpGrid->chSystem_f));
	memcpy(fpNewGrid->chSystem_t, fpGrid->chSystem_t, sizeof(fpGrid->chSystem_t));
	fpNewGrid->daf = fpGrid->daf;
	fpNewGrid->dat = fpGrid->dat;
	fpNewGrid->dbf = fpGrid->dbf;
	fpNewGrid->dbt = fpGrid->dbt;
	fpNewGrid->iH_info = fpGrid->iH_info;
	fpNewGrid->iSubH_info = fpGrid->iSubH_info;
	fpNewGrid->iNumsubgrids = fpGrid->iNumsubgrids;

	// get system date and time
	GetSystemDate(cBuf, DATE_BRIEF);

	// overwrite last 4 characters of existing version field with date (MMYY)
	// ...this will maintain the standard naming convention: NAM_MMYY,
	// where NAM_ is a brief name for the grid
	strncpy(fpNewGrid->chVersion + 4, cBuf, 4);
	
	// build sub grid header array information
	BuildNewHeaderArray((fabs(dELon))*-3600.0, (fabs(dWLon))*-3600.0, (fabs(dSLat))*-3600.0, (fabs(dNLat))*-3600.0, fpGrid, fpNewGrid);	

	if (fpNewGrid->iNumsubgrids == 0)
		*success = 12;
}


void CGridEx::BuildNewHeaderArray(
						double dELon,
						double dWLon,
						double dSLat, 
						double dNLat, 
						file_par *fpGrid,
						file_par *fpNewGrid)
{

	int i, iElim, iWlim, iSlim, iNlim;
	int iLatLimit, iLonLimit;
	double dELonRnd, dWLonRnd, dSLatRnd, dNLatRnd;

	for (i=0; i<fpGrid->iNumsubgrids; i++)
	{
		dELonRnd = dELon;
		dWLonRnd = dWLon;
		dSLatRnd = dSLat;
		dNLatRnd = dNLat;

		RoundLimits(&dELonRnd, &dWLonRnd, &dSLatRnd, &dNLatRnd, fpGrid, i);

		// initialise flags
		iElim = iWlim = iSlim = iNlim = false;
		iLatLimit = iLonLimit = 0;

		if ((dELonRnd > fpGrid->ptrIndex[i].dElong || fabs(dELonRnd - fpGrid->ptrIndex[i].dElong) < dPrec) &&
			(dELonRnd < fpGrid->ptrIndex[i].dWlong || fabs(dELonRnd - fpGrid->ptrIndex[i].dWlong) < dPrec))
		{
			// east limit of extraction is within longitude limits of subgrid
			iElim = true;
			iLonLimit++;
		}

		if ((dWLonRnd > fpGrid->ptrIndex[i].dElong || fabs(dWLonRnd - fpGrid->ptrIndex[i].dElong) < dPrec) &&
			(dWLonRnd < fpGrid->ptrIndex[i].dWlong || fabs(dWLonRnd - fpGrid->ptrIndex[i].dWlong) < dPrec))
		{
			// west limit of extraction is within longitude limits of subgrid
			iWlim = true;
			iLonLimit++;
		}

		if ((dELonRnd < fpGrid->ptrIndex[i].dElong || fabs(dELonRnd - fpGrid->ptrIndex[i].dElong) < dPrec) &&
			(dWLonRnd > fpGrid->ptrIndex[i].dWlong || fabs(dWLonRnd - fpGrid->ptrIndex[i].dWlong) < dPrec))
		{
			// east and west limits of extraction contain both long limits of subgrid
			iLonLimit++;
		}

		if (fabs(dELonRnd - fpGrid->ptrIndex[i].dWlong) < dPrec ||
			fabs(dWLonRnd - fpGrid->ptrIndex[i].dElong) < dPrec)
		{
			iLonLimit = 0;
		}

		if ((dSLatRnd > fpGrid->ptrIndex[i].dSlat || fabs(dSLatRnd - fpGrid->ptrIndex[i].dSlat) < dPrec) &&
			(dSLatRnd < fpGrid->ptrIndex[i].dNlat || fabs(dSLatRnd - fpGrid->ptrIndex[i].dNlat) < dPrec))
		{
			// south limit of extraction is within latitude limits of subgrid
			iSlim = true;
			iLatLimit++;
		}

		if ((dNLatRnd > fpGrid->ptrIndex[i].dSlat || fabs(dNLatRnd - fpGrid->ptrIndex[i].dSlat) < dPrec) &&
			(dNLatRnd < fpGrid->ptrIndex[i].dNlat || fabs(dNLatRnd - fpGrid->ptrIndex[i].dNlat) < dPrec))
		{
			// north limit of extraction is within latitude limits of subgrid
			iNlim = true;
			iLatLimit++;
		}
		
		if ((dSLatRnd < fpGrid->ptrIndex[i].dSlat || fabs(dSLatRnd - fpGrid->ptrIndex[i].dSlat) < dPrec) &&
			(dNLatRnd > fpGrid->ptrIndex[i].dNlat || fabs(dNLatRnd - fpGrid->ptrIndex[i].dNlat) < dPrec))
		{
			// notrh and south limits of extraction contain both lat limits of subgrid
			iLatLimit++;
		}

		if (fabs(dSLatRnd - fpGrid->ptrIndex[i].dNlat) < dPrec ||
			fabs(dNLatRnd - fpGrid->ptrIndex[i].dSlat) < dPrec)
		{
			iLatLimit = 0;
		}

		if ((dELonRnd < fpGrid->ptrIndex[i].dElong) &&
			(dWLonRnd > fpGrid->ptrIndex[i].dWlong) &&
			(dSLatRnd < fpGrid->ptrIndex[i].dSlat) &&
			(dNLatRnd > fpGrid->ptrIndex[i].dNlat))
		{
			// this subgrid is wholly within extraction...copy header info exactly
			memcpy(&fpNewGrid->ptrIndex[i], &fpGrid->ptrIndex[i], sizeof(gridfileindex));			
		}

		// copy subgrid limits
		if (iLatLimit > 0 && iLonLimit > 0)
		{
			/* 
			at least one lat limit AND one long limit were within a subgrid.
			therefore, some (or all) of the sub grid may be copied.
			*/
			memcpy(&fpNewGrid->ptrIndex[i], &fpGrid->ptrIndex[i], sizeof(gridfileindex));
			
			if (iElim)
				fpNewGrid->ptrIndex[i].dElong = dELonRnd;
			
			if (iWlim)
				fpNewGrid->ptrIndex[i].dWlong = dWLonRnd;
			
			if (iSlim)
				fpNewGrid->ptrIndex[i].dSlat = dSLatRnd;
			
			if (iNlim)
				fpNewGrid->ptrIndex[i].dNlat = dNLatRnd;
		}
		else
		{
			// do not copy this subgrid
			strcpy(fpNewGrid->ptrIndex[i].chSubname, "NULL    ");
			strcpy(fpNewGrid->ptrIndex[i].chParent, "        ");
			
			// decrement number of sub grids for grid extraction
			fpNewGrid->iNumsubgrids = fpNewGrid->iNumsubgrids - 1;
		}
	}

	// test for multiple sub grids having equal limits
	if (fpNewGrid->iNumsubgrids > 1)
		RemoveDuplicateSubGrids(fpNewGrid, fpGrid->iNumsubgrids);
}


void CGridEx::RoundLimits(double *dELon, double *dWLon, double *dSLat, double *dNLat, file_par *fpGrid, int iSGIndex)
{
	char chParent[9];
	int i, index, iFoundParent;
	double dInterval, Precision = 1.0e-8;

	iFoundParent = 0;
	index = iSGIndex;
	
	if (strcmp(fpGrid->ptrIndex[iSGIndex].chParent, NOPARENT) == 0)
		iFoundParent = 1;
	else
		strcpy(chParent, fpGrid->ptrIndex[iSGIndex].chParent);
				
	// find parent for current grid (i)
	while (!iFoundParent)
	{
		for (i=0; i<fpGrid->iNumsubgrids; i++)
		{
			if (strcmp(chParent, fpGrid->ptrIndex[i].chSubname) == 0)
			{
				if (strcmp(fpGrid->ptrIndex[i].chParent, NOPARENT) == 0)
					iFoundParent = 1;	// most-parent found
				else
					strcpy(chParent, fpGrid->ptrIndex[i].chParent);

				// sub-parent found...save index, exit for loop
				index = i;
				i = fpGrid->iNumsubgrids + 1;
			}
		}

		if (!iFoundParent && i >= fpGrid->iNumsubgrids)
			iFoundParent = 1;		// an error in the grid - trap from endless loop
	}

	// round north latitude
	dInterval = fmod(fabs(*dNLat - fpGrid->ptrIndex[index].dNlat), fpGrid->ptrIndex[index].dLatinc);
	if (fabs(dInterval - fpGrid->ptrIndex[index].dLatinc) > Precision && dInterval > Precision)			
		*dNLat += dInterval;

	// round south latitude
	dInterval = fmod(fabs(*dSLat - fpGrid->ptrIndex[index].dSlat), fpGrid->ptrIndex[index].dLatinc);
	if (fabs(dInterval - fpGrid->ptrIndex[index].dLatinc) > Precision && dInterval > Precision)			
		*dSLat -= dInterval;
	
	// round west longitude
	dInterval = fmod(fabs(*dWLon - fpGrid->ptrIndex[index].dWlong), fpGrid->ptrIndex[index].dLonginc);
	if (fabs(dInterval - fpGrid->ptrIndex[index].dLonginc) > Precision && dInterval > Precision)			
		*dWLon += dInterval;

	// round east longitude
	dInterval = fmod(fabs(*dELon - fpGrid->ptrIndex[index].dElong), fpGrid->ptrIndex[index].dLonginc);
	if (fabs(dInterval - fpGrid->ptrIndex[index].dLonginc) > Precision && dInterval > Precision)			
		*dELon -= dInterval;

}


void CGridEx::RemoveDuplicateSubGrids(file_par *fpNewGrid, int iNumSubGrids)
{
	int continue_loop, iDuplicateFound;
	int i, j;

	continue_loop = true;
	while (continue_loop)
	{
		for (i=0; i<iNumSubGrids; i++)
		{
			iDuplicateFound = 0;
			if (strcmp(fpNewGrid->ptrIndex[i].chParent, NOPARENT) == 0)
			{
				for (j=0; j<iNumSubGrids; j++)
				{
					if ((strcmp(fpNewGrid->ptrIndex[j].chParent, fpNewGrid->ptrIndex[i].chSubname) == 0) &&
						(fpNewGrid->ptrIndex[j].dElong == fpNewGrid->ptrIndex[i].dElong &&
						fpNewGrid->ptrIndex[j].dWlong == fpNewGrid->ptrIndex[i].dWlong &&
						fpNewGrid->ptrIndex[j].dSlat == fpNewGrid->ptrIndex[i].dSlat && 
						fpNewGrid->ptrIndex[j].dNlat == fpNewGrid->ptrIndex[i].dNlat))
					{
						// sub grid [j] has same limits as its parent [i]...make this the parent
						iDuplicateFound = 1;
						strcpy(fpNewGrid->ptrIndex[j].chParent, NOPARENT);
					}
				}							
			}
			if (iDuplicateFound)
			{
				strcpy(fpNewGrid->ptrIndex[i].chSubname, "NULL    ");
				fpNewGrid->iNumsubgrids --;
			}
		}
		if (!iDuplicateFound)
			continue_loop = false;
	}
}


int CGridEx::CopyGridShifts(file_par *fpGrid, file_par *fpNewGrid)
{
	int i, success;
	char access_mode[4];
	long lGridWidth, lGridHeight;
	double dTemp;
	CString limits;

	FILE *f_in, *f_out;

	// test for type of grid file, then assign appropriate access mode...
	if (m_intGridType == 0)
		strcpy(access_mode, "r");			// file type is ASCII
	else if (m_intGridType == 1)
		strcpy(access_mode, "rb");			// file type is Binary
	else
		return 2;

	// Open the distortion grid file for reading
	if ((f_in = fopen(fpGrid->filename, access_mode)) == NULL)
		return 1;

	// Open the new distortion grid file for writing
	if ((f_out = fopen(fpNewGrid->filename, (m_intNewGridType == 0 ? "w":"wb"))) == NULL)
		return 9;

	// Print Header
	if (m_intNewGridType == 0)
		PrintAsciiHeader(f_out, fpNewGrid, "");		// print header to ASCII file
	else
		PrintBinaryHeader(f_out, fpNewGrid);		// print header to Binary file

	// obtain byte offset for new grid after header is written (for progress bar)
	m_lTotByteCount = ftell(f_out);

	// Compute new grid fundamentals  (see Page 18 of the NTv2 Developer's Guide)	
	for (i=0; i<fpGrid->iNumsubgrids; i++)
	{
		// calculate #bytes for subgrid data
		if (strcmp(fpNewGrid->ptrIndex[i].chSubname,  "NULL    ") != 0)
		{
			// No of Columns
			lGridWidth = (int)((fpNewGrid->ptrIndex[i].dWlong - fpNewGrid->ptrIndex[i].dElong + dPrec) / fpNewGrid->ptrIndex[i].dLonginc);
			// No of Rows
			lGridHeight = (int)((fpNewGrid->ptrIndex[i].dNlat - fpNewGrid->ptrIndex[i].dSlat + dPrec) / fpNewGrid->ptrIndex[i].dLatinc);
			// compute grid shift count
			fpNewGrid->ptrIndex[i].lGscount = (int)((lGridWidth + 1) * (lGridHeight + 1));
			
			if (m_intNewGridType == 0)
				m_lTotByteCount += 238 + (fpNewGrid->ptrIndex[i].lGscount * 42);
			else
				m_lTotByteCount += 176 + (fpNewGrid->ptrIndex[i].lGscount * 4 * sizeof(float));

			// print to log file sub grid specifications
			fprintf(f_log, "  SUB_NAME:%13s\n", fpNewGrid->ptrIndex[i].chSubname);			
			fprintf(f_log, "  N_LAT: %15.6fd\n  S_LAT: %15.6fd\n  W_LONG:%15.6fd\n  E_LONG:%15.6fd\n",
				fpNewGrid->ptrIndex[i].dNlat/3600,
				fpNewGrid->ptrIndex[i].dSlat/3600,
				fabs(fpNewGrid->ptrIndex[i].dWlong/3600),
				fabs(fpNewGrid->ptrIndex[i].dElong/3600));			
			fprintf(f_log, "  LAT_INC:  %12.0fs\n  LONG_INC: %12.0fs\n",
				fpNewGrid->ptrIndex[i].dLatinc,
				fpNewGrid->ptrIndex[i].dLonginc);			
			fprintf(f_log, "  GSCOUNT:  %12d\n\n", fpNewGrid->ptrIndex[i].lGscount);
		}
	}

	success = 0;

	for (i=0; i<fpGrid->iNumsubgrids; i++)
	{
		// Print non - NULL Subgrid data
		if (strcmp(fpNewGrid->ptrIndex[i].chSubname,  "NULL    ") != 0)
		{
			m_strSubGrid = fpNewGrid->ptrIndex[i].chSubname;
			// print sub grid header
			if (m_intNewGridType == 0)
				PrintAsciiSubheader(f_out, fpNewGrid, i);
			else
				PrintBinarySubheader(f_out, fpNewGrid, i);
		
			lGridWidth = (int)((fpNewGrid->ptrIndex[i].dWlong - fpNewGrid->ptrIndex[i].dElong + dPrec) / fpNewGrid->ptrIndex[i].dLonginc);
			lGridHeight = (int)((fpNewGrid->ptrIndex[i].dNlat - fpNewGrid->ptrIndex[i].dSlat + dPrec) / fpNewGrid->ptrIndex[i].dLatinc);
			
			// read and print grid shifts
			if (m_intGridType == 0 && m_intNewGridType == 0)
				success = Asc2Asc(f_in, f_out, fpGrid, fpNewGrid, i, lGridWidth, lGridHeight);		// print ASCII grid shifts to ASCII file
			else if (m_intGridType == 1 && m_intNewGridType == 1)
				success = Bin2Bin(f_in, f_out, fpGrid, fpNewGrid, i, lGridWidth, lGridHeight);		// print Binary grid shifts to Binary file
			else if (m_intGridType == 0 && m_intNewGridType == 1)
				success = Asc2Bin(f_in, f_out, fpGrid, fpNewGrid, i, lGridWidth, lGridHeight);		// print ASCII grid shifts to Binary file
			else
				success = Bin2Asc(f_in, f_out, fpGrid, fpNewGrid, i, lGridWidth, lGridHeight);		// print Binary grid shifts to ASCII file
		}
		// exit for loop on error
		if (success != 0)
			break;
	}

	if (success == 0)
	{
		// Print EOF message
		if (m_intNewGridType == 0)
			fprintf(f_out, "%-8s%10.2e\n", "END", 3.33E+32);
		else
		{
			fwrite("END     ", 8, 1, f_out);
			dTemp = (double) 3.33E+32;
			fwrite(&dTemp, sizeof(double), 1, f_out);
		}
	}
	fclose(f_in);
	fclose(f_out);
	return success;
}


void CGridEx::PrintAsciiHeader(FILE *f_out, file_par *fpNewGrid, char *indent)
{
	fprintf(f_out, "%s%8s%3d\n", indent, "NUM_OREC", fpNewGrid->iH_info);			// Number of header identifiers (NUM_OREC)
	fprintf(f_out, "%s%8s%3d\n", indent, "NUM_SREC", fpNewGrid->iSubH_info);		// Number of sub-header idents (NUM_SREC)
	fprintf(f_out, "%s%8s%3d\n", indent, "NUM_FILE", fpNewGrid->iNumsubgrids);	// number of subgrids in file (NUM_FILE)
	fprintf(f_out, "%s%8s%-8s\n", indent, "GS_TYPE ", fpNewGrid->chGs_type);		// grid shift type (GS_TYPE)
	fprintf(f_out, "%s%8s%-8s\n", indent, "VERSION ", fpNewGrid->chVersion);		// grid file version (VERSION)
	fprintf(f_out, "%s%8s%-8s\n", indent, "SYSTEM_F", fpNewGrid->chSystem_f);		// reference system (SYSTEM_F)
	fprintf(f_out, "%s%8s%-8s\n", indent, "SYSTEM_T", fpNewGrid->chSystem_t);		// reference system (SYSTEM_T)
	fprintf(f_out, "%s%8s%12.3f\n", indent, "MAJOR_F ", fpNewGrid->daf);			// semi major of from system (MAJOR_F)
	fprintf(f_out, "%s%8s%12.3f\n", indent, "MINOR_F ", fpNewGrid->dbf);			// semi minor of from system (MINOR_F)
	fprintf(f_out, "%s%8s%12.3f\n", indent, "MAJOR_T ", fpNewGrid->dat);			// semi major of to system (MAJOR_T)
	fprintf(f_out, "%s%8s%12.3f\n", indent, "MINOR_T ", fpNewGrid->dbt);			// semi minor of to system (MINOR_T)
}


void CGridEx::PrintAsciiSubheader(FILE *f_out, file_par *fpNewGrid, int index)
{
	// print sub header block for sub grid [index]
	fprintf(f_out, "%8s%-8s\n", "SUB_NAME", fpNewGrid->ptrIndex[index].chSubname);		// grid shift type (SUB_NAME)
	fprintf(f_out, "%8s%-8s\n", "PARENT  ", fpNewGrid->ptrIndex[index].chParent);		// grid file version (PARENT)
	fprintf(f_out, "%8s%-8s\n", "CREATED ", fpNewGrid->ptrIndex[index].chCreated);		// reference system (CREATED)
	fprintf(f_out, "%8s%-8s\n", "UPDATED ", fpNewGrid->ptrIndex[index].chUpdated);		// reference system (UPDATED)	
	
	if (strcmp(fpNewGrid->chGs_type, "RADIANS ") == 0)
	{
		// convert all seconds values to radians
		fprintf(f_out, "%8s%15.6f\n", "S_LAT   ", fpNewGrid->ptrIndex[index].dSlat / dRad2Sec);			// reference system (S_LAT)
		fprintf(f_out, "%8s%15.6f\n", "N_LAT   ", fpNewGrid->ptrIndex[index].dNlat / dRad2Sec);			// reference system (N_LAT)
		fprintf(f_out, "%8s%15.6f\n", "E_LONG  ", fpNewGrid->ptrIndex[index].dElong / dRad2Sec);		// reference system (E_LONG)
		fprintf(f_out, "%8s%15.6f\n", "W_LONG  ", fpNewGrid->ptrIndex[index].dWlong / dRad2Sec);		// reference system (W_LONG)
		fprintf(f_out, "%8s%15.6f\n", "LAT_INC ", fpNewGrid->ptrIndex[index].dLatinc / dRad2Sec);		// reference system (LAT_INC)
		fprintf(f_out, "%8s%15.6f\n", "LONG_INC", fpNewGrid->ptrIndex[index].dLonginc / dRad2Sec);		// reference system (LONG_INC)
	}
	else
	{
		fprintf(f_out, "%8s%15.6f\n", "S_LAT   ", fpNewGrid->ptrIndex[index].dSlat);		// reference system (S_LAT)
		fprintf(f_out, "%8s%15.6f\n", "N_LAT   ", fpNewGrid->ptrIndex[index].dNlat);		// reference system (N_LAT)
		fprintf(f_out, "%8s%15.6f\n", "E_LONG  ", fpNewGrid->ptrIndex[index].dElong);		// reference system (E_LONG)
		fprintf(f_out, "%8s%15.6f\n", "W_LONG  ", fpNewGrid->ptrIndex[index].dWlong);		// reference system (W_LONG)
		fprintf(f_out, "%8s%15.6f\n", "LAT_INC ", fpNewGrid->ptrIndex[index].dLatinc);		// reference system (LAT_INC)
		fprintf(f_out, "%8s%15.6f\n", "LONG_INC", fpNewGrid->ptrIndex[index].dLonginc);		// reference system (LONG_INC)
	}
	
	fprintf(f_out, "%8s%6ld\n", "GS_COUNT", fpNewGrid->ptrIndex[index].lGscount);		// reference system (GS_COUNT)
}


void CGridEx::PrintBinaryHeader(FILE *f_out, file_par *fpNewGrid)
{
	#define NULL_PAD 4
	const int OVERVIEW_RECS = 8;
	char cPad[NULL_PAD + 1];
	memset(cPad, '\0', sizeof(cPad));

	// print header block information
	fwrite("NUM_OREC", OVERVIEW_RECS, 1, f_out);				// Number of header identifiers (NUM_OREC)
	fwrite(&fpNewGrid->iH_info, sizeof(int), 1, f_out);
	fwrite(cPad, sizeof(char), NULL_PAD, f_out);				// Pad out extra 4 bytes of null space
	fwrite("NUM_SREC", OVERVIEW_RECS, 1, f_out);				// Number of sub-header idents (NUM_SREC)
	fwrite(&fpNewGrid->iSubH_info, sizeof(int), 1, f_out);
	fwrite(cPad, sizeof(char), NULL_PAD, f_out);				// Pad out extra 4 bytes of null space
	fwrite("NUM_FILE", OVERVIEW_RECS, 1, f_out);				// number of subgrids in file (NUM_FILE)
	fwrite(&fpNewGrid->iNumsubgrids, sizeof(int), 1, f_out);
	fwrite(cPad, sizeof(char), NULL_PAD, f_out);				// Pad out extra 4 bytes of null space
	fwrite("GS_TYPE ", OVERVIEW_RECS, 1, f_out);				// grid shift type (GS_TYPE)
	fwrite(fpNewGrid->chGs_type, OVERVIEW_RECS, 1, f_out);
	fwrite("VERSION ", OVERVIEW_RECS, 1, f_out);				// grid file version (VERSION)
	fwrite(fpNewGrid->chVersion, OVERVIEW_RECS, 1, f_out);
	fwrite("SYSTEM_F", OVERVIEW_RECS, 1, f_out);				// reference system (SYSTEM_F)
	fwrite(fpNewGrid->chSystem_f, OVERVIEW_RECS, 1, f_out);
	fwrite("SYSTEM_T", OVERVIEW_RECS, 1, f_out);				// reference system (SYSTEM_T)
	fwrite(fpNewGrid->chSystem_t, OVERVIEW_RECS, 1, f_out);
	fwrite("MAJOR_F ", OVERVIEW_RECS, 1, f_out);				// semi major of from system (MAJOR_F)
	fwrite(&fpNewGrid->daf, sizeof(double), 1, f_out);
	fwrite("MINOR_F ", OVERVIEW_RECS, 1, f_out);				// semi minor of from system (MINOR_F)
	fwrite(&fpNewGrid->dbf, sizeof(double), 1, f_out);
	fwrite("MAJOR_T ", OVERVIEW_RECS, 1, f_out);				// semi major of to system (MAJOR_T)
	fwrite(&fpNewGrid->dat, sizeof(double), 1, f_out);
	fwrite("MINOR_T ", OVERVIEW_RECS, 1, f_out);				// semi minor of to system (MINOR_T)
	fwrite(&fpNewGrid->dbt, sizeof(double), 1, f_out);
}


void CGridEx::PrintBinarySubheader(FILE *f_out, file_par *fpNewGrid, int index)
{
	const int OVERVIEW_RECS = 8;
	char cPad[NULL_PAD + 1];
	memset(cPad, '\0', sizeof(cPad));
	double dTemp;

	// print sub header block for sub grid [index]
	fwrite("SUB_NAME", OVERVIEW_RECS, 1, f_out);								// grid shift type (SUB_NAME)
	fwrite(&fpNewGrid->ptrIndex[index].chSubname, OVERVIEW_RECS, 1, f_out);
	fwrite("PARENT  ", OVERVIEW_RECS, 1, f_out);								// grid file version (PARENT)
	fwrite(&fpNewGrid->ptrIndex[index].chParent, OVERVIEW_RECS, 1, f_out);
	fwrite("CREATED ", OVERVIEW_RECS, 1, f_out);								// reference system (CREATED)
	fwrite(&fpNewGrid->ptrIndex[index].chCreated, OVERVIEW_RECS, 1, f_out);
	fwrite("UPDATED ", OVERVIEW_RECS, 1, f_out);								// reference system (UPDATED)
	fwrite(&fpNewGrid->ptrIndex[index].chUpdated, OVERVIEW_RECS, 1, f_out);
	
	if (strcmp(fpNewGrid->chGs_type, "RADIANS ") == 0)
	{
		// convert all seconds values to radians
		fwrite("S_LAT   ", OVERVIEW_RECS, 1, f_out);								// reference system (S_LAT)
		dTemp = fpNewGrid->ptrIndex[index].dSlat / dRad2Sec;
		fwrite(&dTemp, sizeof(double), 1, f_out);
		fwrite("N_LAT   ", OVERVIEW_RECS, 1, f_out);								// reference system (N_LAT)
		dTemp = fpNewGrid->ptrIndex[index].dNlat / dRad2Sec;
		fwrite(&dTemp, sizeof(double), 1, f_out);
		fwrite("E_LONG  ", OVERVIEW_RECS, 1, f_out);								// reference system (E_LONG)
		dTemp = fpNewGrid->ptrIndex[index].dElong / dRad2Sec;
		fwrite(&dTemp, sizeof(double), 1, f_out);
		fwrite("W_LONG  ", OVERVIEW_RECS, 1, f_out);								// reference system (W_LONG)
		dTemp = fpNewGrid->ptrIndex[index].dWlong / dRad2Sec;
		fwrite(&dTemp, sizeof(double), 1, f_out);
		fwrite("LAT_INC ", OVERVIEW_RECS, 1, f_out);								// reference system (LAT_INC)
		dTemp = fpNewGrid->ptrIndex[index].dLatinc / dRad2Sec;
		fwrite(&dTemp, sizeof(double), 1, f_out);
		fwrite("LONG_INC", OVERVIEW_RECS, 1, f_out);								// reference system (LONG_INC)
		dTemp = fpNewGrid->ptrIndex[index].dLonginc / dRad2Sec;
		fwrite(&dTemp, sizeof(double), 1, f_out);
	}
	else
	{
		fwrite("S_LAT   ", OVERVIEW_RECS, 1, f_out);								// reference system (S_LAT)
		fwrite(&fpNewGrid->ptrIndex[index].dSlat, sizeof(double), 1, f_out);
		fwrite("N_LAT   ", OVERVIEW_RECS, 1, f_out);								// reference system (N_LAT)
		fwrite(&fpNewGrid->ptrIndex[index].dNlat, sizeof(double), 1, f_out);
		fwrite("E_LONG  ", OVERVIEW_RECS, 1, f_out);								// reference system (E_LONG)
		fwrite(&fpNewGrid->ptrIndex[index].dElong, sizeof(double), 1, f_out);
		fwrite("W_LONG  ", OVERVIEW_RECS, 1, f_out);								// reference system (W_LONG)
		fwrite(&fpNewGrid->ptrIndex[index].dWlong, sizeof(double), 1, f_out);
		fwrite("LAT_INC ", OVERVIEW_RECS, 1, f_out);								// reference system (LAT_INC)
		fwrite(&fpNewGrid->ptrIndex[index].dLatinc, sizeof(double), 1, f_out);
		fwrite("LONG_INC", OVERVIEW_RECS, 1, f_out);								// reference system (LONG_INC)
		fwrite(&fpNewGrid->ptrIndex[index].dLonginc, sizeof(double), 1, f_out);
	}

	fwrite("GS_COUNT", OVERVIEW_RECS, 1, f_out);								// reference system (GS_COUNT)
	fwrite(&fpNewGrid->ptrIndex[index].lGscount, sizeof(int), 1, f_out);
	fwrite(cPad, sizeof(char), NULL_PAD, f_out);								// Pad out extra 4 bytes of null space
}



int CGridEx::Asc2Asc(FILE *f_in, FILE *f_out, file_par *fpGrid, file_par *fpNewGrid, int i, long iGWidth, long iGHeight)
{
	int j, k, success = 0;
	long iA, iRow_Index, iColumn_Index, iGrid_Points_Per_Row;
	float dLatShift, dLonShift, dLatAcc, dLonAcc;
	fpos_t fPos;

	CalcGridIndex(fpGrid->ptrIndex, fpNewGrid->ptrIndex, i, &iRow_Index, &iColumn_Index, &iGrid_Points_Per_Row, &iA);

	fPos = fpGrid->ptrIndex[i].fpGridPos + ((iA - 1) * 42);		// subtract one for zero index
	if ( fsetpos(f_in, &fPos) != 0 )
		return 5;			// Could not set file position from grid file
	
	// for every row and every column	
	for (j=0; j<(iGHeight + 1); j++)
	{
		for (k=0; k<(iGWidth + 1); k++)
		{
			if ((success = ReadAscShifts(f_in, &dLatShift, &dLonShift, &dLatAcc, &dLonAcc)) != 0)
				break;
			PrintAscShifts(f_out, dLatShift, dLonShift, dLatAcc, dLonAcc);
			// progress indicator
			m_dPercentComplete = (double)(ftell(f_out)) / (double)m_lTotByteCount * 100.0;
		}
		fPos += (iGrid_Points_Per_Row * 42);
		if (fsetpos(f_in, &fPos) != 0)  
			success = 5;
		if (success != 0)
			break;
	}
	return success;
}



int CGridEx::Bin2Bin(FILE *f_in, FILE *f_out, file_par *fpGrid, file_par *fpNewGrid, int i, long iGWidth, long iGHeight)
{
	int j, k, success = 0;
	int iLineLength = 4 * sizeof(float);
	long iA, iRow_Index, iColumn_Index, iGrid_Points_Per_Row;
	long lPos;
	float dLatShift, dLonShift, dLatAcc, dLonAcc;
	
	CalcGridIndex(fpGrid->ptrIndex, fpNewGrid->ptrIndex, i, &iRow_Index, &iColumn_Index, &iGrid_Points_Per_Row, &iA);

	lPos = fpGrid->ptrIndex[i].lGridPos + ((iA - 1) * iLineLength);		// subtract one for zero index
	if (fseek(f_in, lPos, SEEK_SET) != 0)
		return 6;			// Could not seek file position from grid file

	// for every row and every column
	for (j=0; j<(iGHeight + 1); j++)
	{
		for (k=0; k<(iGWidth + 1); k++)
		{
			if ((success = ReadBinShifts(f_in, &dLatShift, &dLonShift, &dLatAcc, &dLonAcc)) != 0)
				break;
			PrintBinShifts(f_out, dLatShift, dLonShift, dLatAcc, dLonAcc);
			// progress indicator
			m_dPercentComplete = (double)(ftell(f_out)) / (double)m_lTotByteCount * 100.0;
		}
		lPos += (iGrid_Points_Per_Row * iLineLength);
		if (fseek(f_in, lPos, SEEK_SET) != 0)
			success = 6;			// Could not seek file position from grid file
		if (success != 0)
			break;
	}
	return success;
}



int CGridEx::Asc2Bin(FILE *f_in, FILE *f_out, file_par *fpGrid, file_par *fpNewGrid, int i, long iGWidth, long iGHeight)
{
	int j, k, success = 0;
	long iA, iRow_Index, iColumn_Index, iGrid_Points_Per_Row;
	float dLatShift, dLonShift, dLatAcc, dLonAcc;
	fpos_t fPos;

	CalcGridIndex(fpGrid->ptrIndex, fpNewGrid->ptrIndex, i, &iRow_Index, &iColumn_Index, &iGrid_Points_Per_Row, &iA);

	fPos = fpGrid->ptrIndex[i].fpGridPos + ((iA - 1) * 42);		// subtract one for zero index
	if ( fsetpos(f_in, &fPos) != 0 )  
		return 5;
	
	// for every row and every column
	for (j=0; j<(iGHeight + 1); j++)
	{
		for (k=0; k<(iGWidth + 1); k++)
		{
			if ((success = ReadAscShifts(f_in, &dLatShift, &dLonShift, &dLatAcc, &dLonAcc)) != 0)
				break;
			PrintBinShifts(f_out, dLatShift, dLonShift, dLatAcc, dLonAcc);
			m_dPercentComplete = (double)(ftell(f_out)) / (double)m_lTotByteCount * 100.0;
		}	
		fPos += (iGrid_Points_Per_Row * 42);
		if (fsetpos(f_in, &fPos) != 0)  
			success = 5;
		if (success != 0)
			break;
	}
	return success;
}

int CGridEx::Bin2Asc(FILE *f_in, FILE *f_out, file_par *fpGrid, file_par *fpNewGrid, int i, long iGWidth, long iGHeight)
{
	int j, k, success = 0;
	int iLineLength = 4 * sizeof(float);
	long iA, iRow_Index, iColumn_Index, iGrid_Points_Per_Row;
	long lPos;
	float dLatShift, dLonShift, dLatAcc, dLonAcc;
	
	CalcGridIndex(fpGrid->ptrIndex, fpNewGrid->ptrIndex, i, &iRow_Index, &iColumn_Index, &iGrid_Points_Per_Row, &iA);

	lPos = fpGrid->ptrIndex[i].lGridPos + ((iA - 1) * iLineLength);		// subtract one for zero index
	if (fseek(f_in, lPos, SEEK_SET) != 0)
		return 6;						// Could not seek file position from grid file

	// for every row and every column
	for (j=0; j<(iGHeight + 1); j++)
	{
		for (k=0; k<(iGWidth + 1); k++)
		{
			if ((success = ReadBinShifts(f_in, &dLatShift, &dLonShift, &dLatAcc, &dLonAcc)) != 0)
				break;
			PrintAscShifts(f_out, dLatShift, dLonShift, dLatAcc, dLonAcc);
			// progress indicator
			m_dPercentComplete = (double)(ftell(f_out)) / (double)m_lTotByteCount * 100.0;
		}	
		lPos += (iGrid_Points_Per_Row * iLineLength);
		if (fseek(f_in, lPos, SEEK_SET) != 0)
			success = 6;			// Could not seek file position from grid file
		if (success != 0)
			break;
	}
	return success;
}



void CGridEx::CalcGridIndex(gridfileindex *ptrHeader, gridfileindex *ptrNewHeader, int i, long *n, long *m, long *g, long *a)
{				
	// Compute Row_Index of Grid Node A...
	*n = (int)((ptrNewHeader[i].dSlat - ptrHeader[i].dSlat + dPrec) / ptrHeader[i].dLatinc);

	// Compute Column_Index of Grid Node A...
	*m = (int)((ptrNewHeader[i].dElong - ptrHeader[i].dElong + dPrec) / ptrHeader[i].dLonginc);

	// Compute Grid_Points_Per_Row (No of Columns)
	*g = (int)((ptrHeader[i].dWlong - ptrHeader[i].dElong + dPrec) / ptrHeader[i].dLonginc);
	*g = *g + 1;

	// Compute Record Index for node A (Lower Right node of new grid)
	*a = ((*n) * (*g)) + *m + 1;	// add 1 so first node is one: (1)...ie. not zero-index!
}



int CGridEx::ReadAscShifts(FILE *f_in, float *fLatShift, float *fLonShift, float *fLatAcc, float *fLonAcc)
{
	char cBuf[100];	
	fgets(cBuf, 100, f_in);
	if (sscanf(cBuf, "%f%f%f%f", &fLatShift, &fLonShift, &fLatAcc, &fLonAcc) < 4)
		return 7;
	else
		return 0;
}



int CGridEx::ReadBinShifts(FILE *f_in, float *dLatShift, float *dLonShift, float *dLatAcc, float *dLonAcc)
{
	if ( (fread(dLatShift, sizeof(float), 1, f_in)) < 1 )
		return 8;		
	if ( (fread(dLonShift, sizeof(float), 1, f_in)) < 1 )
		return 8;		
	if ( (fread(dLatAcc, sizeof(float), 1, f_in)) < 1 )
		return 8;
	if ( (fread(dLonAcc, sizeof(float), 1, f_in)) < 1 )
		return 8;
	return 0;
}



void CGridEx::PrintAscShifts(FILE *f_out, float dLatShift, float dLonShift, float dLatAcc, float dLonAcc)
{
	fprintf(f_out, "%10.6f%10.6f%10.6f%10.6f\n", dLatShift, dLonShift, dLatAcc, dLonAcc);
}



void CGridEx::PrintBinShifts(FILE *f_out, float dLatShift, float dLonShift, float dLatAcc, float dLonAcc)
{
	float gsRecord[5];

	gsRecord[0] = dLatShift;
	gsRecord[1] = dLonShift;
	gsRecord[2] = dLatAcc;
	gsRecord[3] = dLonAcc;
	fwrite(gsRecord, sizeof(float), 4, f_out);
}


/////////////////////////////////////////////////////////////////////////
// GetSystemDate: Retrieves the system date based on am local time.
/////////////////////////////////////////////////////////////////////////
// On Entry:  cBuf is an empty char array. format is the format specifier for
//			  the date style.
// On Exit:   The system date/time is printed to cBuf.
/////////////////////////////////////////////////////////////////////////
void CGridEx::GetSystemDate(char *cBuf, const char *format)
{
	// char ampm[] = " am";
	struct tm *today;
	time_t ltime;
	CString strBuf;
  
	// get system date & time
	time(&ltime);
	today = localtime(&ltime);
	strftime(cBuf, 100, format, today);
}



void CGridEx::BuildErrorTable()
{
	memset(m_chError_code, '\0', sizeof(m_chError_code));
	
	strcpy(m_chError_code[0], "The new grid file was successfully extracted.\n");
	strcpy(m_chError_code[1], "The specified grid file could not be opened for reading.\n");
	strcpy(m_chError_code[2], "The specified grid filetype is not supported.\n");
	strcpy(m_chError_code[3], "The specified grid file is corrupt.\n");
	strcpy(m_chError_code[4], "The specified grid file contains errors.\n");
	strcpy(m_chError_code[5], "The file position could not be set in the specified ASCII file.\n");
	strcpy(m_chError_code[6], "The file position could not be set in the specified Binary file.\n");
	strcpy(m_chError_code[7], "The grid shifts could not be read from the specified ASCII file.\n");
	strcpy(m_chError_code[8], "The grid shifts could not be read from the specified Binary file.\n");
	strcpy(m_chError_code[9], "The output grid file could not be opened for writing.\n");
	strcpy(m_chError_code[10], "The output grid filetype is not supported.\n");
	strcpy(m_chError_code[11], "The input and output file names can not be the same.\n");
	strcpy(m_chError_code[12], "No sub grids were found within the specified extraction limits.\n");
	strcpy(m_chError_code[13], "Please enter a northern latitude limit that is north of the southern latitude limit.\n");
	strcpy(m_chError_code[14], "Please enter a western longitude limit that is west of the eastern longitude limit.\n");
}




int CGridEx::DetermineFileType(char *cType)
{
	if (_stricmp(cType, "asc") == 0)
		return 0;		// asc "ASCII" file
	else if (_stricmp(cType, "gsb") == 0)
		return 1;		// gsb "Binary" file
	else
		return -1;		// Unsupported filetype
}

long CGridEx::ReturnExtractProgress()
{
	return (long)m_dPercentComplete;
}

CString CGridEx::ReturnSubGridName()
{
	return m_strSubGrid;
}

int CGridEx::ValidateFileFormats(file_par *fpGrid, file_par *fpNewGrid)
{
	fprintf(f_log, "NTv2 Grid file:    %s  ", fpGrid->filename);
	
	// test for type of grid file, then assign appropriate access mode...
	m_intGridType = DetermineFileType(fpGrid->filetype);
	switch (m_intGridType)
	{
	case 0:
		fprintf(f_log, "(ASCII)\n");
		break;
	case 1:
		fprintf(f_log, "(Binary)\n");
		break;
	default:
		// return error
		fprintf(f_log, "\nError:  %s\n", m_chError_code[2]);
		return 2;
		break;
	}

	fprintf(f_log, "\nOverview Header Block:\n\n");
	PrintAsciiHeader(f_log, fpGrid, "  ");
	
	fprintf(f_log, "\nExtract to file:   %s  ", fpNewGrid->filename);
	m_intNewGridType = DetermineFileType(fpNewGrid->filetype);
	if (m_intNewGridType == -1)
	{
		// print "unknown file type" warning to log file - default will be binary
		fprintf(f_log, "\nWarning:   %sThe new file will be saved in Binary format.\n\n", m_chError_code[2]);
		m_intNewGridType = 1;
	}
	else
		fprintf(f_log, "(%s)\n\n", (m_intNewGridType == 0 ? "ASCII":"Binary"));

	return 0;
}

void CGridEx::OpenLogFileAndWriteHeader(char *cGridFilePath)
{
	CString str;
	char cBuf[61];

	#define DATE_LONG " %A, %d %B %Y, %I:%M:%S %p."			// Wednesday, 24 November 1999 11:19:35 AM.
	
	m_strLogFilePath.Format("%s.log", cGridFilePath);	
	f_log = fopen(m_strLogFilePath, "w");
	
	GetSystemDate(cBuf, DATE_LONG);

	str = "//////////////////     NTv2 Grid Extractor Log File      ////////////////////////\n\nCreated:  ";
	str += cBuf;
	str += "\n\nVersion 1.0   © Natural Resources and Mines, QLD.\n";
	str += "/////////////////////////////////////////////////////////////////////////////////\n\n";

	fprintf(f_log, "%s", str);
}