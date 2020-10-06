#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/audio/AudioSource.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_AudioSource
    {
        PyObject_Component super;
        AudioSource *component;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_AudioSource;

    // Dealloc
    void AudioSource_dealloc(PyObject_AudioSource *self);

    // String represent
    PyObject *AudioSource_str(PyObject_AudioSource *self);

    //! enable
    PyObject *AudioSource_isEnabled(PyObject_AudioSource *self);
    int AudioSource_setEnabled(PyObject_AudioSource *self, PyObject *value);

    //! playOnEnabled
    PyObject *AudioSource_getPlayOnEnabled(PyObject_AudioSource *self);
    int AudioSource_setPlayOnEnabled(PyObject_AudioSource *self, PyObject *value);

    //! stream
    PyObject *AudioSource_isStream(PyObject_AudioSource *self);
    int AudioSource_setStream(PyObject_AudioSource *self, PyObject *value);

    //! path
    PyObject *AudioSource_getPath(PyObject_AudioSource *self);
    int AudioSource_setPath(PyObject_AudioSource *self, PyObject *value);

    //! loop
    PyObject *AudioSource_isLooped(PyObject_AudioSource *self);
    int AudioSource_setLoop(PyObject_AudioSource *self, PyObject *value);

    //! singleInstance
    PyObject *AudioSource_isSingleInstance(PyObject_AudioSource *self);
    int AudioSource_setSingleInstance(PyObject_AudioSource *self, PyObject *value);

    //! volume
    PyObject *AudioSource_getVolume(PyObject_AudioSource *self);
    int AudioSource_setVolume(PyObject_AudioSource *self, PyObject *value);

    //! pan
    PyObject *AudioSource_getPan(PyObject_AudioSource *self);
    int AudioSource_setPan(PyObject_AudioSource *self, PyObject *value);

    //! velocity
    PyObject *AudioSource_getVelocity(PyObject_AudioSource *self);
    int AudioSource_setVelocity(PyObject_AudioSource *self, PyObject *value);

    //! minDistance
    PyObject *AudioSource_getMinDistance(PyObject_AudioSource *self);
    int AudioSource_setMinDistance(PyObject_AudioSource *self, PyObject *value);

    //! maxDistance
    PyObject *AudioSource_getMaxDistance(PyObject_AudioSource *self);
    int AudioSource_setMaxDistance(PyObject_AudioSource *self, PyObject *value);

    //! attenuationModel
    PyObject *AudioSource_getAttenuationModel(PyObject_AudioSource *self);
    int AudioSource_setAttenuationModel(PyObject_AudioSource *self, PyObject *value);

    //! attenuationRollOffFactor
    PyObject *AudioSource_getAttenuationRollOffFactor(PyObject_AudioSource *self);
    int AudioSource_setAttenuationRollOffFactor(PyObject_AudioSource *self, PyObject *value);

    //! dopplerFactor
    PyObject *AudioSource_getDopplerFactor(PyObject_AudioSource *self);
    int AudioSource_setDopplerFactor(PyObject_AudioSource *self, PyObject *value);

    //! Play
    PyObject *AudioSource_play(PyObject_AudioSource *self);

    //! Pause
    PyObject *AudioSource_pause(PyObject_AudioSource *self);

    //! Resume
    PyObject *AudioSource_resume(PyObject_AudioSource *self);

    //! Stop
    PyObject *AudioSource_stop(PyObject_AudioSource *self);

    //! Seek
    PyObject *AudioSource_seek(PyObject_AudioSource *self, PyObject *value);

    //! isPaused
    PyObject *AudioSource_isPaused(PyObject_AudioSource *self);

    //! isStopped
    PyObject *AudioSource_isStopped(PyObject_AudioSource *self);

} // namespace ige::scene
