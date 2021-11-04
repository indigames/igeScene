#include "python/pyUITextField.h"
#include "python/pyUITextField_doc_en.h"

#include "components/gui/UITextField.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UITextField_dealloc(PyObject_UITextField *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *UITextField_str(PyObject_UITextField *self)
    {
        return PyUnicode_FromString("C++ UITextField object");
    }

    PyTypeObject PyTypeObject_UITextField = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.UITextField", /* tp_name */
        sizeof(PyObject_UITextField),                          /* tp_basicsize */
        0,                                                     /* tp_itemsize */
        (destructor)UITextField_dealloc,                       /* tp_dealloc */
        0,                                                     /* tp_print */
        0,                                                     /* tp_getattr */
        0,                                                     /* tp_setattr */
        0,                                                     /* tp_reserved */
        0,                                                     /* tp_repr */
        0,                                                     /* tp_as_number */
        0,                                                     /* tp_as_sequence */
        0,                                                     /* tp_as_mapping */
        0,                                                     /* tp_hash */
        0,                                                     /* tp_call */
        (reprfunc)UITextField_str,                             /* tp_str */
        0,                                                     /* tp_getattro */
        0,                                                     /* tp_setattro */
        0,                                                     /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                    /* tp_flags */
        0,                                                     /* tp_doc */
        0,                                                     /* tp_traverse */
        0,                                                     /* tp_clear */
        0,                                                     /* tp_richcompare */
        0,                                                     /* tp_weaklistoffset */
        0,                                                     /* tp_iter */
        0,                                                     /* tp_iternext */
        0,                                                     /* tp_methods */
        0,                                                     /* tp_members */
        0,                                                     /* tp_getset */
        &PyTypeObject_UIText,                                  /* tp_base */
        0,                                                     /* tp_dict */
        0,                                                     /* tp_descr_get */
        0,                                                     /* tp_descr_set */
        0,                                                     /* tp_dictoffset */
        0,                                                     /* tp_init */
        0,                                                     /* tp_alloc */
        0,                                                     /* tp_new */
        0,                                                     /* tp_free */
    };
} // namespace ige::scene
