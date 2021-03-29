#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIButton.h"

#include "python/pyUIImage.h"

namespace ige::scene
{
    struct PyObject_UIButton
    {
        PyObject_UIImage super;
        UIButton* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_UIButton;

    // Dealloc
    void  UIButton_dealloc(PyObject_UIButton*self);

    // String represent
    PyObject* UIButton_str(PyObject_UIButton*self);
}