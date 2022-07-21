#pragma once
#include <Python.h>

// getInstance
PyDoc_STRVAR(PhysicManager_getInstance_doc,
             "Get PhysicManager singleton instance.\n"
             "\n"
             "PhysicManager.getInstance()\n"
             "\n"
             "Return:\n"
             "    Singleton instance of PhysicManager\n");

// initialize
PyDoc_STRVAR(PhysicManager_initialize_doc,
             "Initialize the Physic world.\n"
             "\n"
             "PhysicManager.getInstance().initialize(numIteration, deformable)\n"
             "\n"
             "Parameters:\n"
             "    numIteration: [int] number of iteration per frame (default: 4)\n"
             "    deformable: [bool] whether the world is deformable (default: False)\n"
             "\n"
             "Return:\n"
             "    True/False\n");

// clear
PyDoc_STRVAR(PhysicManager_clear_doc,
             "Clear the Physic world.\n"
             "\n"
             "PhysicManager.getInstance().clear()\n");

// rayTestClosest
PyDoc_STRVAR(PhysicManager_rayTestClosest_doc,
             "Perform raytest to find closest hit object.\n"
             "\n"
             "PhysicManager.getInstance().rayTestClosest(from, to, group, mask)\n"
             "\n"
             "Parameters:\n"
             "    from: [Vec3] From vector\n"
             "    to: [Vec3] To vector\n"
             "    group: [int] Collision group\n"
             "    mask: [int] Collision mask\n"
             "\n"
             "Return:\n"
             "    (hitObject: SceneObject, hitPosition: Vec3, hitNormal: Vec3) as Tuple, or None \n");

// rayTestClosest
PyDoc_STRVAR(PhysicManager_rayTestAll_doc,
             "Perform raytest to find all hit objects.\n"
             "\n"
             "PhysicManager.getInstance().rayTestAll(from, to, group, mask)\n"
             "\n"
             "Parameters:\n"
             "    from: [Vec3] From vector\n"
             "    to: [Vec3] To vector\n"
             "    group: [int] Collision group\n"
             "    mask: [int] Collision mask\n"
             "\n"
             "Return:\n"
             " Tuple of (hitObject: SceneObject, hitPosition: Vec3, hitNormal: Vec3) as Tuple \n");

// contactTest
PyDoc_STRVAR(PhysicManager_contactTest_doc,
             "Perform contact test.\n"
             "\n"
             "PhysicManager.getInstance().contactTest(collisionObject, group, mask)\n"
             "\n"
             "Parameters:\n"
             "    collisionObject: SceneObject, Rigidbody, or RidgidBody\n"
             "    group: [int] Collision group\n"
             "    mask: [int] Collision mask\n"
             "\n"
             "Return:\n"
             " Tuple of (objectA: SceneObject, objectB: SceneObject, localPosA: Vec3, localPosB: Vec3, worldPosA: Vec3, worldPosB: Vec3, normalB: Vec3) as Tuple \n");

// contactPairTest
PyDoc_STRVAR(PhysicManager_contactPairTest_doc,
             "Perform contact test between pair of objects.\n"
             "\n"
             "PhysicManager.getInstance().contactPairTest(collisionObjectA, collisionObjectB, group, mask)\n"
             "\n"
             "Parameters:\n"
             "    collisionObjectA: SceneObject, Rigidbody, or RidgidBody\n"
             "    collisionObjectB: SceneObject, Rigidbody, or RidgidBody\n"
             "    group: [int] Collision group\n"
             "    mask: [int] Collision mask\n"
             "\n"
             "Return:\n"
             " Tuple of (objectA: SceneObject, objectB: SceneObject, localPosA: Vec3, localPosB: Vec3, worldPosA: Vec3, worldPosB: Vec3, normalB: Vec3) as Tuple \n");

// isDeformable
PyDoc_STRVAR(PhysicManager_isDeformable_doc,
             "Check if the world is deformable.\n"
             "\n"
             "PhysicManager.getInstance().isDeformable()\n"
             "\n"
             "Return:\n"
             " True/False \n");

// numIteration
PyDoc_STRVAR(PhysicManager_numIteration_doc,
             "Number of iteration.\n"
             "Type: int\n");

// frameUpdateRatio
PyDoc_STRVAR(PhysicManager_frameUpdateRatio_doc,
             "Frame update ratio (speedup/slower effects).\n"
             "Type: float\n");

// frameMaxSubStep
PyDoc_STRVAR(PhysicManager_frameMaxSubStep_doc,
             "Frame max simulation sub step.\n"
             "Type: int\n");

// fixedTimeStep
PyDoc_STRVAR(PhysicManager_fixedTimeStep_doc,
             "Fixed time steps.\n"
             "Type: float\n");

// gravity
PyDoc_STRVAR(PhysicManager_gravity_doc,
             "Gravity.\n"
             "Type: Vec3\n");