@echo off
:: File Name create-SysConfig_prm.bat
:: This file is used to fit the definition inside the SystemConfig_prm.h file with the PROJ_TYPE selected
::
:: This file should be called by the makefile. Error checking is not present in this DRAFT release.
::
:: Expects the following parameters:

setlocal

echo     Updating SRSystemConfig_prm.h and SystemConfig_prm.h files...
set PROJ_TYPE=%~1
set PROJ_TYPE=%PROJ_TYPE: =%
set DIRSYSCONF=%2
set DIRSRSYSCONF=%3
set DIRBLD=%4

if %PROJ_TYPE%==HMI goto foundHMI
if %PROJ_TYPE%==ACU goto foundACU
if %PROJ_TYPE%==EXPACU goto foundEXPACU
if %PROJ_TYPE%==EXPHMI goto foundEXPHMI
if %PROJ_TYPE%==ACUHMI goto foundACUHMI
if %PROJ_TYPE%==HMIACU goto foundACUHMI
if %PROJ_TYPE%==EXPACUEXPHMI goto foundEXPACUEXPHMI
if %PROJ_TYPE%==ACUEXPHMI goto foundACUEXPHMI
if %PROJ_TYPE%==HMIEXPACU goto foundHMIEXPACU
echo     WARNING: SystemConfig_prm.h was not updated. "%PROJ_TYPE%" is not supported.
goto done

: foundHMI
set D_ACU_E=DISABLED
set D_HMI_E=ENABLED
set D_EXPACU_E=DISABLED
set D_EXPHMI_E=DISABLED
set N_NODE_N=WIN_UI_ADDRESS
goto updateSysConfig

: foundACU
set D_ACU_E=ENABLED
set D_HMI_E=DISABLED
set D_EXPACU_E=DISABLED
set D_EXPHMI_E=DISABLED
set N_NODE_N=WIN_ACU_ADDRESS
goto updateSysConfig

: foundEXPACU
set D_ACU_E=DISABLED
set D_HMI_E=DISABLED
set D_EXPACU_E=ENABLED
set D_EXPHMI_E=DISABLED
set N_NODE_N=WIN_ACU_EXP_ADDRESS
goto updateSysConfig

: foundEXPHMI
set D_ACU_E=DISABLED
set D_HMI_E=DISABLED
set D_EXPACU_E=DISABLED
set D_EXPHMI_E=ENABLED
set N_NODE_N=WIN_HMI_EXP_ADDRESS
goto updateSysConfig

: foundACUHMI
set D_ACU_E=ENABLED
set D_HMI_E=ENABLED
set D_EXPACU_E=DISABLED
set D_EXPHMI_E=DISABLED
set N_NODE_N=WIN_ACU_ADDRESS
goto updateSysConfig

:foundEXPACUEXPHMI
set D_ACU_E=DISABLED
set D_HMI_E=DISABLED
set D_EXPACU_E=ENABLED
set D_EXPHMI_E=ENABLED
set N_NODE_N=WIN_HMI_EXP_ADDRESS
goto updateSysConfig


:foundACUEXPHMI
set D_ACU_E=ENABLED
set D_HMI_E=DISABLED
set D_EXPACU_E=DISABLED
set D_EXPHMI_E=ENABLED
set N_NODE_N=WIN_ACU_ADDRESS
goto updateSysConfig

:foundHMIEXPACU
set D_ACU_E=DISABLED
set D_HMI_E=ENABLED
set D_EXPACU_E=ENABLED
set D_EXPHMI_E=DISABLED
set N_NODE_N=WIN_UI_ADDRESS


: updateSysConfig
set TEMP_FILE=%DIRSRSYSCONF%\_SR_System_Config_prm_h.temporary
type %DIRSRSYSCONF%\SRSystemConfig_prm.h  | %DIRBLD%\regex.exe D_ACU_E:%D_ACU_E% D_HMI_E:%D_HMI_E% D_EXPACU_E:%D_EXPACU_E% D_EXPHMI_E:%D_EXPHMI_E% N_NODE_N:%N_NODE_N% -f %DIRBLD%\updateSysConfig.regex > %TEMP_FILE%
del %DIRSRSYSCONF%\SRSystemConfig_prm.h
ren %TEMP_FILE% SRSystemConfig_prm.h
set TEMP_FILE=%DIRSYSCONF%\_System_Config_prm_h.temporary
type %DIRSYSCONF%\SystemConfig_prm.h  | %DIRBLD%\regex.exe D_ACU_E:%D_ACU_E% D_HMI_E:%D_HMI_E% D_EXPACU_E:%D_EXPACU_E% D_EXPHMI_E:%D_EXPHMI_E% N_NODE_N:%N_NODE_N% -f %DIRBLD%\updateSysConfig.regex > %TEMP_FILE%
del %DIRSYSCONF%\SystemConfig_prm.h
ren %TEMP_FILE% SystemConfig_prm.h

: done

endlocal