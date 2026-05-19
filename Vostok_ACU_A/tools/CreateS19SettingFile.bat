@echo off
:: File Name CreateS19SettingFile.bat
:: This file is used to extract the Setting File Start address from the MAP file
:: The address is used to create the S19 of the FVT Setting file from the binary file
:: This file should be called by the makefile. Error checking is not present in this DRAFT release.
::
:: Expects the following parameters:

setlocal

set MAP_FILE=%~1

::Extract information about project name and "exe" folder from the map file path
set file=%MAP_FILE%
FOR /F "delims=" %%i IN ("%file%") DO (
set PROJECT_NAME=%%~ni
set EXE_FOLDER=%%~dpi
)

set TOOLS=%~2
set SF_NAME_NO_EXT=%~n3
set SF_NAME=%~n3.bin
set BUILD=%~4
set TEMP=%~5

:: Check if the binary setting file exists
IF NOT EXIST %CD%\%BUILD%\%SF_NAME% goto setting_file_not_exists

::Extract the value the FVT Setting file size in the build folder
FOR /F "usebackq" %%A IN ('%BUILD%\%SF_NAME%') DO set fvt_size=%%~zA

::Extract the value of the Setting File Size written in the .icf file
for /f "tokens=1" %%a in ('type %MAP_FILE% ^| %TOOLS%\regex.exe -f %TOOLS%\SettingFileSize.regex') do set size=%%a

::Check if the FVT setting file has a size greater than the value in the .icf file
if %fvt_size% gtr %size% goto sf_size_mismatch

::Extract the value the Setting File Start address
for /f "tokens=1" %%i in ('type %MAP_FILE% ^| %TOOLS%\regex.exe -f %TOOLS%\SettingFileStartAddress.regex') do set address=%%i

::Create the setting file in the S19 format using the binary source and the address
%TOOLS%\DatasetReadWriteApplication\DatasetReadWriteApplication.exe %BUILD%\%SF_NAME% \o:%TEMP%\%SF_NAME_NO_EXT%.s19 \rt:s \sa:%address%
goto end

: sf_size_mismatch
echo Error:the FVT setting file size is greater than the value set in the ".icf" file; Increase this value or decrease GESE Setting File Size parameter >&2
exit /b 2
goto end

: setting_file_not_exists
echo Error:the binary setting file listed in the project_config.mk does not exist >&2
exit /b 2
goto end

:end
endlocal