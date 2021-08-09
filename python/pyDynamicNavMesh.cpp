#include "python/pyDynamicNavMesh.h"
#include "python/pyDynamicNavMesh_doc_en.h"
#include "components/navigation/DynamicNavMesh.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    void DynamicNavMesh_dealloc(PyObject_DynamicNavMesh *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *DynamicNavMesh_str(PyObject_DynamicNavMesh *self)
    {
        return PyUnicode_FromString("C++ DynamicNavMesh object");
    }

    //! Max obstacles
    PyObject *DynamicNavMesh_getMaxObstacles(PyObject_DynamicNavMesh *self)
    {
        return PyLong_FromLong(self->component->getMaxObstacles());
    }

    int DynamicNavMesh_setMaxObstacles(PyObject_DynamicNavMesh *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setMaxObstacles(val);
            return 0;
        }
        return -1;
    }

    //! Max layers
    PyObject *DynamicNavMesh_getMaxLayers(PyObject_DynamicNavMesh *self)
    {
        return PyLong_FromLong(self->component->getMaxLayers());
    }

    int DynamicNavMesh_setMaxLayers(PyObject_DynamicNavMesh *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setMaxLayers(val);
            return 0;
        }
        return -1;
    }

    // Get/set
    PyGetSetDef DynamicNavMesh_getsets[] = {
        {"maxObstacles", (getter)DynamicNavMesh_getMaxObstacles, (setter)DynamicNavMesh_setMaxObstacles, DynamicNavMesh_maxObstacles_doc, NULL},
        {"maxLayers", (getter)DynamicNavMesh_getMaxLayers, (setter)DynamicNavMesh_setMaxLayers, DynamicNavMesh_maxLayers_doc, NULL},
        {NULL, NULL},
    };

    // Methods
    PyMethodDef DynamicNavMesh_methods[] = {
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_DynamicNavMesh = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.DynamicNavMesh", /* tp_name */
        sizeof(PyObject_DynamicNavMesh),                          /* tp_basicsize */
        0,                                                        /* tp_itemsize */
        (destructor)DynamicNavMesh_dealloc,                       /* tp_dealloc */
        0,                                                        /* tp_print */
        0,                                                        /* tp_getattr */
        0,                                                        /* tp_setattr */
        0,                                                        /* tp_reserved */
        0,                                                        /* tp_repr */
        0,                                                        /* tp_as_number */
        0,                                                        /* tp_as_sequence */
        0,                                                        /* tp_as_mapping */
        0,                                                        /* tp_hash */
        0,                                                        /* tp_call */
        (reprfunc)DynamicNavMesh_str,                             /* tp_str */
        0,                                                        /* tp_getattro */
        0,                                                        /* tp_setattro */
        0,                                                        /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                       /* tp_flags */
        0,                                                        /* tp_doc */
        0,                                                        /* tp_traverse */
        0,                                                        /* tp_clear */
        0,                                                        /* tp_richcompare */
        0,                                                        /* tp_weaklistoffset */
        0,                                                        /* tp_iter */
        0,                                                        /* tp_iternext */
        0,                                                        /* tp_methods */
        0,                                                        /* tp_members */
        DynamicNavMesh_getsets,                                   /* tp_getset */
        &PyTypeObject_NavMesh,                                    /* tp_base */
        0,                                                        /* tp_dict */
        0,                                                        /* tp_descr_get */
        0,                                                        /* tp_descr_set */
        0,                                                        /* tp_dictoffset */
        0,                                                        /* tp_init */
        0,                                                        /* tp_alloc */
        0,                                                        /* tp_new */
        0,                                                        /* tp_free */
    };
} // namespace ige::scene
