#include "python/pyPhysicCapsule.h"
#include "python/pyPhysicCapsule_doc_en.h"

#include "components/physic/PhysicCapsule.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void PhysicCapsule_dealloc(PyObject_PhysicCapsule *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *PhysicCapsule_str(PyObject_PhysicCapsule *self)
    {
        return PyUnicode_FromString("C++ PhysicCapsule object");
    }

    //! size
    PyObject *PhysicCapsule_getRadius(PyObject_PhysicCapsule *self)
    {
        return PyFloat_FromDouble(self->component->getRadius());
    }

    int PhysicCapsule_setRadius(PyObject_PhysicCapsule *self, PyObject *value)
    {
        float radius;
        if (PyArg_ParseTuple(value, "f", &radius))
        {
            self->component->setRadius(radius);
            return 0;
        }
        return -1;
    }

    //! height
    PyObject *PhysicCapsule_getHeight(PyObject_PhysicCapsule *self)
    {
        return PyFloat_FromDouble(self->component->getHeight());
    }

    int PhysicCapsule_setHeight(PyObject_PhysicCapsule *self, PyObject *value)
    {

        float height;
        if (PyArg_ParseTuple(value, "f", &height))
        {
            self->component->setHeight(height);
            return 0;
        }
        return -1;
    }

    // Get/set
    PyGetSetDef PhysicCapsule_getsets[] = {
        {"radius", (getter)PhysicCapsule_getRadius, (setter)PhysicCapsule_setRadius, PhysicCapsule_radius_doc, NULL},
        {"height", (getter)PhysicCapsule_getHeight, (setter)PhysicCapsule_setHeight, PhysicCapsule_height_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_PhysicCapsule = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.PhysicCapsule", /* tp_name */
        sizeof(PyObject_PhysicCapsule),                          /* tp_basicsize */
        0,                                                       /* tp_itemsize */
        (destructor)PhysicCapsule_dealloc,                       /* tp_dealloc */
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
        (reprfunc)PhysicCapsule_str,                             /* tp_str */
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
        PhysicCapsule_getsets,                                   /* tp_getset */
        &PyTypeObject_PhysicBase,                                /* tp_base */
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
