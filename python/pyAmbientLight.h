#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/light/AmbientLight.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_AmbientLight
    {
        PyObject_Component super;
        AmbientLight *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_AmbientLight;

    // Dealloc
    void AmbientLight_dealloc(PyObject_AmbientLight *self);

    // String represent
    PyObject *AmbientLight_str(PyObject_AmbientLight *self);

    //! Sky Color
    PyObject *AmbientLight_getSkyColor(PyObject_AmbientLight *self);
    int AmbientLight_setSkyColor(PyObject_AmbientLight *self, PyObject *value);

    //! Ground Color
    PyObject *AmbientLight_getGroundColor(PyObject_AmbientLight *self);
    int AmbientLight_setGroundColor(PyObject_AmbientLight *self, PyObject *value);

    //! Direction
    PyObject *AmbientLight_getDirection(PyObject_AmbientLight *self);
    int AmbientLight_setDirection(PyObject_AmbientLight *self, PyObject *value);
} // namespace ige::scene
