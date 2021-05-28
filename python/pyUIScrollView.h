#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIScrollView.h"

#include "python/pyUIImage.h"

namespace ige::scene
{
    struct PyObject_UIScrollView
    {
        PyObject_UIImage super;
        UIScrollView* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_UIScrollView;

    // Dealloc
    void  UIScrollView_dealloc(PyObject_UIScrollView *self);

    // String represent
    PyObject* UIScrollView_str(PyObject_UIScrollView *self);
}