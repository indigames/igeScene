#include "python/pyHingeConstraint.h"
#include "python/pyHingeConstraint_doc_en.h"

#include "components/physic/HingeConstraint.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void HingeConstraint_dealloc(PyObject_HingeConstraint *self)
    {
        if (self && self->constraint)
        {
            self->constraint = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *HingeConstraint_str(PyObject_HingeConstraint *self)
    {
        return PyUnicode_FromString("C++ HingeConstraint object");
    }

    // anchor
    PyObject *HingeConstraint_getAnchor(PyObject_HingeConstraint *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAnchor().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int HingeConstraint_setAnchor(PyObject_HingeConstraint *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAnchor(*((btVector3 *)v));
        return 0;
    }

    // axis1
    PyObject *HingeConstraint_getAxis1(PyObject_HingeConstraint *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAxis1().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int HingeConstraint_setAxis1(PyObject_HingeConstraint *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAxis1(*((btVector3 *)v));
        return 0;
    }

    // axis2
    PyObject *HingeConstraint_getAxis2(PyObject_HingeConstraint *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->constraint->getAxis2().m_floats, 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int HingeConstraint_setAxis2(PyObject_HingeConstraint *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->constraint->setAxis2(*((btVector3 *)v));
        return 0;
    }

    // lowerLimit
    PyObject *HingeConstraint_getLowerLimit(PyObject_HingeConstraint *self)
    {
        return PyFloat_FromDouble(self->constraint->getLowerLimit());
    }

    int HingeConstraint_setLowerLimit(PyObject_HingeConstraint *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->constraint->setLowerLimit(val);
            return 0;
        }
        return -1;
    }

    // upperLimit
    PyObject *HingeConstraint_getUpperLimit(PyObject_HingeConstraint *self)
    {
        return PyFloat_FromDouble(self->constraint->getUpperLimit());
    }

    int HingeConstraint_setUpperLimit(PyObject_HingeConstraint *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->constraint->setUpperLimit(val);
            return 0;
        }
        return -1;
    }

    // Get/set
    PyGetSetDef HingeConstraint_getsets[] = {
        {"anchor", (getter)HingeConstraint_getAnchor, (setter)HingeConstraint_setAnchor, HingeConstraint_anchor_doc, NULL},
        {"axis1", (getter)HingeConstraint_getAxis1, (setter)HingeConstraint_setAxis1, HingeConstraint_axis1_doc, NULL},
        {"axis2", (getter)HingeConstraint_getAxis2, (setter)HingeConstraint_setAxis2, HingeConstraint_axis2_doc, NULL},
        {"lowerLimit", (getter)HingeConstraint_getLowerLimit, (setter)HingeConstraint_setLowerLimit, HingeConstraint_lowerLimit_doc, NULL},
        {"upperLimit", (getter)HingeConstraint_getUpperLimit, (setter)HingeConstraint_setUpperLimit, HingeConstraint_upperLimit_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_HingeConstraint = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.HingeConstraint", /* tp_name */
        sizeof(PyObject_HingeConstraint),                          /* tp_basicsize */
        0,                                                         /* tp_itemsize */
        (destructor)HingeConstraint_dealloc,                       /* tp_dealloc */
        0,                                                         /* tp_print */
        0,                                                         /* tp_getattr */
        0,                                                         /* tp_setattr */
        0,                                                         /* tp_reserved */
        0,                                                         /* tp_repr */
        0,                                                         /* tp_as_number */
        0,                                                         /* tp_as_sequence */
        0,                                                         /* tp_as_mapping */
        0,                                                         /* tp_hash */
        0,                                                         /* tp_call */
        (reprfunc)HingeConstraint_str,                             /* tp_str */
        0,                                                         /* tp_getattro */
        0,                                                         /* tp_setattro */
        0,                                                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                        /* tp_flags */
        0,                                                         /* tp_doc */
        0,                                                         /* tp_traverse */
        0,                                                         /* tp_clear */
        0,                                                         /* tp_richcompare */
        0,                                                         /* tp_weaklistoffset */
        0,                                                         /* tp_iter */
        0,                                                         /* tp_iternext */
        0,                                                         /* tp_methods */
        0,                                                         /* tp_members */
        HingeConstraint_getsets,                                   /* tp_getset */
        &PyTypeObject_PhysicConstraint,                            /* tp_base */
        0,                                                         /* tp_dict */
        0,                                                         /* tp_descr_get */
        0,                                                         /* tp_descr_set */
        0,                                                         /* tp_dictoffset */
        0,                                                         /* tp_init */
        0,                                                         /* tp_alloc */
        0,                                                         /* tp_new */
        0,                                                         /* tp_free */
    };
} // namespace ige::scene
