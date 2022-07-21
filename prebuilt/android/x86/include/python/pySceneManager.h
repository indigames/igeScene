#pragma once

#include <Python.h>

#include "components/Component.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    struct PyObject_SceneManager
    {
        PyObject_HEAD
        SceneManager* sceneManager;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_SceneManager;

    // Dealloc
    void  SceneManager_dealloc(PyObject_SceneManager *self);

    // String represent
    PyObject* SceneManager_str(PyObject_SceneManager *self);

    // Get instance
    PyObject* SceneManager_getInstance();

    // Create scene
    PyObject* SceneManager_createScene(PyObject_SceneManager* self, PyObject* value);

    // Load scene
    PyObject* SceneManager_loadScene(PyObject_SceneManager* self, PyObject* value);

    // Unload scene
    PyObject* SceneManager_unloadScene(PyObject_SceneManager* self, PyObject* value);

    // Save scene
    PyObject* SceneManager_saveScene(PyObject_SceneManager* self, PyObject* value);

    // Get current scene
    PyObject* SceneManager_getCurrentScene(PyObject_SceneManager* self);

    // Set current scene
    int SceneManager_setCurrentScene(PyObject_SceneManager* self, PyObject* value);
}
