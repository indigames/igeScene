#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/navigation/NavMesh.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_NavMesh
    {
        PyObject_Component super;
        NavMesh *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_NavMesh;

    // Dealloc
    void NavMesh_dealloc(PyObject_NavMesh *self);

    // String represent
    PyObject *NavMesh_str(PyObject_NavMesh *self);

    //! Tile size
    PyObject *NavMesh_getTileSize(PyObject_NavMesh *self);
    int NavMesh_setTileSize(PyObject_NavMesh *self, PyObject *value);

    //! Cell size
    PyObject *NavMesh_getCellSize(PyObject_NavMesh *self);
    int NavMesh_setCellSize(PyObject_NavMesh *self, PyObject *value);

    //! Cell height
    PyObject *NavMesh_getCellHeight(PyObject_NavMesh *self);
    int NavMesh_setCellHeight(PyObject_NavMesh *self, PyObject *value);

    //! Agent height
    PyObject *NavMesh_getAgentHeight(PyObject_NavMesh *self);
    int NavMesh_setAgentHeight(PyObject_NavMesh *self, PyObject *value);

    //! Agent radius
    PyObject *NavMesh_getAgentRadius(PyObject_NavMesh *self);
    int NavMesh_setAgentRadius(PyObject_NavMesh *self, PyObject *value);

    //! Agent max vertical climb
    PyObject *NavMesh_getAgentMaxClimb(PyObject_NavMesh *self);
    int NavMesh_setAgentMaxClimb(PyObject_NavMesh *self, PyObject *value);

    //! Agent max slope
    PyObject *NavMesh_getAgentMaxSlope(PyObject_NavMesh *self);
    int NavMesh_setAgentMaxSlope(PyObject_NavMesh *self, PyObject *value);

    //! Region minimum size
    PyObject *NavMesh_getRegionMinSize(PyObject_NavMesh *self);
    int NavMesh_setRegionMinSize(PyObject_NavMesh *self, PyObject *value);

    //! Region merge size
    PyObject *NavMesh_getRegionMergeSize(PyObject_NavMesh *self);
    int NavMesh_setRegionMergeSize(PyObject_NavMesh *self, PyObject *value);

    //! Edge max length
    PyObject *NavMesh_getEdgeMaxLength(PyObject_NavMesh *self);
    int NavMesh_setEdgeMaxLength(PyObject_NavMesh *self, PyObject *value);

    //! Edge max error
    PyObject *NavMesh_getEdgeMaxError(PyObject_NavMesh *self);
    int NavMesh_setEdgeMaxError(PyObject_NavMesh *self, PyObject *value);

    //! Detail sampling distance
    PyObject *NavMesh_getDetailSampleDistance(PyObject_NavMesh *self);
    int NavMesh_setDetailSampleDistance(PyObject_NavMesh *self, PyObject *value);

    //! Detail sampling maximum error
    PyObject *NavMesh_getDetailSampleMaxError(PyObject_NavMesh *self);
    int NavMesh_setDetailSampleMaxError(PyObject_NavMesh *self, PyObject *value);

    //! Partitioning type
    PyObject *NavMesh_getPartitionType(PyObject_NavMesh *self);
    int NavMesh_setPartitionType(PyObject_NavMesh *self, PyObject *value);

    //! Bounding box padding
    PyObject *NavMesh_getPadding(PyObject_NavMesh *self);
    int NavMesh_setPadding(PyObject_NavMesh *self, PyObject *value);

    //! Build
    PyObject *NavMesh_build(PyObject_NavMesh *self);

    //! Get AABB
    PyObject *NavMesh_getAABB(PyObject_NavMesh *self);

    //! Get world AABB
    PyObject *NavMesh_getWorldAABB(PyObject_NavMesh *self);

    //! Get number of tiles
    PyObject *NavMesh_getNumTiles(PyObject_NavMesh *self);

    //! Get tile index
    PyObject *NavMesh_getTileIndex(PyObject_NavMesh *self, PyObject *value);

    //! Get tile aabb
    PyObject *NavMesh_getTileAABB(PyObject_NavMesh *self, PyObject *value);

} // namespace ige::scene
