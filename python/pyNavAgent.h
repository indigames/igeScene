#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/navigation/NavAgent.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_NavAgent : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_NavAgent;

    // Dealloc
    void NavAgent_dealloc(PyObject_NavAgent *self);

    // String represent
    PyObject *NavAgent_str(PyObject_NavAgent *self);

    //! ID
    PyObject *NavAgent_getAgentId(PyObject_NavAgent *self);
    int NavAgent_setAgentId(PyObject_NavAgent *self, PyObject *value);

    //! Radius
    PyObject *NavAgent_getRadius(PyObject_NavAgent *self);
    int NavAgent_setRadius(PyObject_NavAgent *self, PyObject *value);

    //! Height
    PyObject *NavAgent_getHeight(PyObject_NavAgent *self);
    int NavAgent_setHeight(PyObject_NavAgent *self, PyObject *value);

    //! Max speed
    PyObject *NavAgent_getMaxSpeed(PyObject_NavAgent *self);
    int NavAgent_setMaxSpeed(PyObject_NavAgent *self, PyObject *value);

    //! Max acceleration
    PyObject *NavAgent_getMaxAcceleration(PyObject_NavAgent *self);
    int NavAgent_setMaxAcceleration(PyObject_NavAgent *self, PyObject *value);

    //! Target position
    PyObject *NavAgent_getTargetPosition(PyObject_NavAgent *self);
    int NavAgent_setTargetPosition(PyObject_NavAgent *self, PyObject *value);

    //! Auto Update Position
    PyObject *NavAgent_isUpdateNodePosition(PyObject_NavAgent *self);
    int NavAgent_setUpdateNodePosition(PyObject_NavAgent *self, PyObject *value);

    //! Query filter type
    PyObject *NavAgent_getQueryFilterType(PyObject_NavAgent *self);
    int NavAgent_setQueryFilterType(PyObject_NavAgent *self, PyObject *value);

    //! Obstacle avoidance type
    PyObject *NavAgent_getObstacleAvoidanceType(PyObject_NavAgent *self);
    int NavAgent_setObstacleAvoidanceType(PyObject_NavAgent *self, PyObject *value);

    //! Navigation quality
    PyObject *NavAgent_getNavigationQuality(PyObject_NavAgent *self);
    int NavAgent_setNavigationQuality(PyObject_NavAgent *self, PyObject *value);

    //! Navigation quality
    PyObject *NavAgent_getNavigationPushiness(PyObject_NavAgent *self);
    int NavAgent_setNavigationPushiness(PyObject_NavAgent *self, PyObject *value);

    //! Is in crowd
    PyObject *NavAgent_isInCrowd(PyObject_NavAgent *self);

    //! Has target
    PyObject *NavAgent_hasTarget(PyObject_NavAgent *self);

    //! Reset target
    PyObject *NavAgent_resetTarget(PyObject_NavAgent *self);

    //! Has arrived
    PyObject *NavAgent_hasArrived(PyObject_NavAgent *self);

    //! Get position
    PyObject *NavAgent_getPosition(PyObject_NavAgent *self);

    //! Get velocity
    PyObject *NavAgent_getVelocity(PyObject_NavAgent *self);

} // namespace ige::scene
