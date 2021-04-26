@echo off
setlocal enabledelayedexpansion

set CURR_DIR=%CD%
set CURR_DRIVE=!CURR_DIR:~0,2!

if "%IGE_BUILDER%"=="" (
    set IGE_BUILDER=%APPDATA%\indigames\igeBuilder
)

if not exist "!IGE_BUILDER!\.git" (
    mkdir "!IGE_BUILDER!"
    git clone https://github.com/indigames/igeBuilder !IGE_BUILDER!
) else (
    set IGE_BUILDER_DRIVE=!IGE_BUILDER:~0,2!
    !IGE_BUILDER_DRIVE!
    cd !IGE_BUILDER!
    
    git fetch --all
    git checkout main
    git pull

    !CURR_DRIVE!
    cd !CURR_DIR!
)

if not exist "!IGE_BUILDER!\build-lib.bat" (
    echo ERROR: IGE_BUILDER was not found
    goto ERROR
)

set PYPIRC="%USERPROFILE%\.pypirc"

if not exist "!PYPIRC!" (
    echo Ensure to have '.pypirc' in HOME dir, or you will need to enter password manually!
    set PYPIRC="%IGE_BUILDER%\pypi\.pypirc"
)

REM Set PYTHON to select different version of pythons
REM If PYTHON was not set, use default python in system
if [%PYTHON%]==[] (
    set PYTHON=python
)

cd %~dp0..
REM Ensure wheel and twine exist
!PYTHON! -m pip install wheel twine --user

REM Build wheel package
!PYTHON! setup.py sdist bdist_wheel

REM Upload to Pypi
!PYTHON! -m twine upload -r testpypi --config-file !PYPIRC! --non-interactive --skip-existing dist/*

cd %CURR_DIR%
echo ALL DONE!
goto :eof

:ERROR
    cd %CURR_DIR%
    echo ERROR OCCURED DURING COMPILING!
    exit /b 1
