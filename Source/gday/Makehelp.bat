@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by GDAY.HPJ. >"hlp\GDAy.hm"
echo. >>"hlp\GDAy.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\GDAy.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\GDAy.hm"
echo. >>"hlp\GDAy.hm"
echo // Prompts (IDP_*) >>"hlp\GDAy.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\GDAy.hm"
echo. >>"hlp\GDAy.hm"
echo // Resources (IDR_*) >>"hlp\GDAy.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\GDAy.hm"
echo. >>"hlp\GDAy.hm"
echo // Dialogs (IDD_*) >>"hlp\GDAy.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\GDAy.hm"
echo. >>"hlp\GDAy.hm"
echo // Frame Controls (IDC_*) >>"hlp\GDAy.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\GDAy.hm"
REM -- Make help for Project GDAY


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\GDAy.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\GDAy.hlp" goto :Error
if not exist "hlp\GDAy.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\GDAy.hlp" Debug
if exist Debug\nul copy "hlp\GDAy.cnt" Debug
if exist Release\nul copy "hlp\GDAy.hlp" Release
if exist Release\nul copy "hlp\GDAy.cnt" Release
echo.
goto :done

:Error
echo hlp\GDAy.hpj(1) : error: Problem encountered creating help file

:done
echo.
