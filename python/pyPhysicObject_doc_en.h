#pragma once

#include <Python.h>

// applyTorque
PyDoc_STRVAR(PhysicObject_applyTorque_doc,
             "Apply torque.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    torque : Vec3\n"
             "        torque to apply\n");

// applyForce
PyDoc_STRVAR(PhysicObject_applyForce_doc,
             "Apply force.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    force : Vec3\n"
             "        force to apply\n"
             "    [Optional]position : Vec3\n"
             "        Position to apply, or center\n");

// applyImpulse
PyDoc_STRVAR(PhysicObject_applyImpulse_doc,
             "Apply impulse.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    impulse : Vec3\n"
             "        impulse to apply\n"
             "    [Opt]position : Vec3\n"
             "        Position to apply, or center\n");

// addConstraint
PyDoc_STRVAR(PhysicObject_addConstraint_doc,
             "Add constraint.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    type:\n"
             "        int: Type of the constraint to be added.\n"
             "----------\n"
             "    constraint : PhysicConstraint\n"
             "        Added constraint\n"
            );

// getConstraints
PyDoc_STRVAR(PhysicObject_getConstraints_doc,
             "Get all constraints.\n"
             "\n"
             "Return\n"
             "----------\n"
             "    constraints : tuple<PhysicConstraint>\n"
            );

// removeConstraint
PyDoc_STRVAR(PhysicObject_removeConstraint_doc,
             "Add constraint.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    constraint:\n"
             "        PhysicConstraint: constraint instance to remove.\n"
             "Return\n"
             "----------\n"
             "    result : bool\n"
            );

// removeConstraints
PyDoc_STRVAR(PhysicObject_removeConstraints_doc,
    "Remove all constraints.\n"
    "\n"
    "Return\n"
    "----------\n"
    "    result: bool\n"
);


// clearForces
PyDoc_STRVAR(PhysicObject_clearForces_doc, "Clear all forces.\n");

// mass
PyDoc_STRVAR(PhysicObject_mass_doc,
             "Mass.\n"
             "Type: float\n");

// friction
PyDoc_STRVAR(PhysicObject_friction_doc,
             "Friction.\n"
             "Type: float\n");

// restitution
PyDoc_STRVAR(PhysicObject_restitution_doc,
             "Restitution.\n"
             "Type: float\n");

// linearVelocity
PyDoc_STRVAR(PhysicObject_linearVelocity_doc,
             "Linear Velocity.\n"
             "Type: Vec3\n");

// angularVelocity
PyDoc_STRVAR(PhysicObject_angularVelocity_doc,
             "Angular Velocity.\n"
             "Type: Vec3\n");

// linearFactor
PyDoc_STRVAR(PhysicObject_linearFactor_doc,
             "Linear Factor.\n"
             "Type: Vec3\n");

// angularFactor
PyDoc_STRVAR(PhysicObject_angularFactor_doc,
             "Angular Factor.\n"
             "Type: Vec3\n");

// isTrigger
PyDoc_STRVAR(PhysicObject_isTrigger_doc,
             "Is trigger.\n"
             "Type: bool\n");

// isKinematic
PyDoc_STRVAR(PhysicObject_isKinematic_doc,
             "Is kinematic.\n"
             "Type: bool\n");

// isEnabled
PyDoc_STRVAR(PhysicObject_isEnabled_doc,
             "Is enabled.\n"
             "Type: bool\n");

// aabb
PyDoc_STRVAR(PhysicObject_aabb_doc,
             "Aabb bounding box.\n"
             "Type: tuple of Vec4\n");

// collisionGroup
PyDoc_STRVAR(PhysicObject_collisionGroup_doc,
             "Collision filter group.\n"
             "Type: int\n");

// collisionMask
PyDoc_STRVAR(PhysicObject_collisionMask_doc,
             "Collision filter mask.\n"
             "Type: int\n");

// continuousDetection
PyDoc_STRVAR(PhysicObject_continuousDetection_doc,
             "Continuous Colision Detection.\n"
             "Type: bool\n");
