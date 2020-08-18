#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/Canvas.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_Canvas
    {
        PyObject_Component super;
        std::shared_ptr<Canvas> component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_Canvas;

    // Dealloc
    void Canvas_dealloc(PyObject_Canvas *self);

    // String represent
    PyObject* Canvas_str(PyObject_Canvas *self);

    // Get design canvas size
    PyObject* Canvas_getDesignCanvasSize(PyObject_Canvas* self);

    // Set design canvas size
    int Canvas_setDesignCanvasSize(PyObject_Canvas* self, PyObject* value);

    // Set target canvas size
    PyObject* Canvas_getTargetCanvasSize(PyObject_Canvas* self);

    // Set target canvas size
    int Canvas_setTargetCanvasSize(PyObject_Canvas* self, PyObject* value);
}