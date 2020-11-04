#pragma once

#include <Python.h>

// applyRepulsionForce
PyDoc_STRVAR(PhysicSoftBody_applyRepulsionForce_doc,
             "Apply repulsion force.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    timeStep : float\n"
             "        Time step\n"
             "    applySpringForce : bool\n"
             "        Whether spring force to apply\n"
             "\n"
             "Return\n"
             "----------\n"
             "    None\n");

// addVelocity
PyDoc_STRVAR(PhysicSoftBody_addVelocity_doc,
             "Add velocity to the body.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    velocity : Vec3\n"
             "        Velocity to add\n"
             "    [Optional] nodeIdx : int\n"
             "        Apply velocity just to the node with provided index.\n"
             "\n"
             "Return\n"
             "----------\n"
             "    None\n");

// getVolume
PyDoc_STRVAR(PhysicSoftBody_getVolume_doc,
             "Get the volume of the body.\n"
             "\n"
             "Return\n"
             "----------\n"
             "    volume : Vec3\n");

// getCenterOfMass
PyDoc_STRVAR(PhysicSoftBody_getCenterOfMass_doc,
             "Get the body center of mass.\n"
             "\n"
             "Return\n"
             "----------\n"
             "    centerOfMass : Vec3\n");

// getNearestNodeIndex
PyDoc_STRVAR(PhysicSoftBody_getNearestNodeIndex_doc,
             "Get nearest node index from a point in 3D coordination.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    position : Vec3\n"
             "        Position\n"
             "\n"
             "Return\n"
             "----------\n"
             "    nodeIdx : int\n");

// getNodePosition
PyDoc_STRVAR(PhysicSoftBody_getNodePosition_doc,
             "Get the position of a node from node index.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    nodeIdx : int\n"
             "        Node index\n"
             "\n"
             "Return\n"
             "----------\n"
             "    position : Vec3\n");

// getNodeNormal
PyDoc_STRVAR(PhysicSoftBody_getNodeNormal_doc,
             "Get the normal of a node from node index.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    nodeIdx : int\n"
             "        Node index\n"
             "\n"
             "Return\n"
             "----------\n"
             "    normal : Vec3\n");

// appendDeformableAnchor
PyDoc_STRVAR(PhysicSoftBody_appendDeformableAnchor_doc,
             "Append deformable anchor to the body.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    nodeIdx : int\n"
             "        Node index\n"
             "    body : RigidBody\n"
             "        Body to add as anchor\n"
             "\n"
             "Return\n"
             "----------\n"
             "    None\n");

// appendAnchor
PyDoc_STRVAR(PhysicSoftBody_appendAnchor_doc,
             "Append anchor to the body.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    nodeIdx : int\n"
             "        Node index\n"
             "    body : RigidBody\n"
             "        Body to add as anchor\n"
             "    [Optional]disableLinkedColission : bool\n"
             "        Disable Linked Colission or not. Default is false.\n"
             "    [Optional]influence : int\n"
             "        influence factor. Default is 1.\n"
             "\n"
             "Return\n"
             "----------\n"
             "    None\n");

// removeAnchor
PyDoc_STRVAR(PhysicSoftBody_removeAnchor_doc,
             "Remove anchor from the body.\n"
             "\n"
             "Parameters\n"
             "----------\n"
             "    nodeIdx : int\n"
             "        Node index\n"
             "\n"
             "Return\n"
             "----------\n"
             "    None\n");

// dampingCoeff
PyDoc_STRVAR(PhysicSoftBody_dampingCoeff_doc,
             "Damping Coefficient [0, 1]. Default is 1.0.\n"
             "Type: float\n");

// linearStiffness
PyDoc_STRVAR(PhysicSoftBody_linearStiffness_doc,
             "Linear stiffness coefficient [0, 1]. Default is 0.5\n"
             "Type: float\n");

// pressureCoeff
PyDoc_STRVAR(PhysicSoftBody_pressureCoeff_doc,
             "Pressure coefficient [-inf, +inf]. Default is 0.\n"
             "Type: float\n");

// volumeConvCoeff
PyDoc_STRVAR(PhysicSoftBody_volumeConvCoeff_doc,
             "Volume conversation coefficient [0, +inf]. Default is 0.\n"
             "Type: float\n");

// dynamicFrictionCoeff
PyDoc_STRVAR(PhysicSoftBody_dynamicFrictionCoeff_doc,
             "Dynamic friction coefficient [0, 1]. Default is 0.2.\n"
             "Type: float\n");

// poseMatchCoeff
PyDoc_STRVAR(PhysicSoftBody_poseMatchCoeff_doc,
             "Pose matching coefficient [0, 1]. Default is 0.\n"
             "Type: float\n");

// velocityFactor
PyDoc_STRVAR(PhysicSoftBody_velocityFactor_doc,
             "Velocity correction factor. Default is 1.0.\n"
             "Type: float\n");

// gravityFactor
PyDoc_STRVAR(PhysicSoftBody_gravityFactor_doc,
             "Gravity factor. Default is 1.0.\n"
             "Type: float\n");

// rigidContactHardness
PyDoc_STRVAR(PhysicSoftBody_rigidContactHardness_doc,
             "Rigid contacts hardness. Default is 1.0.\n"
             "Type: float\n");

// kineticContactHardness
PyDoc_STRVAR(PhysicSoftBody_kineticContactHardness_doc,
             "Kinetic contacts hardness. Default is 0.1.\n"
             "Type: float\n");

// softContactHardness
PyDoc_STRVAR(PhysicSoftBody_softContactHardness_doc,
             "Soft contacts hardness. Default is 1.0.\n"
             "Type: float\n");

// anchorHardness
PyDoc_STRVAR(PhysicSoftBody_anchorHardness_doc,
             "Anchors hardness. Default is 1.0.\n"
             "Type: float\n");

// sleepingThreshold
PyDoc_STRVAR(PhysicSoftBody_sleepingThreshold_doc,
             "Sleeping Threshold. Default is 0.04.\n"
             "Type: float\n");

// restLengthScale
PyDoc_STRVAR(PhysicSoftBody_restLengthScale_doc,
             "Rest Length Scale. Default is 1.0.\n"
             "Type: float\n");

// positionIterNumber
PyDoc_STRVAR(PhysicSoftBody_positionIterNumber_doc,
             "Position iteration number. Default is 1.\n"
             "Type: int\n");

// aeroModel
PyDoc_STRVAR(PhysicSoftBody_aeroModel_doc,
             "Aero model [0, 6].\n"
             "    0: V_Point\n"
             "    1: V_TwoSided\n"
             "    2: V_TwoSidedLiftDrag\n"
             "    3: V_OneSided\n"
             "    4: F_TwoSided\n"
             "    5: F_TwoSidedLiftDrag\n"
             "    6: F_OneSided\n"
             "Default is 1.\n"
             "Type: int\n");

// windVelocity
PyDoc_STRVAR(PhysicSoftBody_windVelocity_doc,
             "Wind Velocity. Default is (0, 0, 0)\n"
             "Type: Vec3\n");

// selfCollision
PyDoc_STRVAR(PhysicSoftBody_selfCollision_doc,
             "Use Self Collision. Default is false.\n"
             "Type: bool\n");

// softCollision
PyDoc_STRVAR(PhysicSoftBody_softCollision_doc,
             "Use soft soft-collision. Default is false.\n"
             "Type: bool\n");

