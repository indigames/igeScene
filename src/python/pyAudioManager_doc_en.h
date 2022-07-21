#pragma once

#include <Python.h>

// getInstance
PyDoc_STRVAR(AudioManager_getInstance_doc,
             "Get AudioManager singleton instance.\n"
             "\n"
             "AudioManager.getInstance()\n"
             "\n"
             "Return:\n"
             "    Singleton instance of AudioManager\n");

// getActiveListener
PyDoc_STRVAR(AudioManager_getActiveListener_doc,
             "Get the active listener instance.\n"
             "\n"
             "AudioManager.getInstance().getActiveListener()\n"
             "\n"
             "Return:\n"
             "    AudioListener or None\n");

// globalVolume
PyDoc_STRVAR(AudioManager_globalVolume_doc,
             "Global volume.\n"
             "Type: float\n");
