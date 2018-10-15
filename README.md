# GDAy
GDAy 3.0 transforms coordinates between GDA94 and GDA2020 datums. GDAy supports conformal and non-conformal (i.e. conformal plus distortion) transformations via bi-linear interpolation from a grid of coordinate shifts, and conformal datum transformations via seven-parameter similarity (Helmert) transformation. GDAy may be used anywhere throughout Australia provided an appropriate grid is selected.
With GDAy, users can perform datum transformations and coordinate conversions using geographical coordinates (ie. latitude and longitude) or Universal Transverse Mercator (UTM) projection coordinates (ie. Easting, Northing and zone), whether working on one datum or between GDA94 and GDA2020. Coordinates may be transformed by entering values in the main GDAy dialog or by file input. 

The supported file types include:
- Formatted Text Files  (*.dat, *.prn, *.txt).
- Comma Separated Values Files  (*.csv).
- QLD Survey Control Database (SCDB) MAN Files  (*.man).
- Queensland Interchange Format (QIF) Files, Version 3  (*.qif).
- NEWGAN Res Files  (*.res).

GDAy Version 3.0 (GDA2020) Installation Files
---------------------------------------------
32-Bit: GDAy_3_0_Setup_x86.msi
64-Bit: GDAy_3_0_Setup_x64.msi

Installation Testing
--------------------
32-Bit: Tested on various Win10 64-Bit (Native) and Win7 32-Bit (Virtual Machine)
64-Bit: Tested on various Win10 64-Bit (Native & Virtual Machine)

Known Issues
------------
GDAy:
  None

GridEx:
  Windows Help Files (.hlp) are no longer supported by Microsoft. There is a PDF User Guide as an alternative.. It was not possible to rebuild the help without conversion to newer methods (Compiled HTML)
  Older versions of Windows that have not been updated may encounter issues with the Universal C Runtime Library. See https://support.microsoft.com/en-us/help/2999226/update-for-universal-c-runtime-in-windows
