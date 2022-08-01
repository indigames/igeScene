#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/constraint/SpringConstraint.h"

#include "python/pyComponent.h"
#include "python/pyPhysicConstraint.h"

namespace ige::scene
{
    struct PyObject_SpringConstraint
    {
        PyObject_PhysicConstraint super;
        SpringConstraint *constraint;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_SpringConstraint;

    // Dealloc
    void SpringConstraint_dealloc(PyObject_SpringConstraint *self);

    // enable
    PyObject *SpringConstraint_getEnable(PyObject_SpringConstraint *self);
    int SpringConstraint_setEnable(PyObject_SpringConstraint *self, PyObject *value);

    // stiffness
    PyObject *SpringConstraint_getStiffness(PyObject_SpringConstraint *self);
    int SpringConstraint_setStiffness(PyObject_SpringConstraint *self, PyObject *value);

    // damping
    PyObject *SpringConstraint_getDamping(PyObject_SpringConstraint *self);
    int SpringConstraint_setDamping(PyObject_SpringConstraint *self, PyObject *value);

    // lowerLimit
    PyObject *SpringConstraint_getLowerLimit(PyObject_SpringConstraint *self);
    int SpringConstraint_setLowerLimit(PyObject_SpringConstraint *self, PyObject *value);

    // upperLimit
    PyObject *SpringConstraint_getUpperLimit(PyObject_SpringConstraint *self);
    int SpringConstraint_setUpperLimit(PyObject_SpringConstraint *self, PyObject *value);

} // namespace ige::scene
