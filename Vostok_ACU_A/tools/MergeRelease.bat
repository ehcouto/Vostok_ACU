@echo off
:: File Name Merge Release.bat
::
:: This file is used to merge the Project S19 with the setting file FVT in a S19 format 
:: to produce a Release version without IAP
::
:: This file is used to merge the Project S19 with the setting file FVT in a S19 format and IAP file in a S19 format 
:: to produce a Release version with IAP
:: This file should be called by the makefile. Error checking is not present in this DRAFT release.
::
:: Expects the following parameters:

setlocal

set EXE_FOLDER=%~1
set TOOLS=%~2
set PROJECT_NAME=%~3
set SF_NAME=%~n4
set FILL_BYTE=%~5
set CONFIG_FOLDER=%~6
set IAP_NAME=%~7

::check if the user want to create a release version with/without IAP
if "%7"=="" goto noIap

::Merging "Project" S19 file with "Setting File FVT S19" file and "IAP" S19 file
%TOOLS%\DatasetReadWriteApplication\DatasetReadWriteApplication.exe %CD%\%EXE_FOLDER%\%PROJECT_NAME%.s19  %CD%\%CONFIG_FOLDER%\%SF_NAME%.s19  %CD%\config\build\%IAP_NAME% \o:%CD%\%EXE_FOLDER%\%PROJECT_NAME%_with_FVT_IAP.s19 \g:f:%FILL_BYTE%
goto end

::Merging "Project" S19 file with "Setting File FVT S19" file
: noIap
%TOOLS%\DatasetReadWriteApplication\DatasetReadWriteApplication.exe %CD%\%EXE_FOLDER%\%PROJECT_NAME%.s19  %CD%\%CONFIG_FOLDER%\%SF_NAME%.s19  \o:%CD%\%EXE_FOLDER%\%PROJECT_NAME%_with_FVT.s19 \g:f:%FILL_BYTE%


: end
endlocal