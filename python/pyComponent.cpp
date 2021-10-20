#include "python/pyComponent.h"
#include "python/pyComponent_doc_en.h"

#include "python/pySceneObject.h"

#include "components/Component.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    void  Component_dealloc(PyObject_Component *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject* Component_str(PyObject_Component *self)
    {
        return PyUnicode_FromString("C++ Component object");
    }

    // Get name
    PyObject* Component_getName(PyObject_Component* self)
    {
        return PyUnicode_FromString(self->component->getName().c_str());
    }

    // Get owner
    PyObject* Component_getOwner(PyObject_Component* self)
    {
        auto *obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
        obj->sceneObject = self->component->getOwner();
        return (PyObject*)obj;
    }

    // Compare function
    static PyObject* Component_richcompare(PyObject* self, PyObject* other, int op)
    {
        if (op == Py_LT || op == Py_LE || op == Py_GT || op == Py_GE)
        {
            return Py_NotImplemented;
        }

        if (self != Py_None && other != Py_None)
        {
            if (other->ob_type == &PyTypeObject_Component)
            {
                auto selfCmp = (PyObject_Component*)(self);
                auto otherCmp = (PyObject_Component*)(other);
                bool eq = (selfCmp->component == otherCmp->component);
                if (op == Py_NE)
                    eq = !eq;
                return eq ? Py_True : Py_False;
            }
            else
            {
                return (op == Py_EQ) ? Py_False : Py_True;
            }
        }
        else if (self == Py_None && other == Py_None)
        {
            return (op == Py_EQ) ? Py_True : Py_False;
        }
        else
        {
            return (op == Py_EQ) ? Py_False: Py_True;
        }
    }

    PyObject* Component_onUpdate(PyObject_Component* self, PyObject* args)
    {
        PyObject* obj = nullptr;
        if (PyArg_ParseTuple(args, "O", &obj))
        {
            if (obj)
            {
                if (PyNumber_Check(obj))
                {
                    float dt = PyFloat_AsDouble(obj);
                    self->component->onUpdate(dt);
                    Py_RETURN_TRUE;
                }
            }
        }
        Py_RETURN_FALSE;
    }

    // Methods definition
    PyMethodDef Component_methods[] = {
        { "onUpdate", (PyCFunction)Component_onUpdate, METH_VARARGS, Component_onUpdate_doc },
        { NULL, NULL }
    };

    // Variable definition
    PyGetSetDef Component_getsets[] = {
        { "name", (getter)Component_getName, NULL, Component_name_doc, NULL },
        { "owner", (getter)Component_getOwner, NULL, Component_owner_doc, NULL },
        { NULL, NULL }
    };

    // Type definition
    PyTypeObject PyTypeObject_Component = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.Component",                       /* tp_name */
        sizeof(PyObject_Component),                 /* tp_basicsize */
        0,                                          /* tp_itemsize */
        (destructor)Component_dealloc,              /* tp_dealloc */
        0,                                          /* tp_print */
        0,                                          /* tp_getattr */
        0,                                          /* tp_setattr */
        0,                                          /* tp_reserved */
        0,                                          /* tp_repr */
        0,                                          /* tp_as_number */
        0,                                          /* tp_as_sequence */
        0,                                          /* tp_as_mapping */
        0,                                          /* tp_hash */
        0,                                          /* tp_call */
        (reprfunc)Component_str,                    /* tp_str */
        0,                                          /* tp_getattro */
        0,                                          /* tp_setattro */
        0,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
        0,                                          /* tp_doc */
        0,                                          /* tp_traverse */
        0,                                          /* tp_clear */
        Component_richcompare,                      /* tp_richcompare */
        0,                                          /* tp_weaklistoffset */
        0,                                          /* tp_iter */
        0,                                          /* tp_iternext */
        Component_methods,                          /* tp_methods */
        0,                                          /* tp_members */
        Component_getsets,                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,                                          /* tp_dictoffset */
        0,                                          /* tp_init */
        0,                                          /* tp_alloc */
        0,                                          /* tp_new */
        0,                                          /* tp_free */
    };
}
