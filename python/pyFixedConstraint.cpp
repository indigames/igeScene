#include "python/pyFixedConstraint.h"
#include "python/pyFixedConstraint_doc_en.h"

#include "components/physic/FixedConstraint.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void FixedConstraint_dealloc(PyObject_FixedConstraint *self)
    {
        if (self && self->constraint)
        {
            self->constraint = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *FixedConstraint_str(PyObject_FixedConstraint *self)
    {
        return PyUnicode_FromString("C++ FixedConstraint object");
    }

    // Get/set
    PyGetSetDef FixedConstraint_getsets[] = {
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_FixedConstraint = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.FixedConstraint", /* tp_name */
        sizeof(PyObject_FixedConstraint),                          /* tp_basicsize */
        0,                                                         /* tp_itemsize */
        (destructor)FixedConstraint_dealloc,                       /* tp_dealloc */
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
        (reprfunc)FixedConstraint_str,                             /* tp_str */
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
        FixedConstraint_getsets,                                   /* tp_getset */
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
