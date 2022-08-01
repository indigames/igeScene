#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UISlider.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_UISlider : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_UISlider;

    // Dealloc
    void  UISlider_dealloc(PyObject_UISlider*self);

    // String represent
    PyObject* UISlider_str(PyObject_UISlider*self);

    //Methods
    PyObject* UISlider_setFillObject(PyObject_UISlider* self, PyObject* value);

    PyObject* UISlider_setHandleObject(PyObject_UISlider* self, PyObject* value);

    //Get set
    PyObject* UISlider_getMin(PyObject_UISlider* self);
    int UISlider_setMin(PyObject_UISlider* self, PyObject* value);

    PyObject* UISlider_getMax(PyObject_UISlider* self);
    int UISlider_setMax(PyObject_UISlider* self, PyObject* value);

    PyObject* UISlider_getValue(PyObject_UISlider* self);
    int UISlider_setValue(PyObject_UISlider* self, PyObject* value);

    PyObject* UISlider_getWholeNumbers(PyObject_UISlider* self);
    int UISlider_setWholeNumbers(PyObject_UISlider* self, PyObject* value);

    // Get color
    PyObject* UISlider_getColor(PyObject_UISlider* self);

    // Set color
    int UISlider_setColor(PyObject_UISlider* self, PyObject* value);

    // Get Press color
    PyObject* UISlider_getPressColor(PyObject_UISlider* self);

    // Set Press color
    int UISlider_setPressColor(PyObject_UISlider* self, PyObject* value);

    // Get Disable color
    PyObject* UISlider_getDisableColor(PyObject_UISlider* self);

    // Set Interactable
    PyObject* UISlider_getInteractable(PyObject_UISlider* self);

    // Get Interactable
    int UISlider_setInteractable(PyObject_UISlider* self, PyObject* value);

    // Set Direction
    PyObject* UISlider_getDirection(PyObject_UISlider* self);

    // Get Direction
    int UISlider_setDirection(PyObject_UISlider* self, PyObject* value);

    // Set Fade Duration
    PyObject* UISlider_getFadeDuration(PyObject_UISlider* self);

    // Get Fade Duration
    int UISlider_setFadeDuration(PyObject_UISlider* self, PyObject* value);
}