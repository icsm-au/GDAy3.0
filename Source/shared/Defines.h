#ifndef _DEFINE_H_
#define _DEFINE_H_

#define THE_APP "GDAy 3.0"

#define HLP_GDAY "GDAy Help Table Of Contents"
#define HLP_VIEW "GDAy User Interface Settings"
#define HLP_INTERACTIVE "Interactive Output File"
#define HLP_LOG "Log File"

#define REGKEY_GDAY30 "SOFTWARE\\Geoscience Australia\\GDAy 3.0\\3.0"
#define REGKEY_GRIDEX "SOFTWARE\\Geoscience Australia\\Grid Extractor"

#define CRD_SYSTEM "Application\\CoordinateSystem"
#define INPUT_DATUM "InputDatum"
#define INPUT_PROJ "InputProj"
#define INPUT_DMS "InputDMS"
#define INPUT_HP "InputHP"
#define OUTPUT_DATUM "OutputDatum"
#define OUTPUT_PROJ "OutputProj"
#define OUTPUT_DMS "OutputDMS"
#define OUTPUT_HP "OutputHP"

#define CUSTOM "Application\\Custom"
#define ADD_PF_HEADER "AddPFileHeader"
#define ADD_PREFIX "AddPrefix"
#define ADD_SYMBOLS "AddSymbols"
#define AUTO_INCR "AutoIncrement"
#define AUTO_NAME "AutoStationName"
#define ADD_STATION_NAME "AddStationName"
#define AUTO_NAME_ALL_STATIONS "AutoNameAllStations"
#define GEO_PREC "GeoPrecision"
#define GRIDTYPE "GridType"
#define HEADER "HeaderInfo"
#define POS_SOUTH "PosSouth"
#define OUTPUT_ZONE_PROMPT "PromptOutputZone"
#define GRIF_FILE_PROMPT "PromptGridFile"
#define MET_PREC "MetPrecision"
#define START_OUTPUT "OnStartOutput"
#define SHOW_PT_NUM "ShowPtNum"
#define START_NUM "StartNumber"

#define FILE_LOCATIONS "Application\\FileLocations"
#define INTERACTIVE_OUTPUT "InteractiveOutput"
#define DISTORTION_GRID "DistortionGrid"
#define LOG_FILE "LogFile"

#define TRANS_PARAMS "Application\\TransformationParameters"
#define TRANS_PARAM "Current"
#define KEYROOT_PARAM_NAT94 "Application\\TransformationParameters\\National AGD84"

#define NAT94 "GDA94 - GDA2020 (National)"

#define GEO "Geographic "
#define GDA94_STR "Geocentric Datum of Australia (GDA) 1994"
#define MGA94_STR "Map Grid of Australia (MGA 1994)"
#define GDA20_STR "Geocentric Datum of Australia (GDA) 2020"
#define MGA20_STR "Map Grid of Australia (MGA 2020)"
#define E "Easting: "
#define N "Northing: "
#define LA "Latitude: "
#define LO "Longitude: "

#define EXPORT_GRIDFILE "Export the grid file to..."
#define SELECT_GRID_FILE "Select the NTv2 distortion grid file..."
#define SAVE_NEW_GRID_FILE "Save the new grid file as..."
#define NO_GRID_SPECIFIED "no_grid_specified"
#define AUS_GSB_MSG "The distortion grid file you have selected is an obsolete binary file format and should be updated.\nTo obtain the latest version of the gridfile for your area, contact your State's land information\nagency from the following website:\n\n\t\t http://www.anzlic.org.au/icsm/gda/links.htm \n\nSee the Online help for details on NTv2 Binary Files."

#define SELECT_OUTFILE "Select the new output file..."
#define SELECT_LOGFILE "Select the new log file..."
#define OPEN_PTS_FILE "Select coordinate input file(s) to transform..."
#define SAVE_PTS_FILE "Save transformed coordinate output file as..."

#define GRID_FILTER "NTv2 Binary Grid File (*.gsb)|*.gsb|NTv2 ASCII Grid File (*.asc)|*.asc||"
#define GSB_FILTER "NTv2 Binary Grid File (*.gsb)|*.gsb||"
#define ASC_FILTER "NTv2 ASCII Grid File (*.asc)|*.asc||"

#define OUT_FILTER "GDAy Output Files (*.out)|*.out;|All Files(*.*)|*.*||"
#define LOG_FILTER "GDAy Log File (*.log)|*.log|All Files(*.*)|*.*||"

#define PTS_FILE_FILTER "Formatted Text Files (*.dat,*.txt,*.prn)|*.dat;*.txt;*.prn;|CSV (Comma delimited) (*.csv)|*.csv|QIF (Queensland Interchange Format) (*.qif)|*.qif;|SCDB man file (*.man)|*.man;|Newgan RES File (*.res)|*.res;|All Files(*.*)|*.*||"	// ASD (AS2482 Specification) (*.asd)|*.asd|
#define DATCSV_FILE_FILTER "Formatted Text Files (*.dat,*.txt,*.prn)|*.dat;*.txt;*.prn;|CSV (Comma delimited) (*.csv)|*.csv|All Files(*.*)|*.*||"
#define QIF_FILE_FILTER "QIF (Queensland Interchange Format) (*.qif)|*.qif;|All Files(*.*)|*.*||"
#define RES_FILE_FILTER "Newgan RES File (*.res)|*.res;|Formatted Text Files (*.dat,*.txt,*.prn)|*.dat;*.txt;*.prn;|CSV (Comma delimited) (*.csv)|*.csv|All Files(*.*)|*.*||"
#define MAN_FILE_FILTER "SCDB man file (*.man)|*.man;|Formatted Text Files (*.dat,*.txt,*.prn)|*.dat;*.txt;*.prn;|CSV (Comma delimited) (*.csv)|*.csv|All Files(*.*)|*.*||"

#define GRIDEX "Grid Extractor"
#define APP_PATH "Path"

#define GSB "gsb"
#define ASC "asc"
#define DAT "dat"
#define TXT "txt"
#define PRN "prn"
#define CSV "csv"
#define QIF "qif"
#define MAN "man"
#define RES "res"
#define OUTPUT "out"
#define LOG "log"

#define _dat 0
#define _txt 1
#define _prn 2
#define _csv 3
#define _qif 4
#define _man 5
#define _res 6

#define SEL_ERR "Please make a unique selection from the list"

#define GDA94 0
#define GDA2020 1
#define GEOGRAPHIC 0
#define PROJECTED  1
#define DMS  0
#define DMIN 1
#define DDEG 2
#define HP_NOTATION 1
#define NULL_ACCURACY -1000.0

#define YES		1
#define YESALL	2
#define NO		3
#define NOALL	4
#define CANCEL	5

#define TYPE_ASC	0
#define TYPE_GSB	1
#define TYPE_DAT	2
#define TYPE_CSV	3
#define TYPE_QIF	4
#define TYPE_MAN	5
#define TYPE_RES	7

#endif