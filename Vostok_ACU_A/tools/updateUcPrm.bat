@echo off
:: File Name create-uc_prm.h
:: This file is used to fit the definition inside the uc_prm.h file with the MICRO selected
::
:: This file should be called by the makefile. Error checking is not present in this DRAFT release.
::
:: Expects the following parameters:

setlocal


set MICRO=%~1
set MICRO=%MICRO: =%
set DIRDRVCFG=%2
set DIRBLD=%3

if %MICRO%==MKE1x-MKE1xF goto foundMKE1x-MKE1xF
if %MICRO%==MKE1x-MKE1xZ goto foundMKE1x-MKE1xZ
goto done


: foundMKE1x-MKE1xF
set MICRO_FAMILY=MKE1xF_FAMILY
goto updateUcPrm

: foundMKE1x-MKE1xZ
set MICRO_FAMILY=MKE1xZ_FAMILY
goto updateUcPrm


: updateUcPrm
set TEMP_FILE=%DIRDRVCFG%\%MICRO%\_uc_prm_h.temporary
type %DIRDRVCFG%\%MICRO%\uc_prm.h  | %DIRBLD%\regex.exe MICRO_FAMILY:%MICRO_FAMILY% -f %DIRBLD%\updateUcPrm.regex > %TEMP_FILE%
del %DIRDRVCFG%\%MICRO%\uc_prm.h
ren %TEMP_FILE% uc_prm.h
echo     Updated %DIRDRVCFG%\%MICRO%\uc_prm.h file

: done

endlocal