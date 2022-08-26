#!/bin/bash

export PATH=/usr/local/bin:$PATH
export PYPIRC=$HOME/.pypirc
export DEPLOY_TARGET=$1
export SCRIPT_PATH=$(greadlink -f "$0");
export SCRIPT_DIR=$(dirname "$SCRIPT_PATH");
export PROJECT_DIR=$SCRIPT_DIR/..

if [ ! -f "$PYPIRC" ]; then
    echo ERROR: Ensure to have '.pypirc' in HOME dir!
    exit -1
fi

if [ -z ${PYTHON+x} ]; then
    export PYTHON=python3.9
fi

# Ensure wheel and twine exist
$PYTHON -m pip install wheel twine --user

pushd $PROJECT_DIR
$PYTHON setup.py sdist bdist_wheel
if [ $? -ne 0 ]; then
    echo ERROR: Compiling failed!
    popd
    exit $?
fi

if [ "$DEPLOY_TARGET" = "testpypi" ]; then
    $PYTHON -m twine upload -r testpypi --config-file "$PYPIRC" --non-interactive --skip-existing dist/*
else
    $PYTHON -m twine upload --config-file "$PYPIRC" --non-interactive --skip-existing dist/*
fi

if [ $? -ne 0 ]; then
    echo ERROR: Uploading failed!
    popd
    exit $?
fi
popd

echo ALL DONE!
