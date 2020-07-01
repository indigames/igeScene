#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/TransformComponent.h"

#include "python/pyComponent.h"
#include "python/pyTransformComponent_doc_en.h"

namespace ige::scene
{
    struct PyObject_TransformComponent
    {
		PyObject_Component super;
		std::shared_ptr<TransformComponent> component;
	};

    // Type declaration
    PyTypeObject PyTypeObject_TransformComponent;

    // Dealloc
    void  TransformComponent_dealloc(PyObject_TransformComponent *self);

    // String represent
    PyObject* TransformComponent_str(PyObject_TransformComponent *self);

    // Get position
    PyObject* TransformComponent_getPosition(PyObject_TransformComponent* self);

    // Set position
    int TransformComponent_setPosition(PyObject_TransformComponent* self, PyObject* value);

    // Get rotation
    PyObject* TransformComponent_getRotation(PyObject_TransformComponent* self);

    // Set rotation
    int TransformComponent_setRotation(PyObject_TransformComponent* self, PyObject* value);

    // Get scale
    PyObject* TransformComponent_getScale(PyObject_TransformComponent* self);

    // Set scale
    int TransformComponent_setScale(PyObject_TransformComponent* self, PyObject* value);

    // Get local transform matrix
    PyObject* TransformComponent_getLocalMatrix(PyObject_TransformComponent* self);

    // Get world position
    PyObject* TransformComponent_getWorldPosition(PyObject_TransformComponent* self);

    // Set world position
    int TransformComponent_setWorldPosition(PyObject_TransformComponent* self, PyObject* value);

    // Get world rotation
    PyObject* TransformComponent_getWorldRotation(PyObject_TransformComponent* self);

    // Set world rotation
    int TransformComponent_setWorldRotation(PyObject_TransformComponent* self, PyObject* value);

    // Get world scale
    PyObject* TransformComponent_getWorldScale(PyObject_TransformComponent* self);

    // Set world scale
    int TransformComponent_setWorldScale(PyObject_TransformComponent* self, PyObject* value);

    // Get world transform matrix
    PyObject* TransformComponent_getWorldMatrix(PyObject_TransformComponent* self);
}