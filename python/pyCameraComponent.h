#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/CameraComponent.h"

#include "python/pyComponent.h"
#include "python/pyCameraComponent_doc_en.h"

namespace ige::scene
{
    struct PyObject_CameraComponent
    {
		PyObject_Component super;
		CameraComponent* component;
	};

    // Type declaration
    PyTypeObject PyTypeObject_CameraComponent;

    // Dealloc
    void  CameraComponent_dealloc(PyObject_CameraComponent *self);

    // String represent
    PyObject* CameraComponent_str(PyObject_CameraComponent *self);

    // Get name
    PyObject* CameraComponent_getName(PyObject_CameraComponent* self);

}
