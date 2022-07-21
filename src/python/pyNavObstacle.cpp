#include "python/pyNavObstacle.h"
#include "python/pyNavObstacle_doc_en.h"

#include "components/navigation/NavObstacle.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void NavObstacle_dealloc(PyObject_NavObstacle *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *NavObstacle_str(PyObject_NavObstacle *self)
    {
        return PyUnicode_FromString("C++ NavObstacle object");
    }

    // Radius
    PyObject *NavObstacle_getRadius(PyObject_NavObstacle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavObstacle>(self->component.lock())->getRadius());
    }

    int NavObstacle_setRadius(PyObject_NavObstacle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavObstacle>(self->component.lock())->setRadius(val);
            return 0;
        }
        return -1;
    }

    // Height
    PyObject *NavObstacle_getHeight(PyObject_NavObstacle *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavObstacle>(self->component.lock())->getHeight());
    }

    int NavObstacle_setHeight(PyObject_NavObstacle *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavObstacle>(self->component.lock())->setHeight(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef NavObstacle_getsets[] = {
        {"radius", (getter)NavObstacle_getRadius, (setter)NavObstacle_setRadius, NavObstacle_radius_doc, NULL},
        {"height", (getter)NavObstacle_getHeight, (setter)NavObstacle_setHeight, NavObstacle_height_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_NavObstacle = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.NavObstacle", /* tp_name */
        sizeof(PyObject_NavObstacle),                          /* tp_basicsize */
        0,                                                     /* tp_itemsize */
        (destructor)NavObstacle_dealloc,                       /* tp_dealloc */
        0,                                                     /* tp_print */
        0,                                                     /* tp_getattr */
        0,                                                     /* tp_setattr */
        0,                                                     /* tp_reserved */
        0,                                                     /* tp_repr */
        0,                                                     /* tp_as_number */
        0,                                                     /* tp_as_sequence */
        0,                                                     /* tp_as_mapping */
        0,                                                     /* tp_hash */
        0,                                                     /* tp_call */
        (reprfunc)NavObstacle_str,                             /* tp_str */
        0,                                                     /* tp_getattro */
        0,                                                     /* tp_setattro */
        0,                                                     /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                    /* tp_flags */
        0,                                                     /* tp_doc */
        0,                                                     /* tp_traverse */
        0,                                                     /* tp_clear */
        0,                                                     /* tp_richcompare */
        0,                                                     /* tp_weaklistoffset */
        0,                                                     /* tp_iter */
        0,                                                     /* tp_iternext */
        0,                                                     /* tp_methods */
        0,                                                     /* tp_members */
        NavObstacle_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                               /* tp_base */
        0,                                                     /* tp_dict */
        0,                                                     /* tp_descr_get */
        0,                                                     /* tp_descr_set */
        0,                                                     /* tp_dictoffset */
        0,                                                     /* tp_init */
        0,                                                     /* tp_alloc */
        0,                                                     /* tp_new */
        0,                                                     /* tp_free */
    };
} // namespace ige::scene
