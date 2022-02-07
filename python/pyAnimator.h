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

    //! isInitialized
    PyObject *Animator_isInitialized(PyObject_Animator *self);

    //! getParameterCount
    PyObject *Animator_getParameterCount(PyObject_Animator *self);

    //! speed
    PyObject *Animator_getSpeed(PyObject_Animator *self);
    int Animator_setSpeed(PyObject_Animator *self, PyObject *value);

    //! updateMode
    PyObject *Animator_getUpdateMode(PyObject_Animator *self);
    int Animator_setUpdateMode(PyObject_Animator *self, PyObject *value);

    //! Get value
    PyObject* Animator_getValue(PyObject_Animator* self, PyObject* value);

    //! Set value
    PyObject* Animator_setValue(PyObject_Animator* self, PyObject* value);

    //! Set trigger
    PyObject* Animator_setTrigger(PyObject_Animator* self, PyObject* value);

    //! Reset trigger
    PyObject* Animator_resetTrigger(PyObject_Animator* self, PyObject* value);

    //! Rebind
    PyObject* Animator_rebind(PyObject_Animator* self);

    //! hasState
    PyObject* Animator_hasState(PyObject_Animator* self, PyObject* value);

    //! play
    PyObject* Animator_play(PyObject_Animator* self, PyObject* value);

} // namespace ige::scene
