#include "python/pyPhysicBase.h"
#include "python/pyPhysicBase_doc_en.h"

#include "components/physic/PhysicBase.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void PhysicBase_dealloc(PyObject_PhysicBase *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *PhysicBase_str(PyObject_PhysicBase *self)
    {
        return PyUnicode_FromString("C++ PhysicBase object");
    }

    //! Apply torque
    void PhysicBase_applyTorque(PyObject_PhysicBase *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (v)
        {
            self->component->applyTorque(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        }
    }

    //! Apply force
    void PhysicBase_applyForce(PyObject_PhysicBase *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (v)
        {
            self->component->applyForce(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        }
    }

    //! Apply impulse
    void PhysicBase_applyImpulse(PyObject_PhysicBase *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (v)
        {
            self->component->applyImpulse(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        }
    }

    //! Clear forces
    void PhysicBase_clearForces(PyObject_PhysicBase *self)
    {
        self->component->clearForces();
    }

    //! Mass
    PyObject *PhysicBase_getMass(PyObject_PhysicBase *self)
    {
        return PyFloat_FromDouble(self->component->getMass());
    }

    int PhysicBase_setMass(PyObject_PhysicBase *self, PyObject *value)
    {
        float mass;
        if (PyArg_ParseTuple(value, "f", &mass))
        {
            self->component->setMass(mass);
            return 0;
        }
        return -1;
    }

    //! Friction
    PyObject *PhysicBase_getFriction(PyObject_PhysicBase *self)
    {
        return PyFloat_FromDouble(self->component->getFriction());
    }

    int PhysicBase_setFriction(PyObject_PhysicBase *self, PyObject *value)
    {
        float friction;
        if (PyArg_ParseTuple(value, "f", &friction))
        {
            self->component->setFriction(friction);
            return 0;
        }
        return -1;
    }

    //! Restitution
    PyObject *PhysicBase_getRestitution(PyObject_PhysicBase *self)
    {
        return PyFloat_FromDouble(self->component->getRestitution());
    }

    int PhysicBase_setRestitution(PyObject_PhysicBase *self, PyObject *value)
    {
        float restitution;
        if (PyArg_ParseTuple(value, "f", &restitution))
        {
            self->component->setRestitution(restitution);
            return 0;
        }
        return -1;
    }

    //! Linear velocity
    PyObject *PhysicBase_getLinearVelocity(PyObject_PhysicBase *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getLinearVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicBase_setLinearVelocity(PyObject_PhysicBase *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setLinearVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Angular velocity
    PyObject *PhysicBase_getAngularVelocity(PyObject_PhysicBase *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getAngularVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicBase_setAngularVelocity(PyObject_PhysicBase *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setAngularVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Linear factor
    PyObject *PhysicBase_getLinearFactor(PyObject_PhysicBase *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getLinearFactor()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicBase_setLinearFactor(PyObject_PhysicBase *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setLinearFactor(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Angular factor
    PyObject *PhysicBase_getAngularFactor(PyObject_PhysicBase *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getAngularFactor()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicBase_setAngularFactor(PyObject_PhysicBase *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setAngularFactor(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Indicate object is a trigger object
    PyObject *PhysicBase_isTrigger(PyObject_PhysicBase *self)
    {
        return PyBool_FromLong(self->component->isTrigger());
    }

    int PhysicBase_setIsTrigger(PyObject_PhysicBase *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setIsTrigger(val);
            return 0;
        }
        return -1;
    }

    //! Indicate object is a kinematic object
    PyObject *PhysicBase_isKinematic(PyObject_PhysicBase *self)
    {
        return PyBool_FromLong(self->component->isKinematic());
    }

    int PhysicBase_setIsKinematic(PyObject_PhysicBase *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setIsKinematic(val);
            return 0;
        }
        return -1;
    }

    //! Enable/Disable physic component
    PyObject *PhysicBase_isEnabled(PyObject_PhysicBase *self)
    {
        return PyBool_FromLong(self->component->isEnabled());
    }

    int PhysicBase_setEnabled(PyObject_PhysicBase *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setEnabled(val);
            return 0;
        }
        return -1;
    }

    //! Get AABB
    PyObject *PhysicBase_getAABB(PyObject_PhysicBase *self)
    {
        btVector3 aabbMin, aabbMax;
        self->component->getAABB(aabbMin, aabbMax);

        vec_obj *min = PyObject_New(vec_obj, _Vec3Type);
        vec_obj *max = PyObject_New(vec_obj, _Vec3Type);
        min->v[0] = aabbMin.getX();
        min->v[1] = aabbMin.getY();
        min->v[2] = aabbMin.getZ();
        min->d = 3;

        max->v[0] = aabbMax.getX();
        max->v[1] = aabbMax.getY();
        max->v[2] = aabbMax.getZ();
        max->d = 3;

        PyObject *tuple = PyTuple_New(2);
        PyTuple_SetItem(tuple, 0, (PyObject *)min);
        PyTuple_SetItem(tuple, 1, (PyObject *)max);
        return tuple;
    }

    //! Collision group
    PyObject *PhysicBase_getCollisionFilterGroup(PyObject_PhysicBase *self)
    {
        return PyLong_FromLong(self->component->getCollisionFilterGroup());
    }

    int PhysicBase_setCollisionFilterGroup(PyObject_PhysicBase *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setCollisionFilterGroup(val);
            return 0;
        }
        return -1;
    }

    //! Collision mask
    PyObject *PhysicBase_getCollisionFilterMask(PyObject_PhysicBase *self)
    {
        return PyLong_FromLong(self->component->getCollisionFilterMask());
    }

    int PhysicBase_setCollisionFilterMask(PyObject_PhysicBase *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setCollisionFilterMask(val);
            return 0;
        }
        return -1;
    }

    //! Continous Collision Detection mode
    PyObject *PhysicBase_isCCD(PyObject_PhysicBase *self)
    {
        return PyBool_FromLong(self->component->isCCD());
    }

    int PhysicBase_setCCD(PyObject_PhysicBase *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setCCD(val);
            return 0;
        }
        return -1;
    }

    // Methods definition
    PyMethodDef PhysicBase_methods[] = {
        {"applyTorque", (PyCFunction)PhysicBase_applyTorque, METH_VARARGS, PhysicBase_applyTorque_doc},
        {"applyForce", (PyCFunction)PhysicBase_applyForce, METH_VARARGS, PhysicBase_applyForce_doc},
        {"applyImpulse", (PyCFunction)PhysicBase_applyImpulse, METH_VARARGS, PhysicBase_applyImpulse_doc},
        {"clearForces", (PyCFunction)PhysicBase_clearForces, METH_NOARGS, PhysicBase_clearForces_doc},
        {NULL, NULL},
    };

    // Get/set
    PyGetSetDef PhysicBase_getsets[] = {
        {"mass", (getter)PhysicBase_getMass, (setter)PhysicBase_setMass, PhysicBase_mass_doc, NULL},
        {"friction", (getter)PhysicBase_getFriction, (setter)PhysicBase_setFriction, PhysicBase_friction_doc, NULL},
        {"restitution", (getter)PhysicBase_getRestitution, (setter)PhysicBase_setRestitution, PhysicBase_restitution_doc, NULL},
        {"linearVelocity", (getter)PhysicBase_getLinearVelocity, (setter)PhysicBase_setLinearVelocity, PhysicBase_linearVelocity_doc, NULL},
        {"angularVelocity", (getter)PhysicBase_getAngularVelocity, (setter)PhysicBase_setAngularVelocity, PhysicBase_angularVelocity_doc, NULL},
        {"linearFactor", (getter)PhysicBase_getLinearFactor, (setter)PhysicBase_setLinearFactor, PhysicBase_linearFactor_doc, NULL},
        {"angularFactor", (getter)PhysicBase_getAngularFactor, (setter)PhysicBase_setAngularFactor, PhysicBase_angularFactor_doc, NULL},
        {"isTrigger", (getter)PhysicBase_isTrigger, (setter)PhysicBase_setIsTrigger, PhysicBase_isTrigger_doc, NULL},
        {"isKinematic", (getter)PhysicBase_isKinematic, (setter)PhysicBase_setIsKinematic, PhysicBase_isKinematic_doc, NULL},
        {"isEnabled", (getter)PhysicBase_isEnabled, (setter)PhysicBase_setEnabled, PhysicBase_isEnabled_doc, NULL},
        {"aabb", (getter)PhysicBase_getAABB, NULL, PhysicBase_isEnabled_doc, NULL},
        {"collisionGroup", (getter)PhysicBase_getCollisionFilterGroup, (setter)PhysicBase_setCollisionFilterGroup, PhysicBase_collisionGroup_doc, NULL},
        {"collisionMask", (getter)PhysicBase_getCollisionFilterMask, (setter)PhysicBase_setCollisionFilterMask, PhysicBase_collisionMask_doc, NULL},
        {"continuousDetection", (getter)PhysicBase_isCCD, (setter)PhysicBase_setCCD, PhysicBase_continuousDetection_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_PhysicBase = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.PhysicBase", /* tp_name */
        sizeof(PyObject_PhysicBase),                          /* tp_basicsize */
        0,                                                    /* tp_itemsize */
        (destructor)PhysicBase_dealloc,                       /* tp_dealloc */
        0,                                                    /* tp_print */
        0,                                                    /* tp_getattr */
        0,                                                    /* tp_setattr */
        0,                                                    /* tp_reserved */
        0,                                                    /* tp_repr */
        0,                                                    /* tp_as_number */
        0,                                                    /* tp_as_sequence */
        0,                                                    /* tp_as_mapping */
        0,                                                    /* tp_hash */
        0,                                                    /* tp_call */
        (reprfunc)PhysicBase_str,                             /* tp_str */
        0,                                                    /* tp_getattro */
        0,                                                    /* tp_setattro */
        0,                                                    /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,             /* tp_flags */
        0,                                                    /* tp_doc */
        0,                                                    /* tp_traverse */
        0,                                                    /* tp_clear */
        0,                                                    /* tp_richcompare */
        0,                                                    /* tp_weaklistoffset */
        0,                                                    /* tp_iter */
        0,                                                    /* tp_iternext */
        PhysicBase_methods,                                   /* tp_methods */
        0,                                                    /* tp_members */
        PhysicBase_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                              /* tp_base */
        0,                                                    /* tp_dict */
        0,                                                    /* tp_descr_get */
        0,                                                    /* tp_descr_set */
        0,                                                    /* tp_dictoffset */
        0,                                                    /* tp_init */
        0,                                                    /* tp_alloc */
        0,                                                    /* tp_new */
        0,                                                    /* tp_free */
    };
} // namespace ige::scene
