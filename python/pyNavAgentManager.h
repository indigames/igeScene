#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/navigation/NavAgentManager.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_NavAgentManager : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_NavAgentManager;

    // Dealloc
    void NavAgentManager_dealloc(PyObject_NavAgentManager *self);

    // String represent
    PyObject *NavAgentManager_str(PyObject_NavAgentManager *self);

    //! NavMesh
    PyObject *NavAgentManager_getNavMesh(PyObject_NavAgentManager *self);
    int NavAgentManager_setNavMesh(PyObject_NavAgentManager *self, PyObject *value);

    //! Max Agent Number
    PyObject *NavAgentManager_getMaxAgentNumber(PyObject_NavAgentManager *self);
    int NavAgentManager_setMaxAgentNumber(PyObject_NavAgentManager *self, PyObject *value);

    //! Max Agent Radius
    PyObject *NavAgentManager_getMaxAgentRadius(PyObject_NavAgentManager *self);
    int NavAgentManager_setMaxAgentRadius(PyObject_NavAgentManager *self, PyObject *value);

    //! getNumQueryFilterTypes
    PyObject *NavAgentManager_getNumQueryFilterTypes(PyObject_NavAgentManager *self);

    //! getNumAreas
    PyObject *NavAgentManager_getNumAreas(PyObject_NavAgentManager *self, PyObject* value);

    //! setAreaCost
    PyObject *NavAgentManager_setAreaCost(PyObject_NavAgentManager *self, PyObject* value);

    //! getAreaCost
    PyObject *NavAgentManager_getAreaCost(PyObject_NavAgentManager *self, PyObject* value);

    //! setIncludeFlags
    PyObject *NavAgentManager_setIncludeFlags(PyObject_NavAgentManager *self, PyObject* value);

    //! getIncludeFlags
    PyObject *NavAgentManager_getIncludeFlags(PyObject_NavAgentManager *self, PyObject* value);

    //! setExcludeFlags
    PyObject *NavAgentManager_setExcludeFlags(PyObject_NavAgentManager *self, PyObject* value);

    //! getExcludeFlags
    PyObject *NavAgentManager_getExcludeFlags(PyObject_NavAgentManager *self, PyObject* value);

    //! getAgents
    PyObject *NavAgentManager_getAgents(PyObject_NavAgentManager *self, PyObject* value);

    //! findNearestPoint
    PyObject *NavAgentManager_findNearestPoint(PyObject_NavAgentManager *self, PyObject* value);

    //! moveAlongSurface
    PyObject *NavAgentManager_moveAlongSurface(PyObject_NavAgentManager *self, PyObject* value);

    //! findPath
    PyObject *NavAgentManager_findPath(PyObject_NavAgentManager *self, PyObject* value);

    //! getRandomPoint
    PyObject *NavAgentManager_getRandomPoint(PyObject_NavAgentManager *self, PyObject* value);

    //! getRandomPointInCircle
    PyObject *NavAgentManager_getRandomPointInCircle(PyObject_NavAgentManager *self, PyObject* value);

    //! getDistanceToWall
    PyObject *NavAgentManager_getDistanceToWall(PyObject_NavAgentManager *self, PyObject* value);

    //! raycast
    PyObject *NavAgentManager_raycast(PyObject_NavAgentManager *self, PyObject* value);

    //! deactivateAllAgents
    PyObject *NavAgentManager_deactivateAllAgents(PyObject_NavAgentManager *self);

    //! reactivateAllAgents
    PyObject *NavAgentManager_reactivateAllAgents(PyObject_NavAgentManager *self);
} // namespace ige::scene
