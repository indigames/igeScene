#pragma once

#include <Python.h>

#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    struct PyObject_Script
    {
        PyObject_HEAD
        std::weak_ptr<SceneObject> owner;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_Script;

    // New/dealloc
    PyObject* Script_new(PyTypeObject *type, PyObject *args, PyObject *kw);
    void Script_dealloc(PyObject_Script *self);

    // Init
    int Script_init(PyObject_Script *self, PyObject *args, PyObject *kw);

    // String represent
    PyObject* Script_str(PyObject_Script *self);

    // Get get owner
    PyObject* Script_getOwner(PyObject_Script* self);
}