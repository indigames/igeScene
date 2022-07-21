#pragma once

#include <Python.h>

// navMesh
PyDoc_STRVAR(NavAgentManager_navMesh_doc,
             "Associated NavMesh instance.\n"
             "Type: NavMesh.\n");

// maxAgentNumber
PyDoc_STRVAR(NavAgentManager_maxAgentNumber_doc,
             "Max number of agents.\n"
             "Type: int\n");

// maxAgentRadius
PyDoc_STRVAR(NavAgentManager_maxAgentRadius_doc,
             "Max radius of agents.\n"
             "Type: float\n");

// getQueryFiltersNumber
PyDoc_STRVAR(NavAgentManager_getQueryFiltersNumber_doc,
             "Return the number of query filters configured in the crowd. Limit to 16.\n"
             "Type: int\n");

// getAreasNumber
PyDoc_STRVAR(NavAgentManager_getAreasNumber_doc,
             "Get the number of configured area in the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "\n"
             "Return: Number of area for this query filter type.\n"
             "    Type: int \n");

// getAreaCost
PyDoc_STRVAR(NavAgentManager_getAreaCost_doc,
             "Get the cost of an area for the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "    areaID: areaId\n"
             "        Type: int\n"
             "\n"
             "Return: area cost.\n"
             "    type: float.\n");

// setAreaCost
PyDoc_STRVAR(NavAgentManager_setAreaCost_doc,
             "Set the cost of an area for the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "    areaID: areaId\n"
             "        Type: int\n"
             "    cost: area cost\n"
             "        Type: float\n"
             "\n"
             "Return: None.\n");

// getIncludeFlags
PyDoc_STRVAR(NavAgentManager_getIncludeFlags_doc,
             "Get the include flags for the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "\n"
             "Return: included flag.\n"
             "    type: int.\n");

// setIncludeFlags
PyDoc_STRVAR(NavAgentManager_setIncludeFlags_doc,
             "Set the include flags for the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "    flags: included flags\n"
             "        Type: int\n"
             "\n"
             "Return: None.\n");

// getExcludeFlags
PyDoc_STRVAR(NavAgentManager_getExcludeFlags_doc,
             "Get the exclude flags for the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "\n"
             "Return: included flag.\n"
             "    type: int.\n");

// setExcludeFlags
PyDoc_STRVAR(NavAgentManager_setExcludeFlags_doc,
             "Set the exclude flags for the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "    flags: included flags\n"
             "        Type: int\n"
             "\n"
             "Return: None.\n");

// getAgents
PyDoc_STRVAR(NavAgentManager_getAgents_doc,
             "Get the list of agents attached to this manager.\n"
             "\n"
             "Return:\n"
             "    Type: list<NavAgent>\n");

// findNearestPoint
PyDoc_STRVAR(NavAgentManager_findNearestPoint_doc,
             "Find the nearest point on the navigation mesh to a given point using the crowd initialized query extent and the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    point: reference point\n"
             "        Type: Vec3\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "\n"
             "Return: nearest point in the mesh.\n"
             "    Type: Vec3\n");

// moveAlongSurface
PyDoc_STRVAR(NavAgentManager_moveAlongSurface_doc,
             "Try to move along the surface from one point to another using the crowd initialized query extentand the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    start: start point\n"
             "        Type: Vec3\n"
             "    end: end point\n"
             "        Type: Vec3\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "    maxVisited: max number of visit\n"
             "        Type: int\n"
             "\n"
             "Return: new position.\n"
             "    Type: Vec3.\n");

// findPath
PyDoc_STRVAR(NavAgentManager_findPath_doc,
             "Find a path between world space points using the crowd initialized query extent and the specified query filter type. Return non-empty list of points if successful.\n"
             "\n"
             "Parameters:\n"
             "    start: start point\n"
             "        Type: Vec3\n"
             "    end: end point\n"
             "        Type: Vec3\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "\n"
             "Return: list of path points.\n"
             "    Type: list<Vec3>\n");

// getRandomPoint
PyDoc_STRVAR(NavAgentManager_getRandomPoint_doc,
             "Return a random point on the navigation mesh using the crowd initialized query extent and the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "\n"
             "Return: a random point in the mesh.\n"
             "    Type: Vec3\n");

// getRandomPointInCircle
PyDoc_STRVAR(NavAgentManager_getRandomPointInCircle_doc,
             "Return distance to wall from a point using the crowd initialized query extent and the specified query filter type. Maximum search radius must be specified.\n"
             "\n"
             "Parameters:\n"
             "    center: center of circle\n"
             "        Type: Vec3\n"
             "    radius: radius of circle\n"
             "        Type: float\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "\n"
             "Return: a random point in a circle in the mesh.\n"
             "    Type: Vec3\n");

// getDistanceToWall
PyDoc_STRVAR(NavAgentManager_getDistanceToWall_doc,
             "Return distance to wall from a point using the crowd initialized query extent and the specified query filter type.\n"
             "\n"
             "Parameters:\n"
             "    point: reference point\n"
             "        Type: Vec3\n"
             "    radius: max search radius\n"
             "        Type: float\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "\n"
             "Return: hit pos.\n"
             "    Type: Vec3\n");

// raycast
PyDoc_STRVAR(NavAgentManager_raycast_doc,
             "Perform a walkability raycast on the navigation mesh between start and end using the crowd initialized query extent and the specified query filter type. Return the point where a wall was hit, or the end point if no walls.\n"
             "\n"
             "Parameters:\n"
             "    start: start point\n"
             "        Type: Vec3\n"
             "    end: end point\n"
             "        Type: Vec3\n"
             "    queryFilterType: filter type\n"
             "        Type: int\n"
             "\n"
             "Return: hit pos.\n"
             "    Type: Vec3\n");

// deactivateAllAgents
PyDoc_STRVAR(NavAgentManager_deactivateAllAgents_doc,
             "Deactivate all attached agents.\n"
             "\n"
             "Return: None.\n");

// reactivateAllAgents
PyDoc_STRVAR(NavAgentManager_reactivateAllAgents_doc,
             "Reactivate all attached agents.\n"
             "\n"
             "Return: None.\n");
