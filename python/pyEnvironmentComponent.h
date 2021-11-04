#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/EnvironmentComponent.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_EnvironmentComponent : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_EnvironmentComponent;

    // Dealloc
    void  EnvironmentComponent_dealloc(PyObject_EnvironmentComponent *self);

    // String represent
    PyObject* EnvironmentComponent_str(PyObject_EnvironmentComponent *self);

    // Get Distance Fog Color
    PyObject* EnvironmentComponent_getDistanceFogColor(PyObject_EnvironmentComponent* self);

    // Set Distance Fog Color
    int EnvironmentComponent_setDistanceFogColor(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Distance Fog Alpha
    PyObject* EnvironmentComponent_getDistanceFogAlpha(PyObject_EnvironmentComponent* self);

    // Set Distance Fog Alpha
    int EnvironmentComponent_setDistanceFogAlpha(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Distance Fog Near
    PyObject* EnvironmentComponent_getDistanceFogNear(PyObject_EnvironmentComponent* self);

    // Set Distance Fog Near
    int EnvironmentComponent_setDistanceFogNear(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Distance Fog Far
    PyObject* EnvironmentComponent_getDistanceFogFar(PyObject_EnvironmentComponent* self);

    // Set Distance Fog Far
    int EnvironmentComponent_setDistanceFogFar(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Shadow Color
    PyObject* EnvironmentComponent_getShadowColor(PyObject_EnvironmentComponent* self);

    // Set Shadow Color
    int EnvironmentComponent_setShadowColor(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Shadow Density
    PyObject* EnvironmentComponent_getShadowDensity(PyObject_EnvironmentComponent* self);

    // Set Shadow Density
    int EnvironmentComponent_setShadowDensity(PyObject_EnvironmentComponent* self, PyObject* value);

    // Get Shadow Wideness
    PyObject* EnvironmentComponent_getShadowWideness(PyObject_EnvironmentComponent* self);

    // Set Shadow Wideness
    int EnvironmentComponent_setShadowWideness(PyObject_EnvironmentComponent* self, PyObject* value);

}
