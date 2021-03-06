#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/FigureComponent.h"

#include "python/pyComponent.h"


namespace ige::scene
{
    struct PyObject_FigureComponent
    {
        PyObject_Component super;
        FigureComponent* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_FigureComponent;

    // Dealloc
    void  FigureComponent_dealloc(PyObject_FigureComponent *self);

    // String represent
    PyObject* FigureComponent_str(PyObject_FigureComponent *self);

    // Get path
    PyObject* FigureComponent_getPath(PyObject_FigureComponent* self);

    // Set path
    int FigureComponent_setPath(PyObject_FigureComponent* self, PyObject* value);
}
