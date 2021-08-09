#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/light/PointLight.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_PointLight
    {
        PyObject_Component super;
        PointLight *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_PointLight;

    // Dealloc
    void PointLight_dealloc(PyObject_PointLight *self);

    // String represent
    PyObject *PointLight_str(PyObject_PointLight *self);

    //! Color
    PyObject *PointLight_getColor(PyObject_PointLight *self);
    int PointLight_setColor(PyObject_PointLight *self, PyObject *value);

    //! Intensity
    PyObject *PointLight_getIntensity(PyObject_PointLight *self);
    int PointLight_setIntensity(PyObject_PointLight *self, PyObject *value);

    //! Range
    PyObject *PointLight_getRange(PyObject_PointLight *self);
    int PointLight_setRange(PyObject_PointLight *self, PyObject *value);

    //! Position
    PyObject *PointLight_getPosition(PyObject_PointLight *self);
} // namespace ige::scene
