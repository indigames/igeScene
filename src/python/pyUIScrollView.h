#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIScrollView.h"

#include "python/pyUIImage.h"

namespace ige::scene
{
    struct PyObject_UIScrollView : PyObject_UIImage {};

    // Type declaration
    extern PyTypeObject PyTypeObject_UIScrollView;

    // Dealloc
    void  UIScrollView_dealloc(PyObject_UIScrollView *self);

    // String represent
    PyObject* UIScrollView_str(PyObject_UIScrollView *self);

    //Methods
    PyObject* UIScrollView_setContent(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_setViewport(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_scrollTo(PyObject_UIScrollView* self, PyObject* value);

    //Get set
    PyObject* UIScrollView_getEnableHorizontal(PyObject_UIScrollView* self);
    int UIScrollView_setEnableHorizontal(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_getEnableVertical(PyObject_UIScrollView* self);
    int UIScrollView_setEnableVertical(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_getHorizontalScrollBar(PyObject_UIScrollView* self);
    int UIScrollView_setHorizontalScrollBar(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_getVerticalScrollBar(PyObject_UIScrollView* self);
    int UIScrollView_setVerticalScrollBar(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_getMovementType(PyObject_UIScrollView* self);
    int UIScrollView_setMovementType(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_getElasticExtra(PyObject_UIScrollView* self);
    int UIScrollView_setElasticExtra(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_getElasticity(PyObject_UIScrollView* self);
    int UIScrollView_setElasticity(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_getIsInertia(PyObject_UIScrollView* self);
    int UIScrollView_setInertia(PyObject_UIScrollView* self, PyObject* value);

    PyObject* UIScrollView_getDecelerationRate(PyObject_UIScrollView* self);
    int UIScrollView_setDecelerationRate(PyObject_UIScrollView* self, PyObject* value);

}