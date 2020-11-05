#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/light/DirectionalLight.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_DirectionalLight
    {
        PyObject_Component super;
        DirectionalLight *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_DirectionalLight;

    // Dealloc
    void DirectionalLight_dealloc(PyObject_DirectionalLight *self);

    // String represent
    PyObject *DirectionalLight_str(PyObject_DirectionalLight *self);

    //! Color
    PyObject *DirectionalLight_getColor(PyObject_DirectionalLight *self);
    int DirectionalLight_setColor(PyObject_DirectionalLight *self, PyObject *value);

    //! Intensity
    PyObject *DirectionalLight_getIntensity(PyObject_DirectionalLight *self);
    int DirectionalLight_setIntensity(PyObject_DirectionalLight *self, PyObject *value);

    //! Direction
    PyObject *DirectionalLight_getDirection(PyObject_DirectionalLight *self);
} // namespace ige::scene
