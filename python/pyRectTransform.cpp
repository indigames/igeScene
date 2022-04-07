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
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* RectTransform_str(PyObject_RectTransform *self)
    {
        return PyUnicode_FromString("C++ RectTransform object");
    }

    // Get anchor
    PyObject* RectTransform_getAnchor(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getAnchor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set anchor
    int RectTransform_setAnchor(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setAnchor(*((Vec4*)v));
        return 0;
    }

    // Get anchor Position
    PyObject* RectTransform_getAnchorPosition(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getAnchoredPosition().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    // Set anchor Position
    int RectTransform_setAnchorPosition(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setAnchoredPosition(*((Vec2*)v));
        return 0;
    }

    // Get pivot
    PyObject* RectTransform_getPivot(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getPivot().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    // Set pivot
    int RectTransform_setPivot(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setPivot(*((Vec2*)v));
        return 0;
    }

    // Get offset
    PyObject* RectTransform_getOffset(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getOffset().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set offset
    int RectTransform_setOffset(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setOffset(*((Vec4*)v));
        return 0;
    }

    // Get size
    PyObject* RectTransform_getSize(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getSize().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    // Set size
    int RectTransform_setSize(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setSize(*((Vec2*)v));
        return 0;
    }

    // Get rect
    PyObject* RectTransform_getRect(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getRect().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Get world position
    PyObject* RectTransform_getWorldPosition(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getPosition().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set world position
    int RectTransform_setWorldPosition(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setPosition(*((Vec3*)v));
        return 0;
    }

    // Get world rotation
    PyObject* RectTransform_getWorldRotation(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto quatObj = PyObject_New(vec_obj, _QuatType);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getRotation().P(), 4, quatObj->v);
        quatObj->d = 4;
        return (PyObject*)quatObj;
    }

    // Set world rotation
    int RectTransform_setWorldRotation(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d1;
        float buff[4];
        float* v1 = pyObjToFloat((PyObject*)value, buff, d1);
        if (!v1) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setRotation(*((Quat*)v1));
        return 0;
    }

    // Get world scale
    PyObject* RectTransform_getWorldScale(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getScale().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set world scale
    int RectTransform_setWorldScale(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setScale(*((Vec3*)v));
        return 0;
    }

    // Get local position
    PyObject* RectTransform_getLocalPosition(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getLocalPosition().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set local position
    int RectTransform_setLocalPosition(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setLocalPosition(*((Vec3*)v));
        return 0;
    }

    // Get local rotation
    PyObject* RectTransform_getLocalRotation(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto quatObj = PyObject_New(vec_obj, _QuatType);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getLocalRotation().P(), 4, quatObj->v);
        quatObj->d = 4;
        return (PyObject*)quatObj;
    }

    // Set local rotation
    int RectTransform_setLocalRotation(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d1;
        float buff[4];
        float* v1 = pyObjToFloat((PyObject*)value, buff, d1);
        if (!v1) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setLocalRotation(*((Quat*)v1));
        return 0;
    }

    // Get local scale
    PyObject* RectTransform_getLocalScale(PyObject_RectTransform* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<RectTransform>(self->component.lock())->getScale().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set local scale
    int RectTransform_setLocalScale(PyObject_RectTransform* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<RectTransform>(self->component.lock())->setLocalScale(*((Vec3*)v));
        return 0;
    }



    PyGetSetDef RectTransform_getsets[] = {
        { "anchor", (getter)RectTransform_getAnchor, (setter)RectTransform_setAnchor, RectTransform_anchor_doc, NULL },
        { "anchorPosition", (getter)RectTransform_getAnchorPosition, (setter)RectTransform_setAnchorPosition, NULL, NULL },
        { "offset", (getter)RectTransform_getOffset, (setter)RectTransform_setOffset, RectTransform_offset_doc, NULL },
        { "pivot", (getter)RectTransform_getPivot, (setter)RectTransform_setPivot, RectTransform_pivot_doc, NULL },
        { "rect", (getter)RectTransform_getRect, nullptr, RectTransform_rect_doc, NULL },
        { "size", (getter)RectTransform_getSize, (setter)RectTransform_setSize, RectTransform_offset_doc, NULL },
        { "position", (getter)RectTransform_getWorldPosition, (setter)RectTransform_setWorldPosition, NULL, NULL },
        { "localPosition", (getter)RectTransform_getLocalPosition, (setter)RectTransform_setLocalPosition, NULL, NULL },
        { "rotation", (getter)RectTransform_getWorldRotation, (setter)RectTransform_setWorldRotation, NULL, NULL },
        { "localRotation", (getter)RectTransform_getLocalRotation, (setter)RectTransform_setLocalRotation, NULL, NULL },
        { "scale", (getter)RectTransform_getWorldScale, (setter)RectTransform_setWorldScale, NULL, NULL },
        { "localScale", (getter)RectTransform_getLocalScale, (setter)RectTransform_setLocalScale, NULL, NULL },
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
