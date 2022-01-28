#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/animation/AnimatorComponent.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_Animator : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_Animator;

    // Dealloc
    void Animator_dealloc(PyObject_Animator *self);

    // String represent
    PyObject *Animator_str(PyObject_Animator *self);

    //! Controller path
    PyObject *Animator_getControllerPath(PyObject_Animator *self);
    int Animator_setControllerPath(PyObject_Animator *self, PyObject *value);
} // namespace ige::scene
