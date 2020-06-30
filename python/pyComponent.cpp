#include "python/pyComponent.h"

#include "components/Component.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
	void  Component_dealloc(PyObject_Component *self)
	{
        if(self && self->component)
        {
            delete self->component;
            self->component = nullptr;
            Py_TYPE(self)->tp_free(self);
        }		
	}

	PyObject* Component_str(PyObject_Component *self)
	{
		char buf[64];
		pyxie_snprintf(buf, 64, "Component object");
		return _PyUnicode_FromASCII(buf, strlen(buf));
	}

    // Get name
    PyObject* Component_getName(PyObject_Component* self)
    {
        Py_RETURN_NONE;
    }

    // Set name
    int Component_setName(PyObject_Component* self, PyObject* value)
    {
        return 0;
    }

    // Get owner
    PyObject* Component_getOwner(PyObject_Component* self)
    {
        Py_RETURN_NONE;
    }

    PyMethodDef Scene_methods[] = {
        // { "initialize", (PyCFunction)Component_initialize, METH_NOARGS, initialize_doc},
        { NULL,    NULL }
    };

    PyGetSetDef Scene_getsets[] = {
        { const_cast<char*>("name"), (getter)Component_getName, (setter)Component_setName, component_name_doc, NULL },
        { NULL, NULL }
    };

    PyTypeObject pyComponentType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.Component",               /* tp_name */
        sizeof(PyObject_Component),            /* tp_basicsize */
        0,                                  /* tp_itemsize */
        (destructor)Component_dealloc,    /* tp_dealloc */
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
        (reprfunc)Component_str,          /* tp_str */
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
        Scene_methods,                    /* tp_methods */
        0,                                  /* tp_members */
        Scene_getsets,                    /* tp_getset */
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
