#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/physic/constraint/Dof6SpringConstraint.h"

#include "python/pyComponent.h"
#include "python/pyPhysicConstraint.h"

namespace ige::scene
{
    struct PyObject_Dof6Constraint
    {
        PyObject_PhysicConstraint super;
        Dof6SpringConstraint *constraint;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_Dof6Constraint;

    // Dealloc
    void Dof6Constraint_dealloc(PyObject_Dof6Constraint *self);

    // linearLowerLimit
    PyObject *Dof6Constraint_getLinearLowerLimit(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearLowerLimit(PyObject_Dof6Constraint *self, PyObject *value);

    // linearUpperLimit
    PyObject *Dof6Constraint_getLinearUpperLimit(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearUpperLimit(PyObject_Dof6Constraint *self, PyObject *value);

    // linearTargetVelocity
    PyObject *Dof6Constraint_getLinearTargetVelocity(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearTargetVelocity(PyObject_Dof6Constraint *self, PyObject *value);

    // linearBounce
    PyObject *Dof6Constraint_getLinearBounce(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearBounce(PyObject_Dof6Constraint *self, PyObject *value);

    // linearSpringEnabled
    PyObject *Dof6Constraint_getLinearSpringEnabled(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearSpringEnabled(PyObject_Dof6Constraint *self, PyObject *value);

    // linearStiffness
    PyObject *Dof6Constraint_getLinearStiffness(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearStiffness(PyObject_Dof6Constraint *self, PyObject *value);

    // linearDamping
    PyObject *Dof6Constraint_getLinearDamping(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearDamping(PyObject_Dof6Constraint *self, PyObject *value);

    // linearMotorEnabled
    PyObject *Dof6Constraint_getLinearMotorEnabled(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearMotorEnabled(PyObject_Dof6Constraint *self, PyObject *value);

    // linearMaxMotorForce
    PyObject *Dof6Constraint_getLinearMaxMotorForce(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearMaxMotorForce(PyObject_Dof6Constraint *self, PyObject *value);

    // linearServoEnabled
    PyObject *Dof6Constraint_getLinearServoEnabled(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearServoEnabled(PyObject_Dof6Constraint *self, PyObject *value);

    // linearServoTarget
    PyObject *Dof6Constraint_getLinearServoTarget(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setLinearServoTarget(PyObject_Dof6Constraint *self, PyObject *value);

    // angularLowerLimit
    PyObject *Dof6Constraint_getAngularLowerLimit(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularLowerLimit(PyObject_Dof6Constraint *self, PyObject *value);

    // angularUpperLimit
    PyObject *Dof6Constraint_getAngularUpperLimit(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularUpperLimit(PyObject_Dof6Constraint *self, PyObject *value);

    // angularTargetVelocity
    PyObject *Dof6Constraint_getAngularTargetVelocity(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularTargetVelocity(PyObject_Dof6Constraint *self, PyObject *value);

    // angularBounce
    PyObject *Dof6Constraint_getAngularBounce(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularBounce(PyObject_Dof6Constraint *self, PyObject *value);

    // angularSpringEnabled
    PyObject *Dof6Constraint_getAngularSpringEnabled(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularSpringEnabled(PyObject_Dof6Constraint *self, PyObject *value);

    // angularStiffness
    PyObject *Dof6Constraint_getAngularStiffness(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularStiffness(PyObject_Dof6Constraint *self, PyObject *value);

    // angularDamping
    PyObject *Dof6Constraint_getAngularDamping(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularDamping(PyObject_Dof6Constraint *self, PyObject *value);

    // angularMotorEnabled
    PyObject *Dof6Constraint_getAngularMotorEnabled(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularMotorEnabled(PyObject_Dof6Constraint *self, PyObject *value);

    // angularMaxMotorForce
    PyObject *Dof6Constraint_getAngularMaxMotorForce(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularMaxMotorForce(PyObject_Dof6Constraint *self, PyObject *value);

    // angularServoEnabled
    PyObject *Dof6Constraint_getAngularServoEnabled(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularServoEnabled(PyObject_Dof6Constraint *self, PyObject *value);

    // angularServoTarget
    PyObject *Dof6Constraint_getAngularServoTarget(PyObject_Dof6Constraint *self);
    int Dof6Constraint_setAngularServoTarget(PyObject_Dof6Constraint *self, PyObject *value);

} // namespace ige::scene
