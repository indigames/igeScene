#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/RectTransform.h"

#include "python/pyTransformComponent.h"

namespace ige::scene
{
    // RectTransform
    struct PyObject_RectTransform : PyObject_TransformComponent {};

    // Type declaration
    extern PyTypeObject PyTypeObject_RectTransform;

    // Dealloc
    void RectTransform_dealloc(PyObject_RectTransform *self);

    // String represent
    PyObject* RectTransform_str(PyObject_RectTransform *self);

    // Get anchor
    PyObject* Rect_getAnchor(PyObject_RectTransform* self);

    // Set anchor
    int RectTransform_setAnchor(PyObject_RectTransform* self, PyObject* value);

    // Get pivot
    PyObject* RectTransform_getPivot(PyObject_RectTransform* self);

    // Set pivot
    int RectTransform_setPivot(PyObject_RectTransform* self, PyObject* value);

    // Get offset
    PyObject* RectTransform_getOffset(PyObject_RectTransform* self);

    // Set offset
    int RectTransform_setOffset(PyObject_RectTransform* self, PyObject* value);

    // Get size
    PyObject* RectTransform_getSize(PyObject_RectTransform* self);

    // Set size
    int RectTransform_setSize(PyObject_RectTransform* self, PyObject* value);

    // Get rect
    PyObject* RectTransform_getRect(PyObject_RectTransform* self);

    // Get position
    PyObject* RectTransform_getPosition(PyObject_RectTransform* self);

    // Set position
    int RectTransform_setPosition(PyObject_RectTransform* self, PyObject* value);

    // Get rotation
    PyObject* RectTransform_getRotation(PyObject_RectTransform* self);

    // Set rotation
    int RectTransform_setRotation(PyObject_RectTransform* self, PyObject* value);

    // Get scale
    PyObject* RectTransform_getScale(PyObject_RectTransform* self);

    // Set rotation
    int RectTransform_setScale(PyObject_RectTransform* self, PyObject* value);
}