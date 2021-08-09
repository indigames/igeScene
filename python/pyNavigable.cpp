#include "python/pyNavigable.h"
#include "python/pyNavigable_doc_en.h"

#include "components/navigation/Navigable.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void Navigable_dealloc(PyObject_Navigable *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *Navigable_str(PyObject_Navigable *self)
    {
        return PyUnicode_FromString("C++ Navigable object");
    }

    // Recursive
    PyObject *Navigable_isRecursive(PyObject_Navigable *self)
    {
        return PyBool_FromLong(self->component->isRecursive());
    }

    int Navigable_setRecursive(PyObject_Navigable *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setRecursive(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef Navigable_getsets[] = {
        {"recursive", (getter)Navigable_isRecursive, (setter)Navigable_setRecursive, Navigable_recursive_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_Navigable = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.Navigable", /* tp_name */
        sizeof(PyObject_Navigable),                          /* tp_basicsize */
        0,                                                   /* tp_itemsize */
        (destructor)Navigable_dealloc,                       /* tp_dealloc */
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
        (reprfunc)Navigable_str,                             /* tp_str */
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
        Navigable_getsets,                                   /* tp_getset */
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
