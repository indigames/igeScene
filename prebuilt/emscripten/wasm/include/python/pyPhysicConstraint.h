#pragma once

#include <Python.h>

#include "components/physic/PhysicConstraint.h"

namespace ige::scene
{
    struct PyObject_PhysicConstraint
    {
        PyObject_HEAD
        PhysicConstraint* constraint;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_PhysicConstraint;

    // Dealloc
    void  PhysicConstraint_dealloc(PyObject_PhysicConstraint *self);

    // String represent
    PyObject* PhysicConstraint_str(PyObject_PhysicConstraint *self);

    // type
    PyObject* PhysicConstraint_getType(PyObject_PhysicConstraint* self);

    // owner
    PyObject* PhysicConstraint_getOwner(PyObject_PhysicConstraint* self);

    // other
    PyObject* PhysicConstraint_getOther(PyObject_PhysicConstraint* self);
    int PhysicConstraint_setOther(PyObject_PhysicConstraint* self, PyObject* value);

    // isEnabled
    PyObject* PhysicConstraint_isEnabled(PyObject_PhysicConstraint* self);
    int PhysicConstraint_setEnabled(PyObject_PhysicConstraint* self, PyObject* value);

    // isBodiesCollisionEnabled
    PyObject* PhysicConstraint_isBodiesCollisionEnabled(PyObject_PhysicConstraint* self);
    int PhysicConstraint_setBodiesCollisionEnabled(PyObject_PhysicConstraint* self, PyObject* value);

    // breakingImpulse
    PyObject* PhysicConstraint_getBreakingImpulse(PyObject_PhysicConstraint* self);
    int PhysicConstraint_setBreakingImpulse(PyObject_PhysicConstraint* self, PyObject* value);
}
