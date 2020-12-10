#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/PhysicManager.h"

namespace ige::scene
{
    struct PyObject_PhysicManager
    {
        PyObject_HEAD;
        PhysicManager *physicManager;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_PhysicManager;

    // Dealloc
    void PhysicManager_dealloc(PyObject_PhysicManager *self);

    // String represent
    PyObject *PhysicManager_str(PyObject_PhysicManager *self);

    // Get instance
    PyObject *PhysicManager_getInstance();

    // Clear
    PyObject *PhysicManager_clear(PyObject_PhysicManager *self);

    // Get deformable
    PyObject *PhysicManager_isDeformable(PyObject_PhysicManager *self);

    // Raytest closest
    PyObject *PhysicManager_rayTestClosest(PyObject_PhysicManager *self, PyObject *value);

    // Raytest all
    PyObject *PhysicManager_rayTestAll(PyObject_PhysicManager *self, PyObject *value);

    // Contact test
    PyObject* PhysicManager_contactTest(PyObject_PhysicManager* self, PyObject* value);

    // Contact pair test
    PyObject* PhysicManager_contactPairTest(PyObject_PhysicManager* self, PyObject* value);

    // Get gravity
    PyObject *PhysicManager_getGravity(PyObject_PhysicManager *self);

    // Set gravity
    int PhysicManager_setGravity(PyObject_PhysicManager *self, PyObject *value);

    // Get number of iteration
    PyObject *PhysicManager_getNumIteration(PyObject_PhysicManager *self);

    // Set number of iteration
    int PhysicManager_setNumIteration(PyObject_PhysicManager *self, PyObject *value);

    // Get frame update ratio (speedup/slower effects)
    PyObject *PhysicManager_getFrameUpdateRatio(PyObject_PhysicManager *self);

    // Set frame update ratio
    int PhysicManager_setFrameUpdateRatio(PyObject_PhysicManager *self, PyObject *value);

    // Get frame max simulation sub step
    PyObject *PhysicManager_getFrameMaxSubStep(PyObject_PhysicManager *self);

    // Set frame max simulation sub step
    int PhysicManager_setFrameMaxSubStep(PyObject_PhysicManager *self, PyObject *value);

    // Get fixed time steps
    PyObject *PhysicManager_getFixedTimeStep(PyObject_PhysicManager *self);

    // Set fixed time steps
    int PhysicManager_setFixedTimeStep(PyObject_PhysicManager *self, PyObject *value);
} // namespace ige::scene
