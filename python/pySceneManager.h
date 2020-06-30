#pragma once

#include <Python.h>

#include "components/Component.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    struct PyObject_SceneManager
    {
		PyObject_HEAD
		std::shared_ptr<SceneManager> sceneManager;
	};

    // Type declaration
    PyTypeObject PyTypeObject_SceneManager;

    // Dealloc    
    void  SceneManager_dealloc(PyObject_SceneManager *self);

    // String represent
    PyObject* SceneManager_str(PyObject_SceneManager *self);

    // Get instance
    PyObject* SceneManager_getInstance();

    // Load scene
    PyObject* SceneManager_loadScene(PyObject_SceneManager* self, PyObject* value);

    // Unload scene
    PyObject* SceneManager_unloadScene(PyObject_SceneManager* self, PyObject* value);

    // Get current scene
    PyObject* SceneManager_getCurrentScene(PyObject_SceneManager* self);

    // Set current scene
    PyObject* SceneManager_setCurrentScene(PyObject_SceneManager* self, PyObject* value);
}
