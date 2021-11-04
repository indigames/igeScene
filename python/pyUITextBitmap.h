#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UITextBitmap.h"

#include "python/pySpriteComponent.h"
#include "python/pyUIText.h"

namespace ige::scene
{
    struct PyObject_UITextBitmap : PyObject_UIText {};

    // Type declaration
    extern PyTypeObject PyTypeObject_UITextBitmap;

    // Dealloc
    void  UITextBitmap_dealloc(PyObject_UITextBitmap *self);

    // String represent
    PyObject* UITextBitmap_str(PyObject_UITextBitmap *self);

    // Get font path
    PyObject* UITextBitmap_getFontPath(PyObject_UITextBitmap* self);

    // Set font path
    int UITextBitmap_setFontPath(PyObject_UITextBitmap* self, PyObject* value);

    // Get text
    PyObject* UITextBitmap_getText(PyObject_UITextBitmap* self);

    // Set text
    int UITextBitmap_setText(PyObject_UITextBitmap* self, PyObject* value);

    // Get font size
    PyObject* UITextBitmap_getFontSize(PyObject_UITextBitmap* self);

    // Set font size
    int UITextBitmap_setFontSize(PyObject_UITextBitmap* self, PyObject* value);

    // Get color
    PyObject* UITextBitmap_getColor(PyObject_UITextBitmap* self);

    // Set color
    int UITextBitmap_setColor(PyObject_UITextBitmap* self, PyObject* value);

}
