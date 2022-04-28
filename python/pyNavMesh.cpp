#include "python/pyNavMesh.h"
#include "python/pyNavMesh_doc_en.h"

#include "components/navigation/NavMesh.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void NavMesh_dealloc(PyObject_NavMesh *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *NavMesh_str(PyObject_NavMesh *self)
    {
        return PyUnicode_FromString("C++ NavMesh object");
    }

    //! Tile size
    PyObject *NavMesh_getTileSize(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getTileSize());
    }

    int NavMesh_setTileSize(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setTileSize(val);
            return 0;
        }
        return -1;
    }

    //! Cell size
    PyObject *NavMesh_getCellSize(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getCellSize());
    }

    int NavMesh_setCellSize(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setCellSize(val);
            return 0;
        }
        return -1;
    }

    //! Cell height
    PyObject *NavMesh_getCellHeight(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getCellHeight());
    }

    int NavMesh_setCellHeight(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setCellHeight(val);
            return 0;
        }
        return -1;
    }

    //! Agent height
    PyObject *NavMesh_getAgentHeight(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getAgentHeight());
    }

    int NavMesh_setAgentHeight(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setAgentHeight(val);
            return 0;
        }
        return -1;
    }

    //! Agent radius
    PyObject *NavMesh_getAgentRadius(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getAgentRadius());
    }

    int NavMesh_setAgentRadius(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setAgentRadius(val);
            return 0;
        }
        return -1;
    }

    //! Agent max vertical climb
    PyObject *NavMesh_getAgentMaxClimb(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getAgentMaxClimb());
    }

    int NavMesh_setAgentMaxClimb(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setAgentMaxClimb(val);
            return 0;
        }
        return -1;
    }

    //! Agent max slope
    PyObject *NavMesh_getAgentMaxSlope(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getAgentMaxSlope());
    }

    int NavMesh_setAgentMaxSlope(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setAgentMaxSlope(val);
            return 0;
        }
        return -1;
    }

    //! Region minimum size
    PyObject *NavMesh_getRegionMinSize(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getRegionMinSize());
    }

    int NavMesh_setRegionMinSize(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setRegionMinSize(val);
            return 0;
        }
        return -1;
    }

    //! Region merge size
    PyObject *NavMesh_getRegionMergeSize(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getRegionMergeSize());
    }

    int NavMesh_setRegionMergeSize(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setRegionMergeSize(val);
            return 0;
        }
        return -1;
    }

    //! Edge max length
    PyObject *NavMesh_getEdgeMaxLength(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getEdgeMaxLength());
    }

    int NavMesh_setEdgeMaxLength(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setEdgeMaxLength(val);
            return 0;
        }
        return -1;
    }

    //! Edge max error
    PyObject *NavMesh_getEdgeMaxError(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getEdgeMaxError());
    }

    int NavMesh_setEdgeMaxError(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setEdgeMaxError(val);
            return 0;
        }
        return -1;
    }

    //! Detail sampling distance
    PyObject *NavMesh_getDetailSampleDistance(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getDetailSampleDistance());
    }

    int NavMesh_setDetailSampleDistance(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setDetailSampleDistance(val);
            return 0;
        }
        return -1;
    }

    //! Detail sampling maximum error
    PyObject *NavMesh_getDetailSampleMaxError(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getDetailSampleMaxError());
    }

    int NavMesh_setDetailSampleMaxError(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setDetailSampleMaxError(val);
            return 0;
        }
        return -1;
    }

    //! Partitioning type
    PyObject *NavMesh_getPartitionType(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong((int)std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getPartitionType());
    }

    int NavMesh_setPartitionType(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setPartitionType((NavMesh::EPartitionType)val);
            return 0;
        }
        return -1;
    }

    //! Bounding box padding
    PyObject *NavMesh_getPadding(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getPadding().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int NavMesh_setPadding(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<NavMesh>(self->component.lock())->setPadding(*((Vec3 *)v));
        return 0;
    }

    //! Build
    PyObject *NavMesh_build(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        if(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->build())
            Py_RETURN_TRUE;
        Py_RETURN_FALSE;
    }

    //! Get AABB
    PyObject *NavMesh_getAABB(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto aabb = std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getBoundingBox();
        auto aabbMinObj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(aabb.MinEdge.P(), 3, aabbMinObj->v);
        aabbMinObj->d = 3;

        auto aabbMaxObj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(aabb.MaxEdge.P(), 3, aabbMaxObj->v);
        aabbMaxObj->d = 3;

        PyObject *tuple = PyTuple_New(2);
        PyTuple_SetItem(tuple, 0, (PyObject *)aabbMinObj);
        PyTuple_SetItem(tuple, 1, (PyObject *)aabbMaxObj);
        return tuple;
    }

    //! Get world AABB
    PyObject *NavMesh_getWorldAABB(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto aabb = std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getWorldBoundingBox();
        auto aabbMinObj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(aabb.MinEdge.P(), 3, aabbMinObj->v);
        aabbMinObj->d = 3;

        auto aabbMaxObj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(aabb.MaxEdge.P(), 3, aabbMaxObj->v);
        aabbMaxObj->d = 3;

        PyObject *tuple = PyTuple_New(2);
        PyTuple_SetItem(tuple, 0, (PyObject *)aabbMinObj);
        PyTuple_SetItem(tuple, 1, (PyObject *)aabbMaxObj);
        return tuple;
    }

    //! Get number of tiles
    PyObject *NavMesh_getNumTiles(PyObject_NavMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vecObj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getNumTiles().P(), 2, vecObj->v);
        vecObj->d = 2;
        return (PyObject *)vecObj;
    }

    //! Get tile index
    PyObject *NavMesh_getTileIndex(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        auto tileIdx = Vec2(-1.f, -1.f);
        if (v)
        {
            tileIdx = std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getTileIndex(*((Vec3 *)v));
        }
        auto vecObj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(tileIdx.P(), 2, vecObj->v);
        vecObj->d = 2;
        return (PyObject *)vecObj;
    }

    //! Get tile aabb
    PyObject *NavMesh_getTileAABB(PyObject_NavMesh *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        AABBox tileAabb;
        if (v)
        {
            tileAabb = std::dynamic_pointer_cast<NavMesh>(self->component.lock())->getTileBoundingBox(*((Vec2 *)v));
        }
        auto aabbMinObj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(tileAabb.MinEdge.P(), 3, aabbMinObj->v);
        aabbMinObj->d = 3;

        auto aabbMaxObj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(tileAabb.MaxEdge.P(), 3, aabbMaxObj->v);
        aabbMaxObj->d = 3;

        PyObject *tuple = PyTuple_New(2);
        PyTuple_SetItem(tuple, 0, (PyObject *)aabbMinObj);
        PyTuple_SetItem(tuple, 1, (PyObject *)aabbMaxObj);
        return tuple;
    }

    //! Getter/setter
    PyGetSetDef NavMesh_getsets[] = {
        {"tileSize", (getter)NavMesh_getTileSize, (setter)NavMesh_setTileSize, NavMesh_tileSize_doc, NULL},
        {"cellSize", (getter)NavMesh_getCellSize, (setter)NavMesh_setCellSize, NavMesh_cellSize_doc, NULL},
        {"cellHeight", (getter)NavMesh_getCellHeight, (setter)NavMesh_setCellHeight, NavMesh_cellHeight_doc, NULL},
        {"agentHeight", (getter)NavMesh_getAgentHeight, (setter)NavMesh_setAgentHeight, NavMesh_agentHeight_doc, NULL},
        {"agentRadius", (getter)NavMesh_getAgentRadius, (setter)NavMesh_setAgentRadius, NavMesh_agentRadius_doc, NULL},
        {"agentMaxClimb", (getter)NavMesh_getAgentMaxClimb, (setter)NavMesh_setAgentMaxClimb, NavMesh_agentMaxClimb_doc, NULL},
        {"agentMaxSlope", (getter)NavMesh_getAgentMaxSlope, (setter)NavMesh_setAgentMaxSlope, NavMesh_agentMaxSlope_doc, NULL},
        {"regionMinSize", (getter)NavMesh_getRegionMinSize, (setter)NavMesh_setRegionMinSize, NavMesh_regionMinSize_doc, NULL},
        {"regionMergeSize", (getter)NavMesh_getRegionMergeSize, (setter)NavMesh_setRegionMergeSize, NavMesh_regionMergeSize_doc, NULL},
        {"edgeMaxLength", (getter)NavMesh_getEdgeMaxLength, (setter)NavMesh_setEdgeMaxLength, NavMesh_edgeMaxLength_doc, NULL},
        {"edgeMaxError", (getter)NavMesh_getEdgeMaxError, (setter)NavMesh_setEdgeMaxError, NavMesh_edgeMaxError_doc, NULL},
        {"detailSampleDistance", (getter)NavMesh_getDetailSampleDistance, (setter)NavMesh_setDetailSampleDistance, NavMesh_detailSampleDistance_doc, NULL},
        {"detailSampleMaxError", (getter)NavMesh_getDetailSampleMaxError, (setter)NavMesh_setDetailSampleMaxError, NavMesh_detDetailSampleMaxError_doc, NULL},
        {"aabbPading", (getter)NavMesh_getPadding, (setter)NavMesh_setPadding, NavMesh_aabbPading_doc, NULL},
        {"partitionType", (getter)NavMesh_getPartitionType, (setter)NavMesh_setPartitionType, NavMesh_partitionType_doc, NULL},
        {NULL, NULL},
    };

    // Methods
    PyMethodDef NavMesh_methods[] = {
        {"build", (PyCFunction)NavMesh_build, METH_NOARGS, NavMesh_build_doc},
        {"getAABB", (PyCFunction)NavMesh_getAABB, METH_NOARGS, NavMesh_getAABB_doc},
        {"getWorldAABB", (PyCFunction)NavMesh_getWorldAABB, METH_NOARGS, NavMesh_getWorldAABB_doc},
        {"getNumTiles", (PyCFunction)NavMesh_getNumTiles, METH_NOARGS, NavMesh_getNumTiles_doc},
        {"getTileIndex", (PyCFunction)NavMesh_getTileIndex, METH_VARARGS, NavMesh_getTileIndex_doc},
        {"getTileAABB", (PyCFunction)NavMesh_getTileAABB, METH_VARARGS, NavMesh_getTileAABB_doc},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_NavMesh = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.NavMesh", /* tp_name */
        sizeof(PyObject_NavMesh),                          /* tp_basicsize */
        0,                                                 /* tp_itemsize */
        (destructor)NavMesh_dealloc,                       /* tp_dealloc */
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
        (reprfunc)NavMesh_str,                             /* tp_str */
        0,                                                 /* tp_getattro */
        0,                                                 /* tp_setattro */
        0,                                                 /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,          /* tp_flags */
        0,                                                 /* tp_doc */
        0,                                                 /* tp_traverse */
        0,                                                 /* tp_clear */
        0,                                                 /* tp_richcompare */
        0,                                                 /* tp_weaklistoffset */
        0,                                                 /* tp_iter */
        0,                                                 /* tp_iternext */
        NavMesh_methods,                                   /* tp_methods */
        0,                                                 /* tp_members */
        NavMesh_getsets,                                   /* tp_getset */
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
