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
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *Particle_str(PyObject_Particle *self)
    {
        return PyUnicode_FromString("C++ Particle object");
    }

    //! Path
    PyObject *Particle_getPath(PyObject_Particle *self)
    {
        return PyUnicode_FromString(self->component->getPath().c_str());
    }

    int Particle_setPath(PyObject_Particle *self, PyObject *value)
    {
        char* val;
        if (PyArg_ParseTuple(value, "s", &val))
        {
            self->component->setPath(std::string(val));
            return 0;
        }
        return -1;
    }

    //! isEnabled
    PyObject *Particle_isEnabled(PyObject_Particle *self)
    {
        return PyBool_FromLong(self->component->isEnabled());
    }

    int Particle_setEnabled(PyObject_Particle *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setEnabled(val);
            return 0;
        }
        return -1;
    }

    //! isLooped
    PyObject *Particle_isLooped(PyObject_Particle *self)
    {
        return PyBool_FromLong(self->component->isLooped());
    }

    int Particle_setLooped(PyObject_Particle *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setLoop(val);
            return 0;
        }
        return -1;
    }

    //! isAutoDrawing
    PyObject *Particle_isAutoDrawing(PyObject_Particle *self)
    {
        return PyBool_FromLong(self->component->isAutoDrawing());
    }

    int Particle_setAutoDrawing(PyObject_Particle *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setAutoDrawing(val);
            return 0;
        }
        return -1;
    }

    //! layer
    PyObject *Particle_getLayer(PyObject_Particle *self)
    {
        return PyLong_FromLong(self->component->getLayer());
    }

    int Particle_setLayer(PyObject_Particle *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setLayer(val);
            return 0;
        }
        return -1;
    }

    //! groupMask
    PyObject *Particle_getGroupMask(PyObject_Particle *self)
    {
        return PyLong_FromLong(self->component->getGroupMask());
    }

    int Particle_setGroupMask(PyObject_Particle *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setGroupMask(val);
            return 0;
        }
        return -1;
    }

    //! speed
    PyObject *Particle_getSpeed(PyObject_Particle *self)
    {
        return PyFloat_FromDouble(self->component->getSpeed());
    }

    int Particle_setSpeed(PyObject_Particle *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setSpeed(val);
            return 0;
        }
        return -1;
    }

    //! timeScale
    PyObject *Particle_getTimeScale(PyObject_Particle *self)
    {
        return PyFloat_FromDouble(self->component->getTimeScale());
    }

    int Particle_setTimeScale(PyObject_Particle *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setTimeScale(val);
            return 0;
        }
        return -1;
    }

    //! targetLocation
    PyObject *Particle_getTargetLocation(PyObject_Particle *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getTargetLocation().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Particle_setTargetLocation(PyObject_Particle *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setTargetLocation(*((Vec3 *)v));
        return 0;
    }

    //! dynamicParameter
    PyObject *Particle_getDynamicParameter(PyObject_Particle *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getDynamicInputParameter().P(), 4, vec3Obj->v);
        vec3Obj->d = 4;
        return (PyObject *)vec3Obj;
    }

    int Particle_setDynamicParameter(PyObject_Particle *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setDynamicInputParameter(*((Vec4 *)v));
        return 0;
    }

    // skyColor
    PyObject *Particle_getColor(PyObject_Particle *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getColor().P(), 4, vec3Obj->v);
        vec3Obj->d = 4;
        return (PyObject *)vec3Obj;
    }

    int Particle_setColor(PyObject_Particle *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setColor(*((Vec4 *)v));
        return 0;
    }

    PyGetSetDef Particle_getsets[] = {
        {"path", (getter)Particle_getPath, (setter)Particle_setPath, Particle_path_doc, NULL},
        {"enable", (getter)Particle_isEnabled, (setter)Particle_setEnabled, Particle_enable_doc, NULL},
        {"loop", (getter)Particle_isLooped, (setter)Particle_setLooped, Particle_loop_doc, NULL},
        {"autoDrawing", (getter)Particle_isAutoDrawing, (setter)Particle_setAutoDrawing, Particle_autoDrawing_doc, NULL},
        {"layer", (getter)Particle_getLayer, (setter)Particle_setLayer, Particle_layer_doc, NULL},
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
        0,                                                  /* tp_methods */
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
