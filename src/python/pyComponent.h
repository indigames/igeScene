#pragma once

#include <Python.h>

#include "components/Component.h"

namespace ige::scene
{
    struct PyObject_Component
    {
        PyObject_HEAD
        std::weak_ptr<Component> component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_Component;

    // Dealloc
    void  Component_dealloc(PyObject_Component *self);

    // String represent
    PyObject* Component_str(PyObject_Component *self);

    // Get name
    PyObject* Component_getName(PyObject_Component* self);

    // Get owner
    PyObject* Component_getOwner(PyObject_Component* self);

    // Enable
    PyObject* Component_isEnabled(PyObject_Component* self);
    int Component_setEnabled(PyObject_Component* self, PyObject* value);

    // Update
    PyObject* Component_onUpdate(PyObject_Component* self, PyObject* args);
}
