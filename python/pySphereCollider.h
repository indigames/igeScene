#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/SphereCollider.h"

#include "python/pyComponent.h"
#include "python/pyRigidbody.h"

namespace ige::scene
{
    struct PyObject_SphereCollider : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_SphereCollider;

    // Dealloc
    void SphereCollider_dealloc(PyObject_SphereCollider *self);

    // String represent
    PyObject *SphereCollider_str(PyObject_SphereCollider *self);

    //! radius
    PyObject *SphereCollider_getRadius(PyObject_SphereCollider *self);
    int SphereCollider_setRadius(PyObject_SphereCollider *self, PyObject *value);

} // namespace ige::scene
