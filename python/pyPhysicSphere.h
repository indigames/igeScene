#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/PhysicSphere.h"

#include "python/pyComponent.h"
#include "python/pyPhysicObject.h"

namespace ige::scene
{
    struct PyObject_PhysicSphere : PyObject_PhysicObject {};

    // Type declaration
    extern PyTypeObject PyTypeObject_PhysicSphere;

    // Dealloc
    void PhysicSphere_dealloc(PyObject_PhysicSphere *self);

    // String represent
    PyObject *PhysicSphere_str(PyObject_PhysicSphere *self);

    //! radius
    PyObject *PhysicSphere_getRadius(PyObject_PhysicSphere *self);
    int PhysicSphere_setRadius(PyObject_PhysicSphere *self, PyObject *value);

} // namespace ige::scene
