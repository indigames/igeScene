#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/navigation/NavArea.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_NavArea : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_NavArea;

    // Dealloc
    void NavArea_dealloc(PyObject_NavArea *self);

    // String represent
    PyObject *NavArea_str(PyObject_NavArea *self);

    //! AreaID
    PyObject *NavArea_getAreaId(PyObject_NavArea *self);
    int NavArea_setAreaId(PyObject_NavArea *self, PyObject *value);

    //! Bounding box
    PyObject *NavArea_getBoundingBox(PyObject_NavArea *self);
    int NavArea_setBoundingBox(PyObject_NavArea *self, PyObject *value);

    //! World bounding box
    PyObject *NavArea_getWorldBoundingBox(PyObject_NavArea *self);
} // namespace ige::scene
