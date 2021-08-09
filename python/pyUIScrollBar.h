#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIScrollBar.h"

#include "python/pyUIImage.h"

namespace ige::scene
{
    struct PyObject_UIScrollBar
    {
        PyObject_UIImage super;
        UIScrollBar* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_UIScrollBar;

    // Dealloc
    void  UIScrollBar_dealloc(PyObject_UIScrollBar *self);

    // String represent
    PyObject* UIScrollBar_str(PyObject_UIScrollBar *self);
}