#include "python/pyRectTransform.h"
#include "python/pyRectTransform_doc_en.h"

#include "components/gui/RectTransform.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    // RectTransform
    void RectTransform_dealloc(PyObject_RectTransform *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject* RectTransform_str(PyObject_RectTransform *self)
    {
        return PyUnicode_FromString("C++ RectTransform object");
    }

    // Get anchor
    PyObject* RectTransform_getAnchor(PyObject_RectTransform* self)
    {
        if (self->component) {
            auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
            vmath_cpy(self->component->getAnchor().P(), 4, vec4Obj->v);
            vec4Obj->d = 4;
            return (PyObject*)vec4Obj;
        }
        Py_RETURN_NONE;
    }

    // Set anchor
    int RectTransform_setAnchor(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component) {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject*)value, buff, d);
            if (!v) return -1;
            self->component->setAnchor(*((Vec4*)v));
            return 0;
        }
        return -1;
    }

    // Get pivot
    PyObject* RectTransform_getPivot(PyObject_RectTransform* self)
    {
        if (self->component) {
            auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
            vmath_cpy(self->component->getPivot().P(), 2, vec2Obj->v);
            vec2Obj->d = 2;
            return (PyObject*)vec2Obj;
        }
        Py_RETURN_NONE;
    }

    // Set pivot
    int RectTransform_setPivot(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component) {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject*)value, buff, d);
            if (!v) return -1;
            self->component->setPivot(*((Vec2*)v));
            return 0;
        }
        return -1;
    }

    // Get offset
    PyObject* RectTransform_getOffset(PyObject_RectTransform* self)
    {
        if (self->component) {
            auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
            vmath_cpy(self->component->getOffset().P(), 4, vec4Obj->v);
            vec4Obj->d = 4;
            return (PyObject*)vec4Obj;
        }
        Py_RETURN_NONE;
    }

    // Set offset
    int RectTransform_setOffset(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component) {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject*)value, buff, d);
            if (!v) return -1;
            self->component->setOffset(*((Vec4*)v));
            return 0;
        }
        return -1;
    }

    // Get size
    PyObject* RectTransform_getSize(PyObject_RectTransform* self)
    {
        if (self->component) {
            auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
            vmath_cpy(self->component->getSize().P(), 2, vec2Obj->v);
            vec2Obj->d = 2;
            return (PyObject*)vec2Obj;
        }
        Py_RETURN_NONE;
    }

    // Set size
    int RectTransform_setSize(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component) {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject*)value, buff, d);
            if (!v) return -1;
            self->component->setSize(*((Vec2*)v));
            return 0;
        }
        return -1;
    }

    // Get rect
    PyObject* RectTransform_getRect(PyObject_RectTransform* self)
    {
        if (self->component) {
            auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
            vmath_cpy(self->component->getRect().P(), 4, vec4Obj->v);
            vec4Obj->d = 4;
            return (PyObject*)vec4Obj;
        }
        Py_RETURN_NONE;
    }

    PyGetSetDef RectTransform_getsets[] = {
        { "anchor", (getter)RectTransform_getAnchor, (setter)RectTransform_setAnchor, RectTransform_anchor_doc, NULL },
        { "offset", (getter)RectTransform_getOffset, (setter)RectTransform_setOffset, RectTransform_offset_doc, NULL },
        { "pivot", (getter)RectTransform_getPivot, (setter)RectTransform_setPivot, RectTransform_pivot_doc, NULL },
        { "rect", (getter)RectTransform_getRect, nullptr, RectTransform_rect_doc, NULL },
        { "size", (getter)RectTransform_getSize, (setter)RectTransform_setSize, RectTransform_offset_doc, NULL },
        { "position", (getter)TransformComponent_getWorldPosition, (setter)TransformComponent_setWorldPosition, NULL, NULL },
        { "localPosition", (getter)TransformComponent_getLocalPosition, (setter)TransformComponent_setLocalPosition, NULL, NULL },
        { NULL, NULL }
    };

    PyTypeObject PyTypeObject_RectTransform = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.RectTransform",                    /* tp_name */
        sizeof(PyObject_RectTransform),              /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)RectTransform_dealloc,           /* tp_dealloc */
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
        (reprfunc)RectTransform_str,                 /* tp_str */
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
        RectTransform_getsets,                       /* tp_getset */
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
