@echo off

if "%1"=="" echo Specify which unit tests you wish to run (e.g. %0 Nucleus)& exit /b 1

cd tests\%1
  if %ERRORLEVEL% neq 0 exit /b 1

  if not exist build mkdir build
  call ceedling clobber

  if %ERRORLEVEL%==0 call ceedling gcov:all utils:gcov

  set UNIT_TESTS_FAILED=%ERRORLEVEL%
cd ..\..

exit /b %UNIT_TESTS_FAILED%
