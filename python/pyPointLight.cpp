#include "python/pyPointLight.h"
#include "python/pyPointLight_doc_en.h"

#include "components/PointLight.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void PointLight_dealloc(PyObject_PointLight *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *PointLight_str(PyObject_PointLight *self)
    {
        return PyUnicode_FromString("C++ PointLight object");
    }

    // Color
    PyObject *PointLight_getColor(PyObject_PointLight *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PointLight_setColor(PyObject_PointLight *self, PyObject *value)
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
    PyObject *PointLight_getIntensity(PyObject_PointLight *self)
    {
        return PyFloat_FromDouble(self->component->getIntensity());
    }

    int PointLight_setIntensity(PyObject_PointLight *self, PyObject *value)
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
    PyObject *PointLight_getRange(PyObject_PointLight *self)
    {
        return PyFloat_FromDouble(self->component->getRange());
    }

    int PointLight_setRange(PyObject_PointLight *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setRange(val);
            return 0;
        }
        return -1;
    }

    // Direction
    PyObject *PointLight_getPosition(PyObject_PointLight *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPosition().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    PyGetSetDef PointLight_getsets[] = {
        {"color", (getter)PointLight_getColor, (setter)PointLight_setColor, PointLight_color_doc, NULL},
        {"intensity", (getter)PointLight_getIntensity, (setter)PointLight_setIntensity, PointLight_intensity_doc, NULL},
        {"range", (getter)PointLight_getRange, (setter)PointLight_setRange, PointLight_range_doc, NULL},
        {"position", (getter)PointLight_getPosition, NULL, PointLight_position_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_PointLight = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.PointLight", /* tp_name */
        sizeof(PyObject_PointLight),                          /* tp_basicsize */
        0,                                                          /* tp_itemsize */
        (destructor)PointLight_dealloc,                       /* tp_dealloc */
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
        (reprfunc)PointLight_str,                             /* tp_str */
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
        PointLight_getsets,                                   /* tp_getset */
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
