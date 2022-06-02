#pragma once

#include <Python.h>

// id
PyDoc_STRVAR(NavArea_id_doc,
             "Area ID.\n"
             "Type: int\n");

// aabb
PyDoc_STRVAR(NavArea_aabb_doc,
             "The AABB box of the area.\n"
             "Type: AABBox\n");

// worldAabb
PyDoc_STRVAR(NavArea_worldAabb_doc,
             "The AABB box of the area, in world space.\n"
             "Type: AABBox\n");
