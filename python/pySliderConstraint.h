#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/SliderConstraint.h"

#include "python/pyComponent.h"
#include "python/pyPhysicConstraint.h"

namespace ige::scene
{
    struct PyObject_SliderConstraint
    {
        PyObject_PhysicConstraint super;
        SliderConstraint *constraint;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_SliderConstraint;

    // Dealloc
    void SliderConstraint_dealloc(PyObject_SliderConstraint *self);

    // lowerLimit
    PyObject *SliderConstraint_getLowerLimit(PyObject_SliderConstraint *self);
    int SliderConstraint_setLowerLimit(PyObject_SliderConstraint *self, PyObject *value);

    // upperLimit
    PyObject *SliderConstraint_getUpperLimit(PyObject_SliderConstraint *self);
    int SliderConstraint_setUpperLimit(PyObject_SliderConstraint *self, PyObject *value);

} // namespace ige::scene
