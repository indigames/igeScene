#include "python/pyDirectionalLight.h"
#include "python/pyDirectionalLight_doc_en.h"

#include "components/DirectionalLight.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void DirectionalLight_dealloc(PyObject_DirectionalLight *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *DirectionalLight_str(PyObject_DirectionalLight *self)
    {
        return PyUnicode_FromString("C++ DirectionalLight object");
    }

    // Color
    PyObject *DirectionalLight_getColor(PyObject_DirectionalLight *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getColor().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int DirectionalLight_setColor(PyObject_DirectionalLight *self, PyObject *value)
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
    PyObject *DirectionalLight_getIntensity(PyObject_DirectionalLight *self)
    {
        return PyFloat_FromDouble(self->component->getIntensity());
    }

    int DirectionalLight_setIntensity(PyObject_DirectionalLight *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setIntensity(val);
            return 0;
        }
        return -1;
    }

    // Direction
    PyObject *DirectionalLight_getDirection(PyObject_DirectionalLight *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getDirection().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    PyGetSetDef DirectionalLight_getsets[] = {
        {"color", (getter)DirectionalLight_getColor, (setter)DirectionalLight_setColor, DirectionalLight_color_doc, NULL},
        {"intensity", (getter)DirectionalLight_getIntensity, (setter)DirectionalLight_getIntensity, DirectionalLight_intensity_doc, NULL},
        {"direction", (getter)DirectionalLight_getDirection, NULL, DirectionalLight_direction_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_DirectionalLight = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.DirectionalLight", /* tp_name */
        sizeof(PyObject_DirectionalLight),                          /* tp_basicsize */
        0,                                                          /* tp_itemsize */
        (destructor)DirectionalLight_dealloc,                       /* tp_dealloc */
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
        (reprfunc)DirectionalLight_str,                             /* tp_str */
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
        DirectionalLight_getsets,                                   /* tp_getset */
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
