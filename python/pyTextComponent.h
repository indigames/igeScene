#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/TextComponent.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_TextComponent : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_TextComponent;

    // Dealloc
    void  TextComponent_dealloc(PyObject_TextComponent*self);

    // String represent
    PyObject* TextComponent_str(PyObject_TextComponent*self);

    // Get text
    PyObject* TextComponent_getText(PyObject_TextComponent* self);

    // Set text
    int TextComponent_setText(PyObject_TextComponent* self, PyObject* value);

    // Get font path
    PyObject* TextComponent_getFontPath(PyObject_TextComponent* self);

    // Set font path
    int TextComponent_setFontPath(PyObject_TextComponent* self, PyObject* value);

    // Get font size
    PyObject* TextComponent_getFontSize(PyObject_TextComponent* self);

    // Set font size
    int TextComponent_setFontSize(PyObject_TextComponent* self, PyObject* value);

    // Get color
    PyObject* TextComponent_getColor(PyObject_TextComponent* self);

    // Set color
    int TextComponent_setColor(PyObject_TextComponent* self, PyObject* value);

    // Billboard setting
    PyObject* TextComponent_isBillboard(PyObject_TextComponent* self);
    int TextComponent_setBillboard(PyObject_TextComponent* self, PyObject* value);
}