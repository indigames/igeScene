#pragma once

#include <Python.h>

#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"
#include "components/ScriptComponent.h"
#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_Script
    {
        PyObject_HEAD
        SceneObject* owner;
        PyObject_Component super;
        ScriptComponent* component;
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

    PyObject* Script_invoke(PyObject_Script* self, PyObject * args);
}