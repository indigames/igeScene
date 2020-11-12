#pragma once

#include <Python.h>
#include "systems/audio/AudioManager.h"

namespace ige::scene
{
    struct PyObject_AudioManager
    {
        PyObject_HEAD;
        AudioManager *audioManager;
    };

    // Type declaration
    extern PyTypeObject PyTypeObject_AudioManager;

    // Dealloc
    void AudioManager_dealloc(PyObject_AudioManager *self);

    // String represent
    PyObject *AudioManager_str(PyObject_AudioManager *self);

    // Get instance
    PyObject *AudioManager_getInstance();

    // globalVolume
    PyObject *AudioManager_getGlobalVolume(PyObject_AudioManager *self);
    int AudioManager_setGlobalVolume(PyObject_AudioManager *self, PyObject *value);

    // getActiveListener
    PyObject *AudioManager_getActiveListener(PyObject_AudioManager *self);
} // namespace ige::scene
