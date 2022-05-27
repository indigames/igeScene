#include "python/pyRigidbody.h"
#include "python/pyRigidbody_doc_en.h"
#include "python/pyPhysicConstraint.h"
#include "python/pyFixedConstraint.h"
#include "python/pyHingeConstraint.h"
#include "python/pySliderConstraint.h"
#include "python/pySpringConstraint.h"
#include "python/pyDof6Constraint.h"

#include "components/physic/Rigidbody.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void Rigidbody_dealloc(PyObject_Rigidbody *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *Rigidbody_str(PyObject_Rigidbody *self)
    {
        return PyUnicode_FromString("C++ Rigidbody object");
    }

    //! Apply torque
    PyObject *Rigidbody_applyTorque(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* pyObj;
        if (PyArg_ParseTuple(value, "O", &pyObj)) {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject*)pyObj, buff, d);
            if (v && d >= 3) {
                std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->applyTorque(PhysicHelper::to_btVector3(*((Vec3*)v)));
            }
        }
        Py_RETURN_NONE;
    }

    //! Apply torque impulse
    PyObject *Rigidbody_applyTorqueImpulse(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* pyObj;
        if (PyArg_ParseTuple(value, "O", &pyObj)) {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject*)pyObj, buff, d);
            if (v && d >= 3) {
                std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->applyTorqueImpulse(PhysicHelper::to_btVector3(*((Vec3*)v)));
            }
        }
        Py_RETURN_NONE;
    }

    //! Apply force
    PyObject *Rigidbody_applyForce(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* pyObj, * pyPosObj = nullptr;
        if (PyArg_ParseTuple(value, "O|O", &pyObj, &pyPosObj)) {
            if (pyObj) {
                int d;
                float buff[4];
                auto v = pyObjToFloat((PyObject*)pyObj, buff, d);
                if (v && d >= 3) {
                    if (pyPosObj) {
                        auto pos = pyObjToFloat((PyObject*)pyPosObj, buff, d);
                        if (pos && d >= 3) {
                            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->applyForce(PhysicHelper::to_btVector3(*((Vec3*)v)), PhysicHelper::to_btVector3(*((Vec3*)pos)));
                            Py_RETURN_NONE;
                        }
                    }
                    std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->applyForce(PhysicHelper::to_btVector3(*((Vec3*)v)));
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Apply impulse
    PyObject *Rigidbody_applyImpulse(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* pyObj, * pyPosObj = nullptr;
        if (PyArg_ParseTuple(value, "O|O", &pyObj, &pyPosObj)) {
            if (pyObj) {
                int d;
                float buff[4];
                auto v = pyObjToFloat((PyObject*)pyObj, buff, d);
                if (v && d >= 3) {
                    if (pyPosObj) {
                        auto pos = pyObjToFloat((PyObject*)pyPosObj, buff, d);
                        if (pos && d >= 3) {
                            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->applyImpulse(PhysicHelper::to_btVector3(*((Vec3*)v)), PhysicHelper::to_btVector3(*((Vec3*)pos)));
                            Py_RETURN_NONE;
                        }
                    }
                    std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->applyImpulse(PhysicHelper::to_btVector3(*((Vec3*)v)));
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Clear forces
    PyObject *Rigidbody_clearForces(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->clearForces();
        Py_RETURN_NONE;
    }

    //! Add constraint
    PyObject* Rigidbody_addConstraint(PyObject_Rigidbody* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int type = -1;
        if (PyArg_ParseTuple(value, "i", &type))
        {
            auto constraint = std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->addConstraint(type);
            if (constraint != nullptr)
            {
                switch (type)
                {
                    case (int)PhysicConstraint::ConstraintType::Fixed:
                    {
                        auto pyConst = (PyObject_FixedConstraint*)(&PyTypeObject_FixedConstraint)->tp_alloc(&PyTypeObject_FixedConstraint, 0);
                        pyConst->constraint = (FixedConstraint*)constraint.get();
                        pyConst->super.constraint = constraint.get();
                        return (PyObject*)pyConst;
                    }
                    case (int)PhysicConstraint::ConstraintType::Hinge:
                    {
                        auto pyConst = (PyObject_HingeConstraint*)(&PyTypeObject_HingeConstraint)->tp_alloc(&PyTypeObject_HingeConstraint, 0);
                        pyConst->constraint = (HingeConstraint*)constraint.get();
                        pyConst->super.constraint = constraint.get();
                        return (PyObject*)pyConst;
                    }
                    case (int)PhysicConstraint::ConstraintType::Slider:
                    {
                        auto pyConst = (PyObject_SliderConstraint*)(&PyTypeObject_SliderConstraint)->tp_alloc(&PyTypeObject_SliderConstraint, 0);
                        pyConst->constraint = (SliderConstraint*)constraint.get();
                        pyConst->super.constraint = constraint.get();
                        return (PyObject*)pyConst;
                    }
                    case (int)PhysicConstraint::ConstraintType::Spring:
                    {
                        auto pyConst = (PyObject_SpringConstraint*)(&PyTypeObject_SpringConstraint)->tp_alloc(&PyTypeObject_SpringConstraint, 0);
                        pyConst->constraint = (SpringConstraint*)constraint.get();
                        pyConst->super.constraint = constraint.get();
                        return (PyObject*)pyConst;
                    }
                    case (int)PhysicConstraint::ConstraintType::Dof6Spring:
                    {
                        auto pyConst = (PyObject_Dof6Constraint*)(&PyTypeObject_Dof6Constraint)->tp_alloc(&PyTypeObject_Dof6Constraint, 0);
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
    PyObject* Rigidbody_removeConstraint(PyObject_Rigidbody* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        if (value->ob_type == &PyTypeObject_PhysicConstraint || value->ob_type == &PyTypeObject_FixedConstraint || value->ob_type == &PyTypeObject_HingeConstraint ||
            value->ob_type == &PyTypeObject_SliderConstraint || value->ob_type == &PyTypeObject_SpringConstraint || value->ob_type == &PyTypeObject_Dof6Constraint)
        {
            auto pyConst = (PyObject_PhysicConstraint*)value;
            if (pyConst && pyConst->constraint)
            {
                std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->removeConstraint(pyConst->constraint);
                Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    //! Get all constraints
    PyObject* Rigidbody_getConstraints(PyObject_Rigidbody* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto len = std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getContraints().size();
        if (len > 0)
        {
            auto constraints = std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getContraints();
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
                            auto pyConst = (PyObject_FixedConstraint*)(&PyTypeObject_FixedConstraint)->tp_alloc(&PyTypeObject_FixedConstraint, 0);
                            pyConst->constraint = (FixedConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                        }
                        break;
                        case (int)PhysicConstraint::ConstraintType::Hinge:
                        {
                            auto pyConst = (PyObject_HingeConstraint*)(&PyTypeObject_HingeConstraint)->tp_alloc(&PyTypeObject_HingeConstraint, 0);
                            pyConst->constraint = (HingeConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                        }
                        break;
                        case (int)PhysicConstraint::ConstraintType::Slider:
                        {
                            auto pyConst = (PyObject_SliderConstraint*)(&PyTypeObject_SliderConstraint)->tp_alloc(&PyTypeObject_SliderConstraint, 0);
                            pyConst->constraint = (SliderConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                        }
                        break;
                        case (int)PhysicConstraint::ConstraintType::Spring:
                        {
                            auto pyConst = (PyObject_SpringConstraint*)(&PyTypeObject_SpringConstraint)->tp_alloc(&PyTypeObject_SpringConstraint, 0);
                            pyConst->constraint = (SpringConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                        }
                        break;
                        case (int)PhysicConstraint::ConstraintType::Dof6Spring:
                        {
                            auto pyConst = (PyObject_Dof6Constraint*)(&PyTypeObject_Dof6Constraint)->tp_alloc(&PyTypeObject_Dof6Constraint, 0);
                            pyConst->constraint = (Dof6SpringConstraint*)constraint.get();
                            pyConst->super.constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
                        }
                        break;
                        default:
                        {
                            auto pyConst = (PyObject_PhysicConstraint*)(&PyTypeObject_PhysicConstraint)->tp_alloc(&PyTypeObject_PhysicConstraint, 0);
                            pyConst->constraint = constraint.get();
                            PyTuple_SetItem(compTuple, i, (PyObject*)pyConst);
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
    PyObject* Rigidbody_removeConstraints(PyObject_Rigidbody* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->removeAllConstraints();
        Py_RETURN_TRUE;
    }

    //! Mass
    PyObject *Rigidbody_getMass(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getMass());
    }

    int Rigidbody_setMass(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setMass(val);
            return 0;
        }
        return -1;
    }

    //! Friction
    PyObject *Rigidbody_getFriction(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getFriction());
    }

    int Rigidbody_setFriction(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setFriction(val);
            return 0;
        }
        return -1;
    }

    //! Restitution
    PyObject *Rigidbody_getRestitution(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getRestitution());
    }

    int Rigidbody_setRestitution(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setRestitution(val);
            return 0;
        }
        return -1;
    }

    //! Linear velocity
    PyObject *Rigidbody_getLinearVelocity(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getLinearVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Rigidbody_setLinearVelocity(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setLinearVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Angular velocity
    PyObject *Rigidbody_getAngularVelocity(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getAngularVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Rigidbody_setAngularVelocity(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setAngularVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Linear factor
    PyObject *Rigidbody_getLinearFactor(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getLinearFactor()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Rigidbody_setLinearFactor(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setLinearFactor(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Angular factor
    PyObject *Rigidbody_getAngularFactor(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getAngularFactor()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Rigidbody_setAngularFactor(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setAngularFactor(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    PyObject* Rigidbody_getLinearSleepingThreshold(PyObject_Rigidbody* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getLinearSleepingThreshold());
    }

    int Rigidbody_setLinearSleepingThreshold(PyObject_Rigidbody* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setLinearSleepingThreshold((uint32_t)PyLong_AsLong(value));
            return 0;
        }
        return -1;
    }

    PyObject* Rigidbody_getAngularSleepingThreshold(PyObject_Rigidbody* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getAngularSleepingThreshold());
    }

    int Rigidbody_setAngularSleepingThreshold(PyObject_Rigidbody* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setAngularSleepingThreshold((uint32_t)PyLong_AsLong(value));
            return 0;
        }
        return -1;
    }

    PyObject* Rigidbody_getActivationState(PyObject_Rigidbody* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getActivationState());
    }

    int Rigidbody_setActivationState(PyObject_Rigidbody* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setActivationState(val);
            return 0;
        }
        return -1;
    }

    //! Indicate object is a trigger object
    PyObject *Rigidbody_isTrigger(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->isTrigger());
    }

    int Rigidbody_setIsTrigger(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setIsTrigger(val);
            return 0;
        }
        return -1;
    }

    //! Indicate object is a kinematic object
    PyObject *Rigidbody_isKinematic(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->isKinematic());
    }

    int Rigidbody_setIsKinematic(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setIsKinematic(val);
            return 0;
        }
        return -1;
    }

    //! Enable/Disable physic component
    PyObject *Rigidbody_isEnabled(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->isEnabled());
    }

    int Rigidbody_setEnabled(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setEnabled(val);
            return 0;
        }
        return -1;
    }

    //! Get AABB
    PyObject *Rigidbody_getAABB(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto aabb = std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getAABB();

        vec_obj *min = PyObject_New(vec_obj, _Vec3Type);
        vec_obj *max = PyObject_New(vec_obj, _Vec3Type);
        min->v[0] = aabb.MinEdge.X();
        min->v[1] = aabb.MinEdge.Y();
        min->v[2] = aabb.MinEdge.Z();
        min->d = 3;

        max->v[0] = aabb.MaxEdge.X();
        max->v[1] = aabb.MaxEdge.Y();
        max->v[2] = aabb.MaxEdge.Z();
        max->d = 3;

        PyObject *tuple = PyTuple_New(2);
        PyTuple_SetItem(tuple, 0, (PyObject *)min);
        PyTuple_SetItem(tuple, 1, (PyObject *)max);
        return tuple;
    }

    //! Collision group
    PyObject *Rigidbody_getCollisionFilterGroup(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getCollisionFilterGroup());
    }

    int Rigidbody_setCollisionFilterGroup(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setCollisionFilterGroup(val);
            return 0;
        }
        return -1;
    }

    //! Collision mask
    PyObject *Rigidbody_getCollisionFilterMask(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getCollisionFilterMask());
    }

    int Rigidbody_setCollisionFilterMask(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setCollisionFilterMask(val);
            return 0;
        }
        return -1;
    }

    //! Continuos Collision Detection mode
    PyObject *Rigidbody_isCCD(PyObject_Rigidbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->isCCD());
    }

    int Rigidbody_setCCD(PyObject_Rigidbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setCCD(val);
            return 0;
        }
        return -1;
    }

    //! Position offset
    PyObject* Rigidbody_getOffset(PyObject_Rigidbody* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->getPositionOffset().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    int Rigidbody_setOffset(PyObject_Rigidbody* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v || d < 3)
            return -1;
        std::dynamic_pointer_cast<Rigidbody>(self->component.lock())->setPositionOffset(*((Vec3*)v));
        return 0;
    }

    // Methods definition
    PyMethodDef Rigidbody_methods[] = {
        {"applyTorque", (PyCFunction)Rigidbody_applyTorque, METH_VARARGS, Rigidbody_applyTorque_doc},
        {"applyTorqueImpulse", (PyCFunction)Rigidbody_applyTorqueImpulse, METH_VARARGS, Rigidbody_applyTorqueImpulse_doc},
        {"applyForce", (PyCFunction)Rigidbody_applyForce, METH_VARARGS, Rigidbody_applyForce_doc},
        {"applyImpulse", (PyCFunction)Rigidbody_applyImpulse, METH_VARARGS, Rigidbody_applyImpulse_doc},
        {"clearForces", (PyCFunction)Rigidbody_clearForces, METH_NOARGS, Rigidbody_clearForces_doc},
        {"addConstraint", (PyCFunction)Rigidbody_addConstraint, METH_VARARGS, Rigidbody_addConstraint_doc},
        {"removeConstraint", (PyCFunction)Rigidbody_removeConstraint, METH_VARARGS, Rigidbody_removeConstraint_doc},
        {"getConstraints", (PyCFunction)Rigidbody_getConstraints, METH_NOARGS, Rigidbody_getConstraints_doc},
        {"removeConstraints", (PyCFunction)Rigidbody_removeConstraints, METH_NOARGS, Rigidbody_removeConstraints_doc},
        {NULL, NULL},
    };

    // Get/set
    PyGetSetDef Rigidbody_getsets[] = {
        {"mass", (getter)Rigidbody_getMass, (setter)Rigidbody_setMass, Rigidbody_mass_doc, NULL},
        {"friction", (getter)Rigidbody_getFriction, (setter)Rigidbody_setFriction, Rigidbody_friction_doc, NULL},
        {"restitution", (getter)Rigidbody_getRestitution, (setter)Rigidbody_setRestitution, Rigidbody_restitution_doc, NULL},
        {"linearVelocity", (getter)Rigidbody_getLinearVelocity, (setter)Rigidbody_setLinearVelocity, Rigidbody_linearVelocity_doc, NULL},
        {"angularVelocity", (getter)Rigidbody_getAngularVelocity, (setter)Rigidbody_setAngularVelocity, Rigidbody_angularVelocity_doc, NULL},
        {"linearFactor", (getter)Rigidbody_getLinearFactor, (setter)Rigidbody_setLinearFactor, Rigidbody_linearFactor_doc, NULL},
        {"angularFactor", (getter)Rigidbody_getAngularFactor, (setter)Rigidbody_setAngularFactor, Rigidbody_angularFactor_doc, NULL},
        {"linearSleepingThreshold", (getter)Rigidbody_getLinearSleepingThreshold, (setter)Rigidbody_setLinearSleepingThreshold, NULL, NULL},
        {"angularSleepingThreshold", (getter)Rigidbody_getAngularSleepingThreshold, (setter)Rigidbody_setAngularSleepingThreshold, NULL, NULL},
        {"isTrigger", (getter)Rigidbody_isTrigger, (setter)Rigidbody_setIsTrigger, Rigidbody_isTrigger_doc, NULL},
        {"isKinematic", (getter)Rigidbody_isKinematic, (setter)Rigidbody_setIsKinematic, Rigidbody_isKinematic_doc, NULL},
        {"isEnabled", (getter)Rigidbody_isEnabled, (setter)Rigidbody_setEnabled, Rigidbody_isEnabled_doc, NULL},
        {"aabb", (getter)Rigidbody_getAABB, NULL, Rigidbody_isEnabled_doc, NULL},
        {"collisionGroup", (getter)Rigidbody_getCollisionFilterGroup, (setter)Rigidbody_setCollisionFilterGroup, Rigidbody_collisionGroup_doc, NULL},
        {"collisionMask", (getter)Rigidbody_getCollisionFilterMask, (setter)Rigidbody_setCollisionFilterMask, Rigidbody_collisionMask_doc, NULL},
        {"continuousDetection", (getter)Rigidbody_isCCD, (setter)Rigidbody_setCCD, Rigidbody_continuousDetection_doc, NULL},
        {"activationState", (getter)Rigidbody_getActivationState, (setter)Rigidbody_setActivationState, Rigidbody_activationState_doc, NULL},
        {"offset", (getter)Rigidbody_getOffset, (setter)Rigidbody_setOffset, Rigidbody_offset_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_Rigidbody = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.Rigidbody", /* tp_name */
        sizeof(PyObject_Rigidbody),                          /* tp_basicsize */
        0,                                                      /* tp_itemsize */
        (destructor)Rigidbody_dealloc,                       /* tp_dealloc */
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
        (reprfunc)Rigidbody_str,                             /* tp_str */
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
        Rigidbody_methods,                                   /* tp_methods */
        0,                                                      /* tp_members */
        Rigidbody_getsets,                                   /* tp_getset */
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
