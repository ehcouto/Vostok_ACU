@echo off

setlocal

:checkIfInRootDirectory
if not exist tools goto checkIfInToolsDirectory
set DIRBAT=.
goto generateUnitTests

:checkIfInToolsDirectory
if not exist ..\tools echo Run createUnitTestCaller from project root directory & pause & goto end
set DIRBAT=..

:generateUnitTests
echo Generating RunUnitTests.bat...
if exist %DIRBAT%\RunUnitTests.bat echo     already exists -- delete existing file before creating a new one
if exist %DIRBAT%\RunUnitTests.bat goto unitTestsDone
copy %DIRBAT%\tools\Templates\RunUnitTests.template %DIRBAT%\RunUnitTests.bat>nul
echo     generated
:unitTestsDone

:end

endlocal
