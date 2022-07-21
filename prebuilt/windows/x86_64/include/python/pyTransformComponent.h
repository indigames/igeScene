#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/TransformComponent.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_TransformComponent: PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_TransformComponent;

    // Dealloc
    void  TransformComponent_dealloc(PyObject_TransformComponent *self);

    // String represent
    PyObject* TransformComponent_str(PyObject_TransformComponent *self);

    // Get position
    PyObject* TransformComponent_getLocalPosition(PyObject_TransformComponent* self);

    // Set position
    int TransformComponent_setLocalPosition(PyObject_TransformComponent* self, PyObject* value);

    // Get rotation
    PyObject* TransformComponent_getLocalRotation(PyObject_TransformComponent* self);

    // Set rotation
    int TransformComponent_setLocalRotation(PyObject_TransformComponent* self, PyObject* value);

    // Get scale
    PyObject* TransformComponent_getLocalScale(PyObject_TransformComponent* self);

    // Set scale
    int TransformComponent_setLocalScale(PyObject_TransformComponent* self, PyObject* value);

    // Get local transform matrix
    PyObject* TransformComponent_getLocalMatrix(PyObject_TransformComponent* self);

    // Get world position
    PyObject* TransformComponent_getWorldPosition(PyObject_TransformComponent* self);

    // Set world position
    int TransformComponent_setWorldPosition(PyObject_TransformComponent* self, PyObject* value);

    // Get world rotation
    PyObject* TransformComponent_getWorldRotation(PyObject_TransformComponent* self);

    // Set world rotation
    int TransformComponent_setWorldRotation(PyObject_TransformComponent* self, PyObject* value);

    // Get world scale
    PyObject* TransformComponent_getWorldScale(PyObject_TransformComponent* self);

    // Set world scale
    int TransformComponent_setWorldScale(PyObject_TransformComponent* self, PyObject* value);

    // Get world transform matrix
    PyObject* TransformComponent_getWorldMatrix(PyObject_TransformComponent* self);


    //! Get world forward, up, right
    PyObject* TransformComponent_getWorldForward(PyObject_TransformComponent* self);
    PyObject* TransformComponent_getWorldUp(PyObject_TransformComponent* self);
    PyObject* TransformComponent_getWorldRight(PyObject_TransformComponent* self);

    //! Get Local forward, up, right
    PyObject* TransformComponent_getLocalForward(PyObject_TransformComponent* self);
    PyObject* TransformComponent_getLocalUp(PyObject_TransformComponent* self);
    PyObject* TransformComponent_getLocalRight(PyObject_TransformComponent* self);

    //! LookAt
    PyObject* TransformComponent_setLookAt(PyObject_TransformComponent* self, PyObject* value);
}
