#include "python/pyAnimator.h"
#include "python/pyAnimator_doc_en.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void Animator_dealloc(PyObject_Animator *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *Animator_str(PyObject_Animator *self)
    {
        return PyUnicode_FromString("C++ Animator object");
    }

	// Controller path
    PyObject *Animator_getControllerPath(PyObject_Animator *self)
	{
		if (self->component.expired()) Py_RETURN_NONE;
		return PyUnicode_FromString(std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->getControllerPath().c_str());
	}
	
    int Animator_setControllerPath(PyObject_Animator *self, PyObject *value)
	{
		if (self->component.expired()) return -1;
		if (PyUnicode_Check(value)) {
            const char* val = PyUnicode_AsUTF8(value);
            std::dynamic_pointer_cast<AnimatorComponent>(self->component.lock())->setControllerPath(std::string(val));
            return 0;
        }
        return -1;
	}

    PyGetSetDef Animator_getsets[] = {
        {"controllerPath", (getter)Animator_getControllerPath, (setter)Animator_setControllerPath, Animator_controllerPath_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_Animator = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.Animator", /* tp_name */
        sizeof(PyObject_Animator),                          /* tp_basicsize */
        0,                                                   /* tp_itemsize */
        (destructor)Animator_dealloc,                       /* tp_dealloc */
        0,                                                   /* tp_print */
        0,                                                   /* tp_getattr */
        0,                                                   /* tp_setattr */
        0,                                                   /* tp_reserved */
        0,                                                   /* tp_repr */
        0,                                                   /* tp_as_number */
        0,                                                   /* tp_as_sequence */
        0,                                                   /* tp_as_mapping */
        0,                                                   /* tp_hash */
        0,                                                   /* tp_call */
        (reprfunc)Animator_str,                             /* tp_str */
        0,                                                   /* tp_getattro */
        0,                                                   /* tp_setattro */
        0,                                                   /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                  /* tp_flags */
        0,                                                   /* tp_doc */
        0,                                                   /* tp_traverse */
        0,                                                   /* tp_clear */
        0,                                                   /* tp_richcompare */
        0,                                                   /* tp_weaklistoffset */
        0,                                                   /* tp_iter */
        0,                                                   /* tp_iternext */
        0,                                                   /* tp_methods */
        0,                                                   /* tp_members */
        Animator_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                             /* tp_base */
        0,                                                   /* tp_dict */
        0,                                                   /* tp_descr_get */
        0,                                                   /* tp_descr_set */
        0,                                                   /* tp_dictoffset */
        0,                                                   /* tp_init */
        0,                                                   /* tp_alloc */
        0,                                                   /* tp_new */
        0,                                                   /* tp_free */
    };
} // namespace ige::scene
