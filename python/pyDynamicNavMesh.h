#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/navigation/DynamicNavMesh.h"

#include "python/pyComponent.h"
#include "python/pyNavMesh.h"

namespace ige::scene
{
    struct PyObject_DynamicNavMesh
    {
        PyObject_NavMesh super;
        DynamicNavMesh *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_DynamicNavMesh;

    // Dealloc
    void DynamicNavMesh_dealloc(PyObject_DynamicNavMesh *self);

    // String represent
    PyObject *DynamicNavMesh_str(PyObject_DynamicNavMesh *self);

    //! Max obstacles
    PyObject *DynamicNavMesh_getMaxObstacles(PyObject_DynamicNavMesh *self);
    int DynamicNavMesh_setMaxObstacles(PyObject_DynamicNavMesh *self, PyObject *value);

    //! Max layers
    PyObject *DynamicNavMesh_getMaxLayers(PyObject_DynamicNavMesh *self);
    int DynamicNavMesh_setMaxLayers(PyObject_DynamicNavMesh *self, PyObject *value);

} // namespace ige::scene
