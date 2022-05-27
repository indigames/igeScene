#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/BoxCollider.h"

#include "python/pyComponent.h"
#include "python/pyRigidbody.h"

namespace ige::scene
{
    struct PyObject_BoxCollider : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_BoxCollider;

    // Dealloc
    void BoxCollider_dealloc(PyObject_BoxCollider *self);

    // String represent
    PyObject *BoxCollider_str(PyObject_BoxCollider *self);

    //! size
    PyObject *BoxCollider_getSize(PyObject_BoxCollider *self);
    int BoxCollider_setSize(PyObject_BoxCollider *self, PyObject *value);

} // namespace ige::scene
