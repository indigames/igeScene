#include "python/pyParticle.h"
#include "python/pyParticle_doc_en.h"

#include "components/particle/Particle.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void Particle_dealloc(PyObject_Particle *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *Particle_str(PyObject_Particle *self)
    {
        return PyUnicode_FromString("C++ Particle object");
    }

    //! Path
    PyObject *Particle_getPath(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(std::dynamic_pointer_cast<Particle>(self->component.lock())->getPath().c_str());
    }

    int Particle_setPath(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyUnicode_Check(value)) {
            const char* val = PyUnicode_AsUTF8(value);
            std::dynamic_pointer_cast<Particle>(self->component.lock())->setPath(std::string(val));
            return 0;
        }
        return -1;
    }

    //! isEnabled
    PyObject *Particle_isEnabled(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Particle>(self->component.lock())->isEnabled());
    }

    int Particle_setEnabled(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (value == NULL || !PyBool_Check(value)) {
            return -1;
        }
        int val = PyObject_IsTrue(value);
        std::dynamic_pointer_cast<Particle>(self->component.lock())->setEnabled((bool)val);
        return 0;
    }

    //! isLooped
    PyObject *Particle_isLooped(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Particle>(self->component.lock())->isLooped());
    }

    int Particle_setLooped(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (value == NULL || !PyBool_Check(value)) {
            return -1;
        }
        int val = PyObject_IsTrue(value);
        std::dynamic_pointer_cast<Particle>(self->component.lock())->setLoop((bool)val);
        return 0;
    }

    //! isAutoDrawing
    PyObject *Particle_isAutoDrawing(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Particle>(self->component.lock())->isAutoDrawing());
    }

    int Particle_setAutoDrawing(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<Particle>(self->component.lock())->setAutoDrawing(val);
            return 0;
        }
        return -1;
    }

    //! groupMask
    PyObject *Particle_getGroupMask(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<Particle>(self->component.lock())->getGroupMask());
    }

    int Particle_setGroupMask(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<Particle>(self->component.lock())->setGroupMask(val);
            return 0;
        }
        return -1;
    }

    //! speed
    PyObject *Particle_getSpeed(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Particle>(self->component.lock())->getSpeed());
    }

    int Particle_setSpeed(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Particle>(self->component.lock())->setSpeed(val);
            return 0;
        }
        return -1;
    }

    //! timeScale
    PyObject *Particle_getTimeScale(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Particle>(self->component.lock())->getTimeScale());
    }

    int Particle_setTimeScale(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Particle>(self->component.lock())->setTimeScale(val);
            return 0;
        }
        return -1;
    }

    //! targetLocation
    PyObject *Particle_getTargetLocation(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<Particle>(self->component.lock())->getTargetLocation().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Particle_setTargetLocation(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<Particle>(self->component.lock())->setTargetLocation(*((Vec3 *)v));
        return 0;
    }

    //! dynamicParameter
    PyObject *Particle_getDynamicParameter(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<Particle>(self->component.lock())->getDynamicInputParameter().P(), 4, vec3Obj->v);
        vec3Obj->d = 4;
        return (PyObject *)vec3Obj;
    }

    int Particle_setDynamicParameter(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<Particle>(self->component.lock())->setDynamicInputParameter(*((Vec4 *)v));
        return 0;
    }

    // skyColor
    PyObject *Particle_getColor(PyObject_Particle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<Particle>(self->component.lock())->getColor().P(), 4, vec3Obj->v);
        vec3Obj->d = 4;
        return (PyObject *)vec3Obj;
    }

    int Particle_setColor(PyObject_Particle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<Particle>(self->component.lock())->setColor(*((Vec4 *)v));
        return 0;
    }

    //! Play
    PyObject* Particle_play(PyObject_Particle* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<Particle>(self->component.lock())->play();
        Py_RETURN_NONE;
    }

    //! Pause
    PyObject* Particle_pause(PyObject_Particle* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<Particle>(self->component.lock())->pause();
        Py_RETURN_NONE;
    }

    //! Resume
    PyObject* Particle_resume(PyObject_Particle* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<Particle>(self->component.lock())->resume();
        Py_RETURN_NONE;
    }

    //! Stop
    PyObject* Particle_stop(PyObject_Particle* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<Particle>(self->component.lock())->stop();
        Py_RETURN_NONE;
    }

    //! isPaused
    PyObject* Particle_isPaused(PyObject_Particle* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Particle>(self->component.lock())->isPaused());
        Py_RETURN_NONE;
    }

    //! isStopped
    PyObject* Particle_isStopped(PyObject_Particle* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Particle>(self->component.lock())->isStopped());
    }

    // Methods definition
    PyMethodDef Particle_methods[] = {
        {"play", (PyCFunction)Particle_play, METH_NOARGS, Particle_play_doc},
        {"pause", (PyCFunction)Particle_pause, METH_NOARGS, Particle_pause_doc},
        {"stop", (PyCFunction)Particle_stop, METH_NOARGS, Particle_stop_doc},
        {"resume", (PyCFunction)Particle_resume, METH_NOARGS, Particle_resume_doc},
        {"isPaused", (PyCFunction)Particle_isPaused, METH_NOARGS, Particle_isPaused_doc},
        {"isStopped", (PyCFunction)Particle_isStopped, METH_NOARGS, Particle_isStopped_doc},
        {NULL, NULL},
    };

    PyGetSetDef Particle_getsets[] = {
        {"path", (getter)Particle_getPath, (setter)Particle_setPath, Particle_path_doc, NULL},
        {"enable", (getter)Particle_isEnabled, (setter)Particle_setEnabled, Particle_enable_doc, NULL},
        {"loop", (getter)Particle_isLooped, (setter)Particle_setLooped, Particle_loop_doc, NULL},
        {"autoDrawing", (getter)Particle_isAutoDrawing, (setter)Particle_setAutoDrawing, Particle_autoDrawing_doc, NULL},
        {"groupMask", (getter)Particle_getGroupMask, (setter)Particle_setGroupMask, Particle_groupMask_doc, NULL},
        {"speed", (getter)Particle_getSpeed, (setter)Particle_setSpeed, Particle_speed_doc, NULL},
        {"timeScale", (getter)Particle_getTimeScale, (setter)Particle_setTimeScale, Particle_timeScale_doc, NULL},
        {"targetLocation", (getter)Particle_getTargetLocation, (setter)Particle_setTargetLocation, Particle_targetLocation_doc, NULL},
        {"dynamicParameter", (getter)Particle_getDynamicParameter, (setter)Particle_setDynamicParameter, Particle_dynamicParameter_doc, NULL},
        {"color", (getter)Particle_getColor, (setter)Particle_setColor, Particle_color_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_Particle = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.Particle", /* tp_name */
        sizeof(PyObject_Particle),                          /* tp_basicsize */
        0,                                                  /* tp_itemsize */
        (destructor)Particle_dealloc,                       /* tp_dealloc */
        0,                                                  /* tp_print */
        0,                                                  /* tp_getattr */
        0,                                                  /* tp_setattr */
        0,                                                  /* tp_reserved */
        0,                                                  /* tp_repr */
        0,                                                  /* tp_as_number */
        0,                                                  /* tp_as_sequence */
        0,                                                  /* tp_as_mapping */
        0,                                                  /* tp_hash */
        0,                                                  /* tp_call */
        (reprfunc)Particle_str,                             /* tp_str */
        0,                                                  /* tp_getattro */
        0,                                                  /* tp_setattro */
        0,                                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                 /* tp_flags */
        0,                                                  /* tp_doc */
        0,                                                  /* tp_traverse */
        0,                                                  /* tp_clear */
        0,                                                  /* tp_richcompare */
        0,                                                  /* tp_weaklistoffset */
        0,                                                  /* tp_iter */
        0,                                                  /* tp_iternext */
        Particle_methods,                                   /* tp_methods */
        0,                                                  /* tp_members */
        Particle_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                            /* tp_base */
        0,                                                  /* tp_dict */
        0,                                                  /* tp_descr_get */
        0,                                                  /* tp_descr_set */
        0,                                                  /* tp_dictoffset */
        0,                                                  /* tp_init */
        0,                                                  /* tp_alloc */
        0,                                                  /* tp_new */
        0,                                                  /* tp_free */
    };
} // namespace ige::scene
