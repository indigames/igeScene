#pragma once

#include <Python.h>

// applyTorque
PyDoc_STRVAR(PhysicBase_applyTorque_doc,
             "Apply torque.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    torque : Vec3\n"
             "        torque to apply\n");

// applyForce
PyDoc_STRVAR(PhysicBase_applyForce_doc,
             "Apply force.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    force : Vec3\n"
             "        force to apply\n"
             "    [Optional]position : Vec3\n"
             "        Position to apply, or center\n");

// applyImpulse
PyDoc_STRVAR(PhysicBase_applyImpulse_doc,
             "Apply impulse.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    impulse : Vec3\n"
             "        impulse to apply\n"
             "    [Opt]position : Vec3\n"
             "        Position to apply, or center\n");

// clearForces
PyDoc_STRVAR(PhysicBase_clearForces_doc, "Clear all forces.\n");

// mass
PyDoc_STRVAR(PhysicBase_mass_doc,
             "Mass.\n"
             "Type: float\n");

// friction
PyDoc_STRVAR(PhysicBase_friction_doc,
             "Friction.\n"
             "Type: float\n");

// restitution
PyDoc_STRVAR(PhysicBase_restitution_doc,
             "Restitution.\n"
             "Type: float\n");

// linearVelocity
PyDoc_STRVAR(PhysicBase_linearVelocity_doc,
             "Linear Velocity.\n"
             "Type: Vec3\n");

// angularVelocity
PyDoc_STRVAR(PhysicBase_angularVelocity_doc,
             "Angular Velocity.\n"
             "Type: Vec3\n");

// linearFactor
PyDoc_STRVAR(PhysicBase_linearFactor_doc,
             "Linear Factor.\n"
             "Type: Vec3\n");

// angularFactor
PyDoc_STRVAR(PhysicBase_angularFactor_doc,
             "Angular Factor.\n"
             "Type: Vec3\n");

// isTrigger
PyDoc_STRVAR(PhysicBase_isTrigger_doc,
             "Is trigger.\n"
             "Type: bool\n");

// isKinematic
PyDoc_STRVAR(PhysicBase_isKinematic_doc,
             "Is kinematic.\n"
             "Type: bool\n");

// isEnabled
PyDoc_STRVAR(PhysicBase_isEnabled_doc,
             "Is enabled.\n"
             "Type: bool\n");

// aabb
PyDoc_STRVAR(PhysicBase_aabb_doc,
             "Aabb bounding box.\n"
             "Type: tuple of Vec4\n");

// collisionGroup
PyDoc_STRVAR(PhysicBase_collisionGroup_doc,
    "Collision filter group.\n"
    "Type: int\n");

// collisionMask
PyDoc_STRVAR(PhysicBase_collisionMask_doc,
    "Collision filter mask.\n"
    "Type: int\n");

// continousDetection
PyDoc_STRVAR(PhysicBase_continousDetection_doc,
    "Continous Colision Detection.\n"
    "Type: bool\n");
