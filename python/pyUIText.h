#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIText.h"

#include "python/pySpriteComponent.h"

namespace ige::scene
{
    struct PyObject_UIText
    {
        PyObject_Component super;
        UIText* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_UIText;

    // Dealloc
    void  UIText_dealloc(PyObject_UIText *self);

    // String represent
    PyObject* UIText_str(PyObject_UIText *self);

    // Get text
    PyObject* UIText_getText(PyObject_UIText* self);

    // Set text
    int UIText_setText(PyObject_UIText* self, PyObject* value);

    // Get font path
    PyObject* UIText_getFontPath(PyObject_UIText* self);

    // Set font path
    int UIText_setFontPath(PyObject_UIText* self, PyObject* value);

    // Get font size
    PyObject* UIText_getFontSize(PyObject_UIText* self);

    // Set font size
    int UIText_setFontSize(PyObject_UIText* self, PyObject* value);

    // Get color
    PyObject* UIText_getColor(PyObject_UIText* self);

    // Set color
    int UIText_setColor(PyObject_UIText* self, PyObject* value);
}
