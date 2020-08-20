#pragma once

#include <Python.h>

#include "components/Component.h"

namespace ige::scene
{
    struct PyObject_SceneObject
    {
        PyObject_HEAD
        SceneObject* sceneObject;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_SceneObject;

    // Dealloc
    void  SceneObject_dealloc(PyObject_SceneObject *self);

    // String represent
    PyObject* SceneObject_str(PyObject_SceneObject *self);

    // Get ID
    PyObject* SceneObject_getId(PyObject_SceneObject* self);

    // Get name
    PyObject* SceneObject_getName(PyObject_SceneObject* self);

    // Set name
    int SceneObject_setName(PyObject_SceneObject* self, PyObject* value);

    // Get active
    PyObject* SceneObject_getActive(PyObject_SceneObject* self);

    // Set active
    int SceneObject_setActive(PyObject_SceneObject* self, PyObject* value);

    // Get selected
    PyObject* SceneObject_getSelected(PyObject_SceneObject* self);

    // Set selected
    int SceneObject_setSelected(PyObject_SceneObject* self, PyObject* value);

    // Get parent
    PyObject* SceneObject_getParent(PyObject_SceneObject* self);

    // Set parent
    int SceneObject_setParent(PyObject_SceneObject* self, PyObject* value);

    // Add child
    PyObject* SceneObject_addChild(PyObject_SceneObject* self, PyObject* value);

    // Remove child
    PyObject* SceneObject_removeChild(PyObject_SceneObject* self, PyObject* value);

    // Get children
    PyObject* SceneObject_getChildren(PyObject_SceneObject* self);

    // Remove children
    PyObject* SceneObject_removeChildren(PyObject_SceneObject* self);

    // Add component
    PyObject* SceneObject_addComponent(PyObject_SceneObject* self, PyObject* value);

    // Remove component
    PyObject* SceneObject_removeComponent(PyObject_SceneObject* self, PyObject* value);

    // Get component by type
    PyObject* SceneObject_getComponent(PyObject_SceneObject* self, PyObject* value);

    // Get components
    PyObject* SceneObject_getComponents(PyObject_SceneObject* self);

    // Remove components
    PyObject* SceneObject_removeComponents(PyObject_SceneObject* self);
}
