#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/collider/CapsuleCollider.h"

#include "python/pyComponent.h"
#include "python/pyRigidbody.h"

namespace ige::scene
{
    struct PyObject_CapsuleCollider : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_CapsuleCollider;

    // Dealloc
    void CapsuleCollider_dealloc(PyObject_CapsuleCollider *self);

    // String represent
    PyObject *CapsuleCollider_str(PyObject_CapsuleCollider *self);

    //! radius
    PyObject *CapsuleCollider_getRadius(PyObject_CapsuleCollider *self);
    int CapsuleCollider_setRadius(PyObject_CapsuleCollider *self, PyObject *value);

    //! height
    PyObject *CapsuleCollider_getHeight(PyObject_CapsuleCollider *self);
    int CapsuleCollider_setHeight(PyObject_CapsuleCollider *self, PyObject *value);
} // namespace ige::scene
