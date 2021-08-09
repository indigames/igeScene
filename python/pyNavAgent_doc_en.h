#pragma once

#include <Python.h>

// id
PyDoc_STRVAR(NavAgent_id_doc,
             "Agent Id.\n"
             "Type: int\n");

// radius
PyDoc_STRVAR(NavAgent_radius_doc,
             "Agent radius.\n"
             "Type: float\n");

// height
PyDoc_STRVAR(NavAgent_height_doc,
             "Agent height\n"
             "Type: float\n");

// maxSpeed
PyDoc_STRVAR(NavAgent_maxSpeed_doc,
             "Agent max speed\n"
             "Type: float\n");

// maxAcceleration
PyDoc_STRVAR(NavAgent_maxAcceleration_doc,
             "Agent max acceleration\n"
             "Type: float\n");

// targetPosition
PyDoc_STRVAR(NavAgent_targetPosition_doc,
             "Agent target position\n"
             "Type: float\n");

// autoUpdatePosition
PyDoc_STRVAR(NavAgent_autoUpdatePosition_doc,
             "Agent auto update position\n"
             "Type: bool\n");

// queryFilterType
PyDoc_STRVAR(NavAgent_queryFilterType_doc,
             "Agent query filter type\n"
             "Type: int\n");

// obstacleAvoidanceType
PyDoc_STRVAR(NavAgent_obstacleAvoidanceType_doc,
             "Agent obstacle avoidance type\n"
             "Type: int\n");

// navigationPushiness
PyDoc_STRVAR(NavAgent_navigationPushiness_doc,
             "Agent navigation pushiness\n"
             "Type: int\n");

// isInCrowd
PyDoc_STRVAR(NavAgent_isInCrowd_doc,
             "Check if the agent is in crowd manager.\n"
             "\n"
             "Return:\n"
             " bool\n");

// hasTarget
PyDoc_STRVAR(NavAgent_hasTarget_doc,
             "Check if the agent has assigned target position.\n"
             "\n"
             "Return:\n"
             " bool\n");

// resetTarget
PyDoc_STRVAR(NavAgent_resetTarget_doc,
             "Reset the target.\n"
             "\n"
             "Return:\n"
             " None\n");

// hasArrived
PyDoc_STRVAR(NavAgent_hasArrived_doc,
             "Check if the agent arrived at the target position.\n"
             "\n"
             "Return:\n"
             " bool\n");

// getPosition
PyDoc_STRVAR(NavAgent_getPosition_doc,
             "Get the agent's current position.\n"
             "\n"
             "Return:\n"
             " Vec3\n");

// getVelocity
PyDoc_STRVAR(NavAgent_getVelocity_doc,
             "Get the agent's current velocity.\n"
             "\n"
             "Return:\n"
             " Vec3\n");
