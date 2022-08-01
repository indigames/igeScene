#pragma once

#include <Python.h>

// tileSize
PyDoc_STRVAR(NavMesh_tileSize_doc,
             "Tile size.\n"
             "    Type: int\n"
             "    Limit: [1, 128]\n"
             "    Default: 64\n");

// cellSize
PyDoc_STRVAR(NavMesh_cellSize_doc,
             "Cell size.\n"
             "    Type: float\n"
             "    Default: 0.3\n");

// cellHeight
PyDoc_STRVAR(NavMesh_cellHeight_doc,
             "Cell Height\n"
             "    Type: float\n"
             "    Default: 0.2\n");

// agentHeight
PyDoc_STRVAR(NavMesh_agentHeight_doc,
             "Agent height.\n"
             "   Type: float\n"
             "   Default: 2.0\n");

// agentRadius
PyDoc_STRVAR(NavMesh_agentRadius_doc,
             "Agent radius.\n"
             "   Type: float\n"
             "   Default: 0.6\n");

// agentMaxClimb
PyDoc_STRVAR(NavMesh_agentMaxClimb_doc,
             "Agent max vertical climb.\n"
             "   Type: float\n"
             "   Default: 0.9\n");

// agentMaxSlope
PyDoc_STRVAR(NavMesh_agentMaxSlope_doc,
             "Agent max slope, the angle in DEG value.\n"
             "   Type: float\n"
             "   Default: 45.0\n");

// regionMinSize
PyDoc_STRVAR(NavMesh_regionMinSize_doc,
             "Region minimum size.\n"
             "   Type: float\n"
             "   Default: 8.0\n");

// regionMergeSize
PyDoc_STRVAR(NavMesh_regionMergeSize_doc,
             "Region merge size.\n"
             "   Type: float\n"
             "   Default: 20.0\n");

// edgeMaxLength
PyDoc_STRVAR(NavMesh_edgeMaxLength_doc,
             "Edge max length.\n"
             "   Type: float\n"
             "   Default: 12.0\n");

// edgeMaxError
PyDoc_STRVAR(NavMesh_edgeMaxError_doc,
             "Edge max error.\n"
             "   Type: float\n"
             "   Default: 1.3\n");

// detailSampleDistance
PyDoc_STRVAR(NavMesh_detailSampleDistance_doc,
             "Detail sampling distance.\n"
             "   Type: float\n"
             "   Default: 0.6\n");

// detDetailSampleMaxError
PyDoc_STRVAR(NavMesh_detDetailSampleMaxError_doc,
             "Detail sampling maximum error.\n"
             "   Type: float\n"
             "   Default: 1.0\n");

// aabbPading
PyDoc_STRVAR(NavMesh_aabbPading_doc,
             "Bounding box padding.\n"
             "   Type: Vec3\n"
             "   Default: (1.0, 1.0., 1.0)\n");

// partitionType
PyDoc_STRVAR(NavMesh_partitionType_doc,
             "Partitioning type. WATERSHED = 0, MONOTONE = 1.\n"
             "   Type: int\n"
             "   Default: 0\n");

// build
PyDoc_STRVAR(NavMesh_build_doc,
             "Build the entire navigation mesh.\n"
             "\n"
             "Return:\n"
             "    Type: bool\n");

// getAABB
PyDoc_STRVAR(NavMesh_getAABB_doc,
             "Return the bounding box of this NavMesh.\n"
             "\n"
             "Return:\n"
             "    Type: tuple<Vec3>\n");

// getWorldAABB
PyDoc_STRVAR(NavMesh_getWorldAABB_doc,
             "Return the bounding box of this NavMesh in world space.\n"
             "\n"
             "Return:\n"
             "    Type: tuple<Vec3>\n");

// getNumTiles
PyDoc_STRVAR(NavMesh_getNumTiles_doc,
             "Return number of tiles by X and Z axis.\n"
             "\n"
             "Return:\n"
             "    Type: tuple(numTilesX, numTilesZ)\n");

// getTileIndex
PyDoc_STRVAR(NavMesh_getTileIndex_doc,
             "Return index of the tile contains the given position, by X and Z axis.\n"
            "\n"
             "Parameters:\n"
             "    position: reference position\n"
             "        Type: Vec3\n"
             "\n"
             "Return:\n"
             "    Type: tuple(xIdx, zIdx)\n");

// getTileAABB
PyDoc_STRVAR(NavMesh_getTileAABB_doc,
             "Return number of tiles by X and Z axis.\n"
            "\n"
             "Parameters:\n"
             "    tileX: index by X axis\n"
             "        Type: int\n"
             "    tileZ: index by Z axis\n"
             "        Type: int\n"
             "\n"
             "Return:\n"
             "    Type: tuple<int>\n");
