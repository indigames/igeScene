#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/light/SpotLight.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_SpotLight
    {
        PyObject_Component super;
        SpotLight *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_SpotLight;

    // Dealloc
    void SpotLight_dealloc(PyObject_SpotLight *self);

    // String represent
    PyObject *SpotLight_str(PyObject_SpotLight *self);

    //! Color
    PyObject *SpotLight_getColor(PyObject_SpotLight *self);
    int SpotLight_setColor(PyObject_SpotLight *self, PyObject *value);

    //! Intensity
    PyObject *SpotLight_getIntensity(PyObject_SpotLight *self);
    int SpotLight_setIntensity(PyObject_SpotLight *self, PyObject *value);

    //! Range
    PyObject *SpotLight_getRange(PyObject_SpotLight *self);
    int SpotLight_setRange(PyObject_SpotLight *self, PyObject *value);

    //! Angle
    PyObject *SpotLight_getAngle(PyObject_SpotLight *self);
    int SpotLight_setAngle(PyObject_SpotLight *self, PyObject *value);

    //! Position
    PyObject *SpotLight_getPosition(PyObject_SpotLight *self);

    //! Direction
    PyObject *SpotLight_getDirection(PyObject_SpotLight *self);
} // namespace ige::scene
