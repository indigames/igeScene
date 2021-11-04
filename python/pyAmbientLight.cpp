#include "python/pyAmbientLight.h"
#include "python/pyAmbientLight_doc_en.h"

#include "components/light/AmbientLight.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void AmbientLight_dealloc(PyObject_AmbientLight *self)
    {
        if (self) 
{        self->component.reset();
        Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *AmbientLight_str(PyObject_AmbientLight *self)
    {
        return PyUnicode_FromString("C++ AmbientLight object");
    }

    // skyColor
    PyObject *AmbientLight_getSkyColor(PyObject_AmbientLight *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<AmbientLight>(self->component.lock())->getSkyColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    int AmbientLight_setSkyColor(PyObject_AmbientLight *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<AmbientLight>(self->component.lock())->setSkyColor(*((Vec3*)v));
        return 0;
    }

    // groundColor
    PyObject *AmbientLight_getGroundColor(PyObject_AmbientLight *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<AmbientLight>(self->component.lock())->getGroundColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    int AmbientLight_setGroundColor(PyObject_AmbientLight *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<AmbientLight>(self->component.lock())->setGroundColor(*((Vec3*)v));
        return 0;
    }

    // direction
    PyObject *AmbientLight_getDirection(PyObject_AmbientLight *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<AmbientLight>(self->component.lock())->getDirection().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    int AmbientLight_setDirection(PyObject_AmbientLight *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<AmbientLight>(self->component.lock())->setDirection(*((Vec3*)v));
        return 0;
    }

    PyGetSetDef AmbientLight_getsets[] = {
        {"skyColor", (getter)AmbientLight_getSkyColor, (setter)AmbientLight_setSkyColor, AmbientLight_skyColor_doc, NULL},
        {"groundColor", (getter)AmbientLight_getGroundColor, (setter)AmbientLight_setGroundColor, AmbientLight_groundColor_doc, NULL},
        {"direction", (getter)AmbientLight_getDirection, (setter)AmbientLight_setDirection, AmbientLight_direction_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_AmbientLight = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.AmbientLight", /* tp_name */
        sizeof(PyObject_AmbientLight),                          /* tp_basicsize */
        0,                                                      /* tp_itemsize */
        (destructor)AmbientLight_dealloc,                       /* tp_dealloc */
        0,                                                      /* tp_print */
        0,                                                      /* tp_getattr */
        0,                                                      /* tp_setattr */
        0,                                                      /* tp_reserved */
        0,                                                      /* tp_repr */
        0,                                                      /* tp_as_number */
        0,                                                      /* tp_as_sequence */
        0,                                                      /* tp_as_mapping */
        0,                                                      /* tp_hash */
        0,                                                      /* tp_call */
        (reprfunc)AmbientLight_str,                             /* tp_str */
        0,                                                      /* tp_getattro */
        0,                                                      /* tp_setattro */
        0,                                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                     /* tp_flags */
        0,                                                      /* tp_doc */
        0,                                                      /* tp_traverse */
        0,                                                      /* tp_clear */
        0,                                                      /* tp_richcompare */
        0,                                                      /* tp_weaklistoffset */
        0,                                                      /* tp_iter */
        0,                                                      /* tp_iternext */
        0, /* tp_methods */                                     //! [IGE]: no methods
        0,                                                      /* tp_members */
        AmbientLight_getsets,                                   /* tp_getset */
        &PyTypeObject_Component, /* tp_base */                  //! [IGE]: inheritance
        0,                                                      /* tp_dict */
        0,                                                      /* tp_descr_get */
        0,                                                      /* tp_descr_set */
        0,                                                      /* tp_dictoffset */
        0,                                                      /* tp_init */
        0,                                                      /* tp_alloc */
        0, /* tp_new */                                         //! [IGE]: no new
        0,                                                      /* tp_free */
    };
} // namespace ige::scene
