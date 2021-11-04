#include "python/pySpringConstraint.h"
#include "python/pySpringConstraint_doc_en.h"

#include "components/physic/SpringConstraint.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void SpringConstraint_dealloc(PyObject_SpringConstraint *self)
    {
        if (self) {
            self->constraint = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *SpringConstraint_str(PyObject_SpringConstraint *self)
    {
        return PyUnicode_FromString("C++ SpringConstraint object");
    }

    // enable
    PyObject *SpringConstraint_getEnable(PyObject_SpringConstraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getEnable().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int SpringConstraint_setEnable(PyObject_SpringConstraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setEnable(*((btVector3 *)v));
        return 0;
    }

    // stiffness
    PyObject *SpringConstraint_getStiffness(PyObject_SpringConstraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getStiffness().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int SpringConstraint_setStiffness(PyObject_SpringConstraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setStiffness(*((btVector3 *)v));
        return 0;
    }

    // damping
    PyObject *SpringConstraint_getDamping(PyObject_SpringConstraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getDamping().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int SpringConstraint_setDamping(PyObject_SpringConstraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setDamping(*((btVector3 *)v));
        return 0;
    }

    // lowerLimit
    PyObject *SpringConstraint_getLowerLimit(PyObject_SpringConstraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLowerLimit().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int SpringConstraint_setLowerLimit(PyObject_SpringConstraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLowerLimit(*((btVector3 *)v));
        return 0;
    }

    // upperLimit
    PyObject *SpringConstraint_getUpperLimit(PyObject_SpringConstraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getUpperLimit().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int SpringConstraint_setUpperLimit(PyObject_SpringConstraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setUpperLimit(*((btVector3 *)v));
        return 0;
    }

    // Get/set
    PyGetSetDef SpringConstraint_getsets[] = {
        {"enable", (getter)SpringConstraint_getEnable, (setter)SpringConstraint_setEnable, SpringConstraint_enable_doc, NULL},
        {"stiffness", (getter)SpringConstraint_getStiffness, (setter)SpringConstraint_setStiffness, SpringConstraint_stiffness_doc, NULL},
        {"damping", (getter)SpringConstraint_getDamping, (setter)SpringConstraint_setDamping, SpringConstraint_damping_doc, NULL},
        {"lowerLimit", (getter)SpringConstraint_getLowerLimit, (setter)SpringConstraint_setLowerLimit, SpringConstraint_lowerLimit_doc, NULL},
        {"upperLimit", (getter)SpringConstraint_getUpperLimit, (setter)SpringConstraint_setUpperLimit, SpringConstraint_upperLimit_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_SpringConstraint = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.SpringConstraint", /* tp_name */
        sizeof(PyObject_SpringConstraint),                          /* tp_basicsize */
        0,                                                          /* tp_itemsize */
        (destructor)SpringConstraint_dealloc,                       /* tp_dealloc */
        0,                                                          /* tp_print */
        0,                                                          /* tp_getattr */
        0,                                                          /* tp_setattr */
        0,                                                          /* tp_reserved */
        0,                                                          /* tp_repr */
        0,                                                          /* tp_as_number */
        0,                                                          /* tp_as_sequence */
        0,                                                          /* tp_as_mapping */
        0,                                                          /* tp_hash */
        0,                                                          /* tp_call */
        (reprfunc)SpringConstraint_str,                             /* tp_str */
        0,                                                          /* tp_getattro */
        0,                                                          /* tp_setattro */
        0,                                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                         /* tp_flags */
        0,                                                          /* tp_doc */
        0,                                                          /* tp_traverse */
        0,                                                          /* tp_clear */
        0,                                                          /* tp_richcompare */
        0,                                                          /* tp_weaklistoffset */
        0,                                                          /* tp_iter */
        0,                                                          /* tp_iternext */
        0,                                                          /* tp_methods */
        0,                                                          /* tp_members */
        SpringConstraint_getsets,                                   /* tp_getset */
        &PyTypeObject_PhysicConstraint,                             /* tp_base */
        0,                                                          /* tp_dict */
        0,                                                          /* tp_descr_get */
        0,                                                          /* tp_descr_set */
        0,                                                          /* tp_dictoffset */
        0,                                                          /* tp_init */
        0,                                                          /* tp_alloc */
        0,                                                          /* tp_new */
        0,                                                          /* tp_free */
    };
} // namespace ige::scene
