#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/EnvironmentComponent.h"

#include "python/pyComponent.h"
#include "python/pyEnvironmentComponent_doc_en.h"

namespace ige::scene
{
    struct PyObject_EnvironmentComponent
    {
		PyObject_Component super;
		EnvironmentComponent* component;
	};

    // Type declaration
    PyTypeObject PyTypeObject_EnvironmentComponent;

    // Dealloc
    void  EnvironmentComponent_dealloc(PyObject_EnvironmentComponent *self);

    // String represent
    PyObject* EnvironmentComponent_str(PyObject_EnvironmentComponent *self);

    // Get name
    PyObject* EnvironmentComponent_getName(PyObject_EnvironmentComponent* self);

}
