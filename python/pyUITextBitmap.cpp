#include "python/pyUITextBitmap.h"
#include "python/pyUITextBitmap_doc_en.h"

#include "components/gui/UIText.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UITextBitmap_dealloc(PyObject_UITextBitmap *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *UITextBitmap_str(PyObject_UITextBitmap *self)
    {
        return PyUnicode_FromString("C++ UITextBitmap object");
    }

    // Get font path
    PyObject *UITextBitmap_getFontPath(PyObject_UITextBitmap *self)
    {
        return PyUnicode_FromString(self->component->getFontPath().c_str());
    }

    // Set font path
    int UITextBitmap_setFontPath(PyObject_UITextBitmap *self, PyObject *value)
    {
        char *val;
        if (PyArg_ParseTuple(value, "s", &val))
        {
            self->component->setFontPath(std::string(val));
            return 0;
        }
        return -1;
    }

    // Get/set
    PyGetSetDef UITextBitmap_getsets[] = {
        {"fontPath", (getter)UIText_getFontPath, (setter)UIText_setFontPath, UIText_fontPath_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_UITextBitmap = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.UITextBitmap", /* tp_name */
        sizeof(PyObject_UITextBitmap),                          /* tp_basicsize */
        0,                                                /* tp_itemsize */
        (destructor)UITextBitmap_dealloc,                       /* tp_dealloc */
        0,                                                /* tp_print */
        0,                                                /* tp_getattr */
        0,                                                /* tp_setattr */
        0,                                                /* tp_reserved */
        0,                                                /* tp_repr */
        0,                                                /* tp_as_number */
        0,                                                /* tp_as_sequence */
        0,                                                /* tp_as_mapping */
        0,                                                /* tp_hash */
        0,                                                /* tp_call */
        (reprfunc)UITextBitmap_str,                             /* tp_str */
        0,                                                /* tp_getattro */
        0,                                                /* tp_setattro */
        0,                                                /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         /* tp_flags */
        0,                                                /* tp_doc */
        0,                                                /* tp_traverse */
        0,                                                /* tp_clear */
        0,                                                /* tp_richcompare */
        0,                                                /* tp_weaklistoffset */
        0,                                                /* tp_iter */
        0,                                                /* tp_iternext */
        0,                                                /* tp_methods */
        0,                                                /* tp_members */
        UITextBitmap_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                          /* tp_base */
        0,                                                /* tp_dict */
        0,                                                /* tp_descr_get */
        0,                                                /* tp_descr_set */
        0,                                                /* tp_dictoffset */
        0,                                                /* tp_init */
        0,                                                /* tp_alloc */
        0,                                                /* tp_new */
        0,                                                /* tp_free */
    };
} // namespace ige::scene
