#include "python/pyEnvironmentComponent.h"
#include "python/pyEnvironmentComponent_doc_en.h"

#include "components/EnvironmentComponent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void EnvironmentComponent_dealloc(PyObject_EnvironmentComponent *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* EnvironmentComponent_str(PyObject_EnvironmentComponent *self)
    {
        return PyUnicode_FromString("C++ EnvironmentComponent object");
    }

    // Get Distance Fog Color
    PyObject* EnvironmentComponent_getDistanceFogColor(PyObject_EnvironmentComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->getDistanceFogColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Distance Fog Color
    int EnvironmentComponent_setDistanceFogColor(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->setDistanceFogColor(*((Vec3*)v));
        return 0;
    }

    // Get Distance Fog Alpha
    PyObject* EnvironmentComponent_getDistanceFogAlpha(PyObject_EnvironmentComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->getDistanceFogAlpha());
    }

    // Set Distance Fog Alpha
    int EnvironmentComponent_setDistanceFogAlpha(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->setDistanceFogAlpha(val);
            return 0;
        }
        return -1;
    }

    // Get Distance Fog Near
    PyObject* EnvironmentComponent_getDistanceFogNear(PyObject_EnvironmentComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->getDistanceFogNear());
    }

    // Set Distance Fog Near
    int EnvironmentComponent_setDistanceFogNear(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->setDistanceFogNear(val);
            return 0;
        }
        return -1;
    }

    // Get Distance Fog Far
    PyObject* EnvironmentComponent_getDistanceFogFar(PyObject_EnvironmentComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->getDistanceFogFar());
    }

    // Set Distance Fog Far
    int EnvironmentComponent_setDistanceFogFar(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->setDistanceFogFar(val);
            return 0;
        }
        return -1;
    }

    // Get Shadow Color
    PyObject* EnvironmentComponent_getShadowColor(PyObject_EnvironmentComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->getShadowColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    // Set Shadow Color
    int EnvironmentComponent_setShadowColor(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->setShadowColor(*((Vec3*)v));
        return 0;
    }

    // Get Shadow Density
    PyObject* EnvironmentComponent_getShadowDensity(PyObject_EnvironmentComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->getShadowDensity());
    }

    // Set Shadow Density
    int EnvironmentComponent_setShadowDensity(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->setShadowDensity(val);
            return 0;
        }
        return -1;
    }

    // Get Shadow Wideness
    PyObject* EnvironmentComponent_getShadowWideness(PyObject_EnvironmentComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->getShadowWideness());
    }

    // Set Shadow Wideness
    int EnvironmentComponent_setShadowWideness(PyObject_EnvironmentComponent* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->setShadowWideness(val);
            return 0;
        }
        return -1;
    }

    // Get environment instance
    PyObject* EnvironmentComponent_getEnvironment(PyObject_EnvironmentComponent* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto env = std::dynamic_pointer_cast<EnvironmentComponent>(self->component.lock())->getEnvironment();
        if (env) {
            auto envObj = (environment_obj*)(&EnvironmentType)->tp_alloc(&EnvironmentType, 0);
            envObj->envSet = env;
            return (PyObject*)envObj;
        }
        Py_RETURN_NONE;
    }

    PyGetSetDef EnvironmentComponent_getsets[] = {
        { "distanceFogColor", (getter)EnvironmentComponent_getDistanceFogColor, (setter)EnvironmentComponent_setDistanceFogColor, EnvironmentComponent_distanceFogColor_doc, NULL },
        { "distanceFogAlpha", (getter)EnvironmentComponent_getDistanceFogAlpha, (setter)EnvironmentComponent_setDistanceFogAlpha, EnvironmentComponent_distanceFogAlpha_doc, NULL },
        { "distanceFogNear", (getter)EnvironmentComponent_getDistanceFogNear, (setter)EnvironmentComponent_setDistanceFogNear, EnvironmentComponent_distanceFogNear_doc, NULL },
        { "distanceFogFar", (getter)EnvironmentComponent_getDistanceFogFar, (setter)EnvironmentComponent_setDistanceFogFar, EnvironmentComponent_distanceFogFar_doc, NULL },
        { "shadowColor", (getter)EnvironmentComponent_getShadowColor, (setter)EnvironmentComponent_setShadowColor, EnvironmentComponent_shadowColor_doc, NULL },
        { "shadowDensity", (getter)EnvironmentComponent_getShadowDensity, (setter)EnvironmentComponent_setShadowDensity, EnvironmentComponent_shadowDensity_doc, NULL },
        { "shadowWideness", (getter)EnvironmentComponent_getShadowWideness, (setter)EnvironmentComponent_setShadowWideness, EnvironmentComponent_shadowWideness_doc, NULL },
        { "environment", (getter)EnvironmentComponent_getEnvironment, NULL, EnvironmentComponent_environment_doc, NULL },
        { NULL, NULL }
    };

    PyTypeObject PyTypeObject_EnvironmentComponent = {
        PyVarObject_HEAD_INIT(NULL, 1)
        "igeScene.Environment",                      /* tp_name */
        sizeof(PyObject_EnvironmentComponent),       /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)EnvironmentComponent_dealloc,    /* tp_dealloc */
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
        (reprfunc)EnvironmentComponent_str,          /* tp_str */
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
        EnvironmentComponent_getsets,                /* tp_getset */
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
