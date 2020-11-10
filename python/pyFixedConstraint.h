#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/FixedConstraint.h"

#include "python/pyComponent.h"
#include "python/pyPhysicConstraint.h"

namespace ige::scene
{
    struct PyObject_FixedConstraint
    {
        PyObject_PhysicConstraint super;
        FixedConstraint *constraint;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_FixedConstraint;

    // Dealloc
    void FixedConstraint_dealloc(PyObject_FixedConstraint *self);

    // String represent
    PyObject *FixedConstraint_str(PyObject_FixedConstraint *self);
} // namespace ige::scene
