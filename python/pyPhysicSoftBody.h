#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/PhysicSoftBody.h"

#include "python/pyPhysicObject.h"

namespace ige::scene
{
    struct PyObject_PhysicSoftBody : PyObject_PhysicObject {};

    // Type declaration
    extern PyTypeObject PyTypeObject_PhysicSoftBody;

    // Dealloc
    void PhysicSoftBody_dealloc(PyObject_PhysicSoftBody *self);

    // String represent
    PyObject *PhysicSoftBody_str(PyObject_PhysicSoftBody *self);

    //! Apply Repulsion Force
    PyObject* PhysicSoftBody_applyRepulsionForce(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Add Velocity
    PyObject* PhysicSoftBody_addVelocity(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Get Volume
    PyObject* PhysicSoftBody_getVolume(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Get Center Of Mass
    PyObject* PhysicSoftBody_getCenterOfMass(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Get nearest node
    PyObject* PhysicSoftBody_getNearestNodeIndex(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Get node position
    PyObject* PhysicSoftBody_getNodePosition(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Get node normal
    PyObject* PhysicSoftBody_getNodeNormal(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Append deformable anchor
    PyObject* PhysicSoftBody_appendDeformableAnchor(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Append anchor
    PyObject* PhysicSoftBody_appendAnchor(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Remove anchor
    PyObject* PhysicSoftBody_removeAnchor(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Damping coefficient
    PyObject *PhysicSoftBody_getDampingCoeff(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setDampingCoeff(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Pressure coefficient
    PyObject *PhysicSoftBody_getPressureCoeff(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setPressureCoeff(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Volume conversation coefficient
    PyObject *PhysicSoftBody_getVolumeConvCoeff(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setVolumeConvCoeff(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Dynamic friction coefficient
    PyObject *PhysicSoftBody_getDynamicFrictionCoeff(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setDynamicFrictionCoeff(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Pose matching coefficient
    PyObject *PhysicSoftBody_getPoseMatchCoeff(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setPoseMatchCoeff(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Repulsion Stiffness
    PyObject *PhysicSoftBody_getRepulsionStiffness(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setRepulsionStiffness(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Sleeping Threshold
    PyObject *PhysicSoftBody_getSleepingThreshold(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setSleepingThreshold(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Rest Length Scale
    PyObject *PhysicSoftBody_getRestLengthScale(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setRestLengthScale(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Gravity factor
    PyObject *PhysicSoftBody_getGravityFactor(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setGravityFactor(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Velocities correction factor
    PyObject *PhysicSoftBody_getVelocityFactor(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setVelocityFactor(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Rigid contacts hardness
    PyObject *PhysicSoftBody_getRigidContactHardness(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setRigidContactHardness(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Kinetic contacts hardness
    PyObject *PhysicSoftBody_getKineticContactHardness(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setKineticContactHardness(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Soft contacts hardness
    PyObject *PhysicSoftBody_getSoftContactHardness(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setSoftContactHardness(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Anchor hardness
    PyObject *PhysicSoftBody_getAnchorHardness(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setAnchorHardness(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Anchor hardness
    PyObject *PhysicSoftBody_getPosIterationNumber(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setPosIterationNumber(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Aero Model
    PyObject *PhysicSoftBody_getAeroModel(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setAeroModel(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Self collision
    PyObject *PhysicSoftBody_isSelfCollision(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setSelfCollision(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Soft soft-collision
    PyObject *PhysicSoftBody_isSoftSoftCollision(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setSoftSoftCollision(PyObject_PhysicSoftBody *self, PyObject *value);

    //! Wind velocity
    PyObject *PhysicSoftBody_getWindVelocity(PyObject_PhysicSoftBody *self);
    int PhysicSoftBody_setWindVelocity(PyObject_PhysicSoftBody *self, PyObject *value);

} // namespace ige::scene
