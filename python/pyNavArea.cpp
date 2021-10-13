#include "python/pyNavArea.h"
#include "python/pyNavArea_doc_en.h"

#include "components/navigation/NavArea.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void NavArea_dealloc(PyObject_NavArea *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *NavArea_str(PyObject_NavArea *self)
    {
        return PyUnicode_FromString("C++ NavArea object");
    }

    // AreaId
    PyObject *NavArea_getAreaId(PyObject_NavArea *self)
    {
        return PyLong_FromLong(self->component->getAreaId());
    }

    int NavArea_setAreaId(PyObject_NavArea *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setAreaId(val);
            return 0;
        }
        return -1;
    }

    // AreaCost
    PyObject *NavArea_getAreaCost(PyObject_NavArea *self)
    {
        return PyFloat_FromDouble(self->component->getAreaCost());
    }

    int NavArea_setAreaCost(PyObject_NavArea *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setAreaCost(val);
            return 0;
        }
        return -1;
    }

    // Bounding box
    PyObject *NavArea_getBoundingBox(PyObject_NavArea *self)
    {
        auto aabb = self->component->getBoundingBox();
        auto min = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(aabb.MinEdge.P(), 3, min->v);
        min->d = 3;
        auto max = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(aabb.MaxEdge.P(), 3, max->v);
        max->d = 3;
        PyObject *tuple = PyTuple_New(2);
        PyTuple_SetItem(tuple, 0, (PyObject *)min);
        PyTuple_SetItem(tuple, 1, (PyObject *)max);
        return tuple;
    }

    int NavArea_setBoundingBox(PyObject_NavArea *self, PyObject *value)
    {
        PyObject *minObj, maxObj;

        if (PyArg_ParseTuple(value, "OO", &minObj, &maxObj))
        {
            int d;
            float buff[4];
            auto min = pyObjToFloat((PyObject *)minObj, buff, d);
            if (!min)
                return -1;
            auto max = pyObjToFloat((PyObject *)minObj, buff, d);
            if (!max)
                return -1;
            self->component->setBoundingBox({*((Vec3 *)min), *((Vec3 *)max)});
            return 0;
        }
        return -1;
    }

    // World bounding box
    PyObject *NavArea_getWorldBoundingBox(PyObject_NavArea *self)
    {
        auto aabb = self->component->getWorldBoundingBox();
        auto min = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(aabb.MinEdge.P(), 3, min->v);
        min->d = 3;
        auto max = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(aabb.MaxEdge.P(), 3, max->v);
        max->d = 3;
        PyObject *tuple = PyTuple_New(2);
        PyTuple_SetItem(tuple, 0, (PyObject *)min);
        PyTuple_SetItem(tuple, 1, (PyObject *)max);
        return tuple;
    }

    PyGetSetDef NavArea_getsets[] = {
        {"id", (getter)NavArea_getAreaId, (setter)NavArea_setAreaId, NavArea_id_doc, NULL},
        {"cost", (getter)NavArea_getAreaCost, (setter)NavArea_setAreaCost, NavArea_cost_doc, NULL},
        {"aabb", (getter)NavArea_getBoundingBox, (setter)NavArea_setBoundingBox, NavArea_aabb_doc, NULL},
        {"worldAabb", (getter)NavArea_getWorldBoundingBox, NULL, NavArea_aabb_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_NavArea = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.NavArea", /* tp_name */
        sizeof(PyObject_NavArea),                          /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        (destructor)NavArea_dealloc,                       /* tp_dealloc */
        0,                                                 /* tp_print */
        0,                                                 /* tp_getattr */
        0,                                                 /* tp_setattr */
        0,                                                 /* tp_reserved */
        0,                                                 /* tp_repr */
        0,                                                 /* tp_as_number */
        0,                                                 /* tp_as_sequence */
        0,                                                 /* tp_as_mapping */
        0,                                                 /* tp_hash */
        0,                                                 /* tp_call */
        (reprfunc)NavArea_str,                             /* tp_str */
        0,                                                 /* tp_getattro */
        0,                                                 /* tp_setattro */
        0,                                                 /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                /* tp_flags */
        0,                                                 /* tp_doc */
        0,                                                 /* tp_traverse */
        0,                                                 /* tp_clear */
        0,                                                 /* tp_richcompare */
        0,                                                 /* tp_weaklistoffset */
        0,                                                 /* tp_iter */
        0,                                                 /* tp_iternext */
        0,                                                 /* tp_methods */
        0,                                                 /* tp_members */
        NavArea_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                           /* tp_base */
        0,                                                 /* tp_dict */
        0,                                                 /* tp_descr_get */
        0,                                                 /* tp_descr_set */
        0,                                                 /* tp_dictoffset */
        0,                                                 /* tp_init */
        0,                                                 /* tp_alloc */
        0,                                                 /* tp_new */
        0,                                                 /* tp_free */
    };
} // namespace ige::scene
