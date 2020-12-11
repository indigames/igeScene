#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/particle/ParticleManager.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_ParticleManager
    {
        PyObject_Component super;
        ParticleManager *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_ParticleManager;

    // Dealloc
    void ParticleManager_dealloc(PyObject_ParticleManager *self);

    // Get instance	
    PyObject *ParticleManager_getInstance();

    // String represent
    PyObject *ParticleManager_str(PyObject_ParticleManager *self);

    //! Culling
    PyObject *ParticleManager_isCullingEnabled(PyObject_ParticleManager *self);
    int ParticleManager_setCullingEnabled(PyObject_ParticleManager *self, PyObject *value);

    //! Culling world size
    PyObject *ParticleManager_getCullingWorldSize(PyObject_ParticleManager *self);
    int ParticleManager_setCullingWorldSize(PyObject_ParticleManager *self, PyObject *value);

    //! Culling layers number
    PyObject *ParticleManager_getCullingLayerNumber(PyObject_ParticleManager *self);
    int ParticleManager_setCullingLayerNumber(PyObject_ParticleManager *self, PyObject *value);

    //! Max particles number
    PyObject *ParticleManager_getMaxParticleNumber(PyObject_ParticleManager *self);
    int ParticleManager_setMaxParticleNumber(PyObject_ParticleManager *self, PyObject *value);

    //! Threads number
    PyObject *ParticleManager_getNumberOfThreads(PyObject_ParticleManager *self);
    int ParticleManager_setNumberOfThreads(PyObject_ParticleManager *self, PyObject *value);

} // namespace ige::scene
