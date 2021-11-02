#include "python/pyDof6Constraint.h"
#include "python/pyDof6Constraint_doc_en.h"

#include "components/physic/Dof6SpringConstraint.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void Dof6Constraint_dealloc(PyObject_Dof6Constraint *self)
    {
        if (self && self->constraint)
        {
            self->constraint = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *Dof6Constraint_str(PyObject_Dof6Constraint *self)
    {
        return PyUnicode_FromString("C++ Dof6Constraint object");
    }

    // linearLowerLimit
    PyObject *Dof6Constraint_getLinearLowerLimit(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearLowerLimit().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearLowerLimit(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearLowerLimit(*((btVector3 *)v));
        return 0;
    }

    // linearUpperLimit
    PyObject *Dof6Constraint_getLinearUpperLimit(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearUpperLimit().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearUpperLimit(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearUpperLimit(*((btVector3 *)v));
        return 0;
    }

    // linearTargetVelocity
    PyObject *Dof6Constraint_getLinearTargetVelocity(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearTargetVelocity().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearTargetVelocity(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearTargetVelocity(*((btVector3 *)v));
        return 0;
    }

    // linearBounce
    PyObject *Dof6Constraint_getLinearBounce(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearBounce().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearBounce(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearBounce(*((btVector3 *)v));
        return 0;
    }

    // linearSpringEnabled
    PyObject *Dof6Constraint_getLinearSpringEnabled(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearSpringEnabled().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearSpringEnabled(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearSpringEnabled(*((btVector3 *)v));
        return 0;
    }

    // linearStiffness
    PyObject *Dof6Constraint_getLinearStiffness(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearStiffness().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearStiffness(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearStiffness(*((btVector3 *)v));
        return 0;
    }

    // linearDamping
    PyObject *Dof6Constraint_getLinearDamping(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearDamping().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearDamping(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearDamping(*((btVector3 *)v));
        return 0;
    }

    // linearMotorEnabled
    PyObject *Dof6Constraint_getLinearMotorEnabled(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearMotorEnabled().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearMotorEnabled(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearMotorEnabled(*((btVector3 *)v));
        return 0;
    }

    // linearMaxMotorForce
    PyObject *Dof6Constraint_getLinearMaxMotorForce(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearMaxMotorForce().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearMaxMotorForce(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearMaxMotorForce(*((btVector3 *)v));
        return 0;
    }

    // linearServoEnabled
    PyObject *Dof6Constraint_getLinearServoEnabled(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearServoEnabled().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearServoEnabled(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearServoEnabled(*((btVector3 *)v));
        return 0;
    }

    // linearServoTarget
    PyObject *Dof6Constraint_getLinearServoTarget(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLinearServoTarget().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setLinearServoTarget(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLinearServoTarget(*((btVector3 *)v));
        return 0;
    }

    // angularLowerLimit
    PyObject *Dof6Constraint_getAngularLowerLimit(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularLowerLimit().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularLowerLimit(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularLowerLimit(*((btVector3 *)v));
        return 0;
    }

    // angularUpperLimit
    PyObject *Dof6Constraint_getAngularUpperLimit(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularUpperLimit().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularUpperLimit(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularUpperLimit(*((btVector3 *)v));
        return 0;
    }

    // angularTargetVelocity
    PyObject *Dof6Constraint_getAngularTargetVelocity(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularTargetVelocity().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularTargetVelocity(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularTargetVelocity(*((btVector3 *)v));
        return 0;
    }

    // angularBounce
    PyObject *Dof6Constraint_getAngularBounce(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularBounce().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularBounce(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularBounce(*((btVector3 *)v));
        return 0;
    }

    // angularSpringEnabled
    PyObject *Dof6Constraint_getAngularSpringEnabled(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularSpringEnabled().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularSpringEnabled(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularSpringEnabled(*((btVector3 *)v));
        return 0;
    }

    // angularStiffness
    PyObject *Dof6Constraint_getAngularStiffness(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularStiffness().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularStiffness(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularStiffness(*((btVector3 *)v));
        return 0;
    }

    // angularDamping
    PyObject *Dof6Constraint_getAngularDamping(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularDamping().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularDamping(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularDamping(*((btVector3 *)v));
        return 0;
    }

    // angularMotorEnabled
    PyObject *Dof6Constraint_getAngularMotorEnabled(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularMotorEnabled().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularMotorEnabled(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularMotorEnabled(*((btVector3 *)v));
        return 0;
    }

    // angularMaxMotorForce
    PyObject *Dof6Constraint_getAngularMaxMotorForce(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularMaxMotorForce().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularMaxMotorForce(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularMaxMotorForce(*((btVector3 *)v));
        return 0;
    }

    // angularServoEnabled
    PyObject *Dof6Constraint_getAngularServoEnabled(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularServoEnabled().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularServoEnabled(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularServoEnabled(*((btVector3 *)v));
        return 0;
    }

    // angularServoTarget
    PyObject *Dof6Constraint_getAngularServoTarget(PyObject_Dof6Constraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAngularServoTarget().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Dof6Constraint_setAngularServoTarget(PyObject_Dof6Constraint *self, PyObject *value)
    {
        if (!self->constraint) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAngularServoTarget(*((btVector3 *)v));
        return 0;
    }

    // Get/set
    PyGetSetDef Dof6Constraint_getsets[] = {
        {"linearLowerLimit", (getter)Dof6Constraint_getLinearLowerLimit, (setter)Dof6Constraint_setLinearLowerLimit, Dof6Constraint_linearLowerLimit_doc, NULL},
        {"linearUpperLimit", (getter)Dof6Constraint_getLinearUpperLimit, (setter)Dof6Constraint_setLinearUpperLimit, Dof6Constraint_linearUpperLimit_doc, NULL},
        {"linearTargetVelocity", (getter)Dof6Constraint_getLinearTargetVelocity, (setter)Dof6Constraint_setLinearTargetVelocity, Dof6Constraint_linearTargetVelocity_doc, NULL},
        {"linearBounce", (getter)Dof6Constraint_getLinearBounce, (setter)Dof6Constraint_setLinearBounce, Dof6Constraint_linearBounce_doc, NULL},
        {"linearSpringEnabled", (getter)Dof6Constraint_getLinearSpringEnabled, (setter)Dof6Constraint_setLinearSpringEnabled, Dof6Constraint_linearSpringEnabled_doc, NULL},
        {"linearStiffness", (getter)Dof6Constraint_getLinearStiffness, (setter)Dof6Constraint_setLinearStiffness, Dof6Constraint_linearStiffness_doc, NULL},
        {"linearDamping", (getter)Dof6Constraint_getLinearDamping, (setter)Dof6Constraint_setLinearDamping, Dof6Constraint_linearDamping_doc, NULL},
        {"linearMotorEnabled", (getter)Dof6Constraint_getLinearMotorEnabled, (setter)Dof6Constraint_setLinearMotorEnabled, Dof6Constraint_linearMotorEnabled_doc, NULL},
        {"linearMaxMotorForce", (getter)Dof6Constraint_getLinearMaxMotorForce, (setter)Dof6Constraint_setLinearMaxMotorForce, Dof6Constraint_linearMaxMotorForce_doc, NULL},
        {"linearServoEnabled", (getter)Dof6Constraint_getLinearServoEnabled, (setter)Dof6Constraint_setLinearServoEnabled, Dof6Constraint_linearServoEnabled_doc, NULL},
        {"linearServoTarget", (getter)Dof6Constraint_getLinearServoTarget, (setter)Dof6Constraint_setLinearServoTarget, Dof6Constraint_linearServoTarget_doc, NULL},
        {"angularLowerLimit", (getter)Dof6Constraint_getAngularLowerLimit, (setter)Dof6Constraint_setAngularLowerLimit, Dof6Constraint_angularLowerLimit_doc, NULL},
        {"angularUpperLimit", (getter)Dof6Constraint_getAngularUpperLimit, (setter)Dof6Constraint_setAngularUpperLimit, Dof6Constraint_angularUpperLimit_doc, NULL},
        {"angularTargetVelocity", (getter)Dof6Constraint_getAngularTargetVelocity, (setter)Dof6Constraint_setAngularTargetVelocity, Dof6Constraint_angularTargetVelocity_doc, NULL},
        {"angularBounce", (getter)Dof6Constraint_getAngularBounce, (setter)Dof6Constraint_setAngularBounce, Dof6Constraint_angularBounce_doc, NULL},
        {"angularSpringEnabled", (getter)Dof6Constraint_getAngularSpringEnabled, (setter)Dof6Constraint_setAngularSpringEnabled, Dof6Constraint_angularSpringEnabled_doc, NULL},
        {"angularStiffness", (getter)Dof6Constraint_getAngularStiffness, (setter)Dof6Constraint_setAngularStiffness, Dof6Constraint_angularStiffness_doc, NULL},
        {"angularDamping", (getter)Dof6Constraint_getAngularDamping, (setter)Dof6Constraint_setAngularDamping, Dof6Constraint_angularDamping_doc, NULL},
        {"angularMotorEnabled", (getter)Dof6Constraint_getAngularMotorEnabled, (setter)Dof6Constraint_setAngularMotorEnabled, Dof6Constraint_angularMotorEnabled_doc, NULL},
        {"angularMaxMotorForce", (getter)Dof6Constraint_getAngularMaxMotorForce, (setter)Dof6Constraint_setAngularMaxMotorForce, Dof6Constraint_angularMaxMotorForce_doc, NULL},
        {"angularServoEnabled", (getter)Dof6Constraint_getAngularServoEnabled, (setter)Dof6Constraint_setAngularServoEnabled, Dof6Constraint_angularServoEnabled_doc, NULL},
        {"angularServoTarget", (getter)Dof6Constraint_getAngularServoTarget, (setter)Dof6Constraint_setAngularServoTarget, Dof6Constraint_angularServoTarget_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_Dof6Constraint = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.Dof6Constraint", /* tp_name */
        sizeof(PyObject_Dof6Constraint),                          /* tp_basicsize */
        0,                                                        /* tp_itemsize */
        (destructor)Dof6Constraint_dealloc,                       /* tp_dealloc */
        0,                                                        /* tp_print */
        0,                                                        /* tp_getattr */
        0,                                                        /* tp_setattr */
        0,                                                        /* tp_reserved */
        0,                                                        /* tp_repr */
        0,                                                        /* tp_as_number */
        0,                                                        /* tp_as_sequence */
        0,                                                        /* tp_as_mapping */
        0,                                                        /* tp_hash */
        0,                                                        /* tp_call */
        (reprfunc)Dof6Constraint_str,                             /* tp_str */
        0,                                                        /* tp_getattro */
        0,                                                        /* tp_setattro */
        0,                                                        /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                       /* tp_flags */
        0,                                                        /* tp_doc */
        0,                                                        /* tp_traverse */
        0,                                                        /* tp_clear */
        0,                                                        /* tp_richcompare */
        0,                                                        /* tp_weaklistoffset */
        0,                                                        /* tp_iter */
        0,                                                        /* tp_iternext */
        0,                                                        /* tp_methods */
        0,                                                        /* tp_members */
        Dof6Constraint_getsets,                                   /* tp_getset */
        &PyTypeObject_PhysicConstraint,                           /* tp_base */
        0,                                                        /* tp_dict */
        0,                                                        /* tp_descr_get */
        0,                                                        /* tp_descr_set */
        0,                                                        /* tp_dictoffset */
        0,                                                        /* tp_init */
        0,                                                        /* tp_alloc */
        0,                                                        /* tp_new */
        0,                                                        /* tp_free */
    };
} // namespace ige::scene
