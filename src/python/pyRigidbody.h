#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/Rigidbody.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_Rigidbody : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_Rigidbody;

    // Dealloc
    void Rigidbody_dealloc(PyObject_Rigidbody *self);

    // String represent
    PyObject *Rigidbody_str(PyObject_Rigidbody *self);

    //! Apply torque
    PyObject* Rigidbody_applyTorque(PyObject_Rigidbody *self, PyObject *value);

    //! Apply torque impulse
    PyObject* Rigidbody_applyTorqueImpulse(PyObject_Rigidbody *self, PyObject *value);

    //! Apply force
    PyObject* Rigidbody_applyForce(PyObject_Rigidbody *self, PyObject *value);

    //! Apply impulse
    PyObject* Rigidbody_applyImpulse(PyObject_Rigidbody *self, PyObject *value);

    //! Clear forces
    PyObject* Rigidbody_clearForces(PyObject_Rigidbody *self);

    //! Add constraint
    PyObject* Rigidbody_addConstraint(PyObject_Rigidbody *self, PyObject* value);

    //! Remove constraint
    PyObject* Rigidbody_removeConstraint(PyObject_Rigidbody *self, PyObject* value);

    //! Get all constraints
    PyObject* Rigidbody_getConstraints(PyObject_Rigidbody *self);

    //! Remove all constraints
    PyObject* Rigidbody_removeConstraints(PyObject_Rigidbody *self);

    //! Mass
    PyObject *Rigidbody_getMass(PyObject_Rigidbody *self);
    int Rigidbody_setMass(PyObject_Rigidbody *self, PyObject *value);

    //! Friction
    PyObject *Rigidbody_getFriction(PyObject_Rigidbody *self);
    int Rigidbody_setFriction(PyObject_Rigidbody *self, PyObject *value);

    //! Restitution
    PyObject *Rigidbody_getRestitution(PyObject_Rigidbody *self);
    int Rigidbody_setRestitution(PyObject_Rigidbody *self, PyObject *value);

    //! Linear velocity
    PyObject *Rigidbody_getLinearVelocity(PyObject_Rigidbody *self);
    int Rigidbody_setLinearVelocity(PyObject_Rigidbody *self, PyObject *value);

    //! Angular velocity
    PyObject *Rigidbody_getAngularVelocity(PyObject_Rigidbody *self);
    int Rigidbody_setAngularVelocity(PyObject_Rigidbody *self, PyObject *value);

    //! Linear factor
    PyObject *Rigidbody_getLinearFactor(PyObject_Rigidbody *self);
    int Rigidbody_setLinearFactor(PyObject_Rigidbody *self, PyObject *value);

    //! Angular factor
    PyObject *Rigidbody_getAngularFactor(PyObject_Rigidbody *self);
    int Rigidbody_setAngularFactor(PyObject_Rigidbody *self, PyObject *value);

    //! Linear Sleeping Threshold
    PyObject* Rigidbody_getLinearSleepingThreshold(PyObject_Rigidbody* self);
    int Rigidbody_setLinearSleepingThreshold(PyObject_Rigidbody* self, PyObject* value);

    //! Angular Sleeping Threshold
    PyObject* Rigidbody_getAngularSleepingThreshold(PyObject_Rigidbody* self);
    int Rigidbody_setAngularSleepingThreshold(PyObject_Rigidbody* self, PyObject* value);

    //! Activation State
    PyObject* Rigidbody_getActivationState(PyObject_Rigidbody* self);
    int Rigidbody_setActivationState(PyObject_Rigidbody* self, PyObject* value);

    //! Indicate object is a trigger object
    PyObject *Rigidbody_isTrigger(PyObject_Rigidbody *self);
    int Rigidbody_setIsTrigger(PyObject_Rigidbody *self, PyObject *value);

    //! Indicate object is a kinematic object
    PyObject *Rigidbody_isKinematic(PyObject_Rigidbody *self);
    int Rigidbody_setIsKinematic(PyObject_Rigidbody *self, PyObject *value);

    //! Enable/Disable physic component
    PyObject *Rigidbody_isEnabled(PyObject_Rigidbody *self);
    int Rigidbody_setEnabled(PyObject_Rigidbody *self, PyObject *value);

    //! Get AABB
    PyObject *Rigidbody_getAABB(PyObject_Rigidbody *self);

    //! Collision group
    PyObject* Rigidbody_getCollisionFilterGroup(PyObject_Rigidbody* self);
    int Rigidbody_setCollisionFilterGroup(PyObject_Rigidbody* self, PyObject* value);

    //! Collision mask
    PyObject* Rigidbody_getCollisionFilterMask(PyObject_Rigidbody* self);
    int Rigidbody_setCollisionFilterMask(PyObject_Rigidbody* self, PyObject* value);

    //! Continuos Collision Detection mode
    PyObject* Rigidbody_isCCD(PyObject_Rigidbody* self);
    int Rigidbody_setCCD(PyObject_Rigidbody* self, PyObject* value);

    //! Position offset
    PyObject* Rigidbody_getOffset(PyObject_Rigidbody* self);
    int Rigidbody_setOffset(PyObject_Rigidbody* self, PyObject* value);
} // namespace ige::scene
