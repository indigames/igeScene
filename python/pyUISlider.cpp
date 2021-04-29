#include "python/pyUISlider.h"
#include "python/pyUISlider_doc_en.h"

#include "components/gui/UISlider.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UISlider_dealloc(PyObject_UISlider *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject* UISlider_str(PyObject_UISlider*self)
    {
        return PyUnicode_FromString("C++ UISlider object");
    }

    PyTypeObject PyTypeObject_UISlider = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.UISlider",                          /* tp_name */
        sizeof(PyObject_UISlider),                    /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)UISlider_dealloc,                 /* tp_dealloc */
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
        (reprfunc)UISlider_str,                       /* tp_str */
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
        0,                                           /* tp_getset */
        & PyTypeObject_Component,                    /* tp_base */  //! [IGE]: inheritance
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