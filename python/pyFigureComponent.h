#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/FigureComponent.h"

#include "python/pyComponent.h"
#include "python/pyFigureComponent_doc_en.h"


namespace ige::scene
{
    struct PyObject_FigureComponent
    {
        PyObject_Component super;
        std::shared_ptr<FigureComponent> component;
    };

    // Type declaration
    PyTypeObject PyTypeObject_FigureComponent;

    // Dealloc
    void  FigureComponent_dealloc(PyObject_FigureComponent *self);

    // String represent
    PyObject* FigureComponent_str(PyObject_FigureComponent *self);

    // Get path
    PyObject* FigureComponent_getPath(PyObject_FigureComponent* self);

    // Set path
    int FigureComponent_setPath(PyObject_FigureComponent* self, PyObject* value);
}
