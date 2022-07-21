#pragma once

#include <Python.h>

// radius
PyDoc_STRVAR(OffMeshLink_radius_doc,
             "OffMeshLink radius.\n"
             "Type: float\n");

// mask
PyDoc_STRVAR(OffMeshLink_mask_doc,
             "OffMeshLink mask.\n"
             "Type: int\n");

// areaId
PyDoc_STRVAR(OffMeshLink_areaId_doc,
             "OffMeshLink areaId.\n"
             "Type: int\n");

// endpoint
PyDoc_STRVAR(OffMeshLink_endpoint_doc,
             "OffMeshLink endpoint scene object, or uuid.\n"
             "Type: SceneObject or string\n");

// bidirectional
PyDoc_STRVAR(OffMeshLink_bidirectional_doc,
             "OffMeshLink bidirectional or unidirectional.\n"
             "Type: SceneObject\n");

