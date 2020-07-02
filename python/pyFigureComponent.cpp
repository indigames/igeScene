#include "python/pyFigureComponent.h"
#include "python/pyFigureComponent_doc_en.h"

#include "components/FigureComponent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void FigureComponent_dealloc(PyObject_FigureComponent *self)
    {
        if(self && self->component)
        {
            self->super.component = nullptr;
            self->component = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* FigureComponent_str(PyObject_FigureComponent *self)
    {
        return PyUnicode_FromString("C++ FigureComponent object");
    }
    
    // Get path
    PyObject* FigureComponent_getPath(PyObject_FigureComponent* self)
    {
        return PyUnicode_FromString(self->component->getPath().c_str());
    }

    // Set path
    int FigureComponent_setPath(PyObject_FigureComponent* self, PyObject* value)
    {
        char* val;
        if (PyArg_ParseTuple(value, "s", &val))
        {
            self->component->setPath(std::string(val));
            return 0;
        }
        return -1;
    }

    PyGetSetDef FigureComponent_getsets[] = {
        { "path", (getter)FigureComponent_getPath, (setter)FigureComponent_setPath, FigureComponent_path_doc, NULL },
        { NULL, NULL }
    };

    PyTypeObject PyTypeObject_FigureComponent = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.FigureComponent",                  /* tp_name */
        sizeof(PyObject_FigureComponent),            /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)FigureComponent_dealloc,         /* tp_dealloc */
        0,                                           /* tp_print */
        0,                                           /* tp_getattr */
        0,                                           /* tp_setattr */
        0,                                           /* tp_reserved */
        0,                                           /* tp_repr */
        0,                                           /* tp_as_number */
        0,                                           /* tp_as_sequence */
        0,                                           /* tp_as_mapping */
        0,                                           /* tp_hash */
        0,                                           /* tp_call */
        (reprfunc)FigureComponent_str,               /* tp_str */
        0,                                           /* tp_getattro */
        0,                                           /* tp_setattro */
        0,                                           /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                          /* tp_flags */
        0,                                           /* tp_doc */
        0,                                           /* tp_traverse */
        0,                                           /* tp_clear */
        0,                                           /* tp_richcompare */
        0,                                           /* tp_weaklistoffset */
        0,                                           /* tp_iter */
        0,                                           /* tp_iternext */
        0,                                           /* tp_methods */  //! [IGE]: no methods
        0,                                           /* tp_members */
        FigureComponent_getsets,                     /* tp_getset */
        &PyTypeObject_Component,                     /* tp_base */  //! [IGE]: inheritance
        0,                                           /* tp_dict */
        0,                                           /* tp_descr_get */
        0,                                           /* tp_descr_set */
        0,                                           /* tp_dictoffset */
        0,                                           /* tp_init */
        0,                                           /* tp_alloc */
        0,                                           /* tp_new */  //! [IGE]: no new
        0,                                           /* tp_free */
    };
}
