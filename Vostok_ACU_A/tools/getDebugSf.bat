@echo off
:: ============================================================================
:: This batch file looks for a file based on the given path and copies it to
:: the specified destination.

setlocal

if "%1"=="" goto showUsage
goto setUpVariables

:: ============================================================================
:: Show usage and exit.
: showUsage
echo.
echo Usage: %0 [source] destination
echo     If 'source' is not specified, then create an invalid setting file at
echo       'destination'.
echo     If 'source' is a file, then copy the file to the 'destination'.
echo     If 'source' is a directory, then copy the newest file with the
echo       correct extension to the 'destination'. If no appropriate file
echo       is found, then create an invalid setting file at the 'destination'.
echo.
echo Parameters:
echo     source        The path of a file or a directory to search.
echo     destination   The path of the file to create or overwrite.
echo.
echo This batch file is not intended to be run manually.
echo It should only be called by the project Makefile.
echo.
pause
goto done

:: ============================================================================
: setUpVariables
if not "%2"=="" goto twoParams

: oneParam
set FILE_DST=%~1
if exist %FILE_DST% set SIZE_DST=%~z1
if not exist %FILE_DST% set SIZE_DST=-1
goto noSettingFile

: twoParams
set SOURCE=%~1
set FILE_DST=%~2
set FOLDER_DST=%~dp2
set NAME_DST=%~nx2
set FILE_EXT=%~x2
if exist %FILE_DST% set SIZE_DST=%~z2
if not exist %FILE_DST% set SIZE_DST=-1

:: ============================================================================
:: Determine what the 'source' is: directory or file.
if exist %SOURCE%\* goto useDirectory
if exist %SOURCE% goto useFile
echo     %0 cannot find "%SOURCE%"
goto noSettingFile

:: ============================================================================
: useDirectory
:: Find the newest file with the 'destination' file's extension in the 'source' folder
for /f "delims=" %%F in ('dir /a-d/b/o-d/tw %SOURCE%\*%FILE_EXT% 2^>nul') do (
    set FILE_SRC=%SOURCE%\%%F
    echo     Using "%%F" from the "%SOURCE%" directory
    goto copySettingFileIfChanged
)
echo     %0 cannot find any *%FILE_EXT% file in the "%SOURCE%" directory
goto noSettingFile

:: ============================================================================
: useFile
set FILE_SRC=%SOURCE%

:: ============================================================================
: copySettingFileIfChanged
:: Always copy the selected setting file if there is no previous setting file
if not exist %FILE_DST% goto copySettingFile
:: Only copy the selected setting file if it is different from the previous setting file
fc /b %FILE_SRC% %FILE_DST%>nul
if not errorlevel 1 goto done

: copySettingFile
copy /b /y %FILE_SRC% %FILE_DST%>nul
:: Set the destination file's date and time to the current date and time
cd %FOLDER_DST%
copy /b /y %NAME_DST% +,,>nul
goto done

:: ============================================================================
: noSettingFile
echo     Debug files will use an invalid setting file. Setting file download is required.
set FILE_SRC=tools\Templates\empty_settingfile.bin
goto copySettingFileIfChanged

:: ============================================================================
: done

endlocal