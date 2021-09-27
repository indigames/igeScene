#pragma once

#include <Python.h>

#include "scene/Scene.h"

namespace ige::scene
{
    struct PyObject_Scene
    {
        PyObject_HEAD
        Scene* scene;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_Scene;

    // New/dealloc
    PyObject* Scene_new(PyTypeObject *type, PyObject *args, PyObject *kw);
    void  Scene_dealloc(PyObject_Scene *self);

    // String represent
    PyObject* Scene_str(PyObject_Scene *self);

    // Get name
    PyObject* Scene_getName(PyObject_Scene* self);

    // Set name
    int Scene_setName(PyObject_Scene* self, PyObject* value);

    // Get active camera
    PyObject* Scene_getActiveCamera(PyObject_Scene *self);

    // Set active camera
    int Scene_setActiveCamera(PyObject_Scene *self, PyObject* value);

    // Create object
    PyObject* Scene_createObject(PyObject_Scene *self, PyObject* args);

    // Remove object
    PyObject* Scene_removeObject(PyObject_Scene *self, PyObject* args);

    // Create object from Prefab
    PyObject* Scene_cloneObject(PyObject_Scene* self, PyObject* args);

    // Find object
    PyObject* Scene_findObject(PyObject_Scene *self, PyObject* args);

    // Find object by Name
    PyObject* Scene_findObjectByName(PyObject_Scene* self, PyObject* args);

    // Get all objects
    PyObject* Scene_getObjects(PyObject_Scene *self);

    // Get root object
    PyObject* Scene_getRoot(PyObject_Scene *self);

    // Get path
    PyObject* Scene_getPath(PyObject_Scene *self);

    // Get showcase
    PyObject* Scene_getShowcase(PyObject_Scene *self);

    // Get environment
    PyObject* Scene_getEnvironment(PyObject_Scene *self);

    // Raycast
    PyObject* Scene_raycast(PyObject_Scene *self, PyObject* args);

    PyObject* Scene_raycastFromCamera(PyObject_Scene* self, PyObject* args);

    PyObject* Scene_raycastUI(PyObject_Scene* self, PyObject* args);
}
