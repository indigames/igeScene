#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/constraint/HingeConstraint.h"

#include "python/pyComponent.h"
#include "python/pyPhysicConstraint.h"

namespace ige::scene
{
    struct PyObject_HingeConstraint
    {
        PyObject_PhysicConstraint super;
        HingeConstraint *constraint;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_HingeConstraint;

    // Dealloc
    void HingeConstraint_dealloc(PyObject_HingeConstraint *self);

    // String represent
    PyObject *HingeConstraint_str(PyObject_HingeConstraint *self);

    // anchor
    PyObject *HingeConstraint_getAnchor(PyObject_HingeConstraint *self);
    int HingeConstraint_setAnchor(PyObject_HingeConstraint *self, PyObject *value);

    // axis1
    PyObject *HingeConstraint_getAxis1(PyObject_HingeConstraint *self);
    int HingeConstraint_setAxis1(PyObject_HingeConstraint *self, PyObject *value);

    // axis2
    PyObject *HingeConstraint_getAxis2(PyObject_HingeConstraint *self);
    int HingeConstraint_setAxis2(PyObject_HingeConstraint *self, PyObject *value);

    // lowerLimit
    PyObject *HingeConstraint_getLowerLimit(PyObject_HingeConstraint *self);
    int HingeConstraint_setLowerLimit(PyObject_HingeConstraint *self, PyObject *value);

    // upperLimit
    PyObject *HingeConstraint_getUpperLimit(PyObject_HingeConstraint *self);
    int HingeConstraint_setUpperLimit(PyObject_HingeConstraint *self, PyObject *value);

} // namespace ige::scene
