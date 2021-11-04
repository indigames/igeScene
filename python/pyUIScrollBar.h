#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIScrollBar.h"

#include "python/pyUIImage.h"

namespace ige::scene
{
    struct PyObject_UIScrollBar : PyObject_UIImage {};

    // Type declaration
    extern PyTypeObject PyTypeObject_UIScrollBar;

    // Dealloc
    void  UIScrollBar_dealloc(PyObject_UIScrollBar *self);

    // String represent
    PyObject* UIScrollBar_str(PyObject_UIScrollBar *self);

    //Methods
    PyObject* UIScrollBar_setHandle(PyObject_UIScrollBar* self, PyObject* value);


    //Get Set
    PyObject* UIScrollBar_getValue(PyObject_UIScrollBar* self);
    int UIScrollBar_setValue(PyObject_UIScrollBar* self, PyObject* value);

    PyObject* UIScrollBar_getSize(PyObject_UIScrollBar* self);
    int UIScrollBar_setSize(PyObject_UIScrollBar* self, PyObject* value);

    // Set Direction
    PyObject* UIScrollBar_getDirection(PyObject_UIScrollBar* self);
    // Get Direction
    int UIScrollBar_setDirection(PyObject_UIScrollBar* self, PyObject* value);

    // Get color
    PyObject* UIScrollBar_getColor(PyObject_UIScrollBar* self);
    // Set color
    int UIScrollBar_setColor(PyObject_UIScrollBar* self, PyObject* value);

    // Get Press color
    PyObject* UIScrollBar_getPressColor(PyObject_UIScrollBar* self);
    // Set Press color
    int UIScrollBar_setPressColor(PyObject_UIScrollBar* self, PyObject* value);

    // Get Disable color
    PyObject* UIScrollBar_getDisableColor(PyObject_UIScrollBar* self);
    // Set Disable color
    int UIScrollBar_setDisableColor(PyObject_UIScrollBar* self, PyObject* value);

    // Set Fade Duration
    PyObject* UIScrollBar_getFadeDuration(PyObject_UIScrollBar* self);
    // Get Fade Duration
    int UIScrollBar_setFadeDuration(PyObject_UIScrollBar* self, PyObject* value);
}