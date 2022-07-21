#include "python/pyAudioSource.h"
#include "python/pyAudioSource_doc_en.h"

#include "components/audio/AudioSource.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void AudioSource_dealloc(PyObject_AudioSource *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *AudioSource_str(PyObject_AudioSource *self)
    {
        return PyUnicode_FromString("C++ AudioSource object");
    }

    //! enable
    PyObject *AudioSource_isEnabled(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->isEnabled());
    }

    int AudioSource_setEnabled(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setEnabled(val);
            return 0;
        }
        return -1;
    }

    //! playOnEnabled
    PyObject *AudioSource_getPlayOnEnabled(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getPlayOnEnabled());
    }

    int AudioSource_setPlayOnEnabled(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setPlayOnEnabled(val);
            return 0;
        }
        return -1;
    }

    //! stream
    PyObject *AudioSource_isStream(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->isStream());
    }

    int AudioSource_setStream(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setStream(val);
            return 0;
        }
        return -1;
    }

    //! loop
    PyObject *AudioSource_isLooped(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->isLooped());
    }

    int AudioSource_setLoop(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setLoop(val);
            return 0;
        }
        return -1;
    }

    //! singleInstance
    PyObject *AudioSource_isSingleInstance(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->isSingleInstance());
    }

    int AudioSource_setSingleInstance(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setSingleInstance(val);
            return 0;
        }
        return -1;
    }

    //! path
    PyObject *AudioSource_getPath(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getPath().c_str());        
    }

    int AudioSource_setPath(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyUnicode_Check(value)) {
            const char* val = PyUnicode_AsUTF8(value);
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setPath(std::string(val));
            return 0;
        }
        return -1;
    }

    //! volume
    PyObject *AudioSource_getVolume(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getVolume());
    }

    int AudioSource_setVolume(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setVolume(val);
            return 0;
        }
        return -1;
    }

    //! pan
    PyObject *AudioSource_getPan(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getPan());
    }

    int AudioSource_setPan(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setPan(val);
            return 0;
        }
        return -1;
    }

    //! minDistance
    PyObject *AudioSource_getMinDistance(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getMinDistance());        
    }

    int AudioSource_setMinDistance(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setMinDistance(val);
            return 0;
        }
        return -1;
    }

    //! maxDistance
    PyObject *AudioSource_getMaxDistance(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getMaxDistance());        
    }

    int AudioSource_setMaxDistance(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setMaxDistance(val);
            return 0;
        }
        return -1;
    }

    //! attenuationModel
    PyObject *AudioSource_getAttenuationModel(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getAttenuationModel());        
    }

    int AudioSource_setAttenuationModel(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setAttenuationModel(val);
            return 0;
        }
        return -1;
    }

    //! attenuationRollOffFactor
    PyObject *AudioSource_getAttenuationRollOffFactor(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getAttenuationRollOffFactor());
    }

    int AudioSource_setAttenuationRollOffFactor(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setAttenuationRollOffFactor(val);
            return 0;
        }
        return -1;
    }

    //! dopplerFactor
    PyObject *AudioSource_getDopplerFactor(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getDopplerFactor());
    }

    int AudioSource_setDopplerFactor(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setDopplerFactor(val);
            return 0;
        }
        return -1;
    }

    //! velocity
    PyObject *AudioSource_getVelocity(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->getVelocity().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject*)vec3Obj;
    }

    int AudioSource_setVelocity(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) return -1;        
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<AudioSource>(self->component.lock())->setVelocity(*((Vec3*)v));
        return 0;        
    }

    //! Play
    PyObject *AudioSource_play(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<AudioSource>(self->component.lock())->play();
        Py_RETURN_NONE;
    }

    //! Pause
    PyObject *AudioSource_pause(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<AudioSource>(self->component.lock())->pause();
        Py_RETURN_NONE;
    }

    //! Resume
    PyObject *AudioSource_resume(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<AudioSource>(self->component.lock())->resume();
        Py_RETURN_NONE;
    }

    //! Stop
    PyObject *AudioSource_stop(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<AudioSource>(self->component.lock())->stop();
        Py_RETURN_NONE;
    }

    //! Seek
    PyObject *AudioSource_seek(PyObject_AudioSource *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;        
        float val;
        if (PyArg_ParseTuple(value, "f", &val)) {
            std::dynamic_pointer_cast<AudioSource>(self->component.lock())->seek(val);
        }        
        Py_RETURN_NONE;
    }

    //! isPaused
    PyObject *AudioSource_isPaused(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->isPaused());        
        Py_RETURN_NONE;
    }

    //! isStopped
    PyObject *AudioSource_isStopped(PyObject_AudioSource *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<AudioSource>(self->component.lock())->isStopped());
    }

    // Methods definition
    PyMethodDef AudioSource_methods[] = {
        {"seek", (PyCFunction)AudioSource_seek, METH_VARARGS, AudioSource_seek_doc},
        {"play", (PyCFunction)AudioSource_play, METH_NOARGS, AudioSource_play_doc},
        {"pause", (PyCFunction)AudioSource_pause, METH_NOARGS, AudioSource_pause_doc},
        {"stop", (PyCFunction)AudioSource_stop, METH_NOARGS, AudioSource_stop_doc},
        {"resume", (PyCFunction)AudioSource_resume, METH_NOARGS, AudioSource_resume_doc},
        {"isPaused", (PyCFunction)AudioSource_isPaused, METH_NOARGS, AudioSource_isPaused_doc},
        {"isStopped", (PyCFunction)AudioSource_isStopped, METH_NOARGS, AudioSource_isStopped_doc},
        {NULL, NULL},
    };

    // Get/set
    PyGetSetDef AudioSource_getsets[] = {
        {"enable", (getter)AudioSource_isEnabled, (setter)AudioSource_setEnabled, AudioSource_enable_doc, NULL},
        {"playOnEnabled", (getter)AudioSource_getPlayOnEnabled, (setter)AudioSource_setPlayOnEnabled, AudioSource_playOnEnabled_doc, NULL},
        {"stream", (getter)AudioSource_isStream, (setter)AudioSource_setStream, AudioSource_stream_doc, NULL},
        {"loop", (getter)AudioSource_isLooped, (setter)AudioSource_setLoop, AudioSource_loop_doc, NULL},
        {"singleInstance", (getter)AudioSource_isSingleInstance, (setter)AudioSource_setSingleInstance, AudioSource_singleInstance_doc, NULL},
        {"path", (getter)AudioSource_getPath, (setter)AudioSource_setPath, AudioSource_path_doc, NULL},
        {"volume", (getter)AudioSource_getVolume, (setter)AudioSource_setVolume, AudioSource_volume_doc, NULL},
        {"pan", (getter)AudioSource_getPan, (setter)AudioSource_setPan, AudioSource_pan_doc, NULL},
        {"velocity", (getter)AudioSource_getVelocity, (setter)AudioSource_setVelocity, AudioSource_velocity_doc, NULL},
        {"minDistance", (getter)AudioSource_getMinDistance, (setter)AudioSource_setMinDistance, AudioSource_minDistance_doc, NULL},
        {"maxDistance", (getter)AudioSource_getMaxDistance, (setter)AudioSource_setMaxDistance, AudioSource_maxDistance_doc, NULL},
        {"attenuationModel", (getter)AudioSource_getAttenuationModel, (setter)AudioSource_setAttenuationModel, AudioSource_attenuationModel_doc, NULL},
        {"attenuationFactor", (getter)AudioSource_getAttenuationRollOffFactor, (setter)AudioSource_setAttenuationRollOffFactor, AudioSource_attenuationFactor_doc, NULL},
        {"dopplerFactor", (getter)AudioSource_getDopplerFactor, (setter)AudioSource_setDopplerFactor, AudioSource_dopplerFactor_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_AudioSource = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.AudioSource", /* tp_name */
        sizeof(PyObject_AudioSource),                          /* tp_basicsize */
        0,                                                     /* tp_itemsize */
        (destructor)AudioSource_dealloc,                       /* tp_dealloc */
        0,                                                     /* tp_print */
        0,                                                     /* tp_getattr */
        0,                                                     /* tp_setattr */
        0,                                                     /* tp_reserved */
        0,                                                     /* tp_repr */
        0,                                                     /* tp_as_number */
        0,                                                     /* tp_as_sequence */
        0,                                                     /* tp_as_mapping */
        0,                                                     /* tp_hash */
        0,                                                     /* tp_call */
        (reprfunc)AudioSource_str,                             /* tp_str */
        0,                                                     /* tp_getattro */
        0,                                                     /* tp_setattro */
        0,                                                     /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                    /* tp_flags */
        0,                                                     /* tp_doc */
        0,                                                     /* tp_traverse */
        0,                                                     /* tp_clear */
        0,                                                     /* tp_richcompare */
        0,                                                     /* tp_weaklistoffset */
        0,                                                     /* tp_iter */
        0,                                                     /* tp_iternext */
        AudioSource_methods,                                   /* tp_methods */
        0,                                                     /* tp_members */
        AudioSource_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                               /* tp_base */
        0,                                                     /* tp_dict */
        0,                                                     /* tp_descr_get */
        0,                                                     /* tp_descr_set */
        0,                                                     /* tp_dictoffset */
        0,                                                     /* tp_init */
        0,                                                     /* tp_alloc */
        0,                                                     /* tp_new */
        0,                                                     /* tp_free */
    };
} // namespace ige::scene
