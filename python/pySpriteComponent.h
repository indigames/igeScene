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

    // Texture
    PyObject* SpriteComponent_getTexture(PyObject_SpriteComponent* self);
    int SpriteComponent_setTexture(PyObject_SpriteComponent* self, PyObject* value);

    // Billboard setting
    PyObject* SpriteComponent_isBillboard(PyObject_SpriteComponent* self);
    int SpriteComponent_setBillboard(PyObject_SpriteComponent* self, PyObject* value);

    // Get color
    PyObject* SpriteComponent_getColor(PyObject_SpriteComponent* self);

    // Set color
    int SpriteComponent_setColor(PyObject_SpriteComponent* self, PyObject* value);

    // Get Fill Method
    PyObject* SpriteComponent_getFillMethod(PyObject_SpriteComponent* self);

    // Set Fill Method
    int SpriteComponent_setFillMethod(PyObject_SpriteComponent* self, PyObject* value);

    // Get Fill Origin
    PyObject* SpriteComponent_getFillOrigin(PyObject_SpriteComponent* self);

    // Set Fill Origin
    int SpriteComponent_setFillOrigin(PyObject_SpriteComponent* self, PyObject* value);

    // Get Fill Amount
    PyObject* SpriteComponent_getFillAmount(PyObject_SpriteComponent* self);

    // Set Fill Amount
    int SpriteComponent_setFillAmount(PyObject_SpriteComponent* self, PyObject* value);

    // Get Fill Clockwise
    PyObject* SpriteComponent_getClockwise(PyObject_SpriteComponent* self);

    // Set Fill Clockwise
    int SpriteComponent_setClockwise(PyObject_SpriteComponent* self, PyObject* value);
}