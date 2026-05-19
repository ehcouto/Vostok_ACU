@echo off

setlocal

:checkIfInRootDirectory
if not exist tools goto checkIfInToolsDirectory
set DIRBAT=.
goto generateBuild

:checkIfInToolsDirectory
if not exist ..\tools echo Run createBatFiles from project root directory & pause & goto end
set DIRBAT=..

:generateBuild
echo Generating build.bat...
if exist %DIRBAT%\build.bat echo     already exists -- delete existing file before creating a new one
if exist %DIRBAT%\build.bat goto buildDone
echo @echo off>%DIRBAT%\build.bat
echo tools\make -f tools\Makefile release>>%DIRBAT%\build.bat
echo     generated
:buildDone

:generateBuildDebug
echo Generating build_debug.bat...
if exist %DIRBAT%\build_debug.bat echo     already exists -- delete existing file before creating a new one
if exist %DIRBAT%\build_debug.bat goto buildDebugDone
echo @echo off>%DIRBAT%\build_debug.bat
echo tools\make -f tools\Makefile debug>>%DIRBAT%\build_debug.bat
echo     generated
:buildDebugDone

:generateMake
echo Generating make.bat...
if exist %DIRBAT%\make.bat echo     already exists -- delete existing file before creating a new one
if exist %DIRBAT%\make.bat goto makeDone
echo @echo off>%DIRBAT%\make.bat
echo tools\make -f tools\Makefile %%*>>%DIRBAT%\make.bat
echo     generated
:makeDone

:end

endlocal
