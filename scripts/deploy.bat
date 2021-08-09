@echo off
setlocal enabledelayedexpansion
set CONAN_REVISIONS_ENABLED=1
set CURR_DIR=%CD%

set PYPIRC="%USERPROFILE%\.pypirc"

if not exist "!PYPIRC!" (
    echo Ensure to have '.pypirc' in HOME dir, or you will need to enter password manually!
)

REM Set PYTHON to select different version of pythons
REM If PYTHON was not set, use default python in system
if [%PYTHON%]==[] (
    set PYTHON=python
)

cd %~dp0..
REM Ensure wheel and twine exist
!PYTHON! -m pip install wheel twine --user
if %ERRORLEVEL% NEQ 0 goto ERROR

REM Build wheel package
!PYTHON! setup.py sdist bdist_wheel
if %ERRORLEVEL% NEQ 0 goto ERROR

REM Upload to Pypi
!PYTHON! -m twine upload -r testpypi --config-file !PYPIRC! --non-interactive --skip-existing dist/*
if %ERRORLEVEL% NEQ 0 goto ERROR

cd %CURR_DIR%
echo ALL DONE!
goto :eof

:ERROR
    cd %CURR_DIR%
    echo ERROR OCCURED DURING COMPILING!
    exit /b 1

