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
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* Component_str(PyObject_Component *self)
    {
        return PyUnicode_FromString("C++ Component object");
    }

    // Get name
    PyObject* Component_getName(PyObject_Component* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(self->component.lock()->getName().c_str());
    }

    // Get owner
    PyObject* Component_getOwner(PyObject_Component* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto *obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
        // TODO: optimize
        obj->sceneObject = self->component.lock()->getOwner()->getSharedPtr();
        return (PyObject*)obj;
    }

    // Compare function
    static PyObject* Component_richcompare(PyObject* self, PyObject* other, int op)
    {
        if (op == Py_LT || op == Py_LE || op == Py_GT || op == Py_GE)
        {
            PyErr_Format(PyExc_TypeError, "Only '==' and '!=' are allowed as comparison operators");
            Py_RETURN_NOTIMPLEMENTED;
        }

        PyObject* result = Py_False;
        if (self != Py_None && other != Py_None)
        {
            if (other->ob_type == &PyTypeObject_Component)
            {
                auto selfCmp = (PyObject_Component*)(self);
                auto otherCmp = (PyObject_Component*)(other);
                if (selfCmp->component.expired() || otherCmp->component.expired()) {
                    result = Py_False;
                }
                else {
                    bool eq = (selfCmp->component.lock()->getInstanceId() == otherCmp->component.lock()->getInstanceId());
                    if (op == Py_NE)
                        eq = !eq;
                    result = eq ? Py_True : Py_False;
                }                
            }
            else
            {
                result = (op == Py_EQ) ? Py_False : Py_True;
            }
        }
        else if (self == Py_None && other == Py_None)
        {
            result = (op == Py_EQ) ? Py_True : Py_False;
        }
        else
        {
            result = (op == Py_EQ) ? Py_False : Py_True;
        }
        Py_INCREF(result);
        return result;
    }

    PyObject* Component_onUpdate(PyObject_Component* self, PyObject* args)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* obj = nullptr;
        if (PyArg_ParseTuple(args, "O", &obj))
        {
            if (obj)
            {
                if (PyNumber_Check(obj))
                {
                    float dt = PyFloat_AsDouble(obj);
                    self->component.lock()->onUpdate(dt);
                    Py_RETURN_TRUE;
                }
            }
        }
        Py_RETURN_FALSE;
    }

    PyObject* Component_isEnabled(PyObject_Component* self) {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Component>(self->component.lock())->isEnabled());
    }

    int Component_setEnabled(PyObject_Component* self, PyObject* value) {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto isActive = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<Component>(self->component.lock())->setEnabled(isActive);
            return 0;
        }
        return -1;
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
        { "enable", (getter)Component_isEnabled, (setter)Component_setEnabled, Component_enable_doc, NULL },
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
