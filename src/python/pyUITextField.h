#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UITextField.h"

#include "python/pyUIText.h"

namespace ige::scene
{
    struct PyObject_UITextField : PyObject_UIText {};

    // Type declaration
    extern PyTypeObject PyTypeObject_UITextField;

    // Dealloc
    void  UITextField_dealloc(PyObject_UITextField *self);

    // String represent
    PyObject* UITextField_str(PyObject_UITextField *self);
}
