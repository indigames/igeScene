#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIMask.h"

#include "python/pyUIImage.h"

namespace ige::scene
{
    struct PyObject_UIMask
    {
        PyObject_UIImage super;
        UIMask* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_UIMask;

    // Dealloc
    void  UIImage_dealloc(PyObject_UIMask *self);

    // String represent
    PyObject* UIImage_str(PyObject_UIMask *self);
}