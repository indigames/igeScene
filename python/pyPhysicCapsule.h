#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/PhysicCapsule.h"

#include "python/pyComponent.h"
#include "python/pyPhysicObject.h"

namespace ige::scene
{
    struct PyObject_PhysicCapsule
    {
        PyObject_PhysicObject super;
        PhysicCapsule *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_PhysicCapsule;

    // Dealloc
    void PhysicCapsule_dealloc(PyObject_PhysicCapsule *self);

    // String represent
    PyObject *PhysicCapsule_str(PyObject_PhysicCapsule *self);

    //! radius
    PyObject *PhysicCapsule_getRadius(PyObject_PhysicCapsule *self);
    int PhysicCapsule_setRadius(PyObject_PhysicCapsule *self, PyObject *value);

    //! height
    PyObject *PhysicCapsule_getHeight(PyObject_PhysicCapsule *self);
    int PhysicCapsule_setHeight(PyObject_PhysicCapsule *self, PyObject *value);
} // namespace ige::scene
