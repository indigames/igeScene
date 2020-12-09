#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/navigation/Navigable.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_Navigable
    {
        PyObject_Component super;
        Navigable *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_Navigable;

    // Dealloc
    void Navigable_dealloc(PyObject_Navigable *self);

    // String represent
    PyObject *Navigable_str(PyObject_Navigable *self);

    //! Recursive
    PyObject *Navigable_isRecursive(PyObject_Navigable *self);
    int Navigable_setRecursive(PyObject_Navigable *self, PyObject *value);
} // namespace ige::scene
