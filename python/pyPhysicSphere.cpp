#include "python/pyPhysicSphere.h"
#include "python/pyPhysicSphere_doc_en.h"

#include "components/physic/PhysicSphere.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void PhysicSphere_dealloc(PyObject_PhysicSphere *self)
    {
        if (self->component)
        {
            self->super.component = nullptr;
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *PhysicSphere_str(PyObject_PhysicSphere *self)
    {
        return PyUnicode_FromString("C++ PhysicSphere object");
    }

    //! size
    PyObject *PhysicSphere_getRadius(PyObject_PhysicSphere *self)
    {
        return PyFloat_FromDouble(self->component->getRadius());
    }

    int PhysicSphere_setRadius(PyObject_PhysicSphere *self, PyObject *value)
    {
        float radius;
        if (PyArg_ParseTuple(value, "f", &radius))
        {
            self->component->setRadius(radius);
            return 0;
        }
        return -1;
    }

    // Get/set
    PyGetSetDef PhysicSphere_getsets[] = {
        {"radius", (getter)PhysicSphere_getRadius, (setter)PhysicSphere_setRadius, PhysicSphere_radius_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_PhysicSphere = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.PhysicSphere", /* tp_name */
        sizeof(PyObject_PhysicSphere),                          /* tp_basicsize */
        0,                                                      /* tp_itemsize */
        (destructor)PhysicSphere_dealloc,                       /* tp_dealloc */
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
        (reprfunc)PhysicSphere_str,                             /* tp_str */
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
        PhysicSphere_getsets,                                   /* tp_getset */
        &PyTypeObject_PhysicObject,                             /* tp_base */
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
