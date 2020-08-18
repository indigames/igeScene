#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/gui/UIImage.h"

#include "python/pySpriteComponent.h"

namespace ige::scene
{
    struct PyObject_UIImage
    {
        PyObject_SpriteComponent super;
        std::shared_ptr<UIImage> component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_UIImage;

    // Dealloc
    void  UIImage_dealloc(PyObject_UIImage *self);

    // String represent
    PyObject* UIImage_str(PyObject_UIImage *self);
}