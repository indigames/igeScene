@echo off

set CALL_DIR=%CD%
set PROJECT_DIR=%~dp0..

cd %PROJECT_DIR%
python %PROJECT_DIR%/build.py
if %ERRORLEVEL% NEQ 0 goto ERROR

cd %CALL_DIR%
echo ALL DONE!
goto :eof

:ERROR
    cd %CALL_DIR%
    echo ERROR OCCURED DURING COMPILING!
    exit /b 1

