#pragma once

#include <Python.h>

//! Module initialization
PyMODINIT_FUNC _PyInit_igeScene();

//! Functions implementation
static PyObject* igeScene_getVersion(PyObject* self);
