@echo off
setlocal enabledelayedexpansion

set CONAN_REVISIONS_ENABLED=1
set CALL_DIR=%CD%
set PROJECT_DIR=%~dp0..

python %PROJECT_DIR%/build.py
if %ERRORLEVEL% NEQ 0 goto ERROR

cd %CALL_DIR%
echo ALL DONE!
goto :eof

:ERROR
    cd %CALL_DIR%
    echo ERROR OCCURED DURING COMPILING!
    exit /b 1

