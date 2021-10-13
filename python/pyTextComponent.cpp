#include "python/pyTextComponent.h"
#include "python/pyTextComponent_doc_en.h"


#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void TextComponent_dealloc(PyObject_TextComponent *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *TextComponent_str(PyObject_TextComponent*self)
    {
        return PyUnicode_FromString("C++ TextComponent object");
    }

    // Get text
    PyObject* TextComponent_getText(PyObject_TextComponent* self)
    {
        return PyUnicode_FromString(self->component->getText().c_str());
    }

    // Set text
    int TextComponent_setText(PyObject_TextComponent* self, PyObject* value)
    {
        if (PyUnicode_Check(value))
        {
            const char* val = PyUnicode_AsUTF8(value);
            self->component->setText(std::string(val));
            return 0;
        }
        return -1;
    }

    // Get font path
    PyObject* TextComponent_getFontPath(PyObject_TextComponent* self)
    {
        return PyUnicode_FromString(self->component->getFontPath().c_str());
    }

    // Set font path
    int TextComponent_setFontPath(PyObject_TextComponent* self, PyObject* value)
    {
        if (PyUnicode_Check(value))
        {
            const char* val = PyUnicode_AsUTF8(value);
            self->component->setFontPath(std::string(val));
            return 0;
        }
        return -1;
    }

    // Get font size
    PyObject* TextComponent_getFontSize(PyObject_TextComponent* self)
    {
        return PyLong_FromLong(self->component->getFontSize());
    }

    // Set font size
    int TextComponent_setFontSize(PyObject_TextComponent* self, PyObject* value)
    {
        self->component->setFontSize(PyLong_AsLong(value));
        return 0;
    }

    // Get color
    PyObject* TextComponent_getColor(PyObject_TextComponent* self)
    {
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set color
    int TextComponent_setColor(PyObject_TextComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        self->component->setColor(*((Vec4*)v));
        return 0;
    }
    
    PyObject* TextComponent_isBillboard(PyObject_TextComponent* self)
    {
        return PyBool_FromLong(self->component->isBillboard());
    }

    int TextComponent_setBillboard(PyObject_TextComponent* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setBillboard(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef TextComponent_getsets[] = {
        {"text", (getter)TextComponent_getText, (setter)TextComponent_setText, TextComponent_text_doc, NULL},
        {"fontPath", (getter)TextComponent_getFontPath, (setter)TextComponent_setFontPath, TextComponent_fontPath_doc, NULL},
        {"fontSize", (getter)TextComponent_getFontSize, (setter)TextComponent_setFontSize, TextComponent_fontSize_doc, NULL},
        {"color", (getter)TextComponent_getColor, (setter)TextComponent_setColor, TextComponent_color_doc, NULL},
        {"isBillboard", (getter)TextComponent_isBillboard, (setter)TextComponent_setBillboard, TextComponent_isBillboard_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_TextComponent = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.Text",          /* tp_name */
        sizeof(PyObject_TextComponent),                          /* tp_basicsize */
        0,                                                         /* tp_itemsize */
        (destructor)TextComponent_dealloc,                       /* tp_dealloc */
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
        (reprfunc)TextComponent_str,                             /* tp_str */
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
        TextComponent_getsets,                                   /* tp_getset */
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
