#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/PhysicObject.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_PhysicObject
    {
        PyObject_Component super;
        PhysicObject *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_PhysicObject;

    // Dealloc
    void PhysicObject_dealloc(PyObject_PhysicObject *self);

    // String represent
    PyObject *PhysicObject_str(PyObject_PhysicObject *self);

    //! Apply torque
    PyObject* PhysicObject_applyTorque(PyObject_PhysicObject *self, PyObject *value);

    //! Apply force
    PyObject* PhysicObject_applyForce(PyObject_PhysicObject *self, PyObject *value);

    //! Apply impulse
    PyObject* PhysicObject_applyImpulse(PyObject_PhysicObject *self, PyObject *value);

    //! Clear forces
    PyObject* PhysicObject_clearForces(PyObject_PhysicObject *self);

    //! Add constraint
    PyObject* PhysicObject_addConstraint(PyObject_PhysicObject *self, PyObject* value);

    //! Remove constraint
    PyObject* PhysicObject_removeConstraint(PyObject_PhysicObject *self, PyObject* value);

    //! Get all constraints
    PyObject* PhysicObject_getConstraints(PyObject_PhysicObject *self);

    //! Remove all constraints
    PyObject* PhysicObject_removeConstraints(PyObject_PhysicObject *self);

    //! Mass
    PyObject *PhysicObject_getMass(PyObject_PhysicObject *self);
    int PhysicObject_setMass(PyObject_PhysicObject *self, PyObject *value);

    //! Friction
    PyObject *PhysicObject_getFriction(PyObject_PhysicObject *self);
    int PhysicObject_setFriction(PyObject_PhysicObject *self, PyObject *value);

    //! Restitution
    PyObject *PhysicObject_getRestitution(PyObject_PhysicObject *self);
    int PhysicObject_setRestitution(PyObject_PhysicObject *self, PyObject *value);

    //! Linear velocity
    PyObject *PhysicObject_getLinearVelocity(PyObject_PhysicObject *self);
    int PhysicObject_setLinearVelocity(PyObject_PhysicObject *self, PyObject *value);

    //! Angular velocity
    PyObject *PhysicObject_getAngularVelocity(PyObject_PhysicObject *self);
    int PhysicObject_setAngularVelocity(PyObject_PhysicObject *self, PyObject *value);

    //! Linear factor
    PyObject *PhysicObject_getLinearFactor(PyObject_PhysicObject *self);
    int PhysicObject_setLinearFactor(PyObject_PhysicObject *self, PyObject *value);

    //! Angular factor
    PyObject *PhysicObject_getAngularFactor(PyObject_PhysicObject *self);
    int PhysicObject_setAngularFactor(PyObject_PhysicObject *self, PyObject *value);

    //! Linear Sleeping Threshold
    PyObject* PhysicObject_getLinearSleepingThreshold(PyObject_PhysicObject* self);
    int PhysicObject_setLinearSleepingThreshold(PyObject_PhysicObject* self, PyObject* value);

    //! Angular Sleeping Threshold
    PyObject* PhysicObject_getAngularSleepingThreshold(PyObject_PhysicObject* self);
    int PhysicObject_setAngularSleepingThreshold(PyObject_PhysicObject* self, PyObject* value);

    //! Set Activation State
    int PhysicObject_setActivationState(PyObject_PhysicObject* self, PyObject* value);

    //! Indicate object is a trigger object
    PyObject *PhysicObject_isTrigger(PyObject_PhysicObject *self);
    int PhysicObject_setIsTrigger(PyObject_PhysicObject *self, PyObject *value);

    //! Indicate object is a kinematic object
    PyObject *PhysicObject_isKinematic(PyObject_PhysicObject *self);
    int PhysicObject_setIsKinematic(PyObject_PhysicObject *self, PyObject *value);

    //! Enable/Disable physic component
    PyObject *PhysicObject_isEnabled(PyObject_PhysicObject *self);
    int PhysicObject_setEnabled(PyObject_PhysicObject *self, PyObject *value);

    //! Get AABB
    PyObject *PhysicObject_getAABB(PyObject_PhysicObject *self);

    //! Collision group
    PyObject* PhysicObject_getCollisionFilterGroup(PyObject_PhysicObject* self);
    int PhysicObject_setCollisionFilterGroup(PyObject_PhysicObject* self, PyObject* value);

    //! Collision mask
    PyObject* PhysicObject_getCollisionFilterMask(PyObject_PhysicObject* self);
    int PhysicObject_setCollisionFilterMask(PyObject_PhysicObject* self, PyObject* value);

    //! Continuos Collision Detection mode
    PyObject* PhysicObject_isCCD(PyObject_PhysicObject* self);
    int PhysicObject_setCCD(PyObject_PhysicObject* self, PyObject* value);

} // namespace ige::scene
