#include "python/pySphereCollider.h"
#include "python/pySphereCollider_doc_en.h"

#include "components/physic/SphereCollider.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void SphereCollider_dealloc(PyObject_SphereCollider *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *SphereCollider_str(PyObject_SphereCollider *self)
    {
        return PyUnicode_FromString("C++ SphereCollider object");
    }

    //! size
    PyObject *SphereCollider_getRadius(PyObject_SphereCollider *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<SphereCollider>(self->component.lock())->getRadius());
    }

    int SphereCollider_setRadius(PyObject_SphereCollider *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<SphereCollider>(self->component.lock())->setRadius(val);
            return 0;
        }
        return -1;
    }

    // Get/set
    PyGetSetDef SphereCollider_getsets[] = {
        {"radius", (getter)SphereCollider_getRadius, (setter)SphereCollider_setRadius, SphereCollider_radius_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_SphereCollider = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.SphereCollider", /* tp_name */
        sizeof(PyObject_SphereCollider),                          /* tp_basicsize */
        0,                                                      /* tp_itemsize */
        (destructor)SphereCollider_dealloc,                     /* tp_dealloc */
        0,                                                      /* tp_print */
        0,                                                      /* tp_getattr */
        0,                                                      /* tp_setattr */
        0,                                                      /* tp_reserved */
        0,                                                      /* tp_repr */
        0,                                                      /* tp_as_number */
        0,                                                      /* tp_as_sequence */
        0,                                                      /* tp_as_mapping */
        0,                                                      /* tp_hash */
        0,                                                      /* tp_call */
        (reprfunc)SphereCollider_str,                           /* tp_str */
        0,                                                      /* tp_getattro */
        0,                                                      /* tp_setattro */
        0,                                                      /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                     /* tp_flags */
        0,                                                      /* tp_doc */
        0,                                                      /* tp_traverse */
        0,                                                      /* tp_clear */
        0,                                                      /* tp_richcompare */
        0,                                                      /* tp_weaklistoffset */
        0,                                                      /* tp_iter */
        0,                                                      /* tp_iternext */
        0,                                                      /* tp_methods */
        0,                                                      /* tp_members */
        SphereCollider_getsets,                                 /* tp_getset */
        &PyTypeObject_Component,                                /* tp_base */
        0,                                                      /* tp_dict */
        0,                                                      /* tp_descr_get */
        0,                                                      /* tp_descr_set */
        0,                                                      /* tp_dictoffset */
        0,                                                      /* tp_init */
        0,                                                      /* tp_alloc */
        0,                                                      /* tp_new */
        0,                                                      /* tp_free */
    };
} // namespace ige::scene
