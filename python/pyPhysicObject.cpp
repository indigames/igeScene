#include "python/pyPhysicObject.h"
#include "python/pyPhysicObject_doc_en.h"
#include "python/pyPhysicConstraint.h"
#include "python/pyFixedConstraint.h"
#include "python/pyHingeConstraint.h"
#include "python/pySliderConstraint.h"
#include "python/pySpringConstraint.h"
#include "python/pyDof6Constraint.h"

#include "components/physic/PhysicObject.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void PhysicObject_dealloc(PyObject_PhysicObject *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *PhysicObject_str(PyObject_PhysicObject *self)
    {
        return PyUnicode_FromString("C++ PhysicObject object");
    }

    //! Apply torque
    PyObject *PhysicObject_applyTorque(PyObject_PhysicObject *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (v)
        {
            self->component->applyTorque(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        }
        Py_RETURN_NONE;
    }

    //! Apply force
    PyObject *PhysicObject_applyForce(PyObject_PhysicObject *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (v)
        {
            self->component->applyForce(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        }
        Py_RETURN_NONE;
    }

    //! Apply impulse
    PyObject *PhysicObject_applyImpulse(PyObject_PhysicObject *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (v)
        {
            self->component->applyImpulse(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        }
        Py_RETURN_NONE;
    }

    //! Clear forces
    PyObject *PhysicObject_clearForces(PyObject_PhysicObject *self)
    {
        self->component->clearForces();
        Py_RETURN_NONE;
    }

    //! Add constraint
    PyObject* PhysicObject_addConstraint(PyObject_PhysicObject* self, PyObject* value)
    {
        int type = -1;
        if (PyArg_ParseTuple(value, "i", &type))
        {
            auto constraint = self->component->addConstraint(type);
            if (constraint != nullptr)
            {
                switch (type)
                {
                    case (int)PhysicConstraint::ConstraintType::Fixed:
                    {
                        auto pyConst = PyObject_New(PyObject_FixedConstraint, &PyTypeObject_FixedConstraint);
                        pyConst->constraint = (FixedConstraint*)constraint.get();
                        pyConst->super.constraint = constraint.get();
                        return (PyObject*)pyConst;
                    }                    
                    case (int)PhysicConstraint::ConstraintType::Hinge:
                    {
                        auto pyConst = PyObject_New(PyObject_HingeConstraint, &PyTypeObject_HingeConstraint);
                        pyConst->constraint = (HingeConstraint*)constraint.get();
                        pyConst->super.constraint = constraint.get();
                        return (PyObject*)pyConst;
                    }                    
                    case (int)PhysicConstraint::ConstraintType::Slider:
                    {
                        auto pyConst = PyObject_New(PyObject_SliderConstraint, &PyTypeObject_SliderConstraint);
                        pyConst->constraint = (SliderConstraint*)constraint.get();
                        pyConst->super.constraint = constraint.get();
                        return (PyObject*)pyConst;
                    }
                    case (int)PhysicConstraint::ConstraintType::Spring:
                    {
                        auto pyConst = PyObject_New(PyObject_SpringConstraint, &PyTypeObject_SpringConstraint);
                        pyConst->constraint = (SpringConstraint*)constraint.get();
                        pyConst->super.constraint = constraint.get();
                        return (PyObject*)pyConst;
                    }
                    case (int)PhysicConstraint::ConstraintType::Dof6Spring:
                    {
                        auto pyConst = PyObject_New(PyObject_Dof6Constraint, &PyTypeObject_Dof6Constraint);
                        pyConst->constraint = (Dof6SpringConstraint*)constraint.get();
                        pyConst->super.constraint = constraint.get();
                        return (PyObject*)pyConst;
                    }
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Remove constraint
    PyObject* PhysicObject_removeConstraint(PyObject_PhysicObject* self, PyObject* value)
    {
        if (value->ob_type == &PyTypeObject_PhysicConstraint || value->ob_type == &PyTypeObject_FixedConstraint || value->ob_type == &PyTypeObject_HingeConstraint ||
            value->ob_type == &PyTypeObject_SliderConstraint || value->ob_type == &PyTypeObject_SpringConstraint || value->ob_type == &PyTypeObject_Dof6Constraint)
        {
            auto pyConst = (PyObject_PhysicConstraint*)value;
            if (pyConst && pyConst->constraint)
            {
                self->component->removeConstraint(pyConst->constraint);
                Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    //! Get all constraints
    PyObject* PhysicObject_getConstraints(PyObject_PhysicObject* self)
    {
        auto len = self->component->getContraints().size();
        if (len > 0)
        {
            auto constraints = self->component->getContraints();
            auto compTuple = PyTuple_New(len);
            for (int i = 0; i < len; ++i)
            {
                if (constraints[i] == nullptr)
                    continue;

                auto constraint = constraints[i];
                if (constraint != nullptr)
                {
                    switch ((int)constraint->getType())
                    {
                        case (int)PhysicConstraint::ConstraintType::Fixed:
                        {
                            auto pyConst = PyObject_New(PyObject_FixedConstraint, &PyTypeObject_FixedConstraint);
                            pyConst->constraint = (FixedConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                            Py_XDECREF(pyConst);
                        }
                        break;
                        case (int)PhysicConstraint::ConstraintType::Hinge:
                        {
                            auto pyConst = PyObject_New(PyObject_HingeConstraint, &PyTypeObject_HingeConstraint);
                            pyConst->constraint = (HingeConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                            Py_XDECREF(pyConst);
                        }
                        break;
                        case (int)PhysicConstraint::ConstraintType::Slider:
                        {
                            auto pyConst = PyObject_New(PyObject_SliderConstraint, &PyTypeObject_SliderConstraint);
                            pyConst->constraint = (SliderConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                            Py_XDECREF(pyConst);
                        }
                        break;
                        case (int)PhysicConstraint::ConstraintType::Spring:
                        {
                            auto pyConst = PyObject_New(PyObject_SpringConstraint, &PyTypeObject_SpringConstraint);
                            pyConst->constraint = (SpringConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                            Py_XDECREF(pyConst);
                        }
                        break;
                        case (int)PhysicConstraint::ConstraintType::Dof6Spring:
                        {
                            auto pyConst = PyObject_New(PyObject_Dof6Constraint, &PyTypeObject_Dof6Constraint);
                            pyConst->constraint = (Dof6SpringConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                            Py_XDECREF(pyConst);
                        }
                        break;
                        default:
                        {
                            auto pyConst = PyObject_New(PyObject_PhysicConstraint, &PyTypeObject_PhysicConstraint);
                            pyConst->constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                            Py_XDECREF(pyConst);
                        }
                        break;
                    }
                }
            }
            return (PyObject*)compTuple;
        }
        Py_RETURN_NONE;
    }

    //! Remove all constraints
    PyObject* PhysicObject_removeConstraints(PyObject_PhysicObject* self)
    {
        self->component->removeAllConstraints();
        Py_RETURN_TRUE;
    }

    //! Mass
    PyObject *PhysicObject_getMass(PyObject_PhysicObject *self)
    {
        return PyFloat_FromDouble(self->component->getMass());
    }

    int PhysicObject_setMass(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_getFriction(PyObject_PhysicObject *self)
    {
        return PyFloat_FromDouble(self->component->getFriction());
    }

    int PhysicObject_setFriction(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_getRestitution(PyObject_PhysicObject *self)
    {
        return PyFloat_FromDouble(self->component->getRestitution());
    }

    int PhysicObject_setRestitution(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_getLinearVelocity(PyObject_PhysicObject *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getLinearVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicObject_setLinearVelocity(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_getAngularVelocity(PyObject_PhysicObject *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getAngularVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicObject_setAngularVelocity(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_getLinearFactor(PyObject_PhysicObject *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getLinearFactor()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicObject_setLinearFactor(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_getAngularFactor(PyObject_PhysicObject *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getAngularFactor()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicObject_setAngularFactor(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_isTrigger(PyObject_PhysicObject *self)
    {
        return PyBool_FromLong(self->component->isTrigger());
    }

    int PhysicObject_setIsTrigger(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_isKinematic(PyObject_PhysicObject *self)
    {
        return PyBool_FromLong(self->component->isKinematic());
    }

    int PhysicObject_setIsKinematic(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_isEnabled(PyObject_PhysicObject *self)
    {
        return PyBool_FromLong(self->component->isEnabled());
    }

    int PhysicObject_setEnabled(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_getAABB(PyObject_PhysicObject *self)
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
    PyObject *PhysicObject_getCollisionFilterGroup(PyObject_PhysicObject *self)
    {
        return PyLong_FromLong(self->component->getCollisionFilterGroup());
    }

    int PhysicObject_setCollisionFilterGroup(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_getCollisionFilterMask(PyObject_PhysicObject *self)
    {
        return PyLong_FromLong(self->component->getCollisionFilterMask());
    }

    int PhysicObject_setCollisionFilterMask(PyObject_PhysicObject *self, PyObject *value)
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
    PyObject *PhysicObject_isCCD(PyObject_PhysicObject *self)
    {
        return PyBool_FromLong(self->component->isCCD());
    }

    int PhysicObject_setCCD(PyObject_PhysicObject *self, PyObject *value)
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
    PyMethodDef PhysicObject_methods[] = {
        {"applyTorque", (PyCFunction)PhysicObject_applyTorque, METH_VARARGS, PhysicObject_applyTorque_doc},
        {"applyForce", (PyCFunction)PhysicObject_applyForce, METH_VARARGS, PhysicObject_applyForce_doc},
        {"applyImpulse", (PyCFunction)PhysicObject_applyImpulse, METH_VARARGS, PhysicObject_applyImpulse_doc},
        {"clearForces", (PyCFunction)PhysicObject_clearForces, METH_NOARGS, PhysicObject_clearForces_doc},
        {"addConstraint", (PyCFunction)PhysicObject_addConstraint, METH_VARARGS, PhysicObject_addConstraint_doc},
        {"removeConstraint", (PyCFunction)PhysicObject_removeConstraint, METH_VARARGS, PhysicObject_removeConstraint_doc},
        {"getConstraints", (PyCFunction)PhysicObject_getConstraints, METH_NOARGS, PhysicObject_getConstraints_doc},
        {"removeConstraints", (PyCFunction)PhysicObject_removeConstraints, METH_NOARGS, PhysicObject_removeConstraints_doc},
        {NULL, NULL},
    };

    // Get/set
    PyGetSetDef PhysicObject_getsets[] = {
        {"mass", (getter)PhysicObject_getMass, (setter)PhysicObject_setMass, PhysicObject_mass_doc, NULL},
        {"friction", (getter)PhysicObject_getFriction, (setter)PhysicObject_setFriction, PhysicObject_friction_doc, NULL},
        {"restitution", (getter)PhysicObject_getRestitution, (setter)PhysicObject_setRestitution, PhysicObject_restitution_doc, NULL},
        {"linearVelocity", (getter)PhysicObject_getLinearVelocity, (setter)PhysicObject_setLinearVelocity, PhysicObject_linearVelocity_doc, NULL},
        {"angularVelocity", (getter)PhysicObject_getAngularVelocity, (setter)PhysicObject_setAngularVelocity, PhysicObject_angularVelocity_doc, NULL},
        {"linearFactor", (getter)PhysicObject_getLinearFactor, (setter)PhysicObject_setLinearFactor, PhysicObject_linearFactor_doc, NULL},
        {"angularFactor", (getter)PhysicObject_getAngularFactor, (setter)PhysicObject_setAngularFactor, PhysicObject_angularFactor_doc, NULL},
        {"isTrigger", (getter)PhysicObject_isTrigger, (setter)PhysicObject_setIsTrigger, PhysicObject_isTrigger_doc, NULL},
        {"isKinematic", (getter)PhysicObject_isKinematic, (setter)PhysicObject_setIsKinematic, PhysicObject_isKinematic_doc, NULL},
        {"isEnabled", (getter)PhysicObject_isEnabled, (setter)PhysicObject_setEnabled, PhysicObject_isEnabled_doc, NULL},
        {"aabb", (getter)PhysicObject_getAABB, NULL, PhysicObject_isEnabled_doc, NULL},
        {"collisionGroup", (getter)PhysicObject_getCollisionFilterGroup, (setter)PhysicObject_setCollisionFilterGroup, PhysicObject_collisionGroup_doc, NULL},
        {"collisionMask", (getter)PhysicObject_getCollisionFilterMask, (setter)PhysicObject_setCollisionFilterMask, PhysicObject_collisionMask_doc, NULL},
        {"continuousDetection", (getter)PhysicObject_isCCD, (setter)PhysicObject_setCCD, PhysicObject_continuousDetection_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_PhysicObject = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.PhysicObject", /* tp_name */
        sizeof(PyObject_PhysicObject),                          /* tp_basicsize */
        0,                                                      /* tp_itemsize */
        (destructor)PhysicObject_dealloc,                       /* tp_dealloc */
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
        (reprfunc)PhysicObject_str,                             /* tp_str */
        0,                                                      /* tp_getattro */
        0,                                                      /* tp_setattro */
        0,                                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,               /* tp_flags */
        0,                                                      /* tp_doc */
        0,                                                      /* tp_traverse */
        0,                                                      /* tp_clear */
        0,                                                      /* tp_richcompare */
        0,                                                      /* tp_weaklistoffset */
        0,                                                      /* tp_iter */
        0,                                                      /* tp_iternext */
        PhysicObject_methods,                                   /* tp_methods */
        0,                                                      /* tp_members */
        PhysicObject_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                                /* tp_base */
        0,                                                      /* tp_dict */
        0,                                                      /* tp_descr_get */
        0,                                                      /* tp_descr_set */
        0,                                                      /* tp_dictoffset */
        0,                                                      /* tp_init */
        0,                                                      /* tp_alloc */
        0,                                                      /* tp_new */
        0,                                                      /* tp_free */
    };
} // namespace ige::scene
