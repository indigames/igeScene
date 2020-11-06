#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/PhysicBox.h"

#include "python/pyComponent.h"
#include "python/pyPhysicObject.h"

namespace ige::scene
{
    struct PyObject_PhysicBox
    {
        PyObject_PhysicObject super;
        PhysicBox *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_PhysicBox;

    // Dealloc
    void PhysicBox_dealloc(PyObject_PhysicBox *self);

    // String represent
    PyObject *PhysicBox_str(PyObject_PhysicBox *self);

    //! size
    PyObject *PhysicBox_getSize(PyObject_PhysicBox *self);
    int PhysicBox_setSize(PyObject_PhysicBox *self, PyObject *value);

} // namespace ige::scene
