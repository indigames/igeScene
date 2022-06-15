#include "python/pyAudioListener.h"
#include "python/pyAudioListener_doc_en.h"

#include "components/audio/AudioListener.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void AudioListener_dealloc(PyObject_AudioListener *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *AudioListener_str(PyObject_AudioListener *self)
    {
        return PyUnicode_FromString("C++ AudioListener object");
    }

    //! enable
    PyObject *AudioListener_isEnabled(PyObject_AudioListener *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<AudioListener>(self->component.lock())->isEnabled());
    }

    int AudioListener_setEnabled(PyObject_AudioListener *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto isEnabled = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<AudioListener>(self->component.lock())->setEnabled(isEnabled);
            return 0;
        }
        return -1;
    }

    // Get/set
    PyGetSetDef AudioListener_getsets[] = {
        {"enable", (getter)AudioListener_isEnabled, (setter)AudioListener_setEnabled, AudioListener_enable_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_AudioListener = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.AudioListener", /* tp_name */
        sizeof(PyObject_AudioListener),                          /* tp_basicsize */
        0,                                                       /* tp_itemsize */
        (destructor)AudioListener_dealloc,                       /* tp_dealloc */
        0,                                                       /* tp_print */
        0,                                                       /* tp_getattr */
        0,                                                       /* tp_setattr */
        0,                                                       /* tp_reserved */
        0,                                                       /* tp_repr */
        0,                                                       /* tp_as_number */
        0,                                                       /* tp_as_sequence */
        0,                                                       /* tp_as_mapping */
        0,                                                       /* tp_hash */
        0,                                                       /* tp_call */
        (reprfunc)AudioListener_str,                             /* tp_str */
        0,                                                       /* tp_getattro */
        0,                                                       /* tp_setattro */
        0,                                                       /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                      /* tp_flags */
        0,                                                       /* tp_doc */
        0,                                                       /* tp_traverse */
        0,                                                       /* tp_clear */
        0,                                                       /* tp_richcompare */
        0,                                                       /* tp_weaklistoffset */
        0,                                                       /* tp_iter */
        0,                                                       /* tp_iternext */
        0,                                                       /* tp_methods */
        0,                                                       /* tp_members */
        AudioListener_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                                 /* tp_base */
        0,                                                       /* tp_dict */
        0,                                                       /* tp_descr_get */
        0,                                                       /* tp_descr_set */
        0,                                                       /* tp_dictoffset */
        0,                                                       /* tp_init */
        0,                                                       /* tp_alloc */
        0,                                                       /* tp_new */
        0,                                                       /* tp_free */
    };
} // namespace ige::scene
