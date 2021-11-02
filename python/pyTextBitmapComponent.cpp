#include "python/pyTextBitmapComponent.h"
#include "python/pyTextComponent_doc_en.h"


#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void TextBitmapComponent_dealloc(PyObject_TextBitmapComponent *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *TextBitmapComponent_str(PyObject_TextBitmapComponent*self)
    {
        return PyUnicode_FromString("C++ TextBitmapComponent object");
    }

    // Get text
    PyObject* TextBitmapComponent_getText(PyObject_TextBitmapComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyUnicode_FromString(self->component->getText().c_str());
    }

    // Set text
    int TextBitmapComponent_setText(PyObject_TextBitmapComponent* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyUnicode_Check(value))
        {
            const char* val = PyUnicode_AsUTF8(value);
            self->component->setText(std::string(val));
            return 0;
        }
        return -1;
    }

    // Get font path
    PyObject* TextBitmapComponent_getFontPath(PyObject_TextBitmapComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyUnicode_FromString(self->component->getFontPath().c_str());
    }

    // Set font path
    int TextBitmapComponent_setFontPath(PyObject_TextBitmapComponent* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyUnicode_Check(value))
        {
            const char* val = PyUnicode_AsUTF8(value);
            self->component->setFontPath(std::string(val));
            return 0;
        }
        return -1;
    }

    // Get font size
    PyObject* TextBitmapComponent_getFontSize(PyObject_TextBitmapComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyLong_FromLong(self->component->getFontSize());
    }

    // Set font size
    int TextBitmapComponent_setFontSize(PyObject_TextBitmapComponent* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setFontSize(val);
            return 0;
        }
        return -1;
    }

    // Get color
    PyObject* TextBitmapComponent_getColor(PyObject_TextBitmapComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set color
    int TextBitmapComponent_setColor(PyObject_TextBitmapComponent* self, PyObject* value)
    {
        if (!self->component) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        self->component->setColor(*((Vec4*)v));
        return 0;
    }
    
    PyObject* TextBitmapComponent_isBillboard(PyObject_TextBitmapComponent* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->isBillboard());
    }

    int TextBitmapComponent_setBillboard(PyObject_TextBitmapComponent* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setBillboard(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef TextBitmapComponent_getsets[] = {
        {"text", (getter)TextBitmapComponent_getText, (setter)TextBitmapComponent_setText, TextComponent_text_doc, NULL},
        {"fontPath", (getter)TextBitmapComponent_getFontPath, (setter)TextBitmapComponent_setFontPath, TextComponent_fontPath_doc, NULL},
        {"fontSize", (getter)TextBitmapComponent_getFontSize, (setter)TextBitmapComponent_setFontSize, TextComponent_fontSize_doc, NULL},
        {"color", (getter)TextBitmapComponent_getColor, (setter)TextBitmapComponent_setColor, TextComponent_color_doc, NULL},
        {"isBillboard", (getter)TextBitmapComponent_isBillboard, (setter)TextBitmapComponent_setBillboard, TextComponent_isBillboard_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_TextBitmapComponent = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.TextBitmap",          /* tp_name */
        sizeof(PyObject_TextBitmapComponent),                          /* tp_basicsize */
        0,                                                         /* tp_itemsize */
        (destructor)TextBitmapComponent_dealloc,                       /* tp_dealloc */
        0,                                                         /* tp_print */
        0,                                                         /* tp_getattr */
        0,                                                         /* tp_setattr */
        0,                                                         /* tp_reserved */
        0,                                                         /* tp_repr */
        0,                                                         /* tp_as_number */
        0,                                                         /* tp_as_sequence */
        0,                                                         /* tp_as_mapping */
        0,                                                         /* tp_hash */
        0,                                                         /* tp_call */
        (reprfunc)TextBitmapComponent_str,                             /* tp_str */
        0,                                                         /* tp_getattro */
        0,                                                         /* tp_setattro */
        0,                                                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                  /* tp_flags */
        0,                                                         /* tp_doc */
        0,                                                         /* tp_traverse */
        0,                                                         /* tp_clear */
        0,                                                         /* tp_richcompare */
        0,                                                         /* tp_weaklistoffset */
        0,                                                         /* tp_iter */
        0,                                                         /* tp_iternext */
        0,                                                         /* tp_methods */
        0,                                                         /* tp_members */
        TextBitmapComponent_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                                   /* tp_base */
        0,                                                         /* tp_dict */
        0,                                                         /* tp_descr_get */
        0,                                                         /* tp_descr_set */
        0,                                                         /* tp_dictoffset */
        0,                                                         /* tp_init */
        0,                                                         /* tp_alloc */
        0,                                                         /* tp_new */
        0,                                                         /* tp_free */
    };
} // namespace ige::scene
