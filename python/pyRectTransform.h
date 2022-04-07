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

    // Get anchor Position
    PyObject* RectTransform_getAnchorPosition(PyObject_RectTransform* self);
    // Set anchor Position
    int RectTransform_setAnchorPosition(PyObject_RectTransform* self, PyObject* value);

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
    PyObject* RectTransform_getWorldPosition(PyObject_RectTransform* self);

    // Set position
    int RectTransform_setWorldPosition(PyObject_RectTransform* self, PyObject* value);

    // Get rotation
    PyObject* RectTransform_getWorldRotation(PyObject_RectTransform* self);

    // Set rotation
    int RectTransform_setWorldRotation(PyObject_RectTransform* self, PyObject* value);

    // Get scale
    PyObject* RectTransform_getWorldScale(PyObject_RectTransform* self);

    // Set rotation
    int RectTransform_setWorldScale(PyObject_RectTransform* self, PyObject* value);

    // Get local position
    PyObject* RectTransform_getLocalPosition(PyObject_RectTransform* self);

    // Set local position
    int RectTransform_setLocalPosition(PyObject_RectTransform* self, PyObject* value);

    // Get local rotation
    PyObject* RectTransform_getLocalRotation(PyObject_RectTransform* self);

    // Set local rotation
    int RectTransform_setLocalRotation(PyObject_RectTransform* self, PyObject* value);

    // Get local scale
    PyObject* RectTransform_getLocalScale(PyObject_RectTransform* self);

    // Set local rotation
    int RectTransform_setLocalScale(PyObject_RectTransform* self, PyObject* value);
}