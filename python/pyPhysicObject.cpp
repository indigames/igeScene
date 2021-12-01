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
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *PhysicObject_str(PyObject_PhysicObject *self)
    {
        return PyUnicode_FromString("C++ PhysicObject object");
    }

    //! Apply torque
    PyObject *PhysicObject_applyTorque(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* pyObj;
        if (PyArg_ParseTuple(value, "O", &pyObj)) {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject*)pyObj, buff, d);
            if (v && d >= 3) {
                std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->applyTorque(PhysicHelper::to_btVector3(*((Vec3*)v)));
            }
        }
        Py_RETURN_NONE;
    }

    //! Apply torque impulse
    PyObject *PhysicObject_applyTorqueImpulse(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* pyObj;
        if (PyArg_ParseTuple(value, "O", &pyObj)) {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject*)pyObj, buff, d);
            if (v && d >= 3) {
                std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->applyTorqueImpulse(PhysicHelper::to_btVector3(*((Vec3*)v)));
            }
        }
        Py_RETURN_NONE;
    }

    //! Apply force
    PyObject *PhysicObject_applyForce(PyObject_PhysicObject *self, PyObject *value)
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
                            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->applyForce(PhysicHelper::to_btVector3(*((Vec3*)v)), PhysicHelper::to_btVector3(*((Vec3*)pos)));
                            Py_RETURN_NONE;
                        }
                    }
                    std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->applyForce(PhysicHelper::to_btVector3(*((Vec3*)v)));
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Apply impulse
    PyObject *PhysicObject_applyImpulse(PyObject_PhysicObject *self, PyObject *value)
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
                            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->applyImpulse(PhysicHelper::to_btVector3(*((Vec3*)v)), PhysicHelper::to_btVector3(*((Vec3*)pos)));
                            Py_RETURN_NONE;
                        }
                    }
                    std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->applyImpulse(PhysicHelper::to_btVector3(*((Vec3*)v)));
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Clear forces
    PyObject *PhysicObject_clearForces(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->clearForces();
        Py_RETURN_NONE;
    }

    //! Add constraint
    PyObject* PhysicObject_addConstraint(PyObject_PhysicObject* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int type = -1;
        if (PyArg_ParseTuple(value, "i", &type))
        {
            auto constraint = std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->addConstraint(type);
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
    PyObject* PhysicObject_removeConstraint(PyObject_PhysicObject* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        if (value->ob_type == &PyTypeObject_PhysicConstraint || value->ob_type == &PyTypeObject_FixedConstraint || value->ob_type == &PyTypeObject_HingeConstraint ||
            value->ob_type == &PyTypeObject_SliderConstraint || value->ob_type == &PyTypeObject_SpringConstraint || value->ob_type == &PyTypeObject_Dof6Constraint)
        {
            auto pyConst = (PyObject_PhysicConstraint*)value;
            if (pyConst && pyConst->constraint)
            {
                std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->removeConstraint(pyConst->constraint);
                Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    //! Get all constraints
    PyObject* PhysicObject_getConstraints(PyObject_PhysicObject* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto len = std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getContraints().size();
        if (len > 0)
        {
            auto constraints = std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getContraints();
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
    PyObject* PhysicObject_removeConstraints(PyObject_PhysicObject* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->removeAllConstraints();
        Py_RETURN_TRUE;
    }

    //! Mass
    PyObject *PhysicObject_getMass(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getMass());
    }

    int PhysicObject_setMass(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setMass(val);
            return 0;
        }
        return -1;
    }

    //! Friction
    PyObject *PhysicObject_getFriction(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getFriction());
    }

    int PhysicObject_setFriction(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setFriction(val);
            return 0;
        }
        return -1;
    }

    //! Restitution
    PyObject *PhysicObject_getRestitution(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getRestitution());
    }

    int PhysicObject_setRestitution(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setRestitution(val);
            return 0;
        }
        return -1;
    }

    //! Linear velocity
    PyObject *PhysicObject_getLinearVelocity(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getLinearVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicObject_setLinearVelocity(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setLinearVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Angular velocity
    PyObject *PhysicObject_getAngularVelocity(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getAngularVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicObject_setAngularVelocity(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setAngularVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Linear factor
    PyObject *PhysicObject_getLinearFactor(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getLinearFactor()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicObject_setLinearFactor(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setLinearFactor(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    //! Angular factor
    PyObject *PhysicObject_getAngularFactor(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getAngularFactor()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicObject_setAngularFactor(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setAngularFactor(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    PyObject* PhysicObject_getLinearSleepingThreshold(PyObject_PhysicObject* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getLinearSleepingThreshold());
    }

    int PhysicObject_setLinearSleepingThreshold(PyObject_PhysicObject* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setLinearSleepingThreshold((uint32_t)PyLong_AsLong(value));
            return 0;
        }
        return -1;
    }

    PyObject* PhysicObject_getAngularSleepingThreshold(PyObject_PhysicObject* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getAngularSleepingThreshold());
    }

    int PhysicObject_setAngularSleepingThreshold(PyObject_PhysicObject* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setAngularSleepingThreshold((uint32_t)PyLong_AsLong(value));
            return 0;
        }
        return -1;
    }

    PyObject* PhysicObject_getActivationState(PyObject_PhysicObject* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getActivationState());
    }

    int PhysicObject_setActivationState(PyObject_PhysicObject* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setActivationState(val);
            return 0;
        }
        return -1;
    }

    //! Indicate object is a trigger object
    PyObject *PhysicObject_isTrigger(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->isTrigger());
    }

    int PhysicObject_setIsTrigger(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setIsTrigger(val);
            return 0;
        }
        return -1;
    }

    //! Indicate object is a kinematic object
    PyObject *PhysicObject_isKinematic(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->isKinematic());
    }

    int PhysicObject_setIsKinematic(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setIsKinematic(val);
            return 0;
        }
        return -1;
    }

    //! Enable/Disable physic component
    PyObject *PhysicObject_isEnabled(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->isEnabled());
    }

    int PhysicObject_setEnabled(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setEnabled(val);
            return 0;
        }
        return -1;
    }

    //! Get AABB
    PyObject *PhysicObject_getAABB(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto aabb = std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getAABB();

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
    PyObject *PhysicObject_getCollisionFilterGroup(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getCollisionFilterGroup());
    }

    int PhysicObject_setCollisionFilterGroup(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setCollisionFilterGroup(val);
            return 0;
        }
        return -1;
    }

    //! Collision mask
    PyObject *PhysicObject_getCollisionFilterMask(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getCollisionFilterMask());
    }

    int PhysicObject_setCollisionFilterMask(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setCollisionFilterMask(val);
            return 0;
        }
        return -1;
    }

    //! Continuos Collision Detection mode
    PyObject *PhysicObject_isCCD(PyObject_PhysicObject *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->isCCD());
    }

    int PhysicObject_setCCD(PyObject_PhysicObject *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setCCD(val);
            return 0;
        }
        return -1;
    }

    //! Position offset
    PyObject* PhysicObject_getOffset(PyObject_PhysicObject* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->getPositionOffset().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    int PhysicObject_setOffset(PyObject_PhysicObject* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v || d < 3)
            return -1;
        std::dynamic_pointer_cast<PhysicObject>(self->component.lock())->setPositionOffset(*((Vec3*)v));
        return 0;
    }

    // Methods definition
    PyMethodDef PhysicObject_methods[] = {
        {"applyTorque", (PyCFunction)PhysicObject_applyTorque, METH_VARARGS, PhysicObject_applyTorque_doc},
        {"applyTorqueImpulse", (PyCFunction)PhysicObject_applyTorqueImpulse, METH_VARARGS, PhysicObject_applyTorqueImpulse_doc},
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
        {"linearSleepingThreshold", (getter)PhysicObject_getLinearSleepingThreshold, (setter)PhysicObject_setLinearSleepingThreshold, NULL, NULL},
        {"angularSleepingThreshold", (getter)PhysicObject_getAngularSleepingThreshold, (setter)PhysicObject_setAngularSleepingThreshold, NULL, NULL},
        {"isTrigger", (getter)PhysicObject_isTrigger, (setter)PhysicObject_setIsTrigger, PhysicObject_isTrigger_doc, NULL},
        {"isKinematic", (getter)PhysicObject_isKinematic, (setter)PhysicObject_setIsKinematic, PhysicObject_isKinematic_doc, NULL},
        {"isEnabled", (getter)PhysicObject_isEnabled, (setter)PhysicObject_setEnabled, PhysicObject_isEnabled_doc, NULL},
        {"aabb", (getter)PhysicObject_getAABB, NULL, PhysicObject_isEnabled_doc, NULL},
        {"collisionGroup", (getter)PhysicObject_getCollisionFilterGroup, (setter)PhysicObject_setCollisionFilterGroup, PhysicObject_collisionGroup_doc, NULL},
        {"collisionMask", (getter)PhysicObject_getCollisionFilterMask, (setter)PhysicObject_setCollisionFilterMask, PhysicObject_collisionMask_doc, NULL},
        {"continuousDetection", (getter)PhysicObject_isCCD, (setter)PhysicObject_setCCD, PhysicObject_continuousDetection_doc, NULL},
        {"activationState", (getter)PhysicObject_getActivationState, (setter)PhysicObject_setActivationState, PhysicObject_activationState_doc, NULL},
        {"offset", (getter)PhysicObject_getOffset, (setter)PhysicObject_setOffset, PhysicObject_offset_doc, NULL},
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
