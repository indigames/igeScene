#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/Softbody.h"

#include "python/pyRigidbody.h"

namespace ige::scene
{
    struct PyObject_Softbody : PyObject_Rigidbody {};

    // Type declaration
    extern PyTypeObject PyTypeObject_Softbody;

    // Dealloc
    void Softbody_dealloc(PyObject_Softbody *self);

    // String represent
    PyObject *Softbody_str(PyObject_Softbody *self);

    //! Apply Repulsion Force
    PyObject* Softbody_applyRepulsionForce(PyObject_Softbody *self, PyObject *value);

    //! Add Velocity
    PyObject* Softbody_addVelocity(PyObject_Softbody *self, PyObject *value);

    //! Get Volume
    PyObject* Softbody_getVolume(PyObject_Softbody *self, PyObject *value);

    //! Get Center Of Mass
    PyObject* Softbody_getCenterOfMass(PyObject_Softbody *self, PyObject *value);

    //! Get nearest node
    PyObject* Softbody_getNearestNodeIndex(PyObject_Softbody *self, PyObject *value);

    //! Get node position
    PyObject* Softbody_getNodePosition(PyObject_Softbody *self, PyObject *value);

    //! Get node normal
    PyObject* Softbody_getNodeNormal(PyObject_Softbody *self, PyObject *value);

    //! Append deformable anchor
    PyObject* Softbody_appendDeformableAnchor(PyObject_Softbody *self, PyObject *value);

    //! Append anchor
    PyObject* Softbody_appendAnchor(PyObject_Softbody *self, PyObject *value);

    //! Remove anchor
    PyObject* Softbody_removeAnchor(PyObject_Softbody *self, PyObject *value);

    //! Damping coefficient
    PyObject *Softbody_getDampingCoeff(PyObject_Softbody *self);
    int Softbody_setDampingCoeff(PyObject_Softbody *self, PyObject *value);

    //! Pressure coefficient
    PyObject *Softbody_getPressureCoeff(PyObject_Softbody *self);
    int Softbody_setPressureCoeff(PyObject_Softbody *self, PyObject *value);

    //! Volume conversation coefficient
    PyObject *Softbody_getVolumeConvCoeff(PyObject_Softbody *self);
    int Softbody_setVolumeConvCoeff(PyObject_Softbody *self, PyObject *value);

    //! Dynamic friction coefficient
    PyObject *Softbody_getDynamicFrictionCoeff(PyObject_Softbody *self);
    int Softbody_setDynamicFrictionCoeff(PyObject_Softbody *self, PyObject *value);

    //! Pose matching coefficient
    PyObject *Softbody_getPoseMatchCoeff(PyObject_Softbody *self);
    int Softbody_setPoseMatchCoeff(PyObject_Softbody *self, PyObject *value);

    //! Repulsion Stiffness
    PyObject *Softbody_getRepulsionStiffness(PyObject_Softbody *self);
    int Softbody_setRepulsionStiffness(PyObject_Softbody *self, PyObject *value);

    //! Sleeping Threshold
    PyObject *Softbody_getSleepingThreshold(PyObject_Softbody *self);
    int Softbody_setSleepingThreshold(PyObject_Softbody *self, PyObject *value);

    //! Rest Length Scale
    PyObject *Softbody_getRestLengthScale(PyObject_Softbody *self);
    int Softbody_setRestLengthScale(PyObject_Softbody *self, PyObject *value);

    //! Gravity factor
    PyObject *Softbody_getGravityFactor(PyObject_Softbody *self);
    int Softbody_setGravityFactor(PyObject_Softbody *self, PyObject *value);

    //! Velocities correction factor
    PyObject *Softbody_getVelocityFactor(PyObject_Softbody *self);
    int Softbody_setVelocityFactor(PyObject_Softbody *self, PyObject *value);

    //! Rigid contacts hardness
    PyObject *Softbody_getRigidContactHardness(PyObject_Softbody *self);
    int Softbody_setRigidContactHardness(PyObject_Softbody *self, PyObject *value);

    //! Kinetic contacts hardness
    PyObject *Softbody_getKineticContactHardness(PyObject_Softbody *self);
    int Softbody_setKineticContactHardness(PyObject_Softbody *self, PyObject *value);

    //! Soft contacts hardness
    PyObject *Softbody_getSoftContactHardness(PyObject_Softbody *self);
    int Softbody_setSoftContactHardness(PyObject_Softbody *self, PyObject *value);

    //! Anchor hardness
    PyObject *Softbody_getAnchorHardness(PyObject_Softbody *self);
    int Softbody_setAnchorHardness(PyObject_Softbody *self, PyObject *value);

    //! Anchor hardness
    PyObject *Softbody_getPosIterationNumber(PyObject_Softbody *self);
    int Softbody_setPosIterationNumber(PyObject_Softbody *self, PyObject *value);

    //! Aero Model
    PyObject *Softbody_getAeroModel(PyObject_Softbody *self);
    int Softbody_setAeroModel(PyObject_Softbody *self, PyObject *value);

    //! Self collision
    PyObject *Softbody_isSelfCollision(PyObject_Softbody *self);
    int Softbody_setSelfCollision(PyObject_Softbody *self, PyObject *value);

    //! Soft soft-collision
    PyObject *Softbody_isSoftSoftCollision(PyObject_Softbody *self);
    int Softbody_setSoftSoftCollision(PyObject_Softbody *self, PyObject *value);

    //! Wind velocity
    PyObject *Softbody_getWindVelocity(PyObject_Softbody *self);
    int Softbody_setWindVelocity(PyObject_Softbody *self, PyObject *value);

} // namespace ige::scene
