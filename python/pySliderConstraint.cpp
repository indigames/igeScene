#include "python/pySliderConstraint.h"
#include "python/pySliderConstraint_doc_en.h"

#include "components/physic/SliderConstraint.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void SliderConstraint_dealloc(PyObject_SliderConstraint *self)
    {
        if (self && self->constraint)
        {
            self->constraint = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *SliderConstraint_str(PyObject_SliderConstraint *self)
    {
        return PyUnicode_FromString("C++ SliderConstraint object");
    }

    // lowerLimit
    PyObject *SliderConstraint_getLowerLimit(PyObject_SliderConstraint *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getLowerLimit().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int SliderConstraint_setLowerLimit(PyObject_SliderConstraint *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setLowerLimit(*((btVector3 *)v));
        return 0;
    }

    // upperLimit
    PyObject *SliderConstraint_getUpperLimit(PyObject_SliderConstraint *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getUpperLimit().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int SliderConstraint_setUpperLimit(PyObject_SliderConstraint *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setUpperLimit(*((btVector3 *)v));
        return 0;
    }

    // Get/set
    PyGetSetDef SliderConstraint_getsets[] = {
        {"lowerLimit", (getter)SliderConstraint_getLowerLimit, (setter)SliderConstraint_setLowerLimit, SliderConstraint_lowerLimit_doc, NULL},
        {"upperLimit", (getter)SliderConstraint_getUpperLimit, (setter)SliderConstraint_setUpperLimit, SliderConstraint_upperLimit_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_SliderConstraint = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.SliderConstraint", /* tp_name */
        sizeof(PyObject_SliderConstraint),                          /* tp_basicsize */
        0,                                                          /* tp_itemsize */
        (destructor)SliderConstraint_dealloc,                       /* tp_dealloc */
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
        (reprfunc)SliderConstraint_str,                             /* tp_str */
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
        SliderConstraint_getsets,                                   /* tp_getset */
        &PyTypeObject_PhysicConstraint,                             /* tp_base */
        0,                                                          /* tp_dict */
        0,                                                          /* tp_descr_get */
        0,                                                          /* tp_descr_set */
        0,                                                          /* tp_dictoffset */
        0,                                                          /* tp_init */
        0,                                                          /* tp_alloc */
        0,                                                          /* tp_new */
        0,                                                          /* tp_free */
    };
} // namespace ige::scene
