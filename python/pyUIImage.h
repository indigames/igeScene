#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIImage.h"

#include "python/pySpriteComponent.h"

namespace ige::scene
{
    struct PyObject_UIImage : PyObject_SpriteComponent {};

    // Type declaration
    extern PyTypeObject PyTypeObject_UIImage;

    // Dealloc
    void  UIImage_dealloc(PyObject_UIImage *self);

    // String represent
    PyObject* UIImage_str(PyObject_UIImage *self);

    // Get path
    PyObject* UIImage_getPath(PyObject_UIImage* self);

    // Set path
    int UIImage_setPath(PyObject_UIImage* self, PyObject* value);

    // Get color
    PyObject* UIImage_getColor(PyObject_UIImage* self);

    // Set color
    int UIImage_setColor(PyObject_UIImage* self, PyObject* value);

    // Get Fill Method
    PyObject* UIImage_getFillMethod(PyObject_UIImage* self);

    // Set Fill Method
    int UIImage_setFillMethod(PyObject_UIImage* self, PyObject* value);

    // Get Fill Origin
    PyObject* UIImage_getFillOrigin(PyObject_UIImage* self);

    // Set Fill Origin
    int UIImage_setFillOrigin(PyObject_UIImage* self, PyObject* value);

    // Get Fill Amount
    PyObject* UIImage_getFillAmount(PyObject_UIImage* self);

    // Set Fill Amount
    int UIImage_setFillAmount(PyObject_UIImage* self, PyObject* value);

    // Get Fill Clockwise
    PyObject* UIImage_getClockwise(PyObject_UIImage* self);

    // Set Fill Clockwise
    int UIImage_setClockwise(PyObject_UIImage* self, PyObject* value);

    // Set Interactable
    PyObject* UIImage_getInteractable(PyObject_UIImage* self);

    // Set Interactable
    int UIImage_setInteractable(PyObject_UIImage* self, PyObject* value);
}