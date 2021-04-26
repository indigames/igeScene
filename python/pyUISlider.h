#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UISlider.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_UISlider
    {
        PyObject_Component super;
        UISlider* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_UISlider;

    // Dealloc
    void  UISlider_dealloc(PyObject_UISlider*self);

    // String represent
    PyObject* UISlider_str(PyObject_UISlider*self);
}