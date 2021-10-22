#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/SpriteComponent.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_SpriteComponent
    {
        PyObject_Component super;
        SpriteComponent* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_SpriteComponent;

    // Dealloc
    void  SpriteComponent_dealloc(PyObject_SpriteComponent *self);

    // String represent
    PyObject* SpriteComponent_str(PyObject_SpriteComponent *self);

    // Get path
    PyObject* SpriteComponent_getPath(PyObject_SpriteComponent* self);

    // Set path
    int SpriteComponent_setPath(PyObject_SpriteComponent* self, PyObject* value);

    // Get size
    PyObject* SpriteComponent_getSize(PyObject_SpriteComponent* self);

    // Set size
    int SpriteComponent_setSize(PyObject_SpriteComponent* self, PyObject* value);

    // Billboard setting
    PyObject* SpriteComponent_isBillboard(PyObject_SpriteComponent* self);
    int SpriteComponent_setBillboard(PyObject_SpriteComponent* self, PyObject* value);

    // Get color
    PyObject* SpriteComponent_getColor(PyObject_SpriteComponent* self);

    // Set color
    int SpriteComponent_setColor(PyObject_SpriteComponent* self, PyObject* value);
}