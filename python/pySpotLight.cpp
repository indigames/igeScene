#include "python/pySpotLight.h"
#include "python/pySpotLight_doc_en.h"

#include "components/light/SpotLight.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void SpotLight_dealloc(PyObject_SpotLight *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *SpotLight_str(PyObject_SpotLight *self)
    {
        return PyUnicode_FromString("C++ SpotLight object");
    }

    // Color
    PyObject *SpotLight_getColor(PyObject_SpotLight *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int SpotLight_setColor(PyObject_SpotLight *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setColor(*((Vec3 *)v));
        return 0;
    }

    //! Intensity
    PyObject *SpotLight_getIntensity(PyObject_SpotLight *self)
    {
        return PyFloat_FromDouble(self->component->getIntensity());
    }

    int SpotLight_setIntensity(PyObject_SpotLight *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setIntensity(val);
            return 0;
        }
        return -1;
    }

    //! Range
    PyObject *SpotLight_getRange(PyObject_SpotLight *self)
    {
        return PyFloat_FromDouble(self->component->getRange());
    }

    int SpotLight_setRange(PyObject_SpotLight *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setRange(val);
            return 0;
        }
        return -1;
    }

    //! Angle
    PyObject *SpotLight_getAngle(PyObject_SpotLight *self)
    {
        return PyFloat_FromDouble(self->component->getAngle());
    }

    int SpotLight_setAngle(PyObject_SpotLight *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setAngle(val);
            return 0;
        }
        return -1;
    }

    // Position
    PyObject *SpotLight_getPosition(PyObject_SpotLight *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPosition().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    // Direction
    PyObject *SpotLight_getDirection(PyObject_SpotLight *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getDirection().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    PyGetSetDef SpotLight_getsets[] = {
        {"color", (getter)SpotLight_getColor, (setter)SpotLight_setColor, SpotLight_color_doc, NULL},
        {"intensity", (getter)SpotLight_getIntensity, (setter)SpotLight_setIntensity, SpotLight_intensity_doc, NULL},
        {"range", (getter)SpotLight_getRange, (setter)SpotLight_setRange, SpotLight_range_doc, NULL},
        {"angle", (getter)SpotLight_getAngle, (setter)SpotLight_setAngle, SpotLight_angle_doc, NULL},
        {"position", (getter)SpotLight_getPosition, NULL, SpotLight_position_doc, NULL},
        {"direction", (getter)SpotLight_getDirection, NULL, SpotLight_direction_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_SpotLight = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.SpotLight", /* tp_name */
        sizeof(PyObject_SpotLight),                          /* tp_basicsize */
        0,                                                          /* tp_itemsize */
        (destructor)SpotLight_dealloc,                       /* tp_dealloc */
        0,                                                          /* tp_print */
        0,                                                          /* tp_getattr */
        0,                                                          /* tp_setattr */
        0,                                                          /* tp_reserved */
        0,                                                          /* tp_repr */
        0,                                                          /* tp_as_number */
        0,                                                          /* tp_as_sequence */
        0,                                                          /* tp_as_mapping */
        0,                                                          /* tp_hash */
        0,                                                          /* tp_call */
        (reprfunc)SpotLight_str,                             /* tp_str */
        0,                                                          /* tp_getattro */
        0,                                                          /* tp_setattro */
        0,                                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                         /* tp_flags */
        0,                                                          /* tp_doc */
        0,                                                          /* tp_traverse */
        0,                                                          /* tp_clear */
        0,                                                          /* tp_richcompare */
        0,                                                          /* tp_weaklistoffset */
        0,                                                          /* tp_iter */
        0,                                                          /* tp_iternext */
        0,                                                          /* tp_methods */
        0,                                                          /* tp_members */
        SpotLight_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                                    /* tp_base */
        0,                                                          /* tp_dict */
        0,                                                          /* tp_descr_get */
        0,                                                          /* tp_descr_set */
        0,                                                          /* tp_dictoffset */
        0,                                                          /* tp_init */
        0, /* tp_alloc */ 0,                                        /* tp_new */
        0,                                                          /* tp_free */
    };
} // namespace ige::scene
