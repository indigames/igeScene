#include "python/pyCapsuleCollider.h"
#include "python/pyCapsuleCollider_doc_en.h"

#include "components/physic/collider/CapsuleCollider.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void CapsuleCollider_dealloc(PyObject_CapsuleCollider *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *CapsuleCollider_str(PyObject_CapsuleCollider *self)
    {
        return PyUnicode_FromString("C++ CapsuleCollider object");
    }

    //! size
    PyObject *CapsuleCollider_getRadius(PyObject_CapsuleCollider *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CapsuleCollider>(self->component.lock())->getRadius());
    }

    int CapsuleCollider_setRadius(PyObject_CapsuleCollider *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CapsuleCollider>(self->component.lock())->setRadius(val);
            return 0;
        }
        return -1;
    }

    //! height
    PyObject *CapsuleCollider_getHeight(PyObject_CapsuleCollider *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<CapsuleCollider>(self->component.lock())->getHeight());
    }

    int CapsuleCollider_setHeight(PyObject_CapsuleCollider *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<CapsuleCollider>(self->component.lock())->setHeight(val);
            return 0;
        }
        return -1;
    }

    // Get/set
    PyGetSetDef CapsuleCollider_getsets[] = {
        {"radius", (getter)CapsuleCollider_getRadius, (setter)CapsuleCollider_setRadius, CapsuleCollider_radius_doc, NULL},
        {"height", (getter)CapsuleCollider_getHeight, (setter)CapsuleCollider_setHeight, CapsuleCollider_height_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_CapsuleCollider = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.CapsuleCollider", /* tp_name */
        sizeof(PyObject_CapsuleCollider),                          /* tp_basicsize */
        0,                                                       /* tp_itemsize */
        (destructor)CapsuleCollider_dealloc,                     /* tp_dealloc */
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
        (reprfunc)CapsuleCollider_str,                           /* tp_str */
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
        CapsuleCollider_getsets,                                 /* tp_getset */
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
