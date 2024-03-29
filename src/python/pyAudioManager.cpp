#include "python/pyAudioManager.h"
#include "python/pyAudioManager_doc_en.h"
#include "python/pyAudioListener.h"

#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void AudioManager_dealloc(PyObject_AudioManager *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *AudioManager_str(PyObject_AudioManager *self)
    {
        return PyUnicode_FromString("C++ AudioManager object");
    }

    // Get singleton instance
    PyObject *AudioManager_getInstance()
    {
        if (SceneManager::getInstance()->getCurrentScene())
        {
            auto audioManager = SceneManager::getInstance()->getCurrentScene()->getRoot()->getComponent<AudioManager>();
            if (audioManager) {
                auto* self = (PyObject_AudioManager*)(&PyTypeObject_AudioManager)->tp_alloc(&PyTypeObject_AudioManager, 0);
                self->component = audioManager;
                return (PyObject*)self;
            }
        }
        Py_RETURN_NONE;
    }

    // getActiveListener
    PyObject *AudioManager_getActiveListener(PyObject_AudioManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        if (!std::dynamic_pointer_cast<AudioManager>(self->component.lock())->getActiveListener().has_value())
            Py_RETURN_NONE;
        auto listenerRef = (*(std::dynamic_pointer_cast<AudioManager>(self->component.lock())->getActiveListener())).get();
        auto listener = listenerRef.getOwner()->getComponent(listenerRef.getInstanceId());
        auto* listenerObj = (PyObject_AudioListener*)(&PyTypeObject_AudioListener)->tp_alloc(&PyTypeObject_AudioListener, 0);
        listenerObj->component = listener;
        return (PyObject*)listenerObj;
        
    }

    // globalVolume
    PyObject *AudioManager_getGlobalVolume(PyObject_AudioManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<AudioManager>(self->component.lock())->getGlobalVolume());
    }

    int AudioManager_setGlobalVolume(PyObject_AudioManager *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<AudioManager>(self->component.lock())->setGlobalVolume(val);
            return 0;
        }
        return -1;
    }

    // Methods
    PyMethodDef AudioManager_methods[] = {
        {"getInstance", (PyCFunction)AudioManager_getInstance, METH_NOARGS | METH_STATIC, AudioManager_getInstance_doc},
        {"getActiveListener", (PyCFunction)AudioManager_getActiveListener, METH_NOARGS, AudioManager_getActiveListener_doc},
        {NULL, NULL},
    };

    // Get/set
    PyGetSetDef AudioManager_getsets[] = {
        {"globalVolume", (getter)AudioManager_getGlobalVolume, (setter)AudioManager_setGlobalVolume, AudioManager_globalVolume_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_AudioManager = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.AudioManager", /* tp_name */
        sizeof(PyObject_AudioManager),                          /* tp_basicsize */
        0,                                                      /* tp_itemsize */
        (destructor)AudioManager_dealloc,                       /* tp_dealloc */
        0,                                                      /* tp_print */
        0,                                                      /* tp_getattr */
        0,                                                      /* tp_setattr */
        0,                                                      /* tp_reserved */
        0,                                                      /* tp_repr */
        0,                                                      /* tp_as_number */
        0,                                                      /* tp_as_sequence */
        0,                                                      /* tp_as_mapping */
        0,                                                      /* tp_hash */
        0,                                                      /* tp_call */
        (reprfunc)AudioManager_str,                             /* tp_str */
        0,                                                      /* tp_getattro */
        0,                                                      /* tp_setattro */
        0,                                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                     /* tp_flags */
        0,                                                      /* tp_doc */
        0,                                                      /* tp_traverse */
        0,                                                      /* tp_clear */
        0,                                                      /* tp_richcompare */
        0,                                                      /* tp_weaklistoffset */
        0,                                                      /* tp_iter */
        0,                                                      /* tp_iternext */
        AudioManager_methods,                                   /* tp_methods */
        0,                                                      /* tp_members */
        AudioManager_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                                /* tp_base */
        0,                                                      /* tp_dict */
        0,                                                      /* tp_descr_get */
        0,                                                      /* tp_descr_set */
        0,                                                      /* tp_dictoffset */
        0,                                                      /* tp_init */
        0,                                                      /* tp_alloc */
        0,                                                      /* tp_new */
        0,                                                      /* tp_free */
    };
} // namespace ige::scene
