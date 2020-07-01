#include "python/pyComponent.h"
#include "python/pySceneObject.h"

#include "components/Component.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    void  Component_dealloc(PyObject_Component *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* Component_str(PyObject_Component *self)
    {
        return PyUnicode_FromString("C++ Component object");;
    }

    // Get name
    PyObject* Component_getName(PyObject_Component* self)
    {
        return PyUnicode_FromString(self->component->getName().c_str());
    }

    // Get owner
    PyObject* Component_getOwner(PyObject_Component* self)
    {
        if(self->component && self->component->hasOwner())
        {
            auto *obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = self->component->getOwner();
            return (PyObject*)obj;
        }
        Py_RETURN_NONE;
    }

    PyGetSetDef Component_getsets[] = {
        { const_cast<char*>("name"), (getter)Component_getName, NULL, Component_name_doc, NULL },
        { const_cast<char*>("owner"), (getter)Component_getOwner, NULL, Component_owner_doc, NULL },
        { NULL, NULL }
    };

    PyTypeObject PyTypeObject_Component = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.Component",               /* tp_name */
        sizeof(PyObject_Component),         /* tp_basicsize */
        0,                                  /* tp_itemsize */
        (destructor)Component_dealloc,      /* tp_dealloc */
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
        (reprfunc)Component_str,            /* tp_str */
        0,                                  /* tp_getattro */
        0,                                  /* tp_setattro */
        0,                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                 /* tp_flags */
        0,                                  /* tp_doc */
        0,                                  /* tp_traverse */
        0,                                  /* tp_clear */
        0,                                  /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,                                  /* tp_iter */
        0,                                  /* tp_iternext */
        0,                                  /* tp_methods */  //! [IGE]: no methods
        0,                                  /* tp_members */
        Component_getsets,                  /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        0,                                  /* tp_new */  //! [IGE]: abstract class
        0,                                  /* tp_free */
    };
}
