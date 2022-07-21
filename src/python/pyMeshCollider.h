#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/collider/MeshCollider.h"

#include "python/pyRigidbody.h"

namespace ige::scene
{
    struct PyObject_MeshCollider : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_MeshCollider;

    // Dealloc
    void MeshCollider_dealloc(PyObject_MeshCollider *self);

    // String represent
    PyObject *MeshCollider_str(PyObject_MeshCollider *self);

    //! Convex
    PyObject *MeshCollider_isConvex(PyObject_MeshCollider *self);
    int MeshCollider_setConvex(PyObject_MeshCollider *self, PyObject *value);

    //! Model path
    PyObject *MeshCollider_getPath(PyObject_MeshCollider *self);
    int MeshCollider_setPath(PyObject_MeshCollider *self, PyObject *value);
} // namespace ige::scene
