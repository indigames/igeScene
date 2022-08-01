#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/particle/Particle.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_Particle : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_Particle;

    // Dealloc
    void Particle_dealloc(PyObject_Particle *self);

    // String represent
    PyObject *Particle_str(PyObject_Particle *self);

    //! Path
    PyObject *Particle_getPath(PyObject_Particle *self);
    int Particle_setPath(PyObject_Particle *self, PyObject *value);

    //! isEnabled
    PyObject *Particle_isEnabled(PyObject_Particle *self);
    int Particle_setEnabled(PyObject_Particle *self, PyObject *value);

    //! isLooped
    PyObject *Particle_isLooped(PyObject_Particle *self);
    int Particle_setLooped(PyObject_Particle *self, PyObject *value);

    //! isAutoDrawing
    PyObject *Particle_isAutoDrawing(PyObject_Particle *self);
    int Particle_setAutoDrawing(PyObject_Particle *self, PyObject *value);

    //! groupMask
    PyObject *Particle_getGroupMask(PyObject_Particle *self);
    int Particle_setGroupMask(PyObject_Particle *self, PyObject *value);

    //! speed
    PyObject *Particle_getSpeed(PyObject_Particle *self);
    int Particle_setSpeed(PyObject_Particle *self, PyObject *value);

    //! timeScale
    PyObject *Particle_getTimeScale(PyObject_Particle *self);
    int Particle_setTimeScale(PyObject_Particle *self, PyObject *value);

    //! targetLocation
    PyObject *Particle_getTargetLocation(PyObject_Particle *self);
    int Particle_setTargetLocation(PyObject_Particle *self, PyObject *value);

    //! dynamicParameter
    PyObject *Particle_getDynamicParameter(PyObject_Particle *self);
    int Particle_setDynamicParameter(PyObject_Particle *self, PyObject *value);

    //! Color
    PyObject *Particle_getColor(PyObject_Particle *self);
    int Particle_setColor(PyObject_Particle *self, PyObject *value);

    //! Play
    PyObject* Particle_play(PyObject_Particle* self);

    //! Pause
    PyObject* Particle_pause(PyObject_Particle* self);

    //! Resume
    PyObject* Particle_resume(PyObject_Particle* self);

    //! Stop
    PyObject* Particle_stop(PyObject_Particle* self);

    //! isPaused
    PyObject* Particle_isPaused(PyObject_Particle* self);

    //! isStopped
    PyObject* Particle_isStopped(PyObject_Particle* self);
} // namespace ige::scene
