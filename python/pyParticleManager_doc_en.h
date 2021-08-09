#pragma once

#include <Python.h>

// enableCulling
PyDoc_STRVAR(ParticleManager_enableCulling_doc,
             "Enable culling.\n"
             "Type: bool\n"
             "Default: True.\n");

// cullingWorldSize
PyDoc_STRVAR(ParticleManager_cullingWorldSize_doc,
             "Culling world size.\n"
             "Type: Vec3\n"
             "Default: (1000, 1000, 1000)\n");

// cullingLayerNumber
PyDoc_STRVAR(ParticleManager_cullingLayerNumber_doc,
             "Culling layer number. Larger number produces more efficient culling, but it also uses a lot more of memory.\n"
             "Type: int\n"
             "Default: 4\n");

// maxParticleNumber
PyDoc_STRVAR(ParticleManager_maxParticleNumber_doc,
             "Max number of particles.\n"
             "Type: bool\n"
             "Default: 2000\n");

// threadNumber
PyDoc_STRVAR(ParticleManager_threadNumber_doc,
             "Number of threads.\n"
             "Type: int\n"
             " Default: 1");

// getInstance
PyDoc_STRVAR(ParticleManager_getInstance_doc,
    "Get ParticleManager singleton instance.\n"
    "\n"
    "ParticleManager.getInstance()\n"
    "\n"
    "Return:\n"
    "    Singleton instance of ParticleManager\n");
