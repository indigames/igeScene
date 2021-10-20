#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIButton.h"

#include "python/pyUIImage.h"

namespace ige::scene
{
    struct PyObject_UIButton
    {
        PyObject_UIImage super;
        UIButton* component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_UIButton;

    // Dealloc
    void  UIButton_dealloc(PyObject_UIButton*self);

    // String represent
    PyObject* UIButton_str(PyObject_UIButton* self);

    // Get path
    PyObject* UIButton_getPath(PyObject_UIButton* self);

    // Set path
    int UIButton_setPath(PyObject_UIButton* self, PyObject* value);

    // Get press path
    PyObject* UIButton_getPressPath(PyObject_UIButton* self);

    // Set press path
    int UIButton_setPressPath(PyObject_UIButton* self, PyObject* value);

    // Get selected path
    PyObject* UIButton_getSelectedPath(PyObject_UIButton* self);

    // Set selected path
    int UIButton_setSelectedPath(PyObject_UIButton* self, PyObject* value);

    // Get disable path
    PyObject* UIButton_getDisablePath(PyObject_UIButton* self);

    // Set disable path
    int UIButton_setDisablePath(PyObject_UIButton* self, PyObject* value);


    // Get color
    PyObject* UIButton_getColor(PyObject_UIButton* self);

    // Set color
    int UIButton_setColor(PyObject_UIButton* self, PyObject* value);

    // Get Press color
    PyObject* UIButton_getPressColor(PyObject_UIButton* self);

    // Set Press color
    int UIButton_setPressColor(PyObject_UIButton* self, PyObject* value);

    // Get Selected color
    PyObject* UIButton_getSelectedColor(PyObject_UIButton* self);

    // Set Selected color
    int UIButton_setSelectedColor(PyObject_UIButton* self, PyObject* value);

    // Get Disable color
    PyObject* UIButton_getDisableColor(PyObject_UIButton* self);

    // Set Disable color
    int UIButton_setDisableColor(PyObject_UIButton* self, PyObject* value);

    // Get Transition Mode
    PyObject* UIButton_getTransitionMode(PyObject_UIButton* self);

    // Set Transition Mode
    int UIButton_setTransitionMode(PyObject_UIButton* self, PyObject* value);

    // Get Fade Duration
    PyObject* UIButton_getFadeDuration(PyObject_UIButton* self);

    // Set Fade Duration
    int UIButton_setFadeDuration(PyObject_UIButton* self, PyObject* value);
}