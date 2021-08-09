#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/PhysicMesh.h"

#include "python/pyPhysicObject.h"

namespace ige::scene
{
    struct PyObject_PhysicMesh
    {
        PyObject_PhysicObject super;
        PhysicMesh *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_PhysicMesh;

    // Dealloc
    void PhysicMesh_dealloc(PyObject_PhysicMesh *self);

    // String represent
    PyObject *PhysicMesh_str(PyObject_PhysicMesh *self);

    //! Convex
    PyObject *PhysicMesh_isConvex(PyObject_PhysicMesh *self);
    int PhysicMesh_setConvex(PyObject_PhysicMesh *self, PyObject *value);

    //! Model path
    PyObject *PhysicMesh_getPath(PyObject_PhysicMesh *self);
    int PhysicMesh_setPath(PyObject_PhysicMesh *self, PyObject *value);
} // namespace ige::scene
