#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/navigation/OffMeshLink.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_OffMeshLink
    {
        PyObject_Component super;
        OffMeshLink *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_OffMeshLink;

    // Dealloc
    void OffMeshLink_dealloc(PyObject_OffMeshLink *self);

    // String represent
    PyObject *OffMeshLink_str(PyObject_OffMeshLink *self);

    //! Radius
    PyObject *OffMeshLink_getRadius(PyObject_OffMeshLink *self);
    int OffMeshLink_setRadius(PyObject_OffMeshLink *self, PyObject *value);

    //! Mask
    PyObject *OffMeshLink_getMask(PyObject_OffMeshLink *self);
    int OffMeshLink_setMask(PyObject_OffMeshLink *self, PyObject *value);

    //! AreaId
    PyObject *OffMeshLink_getAreaId(PyObject_OffMeshLink *self);
    int OffMeshLink_setAreaId(PyObject_OffMeshLink *self, PyObject *value);

    //! Endpoint
    PyObject *OffMeshLink_getEndPoint(PyObject_OffMeshLink *self);
    int OffMeshLink_setEndPoint(PyObject_OffMeshLink *self, PyObject *value);

    //! Bidirectional
    PyObject *OffMeshLink_isBidirectional(PyObject_OffMeshLink *self);
    int OffMeshLink_setBidirectional(PyObject_OffMeshLink *self, PyObject *value);
} // namespace ige::scene
