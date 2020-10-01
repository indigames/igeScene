#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/PhysicBase.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_PhysicBase
    {
        PyObject_Component super;
        PhysicBase *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_PhysicBase;

    // Dealloc
    void PhysicBase_dealloc(PyObject_PhysicBase *self);

    // String represent
    PyObject *PhysicBase_str(PyObject_PhysicBase *self);

    //! Apply torque
    void PhysicBase_applyTorque(PyObject_PhysicBase *self, PyObject *value);

    //! Apply force
    void PhysicBase_applyForce(PyObject_PhysicBase *self, PyObject *value);

    //! Apply impulse
    void PhysicBase_applyImpulse(PyObject_PhysicBase *self, PyObject *value);

    //! Clear forces
    void PhysicBase_clearForces(PyObject_PhysicBase *self);

    //! Mass
    PyObject *PhysicBase_getMass(PyObject_PhysicBase *self);
    int PhysicBase_setMass(PyObject_PhysicBase *self, PyObject *value);

    //! Friction
    PyObject *PhysicBase_getFriction(PyObject_PhysicBase *self);
    int PhysicBase_setFriction(PyObject_PhysicBase *self, PyObject *value);

    //! Restitution
    PyObject *PhysicBase_getRestitution(PyObject_PhysicBase *self);
    int PhysicBase_setRestitution(PyObject_PhysicBase *self, PyObject *value);

    //! Linear velocity
    PyObject *PhysicBase_getLinearVelocity(PyObject_PhysicBase *self);
    int PhysicBase_setLinearVelocity(PyObject_PhysicBase *self, PyObject *value);

    //! Angular velocity
    PyObject *PhysicBase_getAngularVelocity(PyObject_PhysicBase *self);
    int PhysicBase_setAngularVelocity(PyObject_PhysicBase *self, PyObject *value);

    //! Linear factor
    PyObject *PhysicBase_getLinearFactor(PyObject_PhysicBase *self);
    int PhysicBase_setLinearFactor(PyObject_PhysicBase *self, PyObject *value);

    //! Angular factor
    PyObject *PhysicBase_getAngularFactor(PyObject_PhysicBase *self);
    int PhysicBase_setAngularFactor(PyObject_PhysicBase *self, PyObject *value);

    //! Indicate object is a trigger object
    PyObject *PhysicBase_isTrigger(PyObject_PhysicBase *self);
    int PhysicBase_setIsTrigger(PyObject_PhysicBase *self, PyObject *value);

    //! Indicate object is a kinematic object
    PyObject *PhysicBase_isKinematic(PyObject_PhysicBase *self);
    int PhysicBase_setIsKinematic(PyObject_PhysicBase *self, PyObject *value);

    //! Enable/Disable physic component
    PyObject *PhysicBase_isEnabled(PyObject_PhysicBase *self);
    int PhysicBase_setEnabled(PyObject_PhysicBase *self, PyObject *value);

    //! Get AABB
    PyObject *PhysicBase_getAABB(PyObject_PhysicBase *self);

    //! Collision group
    PyObject* PhysicBase_getCollisionFilterGroup(PyObject_PhysicBase* self);
    int PhysicBase_setCollisionFilterGroup(PyObject_PhysicBase* self, PyObject* value);

    //! Collision mask
    PyObject* PhysicBase_getCollisionFilterMask(PyObject_PhysicBase* self);
    int PhysicBase_setCollisionFilterMask(PyObject_PhysicBase* self, PyObject* value);

} // namespace ige::scene
