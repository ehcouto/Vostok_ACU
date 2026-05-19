@echo off
:: =============================================================================
:: This batch file generates the merged output files for a project.
:: This batch file should be called by the makefile. Error checking is limited.

setlocal

if "%1"=="" goto showUsage
if "%2"=="" goto showUsage
if "%3"=="" goto showUsage
goto setUpMergeVars

:: =============================================================================
:: Show usage and exit.
: showUsage
echo.
echo Usage: merge.bat [-f FILL_BYTE] infile1 infile2 [infile3] outfile
echo     This batch file is not intended to be run manually.
echo     It should only be called by the project Makefile.
echo.
echo Parameters:
echo     FILL_BYTE  The byte value used to fill unused gaps in flash memory
echo     infileN    Input file path and (optionally) filename. If infileN
echo                refers to a file, then that file is used. If infileN
echo                refers to a directory, then the newest *.s19 file in that
echo                directory is used
echo     outfile    Output file path and filename
echo.
pause
goto done

:: =============================================================================
:: Set merge utility.
: setUpMergeVars
if exist DatasetReadWriteApplication.exe set MERGE=DatasetReadWriteApplication.exe
if exist DatasetReadWriteApplication\DatasetReadWriteApplication.exe set MERGE=DatasetReadWriteApplication\DatasetReadWriteApplication.exe
if exist tools\DatasetReadWriteApplication\DatasetReadWriteApplication.exe set MERGE=tools\DatasetReadWriteApplication\DatasetReadWriteApplication.exe
if "%MERGE%"=="" echo Merge.bat cannot find DatasetReadWriteApplication utility & exit 1
:: Set default parameters.
set FILL_INSTR=
set FILE1=
set FILE2=
set FILE3=
set FILE4=

:: =============================================================================
:: Parse the batch parameters.
: parseParams
if "%1"=="" goto merge
if "%1"=="-f" goto setFillByte
if exist %1 if exist %~1\nul goto checkDirectory
set FILE=%1
shift
goto setFile

: setFillByte
shift
set FILL_INSTR=\g:f:%~1
shift
goto parseParams

: checkDirectory
set PATH_FILE=%~1
for /f "delims=" %%I in ('dir /a-d/b/o-d/tw %PATH_FILE%\*.s19 2^>nul') do (
    set FILE=%PATH_FILE%\%%I
    shift
    goto setFile
)
echo Merge.bat cannot find any *.S19 file in the "%~1" directory & exit 1

: setFile
set FILE1=%FILE2%
set FILE2=%FILE3%
set FILE3=%FILE4%
set FILE4=%FILE%
goto parseParams

:: =============================================================================
:: Merge the specified files
: merge

if not "%FILE1%"=="" if not exist %FILE1% echo Merge.bat cannot find "%FILE1%" & exit 1
if "%FILE2%"=="" echo Merge.bat is missing an input filename & exit 1
if not exist %FILE2% echo Merge.bat cannot find "%FILE2%" & exit 1
if "%FILE3%"=="" echo Merge.bat is missing an input filename & exit 1
if not exist %FILE3% echo Merge.bat cannot find "%FILE3%" & exit 1
if "%FILE4%"=="" echo Merge.bat is missing the output filename & exit 1

echo.
echo Merging...
echo     %MERGE% %FILE1% %FILE2% %FILE3% \o:%FILE4% %FILL_INSTR%
%MERGE% %FILE1% %FILE2% %FILE3% \o:%FILE4% %FILL_INSTR%

:: =============================================================================
: done

endlocal