#include "python/pyTransformComponent.h"
#include "python/pyTransformComponent_doc_en.h"

#include "components/TransformComponent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void TransformComponent_dealloc(PyObject_TransformComponent *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* TransformComponent_str(PyObject_TransformComponent *self)
    {
        return PyUnicode_FromString("C++ TransformComponent object");
    }

    // Get position
    PyObject* TransformComponent_getPosition(PyObject_TransformComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPosition().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set position
    int TransformComponent_setPosition(PyObject_TransformComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setPosition(*((Vec3*)v));
        return 0;
    }

    // Get rotation
    PyObject* TransformComponent_getRotation(PyObject_TransformComponent* self)
    {
        auto quatObj = PyObject_New(vec_obj, _QuatType);
        vmath_cpy(self->component->getRotation().P(), 4, quatObj->v);
        quatObj->d = 4;
        return (PyObject*)quatObj;
    }

    // Set rotation
    int TransformComponent_setRotation(PyObject_TransformComponent* self, PyObject* value)
    {
        int d1;
        float buff[4];
        float* v1 = pyObjToFloat((PyObject*)value, buff, d1);
        if (!v1) return -1;
        self->component->setRotation(*((Quat*)v1));
        return 0;
    }

    // Get scale
    PyObject* TransformComponent_getScale(PyObject_TransformComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getScale().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set scale
    int TransformComponent_setScale(PyObject_TransformComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setScale(*((Vec3*)v));
        return 0;
    }

    // Get local transform matrix
    PyObject* TransformComponent_getLocalMatrix(PyObject_TransformComponent* self)
    {
        auto m4obj = PyObject_New(mat_obj, _Mat44Type);
        if (!m4obj) return NULL;
        auto mat = self->component->getLocalMatrix();
        vmath_cpy(mat.P(), 16, m4obj->m);
        m4obj->d = 4;
        return (PyObject*)m4obj;
    }

    // Get world position
    PyObject* TransformComponent_getWorldPosition(PyObject_TransformComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getWorldPosition().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set world position
    int TransformComponent_setWorldPosition(PyObject_TransformComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setWorldPosition(*((Vec3*)v));
        return 0;
    }

    // Get world rotation
    PyObject* TransformComponent_getWorldRotation(PyObject_TransformComponent* self)
    {
        auto quatObj = PyObject_New(vec_obj, _QuatType);
        vmath_cpy(self->component->getWorldRotation().P(), 4, quatObj->v);
        quatObj->d = 4;
        return (PyObject*)quatObj;
    }

    // Set world rotation
    int TransformComponent_setWorldRotation(PyObject_TransformComponent* self, PyObject* value)
    {
        int d1;
        float buff[4];
        float* v1 = pyObjToFloat((PyObject*)value, buff, d1);
        if (!v1) return -1;
        self->component->setWorldRotation(*((Quat*)v1));
        return 0;
    }

    // Get world scale
    PyObject* TransformComponent_getWorldScale(PyObject_TransformComponent* self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getWorldScale().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set world scale
    int TransformComponent_setWorldScale(PyObject_TransformComponent* self, PyObject* value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        self->component->setWorldScale(*((Vec3*)v));
        return 0;
    }

    // Get world transform matrix
    PyObject* TransformComponent_getWorldMatrix(PyObject_TransformComponent* self)
    {
        auto m4obj = PyObject_New(mat_obj, _Mat44Type);
        if (!m4obj) return NULL;
        auto mat = self->component->getWorldMatrix();
        vmath_cpy(mat.P(), 16, m4obj->m);
        m4obj->d = 4;
        return (PyObject*)m4obj;
    }

    // Variable definition
    PyGetSetDef TransformComponent_getsets[] = {
        { "position", (getter)TransformComponent_getPosition, (setter)TransformComponent_setPosition, TransformComponent_position_doc, NULL },
        { "rotation", (getter)TransformComponent_getRotation, (setter)TransformComponent_setRotation, TransformComponent_rotation_doc, NULL },
        { "scale", (getter)TransformComponent_getScale, (setter)TransformComponent_setScale, TransformComponent_scale_doc, NULL },
        { "localMatrix", (getter)TransformComponent_getLocalMatrix, NULL, TransformComponent_localMatrix_doc, NULL },
        { "worldPosition", (getter)TransformComponent_getWorldPosition, (setter)TransformComponent_setWorldPosition, TransformComponent_worldPosition_doc, NULL },
        { "worldRotation", (getter)TransformComponent_getWorldRotation, (setter)TransformComponent_setWorldRotation, TransformComponent_worldRotation_doc, NULL },
        { "worldScale", (getter)TransformComponent_getWorldScale, (setter)TransformComponent_setWorldScale, TransformComponent_worldScale_doc, NULL },
        { "worldMatrix", (getter)TransformComponent_getWorldMatrix, NULL, TransformComponent_worldMatrix_doc, NULL },
        { NULL, NULL }
    };

    // Type definition
    PyTypeObject PyTypeObject_TransformComponent = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.TransformComponent",               /* tp_name */
        sizeof(PyObject_TransformComponent),         /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)TransformComponent_dealloc,      /* tp_dealloc */
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
        (reprfunc)TransformComponent_str,            /* tp_str */
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
        TransformComponent_getsets,                  /* tp_getset */
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
