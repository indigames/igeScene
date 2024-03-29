#include "python/pyUIText.h"
#include "python/pyUIText_doc_en.h"

#include "components/gui/UIText.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UIText_dealloc(PyObject_UIText *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *UIText_str(PyObject_UIText *self)
    {
        return PyUnicode_FromString("C++ UIText object");
    }

    // Get text
    PyObject *UIText_getText(PyObject_UIText *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(std::dynamic_pointer_cast<UIText>(self->component.lock())->getText().c_str());
    }

    // Set text
    int UIText_setText(PyObject_UIText *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyUnicode_Check(value))
        {
            const char* val = PyUnicode_AsUTF8(value);
            std::dynamic_pointer_cast<UIText>(self->component.lock())->setText(std::string(val));
            return 0;
        }
        return -1;
    }

    // Get font path
    PyObject *UIText_getFontPath(PyObject_UIText *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(std::dynamic_pointer_cast<UIText>(self->component.lock())->getFontPath().c_str());
    }

    // Set font path
    int UIText_setFontPath(PyObject_UIText *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyUnicode_Check(value))
        {
            const char* val = PyUnicode_AsUTF8(value);
            std::dynamic_pointer_cast<UIText>(self->component.lock())->setFontPath(std::string(val));
            return 0;
        }
        return -1;
    }

    // Get font size
    PyObject *UIText_getFontSize(PyObject_UIText *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<UIText>(self->component.lock())->getFontSize());
    }

    // Set font size
    int UIText_setFontSize(PyObject_UIText *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<UIText>(self->component.lock())->setFontSize(val);
            return 0;
        }
        return -1;
    }

    // Get color
    PyObject *UIText_getColor(PyObject_UIText *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<UIText>(self->component.lock())->getColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject *)vec4Obj;
    }

    // Set color
    int UIText_setColor(PyObject_UIText *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<UIText>(self->component.lock())->setColor(*((Vec4 *)v));
        return 0;
    }

    // Get/set
    PyGetSetDef UIText_getsets[] = {
        {"text", (getter)UIText_getText, (setter)UIText_setText, UIText_text_doc, NULL},
        {"fontPath", (getter)UIText_getFontPath, (setter)UIText_setFontPath, UIText_fontPath_doc, NULL},
        {"fontSize", (getter)UIText_getFontSize, (setter)UIText_setFontSize, UIText_fontSize_doc, NULL},
        {"color", (getter)UIText_getColor, (setter)UIText_setColor, UIText_color_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_UIText = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.UIText", /* tp_name */
        sizeof(PyObject_UIText),                          /* tp_basicsize */
        0,                                                /* tp_itemsize */
        (destructor)UIText_dealloc,                       /* tp_dealloc */
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
        (reprfunc)UIText_str,                             /* tp_str */
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
        UIText_getsets,                                   /* tp_getset */
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
