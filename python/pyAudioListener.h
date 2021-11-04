#pragma once

#include <Python.h>

#include "components/Component.h"
#include "components/audio/AudioListener.h"

#include "python/pyComponent.h"

namespace ige::scene
{
    struct PyObject_AudioListener : PyObject_Component {};

    // Type declaration
    extern PyTypeObject PyTypeObject_AudioListener;

    // Dealloc
    void AudioListener_dealloc(PyObject_AudioListener *self);

    // String represent
    PyObject *AudioListener_str(PyObject_AudioListener *self);

    //! enable
    PyObject *AudioListener_isEnabled(PyObject_AudioListener *self);
    int AudioListener_setEnabled(PyObject_AudioListener *self, PyObject *value);
} // namespace ige::scene
