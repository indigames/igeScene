#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/TextBitmapComponent.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_TextBitmapComponent : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_TextBitmapComponent;

    // Dealloc
    void  TextBitmapComponent_dealloc(PyObject_TextBitmapComponent*self);

    // String represent
    PyObject* TextBitmapComponent_str(PyObject_TextBitmapComponent*self);

    // Get text
    PyObject* TextBitmapComponent_getText(PyObject_TextBitmapComponent* self);

    // Set text
    int TextBitmapComponent_setText(PyObject_TextBitmapComponent* self, PyObject* value);

    // Get font path
    PyObject* TextBitmapComponent_getFontPath(PyObject_TextBitmapComponent* self);

    // Set font path
    int TextBitmapComponent_setFontPath(PyObject_TextBitmapComponent* self, PyObject* value);

    // Get font size
    PyObject* TextBitmapComponent_getFontSize(PyObject_TextBitmapComponent* self);

    // Set font size
    int TextBitmapComponent_setFontSize(PyObject_TextBitmapComponent* self, PyObject* value);

    // Get color
    PyObject* TextBitmapComponent_getColor(PyObject_TextBitmapComponent* self);

    // Set color
    int TextBitmapComponent_setColor(PyObject_TextBitmapComponent* self, PyObject* value);

    // Billboard setting
    PyObject* TextBitmapComponent_isBillboard(PyObject_TextBitmapComponent* self);
    int TextBitmapComponent_setBillboard(PyObject_TextBitmapComponent* self, PyObject* value);
}