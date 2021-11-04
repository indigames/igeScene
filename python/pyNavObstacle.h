#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/navigation/NavObstacle.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_NavObstacle : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_NavObstacle;

    // Dealloc
    void NavObstacle_dealloc(PyObject_NavObstacle *self);

    // String represent
    PyObject *NavObstacle_str(PyObject_NavObstacle *self);

    //! Radius
    PyObject *NavObstacle_getRadius(PyObject_NavObstacle *self);
    int NavObstacle_setRadius(PyObject_NavObstacle *self, PyObject *value);

    //! Height
    PyObject *NavObstacle_getHeight(PyObject_NavObstacle *self);
    int NavObstacle_setHeight(PyObject_NavObstacle *self, PyObject *value);
} // namespace ige::scene
