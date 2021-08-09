#include "python/pyScript.h"
#include "python/pyScript_doc_en.h"

#include "python/pySceneObject.h"

namespace ige::scene
{
    // Allocation
    PyObject* Script_new(PyTypeObject* type, PyObject* args, PyObject* kw)
    {
        PyObject* owner;
        PyObject_Script* self = nullptr;
        if (PyArg_ParseTuple(args, "|O", &owner)) {
            self = (PyObject_Script*)type->tp_alloc(type, 0);
            self->owner = owner ? ((PyObject_SceneObject*)owner)->sceneObject : nullptr;
        }
        return (PyObject*)self;
    }

    // Deallocation
    void  Script_dealloc(PyObject_Script *self)
    {
        if(self)
        {
            self->owner = nullptr;
        }
        Py_TYPE(self)->tp_free(self);
    }

    // Init
    int Script_init(PyObject_Script* self, PyObject* args, PyObject* kw)
    {
        return 0;
    }

    // String representation
    PyObject* Script_str(PyObject_Script *self)
    {
        return PyUnicode_FromString("C++ Script object");
    }

    // Get get owner
    PyObject* Script_getOwner(PyObject_Script* self)
    {
        auto *obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
        obj->sceneObject = self->owner;
        return (PyObject*)obj;
    }

    // Get/Set
    PyGetSetDef Script_getsets[] = {
        { "owner", (getter)Script_getOwner, NULL, Script_owner_doc, NULL },
        { NULL, NULL }
    };

    // Type declaration
    PyTypeObject PyTypeObject_Script = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.Script",                  /* tp_name */
        sizeof(PyObject_Script),            /* tp_basicsize */
        0,                                  /* tp_itemsize */
        (destructor)Script_dealloc,         /* tp_dealloc */
        0,                                  /* tp_print */
        0,                                  /* tp_getattr */
        0,                                  /* tp_setattr */
        0,                                  /* tp_reserved */
        0,                                  /* tp_repr */
        0,                                  /* tp_as_number */
        0,                                  /* tp_as_sequence */
        0,                                  /* tp_as_mapping */
        0,                                  /* tp_hash */
        0,                                  /* tp_call */
        (reprfunc)Script_str,               /* tp_str */
        0,                                  /* tp_getattro */
        0,                                  /* tp_setattro */
        0,                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
        0,                                  /* tp_doc */
        0,                                  /* tp_traverse */
        0,                                  /* tp_clear */
        0,                                  /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,                                  /* tp_iter */
        0,                                  /* tp_iternext */
        0,                                  /* tp_methods */
        0,                                  /* tp_members */
        Script_getsets,                     /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        (initproc)Script_init,              /* tp_init */
        0,                                  /* tp_alloc */
        Script_new,                         /* tp_new */
        0,                                  /* tp_free */
    };
}
