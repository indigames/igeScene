#include "python/pyNavAgentManager.h"
#include "python/pyNavAgentManager_doc_en.h"
#include "python/pyNavMesh.h"
#include "python/pyDynamicNavMesh.h"
#include "python/pyNavAgent.h"

#include "components/navigation/NavAgentManager.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void NavAgentManager_dealloc(PyObject_NavAgentManager *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *NavAgentManager_str(PyObject_NavAgentManager *self)
    {
        return PyUnicode_FromString("C++ NavAgentManager object");
    }

    //! NavMesh
    PyObject *NavAgentManager_getNavMesh(PyObject_NavAgentManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto obj = (PyObject_NavMesh*)(&PyTypeObject_NavMesh)->tp_alloc(&PyTypeObject_NavMesh, 0);
        obj->component = std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getNavMesh();
        return (PyObject*)obj;
    }

    int NavAgentManager_setNavMesh(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (value->ob_type == &PyTypeObject_NavMesh ||
            value->ob_type == &PyTypeObject_DynamicNavMesh) {
            auto pyNavMesh = (PyObject_NavMesh*)value;
            if (pyNavMesh && !pyNavMesh->component.expired()) {
                std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->setNavMesh(std::dynamic_pointer_cast<NavMesh>(pyNavMesh->component.lock()));
                return 0;
            }
        }
        return -1;
    }

    //! Max Agent Number
    PyObject *NavAgentManager_getMaxAgentNumber(PyObject_NavAgentManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getMaxAgentNumber());
    }

    int NavAgentManager_setMaxAgentNumber(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->setMaxAgentNumber(val);
            return 0;
        }
        return -1;
    }

    //! Max Agent Radius
    PyObject *NavAgentManager_getMaxAgentRadius(PyObject_NavAgentManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getMaxAgentRadius());
    }

    int NavAgentManager_setMaxAgentRadius(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->setMaxAgentRadius(val);
            return 0;
        }
        return -1;
    }

    //! getNumQueryFilterTypes
    PyObject *NavAgentManager_getNumQueryFilterTypes(PyObject_NavAgentManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getNumQueryFilterTypes());
    }

    //! getNumAreas
    PyObject *NavAgentManager_getNumAreas(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int val;
        if (PyArg_ParseTuple(value, "i", &val)) {
            PyLong_FromLong(std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getNumAreas(val));
        }
        return PyLong_FromLong(0);
    }

    //! getAreaCost
    PyObject *NavAgentManager_getAreaCost(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int type, id;
        if (PyArg_ParseTuple(value, "ii", &type, &id) && type >= 0 && id >= 0)
            return PyFloat_FromDouble(std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getAreaCost(type, id));
        return PyFloat_FromDouble(1.f);
    }

    //! setAreaCost
    PyObject *NavAgentManager_setAreaCost(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int type, id;
        float cost;
        if (PyArg_ParseTuple(value, "iif", &type, &id, &cost) && type >= 0 && id >= 0) {
            std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->setAreaCost(type, id, cost);
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }

    //! getIncludeFlags
    PyObject *NavAgentManager_getIncludeFlags(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int type;
        if (PyArg_ParseTuple(value, "i", &type) && type >= 0)
            return PyLong_FromLong(std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getIncludeFlags(type));
        return PyLong_FromLong(0);
    }

    //! setIncludeFlags
    PyObject *NavAgentManager_setIncludeFlags(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int type, val;
        if (PyArg_ParseTuple(value, "i", &type, &val) && type >= 0) {
            std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->setIncludeFlags(type, (uint16_t)val);
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }

    //! getExcludeFlags
    PyObject *NavAgentManager_getExcludeFlags(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int type;
        if (PyArg_ParseTuple(value, "i", &type) && type >= 0)
            return PyLong_FromLong(std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getExcludeFlags(type));
        return PyLong_FromLong(0);
    }

    //! setExcludeFlags
    PyObject *NavAgentManager_setExcludeFlags(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int type, val;
        if (PyArg_ParseTuple(value, "i", &type, &val) && type >= 0) {
            std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->setExcludeFlags(type, (uint16_t)val);
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }

    //! getAgents
    PyObject *NavAgentManager_getAgents(PyObject_NavAgentManager *self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto agents = std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getAgents();
        PyObject* pyList = PyList_New(0);
        for(int i = 0; i < agents.size(); ++i) {
            if (agents[i].expired()) continue;
            auto obj = (PyObject_NavAgent*)(&PyTypeObject_NavAgent)->tp_alloc(&PyTypeObject_NavAgent, 0);
            obj->component = agents[i].lock();
            PyList_Append(pyList, (PyObject*)obj);
            Py_XDECREF(obj);
        }
        return (PyObject*)pyList;
    }

    //! findNearestPoint
    PyObject *NavAgentManager_findNearestPoint(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* posObj = nullptr;
        int type;
        Vec3 nearestPos;
        if (PyArg_ParseTuple(value, "Oi", &posObj, &type))
        {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject *)posObj, buff, d);
            nearestPos = std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->findNearestPoint(*((Vec3 *)v), type);
        }
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(nearestPos.P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    //! moveAlongSurface
    PyObject *NavAgentManager_moveAlongSurface(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* startObj = nullptr;
        PyObject* endObj = nullptr;
        int type;
        int maxVisit = 3;
        auto newPos = std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getOwner()->getTransform()->getPosition();
        if (PyArg_ParseTuple(value, "OOi|i", &startObj, &endObj, &type), maxVisit)
        {
            int d;
            float buff[4];
            auto start = pyObjToFloat((PyObject *)startObj, buff, d);
            auto end = pyObjToFloat((PyObject *)endObj, buff, d);
            newPos = std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->moveAlongSurface(*((Vec3 *)start), *((Vec3 *)end), type, maxVisit);
        }
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(newPos.P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    //! findPath
    PyObject *NavAgentManager_findPath(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* startObj = nullptr;
        PyObject* endObj = nullptr;
        int type;
        std::vector<Vec3> points;
        if (PyArg_ParseTuple(value, "OOi|i", &startObj, &endObj, &type))
        {
            int d;
            float buff[4];
            auto start = pyObjToFloat((PyObject *)startObj, buff, d);
            auto end = pyObjToFloat((PyObject *)endObj, buff, d);
            std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->findPath(points, *((Vec3 *)start), *((Vec3 *)end), type);
        }

        PyObject* pyList = PyList_New(0);
        for(int i = 0; i < points.size(); ++i)
        {
            auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(points[i].P(), 3, vec3Obj->v);
            vec3Obj->d = 3;
            PyList_Append(pyList, (PyObject*)vec3Obj);
            Py_XDECREF(vec3Obj);
        }
        return (PyObject*)pyList;
    }

    //! getRandomPoint
    PyObject *NavAgentManager_getRandomPoint(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int type;
        Vec3 point;
        if (PyArg_ParseTuple(value, "i", &type) && type >= 0)
        {
            point = std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getRandomPoint(type);
        }
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(point.P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    //! getRandomPointInCircle
    PyObject *NavAgentManager_getRandomPointInCircle(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* centerObj = nullptr;
        float radius;
        int type;
        Vec3 point;
        if (PyArg_ParseTuple(value, "Ofi", &centerObj, &radius, &type) && type >= 0)
        {
            int d;
            float buff[4];
            auto center = pyObjToFloat((PyObject *)centerObj, buff, d);
            point = std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getRandomPointInCircle(*((Vec3 *)center), radius, type);
        }
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(point.P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    //! getDistanceToWall
    PyObject *NavAgentManager_getDistanceToWall(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* pointObj = nullptr;
        float radius;
        int type;
        float distance = 10000.f;
        if (PyArg_ParseTuple(value, "Ofi", &pointObj, &radius, &type) && type >= 0)
        {
            int d;
            float buff[4];
            auto center = pyObjToFloat((PyObject *)pointObj, buff, d);
            distance = std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->getDistanceToWall(*((Vec3 *)center), radius, type);
        }
        return PyFloat_FromDouble(distance);
    }

    //! raycast
    PyObject *NavAgentManager_raycast(PyObject_NavAgentManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* startObj = nullptr;
        PyObject* endObj = nullptr;
        int type;
        float distance = 1000000.f;
        Vec3 hitPos;
        if (PyArg_ParseTuple(value, "OOi", &startObj, &endObj, &type) && type >= 0)
        {
            int d;
            float buff[4];
            auto start = pyObjToFloat((PyObject *)startObj, buff, d);
            auto end = pyObjToFloat((PyObject *)endObj, buff, d);
            hitPos = std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->raycast(*((Vec3 *)start), *((Vec3 *)end), type);
        }
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(hitPos.P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    //! deactivateAllAgents
    PyObject *NavAgentManager_deactivateAllAgents(PyObject_NavAgentManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->deactivateAllAgents();
        Py_RETURN_NONE;
    }

    //! reactivateAllAgents
    PyObject *NavAgentManager_reactivateAllAgents(PyObject_NavAgentManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<NavAgentManager>(self->component.lock())->reactivateAllAgents();
        Py_RETURN_NONE;
    }

    PyGetSetDef NavAgentManager_getsets[] = {
        {"navMesh", (getter)NavAgentManager_getNavMesh, (setter)NavAgentManager_setNavMesh, NavAgentManager_navMesh_doc, NULL},
        {"maxAgentNumber", (getter)NavAgentManager_getMaxAgentNumber, (setter)NavAgentManager_setMaxAgentNumber, NavAgentManager_maxAgentNumber_doc, NULL},
        {"maxAgentRadius", (getter)NavAgentManager_getMaxAgentRadius, (setter)NavAgentManager_setMaxAgentRadius, NavAgentManager_maxAgentRadius_doc, NULL},
        {NULL, NULL},
    };

    // Methods
    PyMethodDef NavAgentManager_methods[] = {
        {"getQueryFiltersNumber", (PyCFunction)NavAgentManager_getNumQueryFilterTypes, METH_NOARGS, NavAgentManager_getQueryFiltersNumber_doc},
        {"getAreasNumber", (PyCFunction)NavAgentManager_getNumAreas, METH_VARARGS, NavAgentManager_getAreasNumber_doc},
        {"getAreaCost", (PyCFunction)NavAgentManager_getAreaCost, METH_VARARGS, NavAgentManager_getAreaCost_doc},
        {"setAreaCost", (PyCFunction)NavAgentManager_setAreaCost, METH_VARARGS, NavAgentManager_getAreaCost_doc},
        {"getIncludeFlags", (PyCFunction)NavAgentManager_getIncludeFlags, METH_VARARGS, NavAgentManager_getIncludeFlags_doc},
        {"setIncludeFlags", (PyCFunction)NavAgentManager_setIncludeFlags, METH_VARARGS, NavAgentManager_setIncludeFlags_doc},
        {"getExcludeFlags", (PyCFunction)NavAgentManager_getExcludeFlags, METH_VARARGS, NavAgentManager_getExcludeFlags_doc},
        {"setExcludeFlags", (PyCFunction)NavAgentManager_setExcludeFlags, METH_VARARGS, NavAgentManager_setExcludeFlags_doc},
        {"getAgents", (PyCFunction)NavAgentManager_getAgents, METH_NOARGS, NavAgentManager_getAgents_doc},
        {"findNearestPoint", (PyCFunction)NavAgentManager_findNearestPoint, METH_VARARGS, NavAgentManager_findNearestPoint_doc},
        {"moveAlongSurface", (PyCFunction)NavAgentManager_moveAlongSurface, METH_VARARGS, NavAgentManager_moveAlongSurface_doc},
        {"findPath", (PyCFunction)NavAgentManager_findPath, METH_VARARGS, NavAgentManager_findPath_doc},
        {"getRandomPoint", (PyCFunction)NavAgentManager_getRandomPoint, METH_VARARGS, NavAgentManager_getRandomPoint_doc},
        {"getRandomPointInCircle", (PyCFunction)NavAgentManager_getRandomPointInCircle, METH_VARARGS, NavAgentManager_getRandomPointInCircle_doc},
        {"getDistanceToWall", (PyCFunction)NavAgentManager_getDistanceToWall, METH_VARARGS, NavAgentManager_getDistanceToWall_doc},
        {"raycast", (PyCFunction)NavAgentManager_raycast, METH_VARARGS, NavAgentManager_raycast_doc},
        {"deactivateAllAgents", (PyCFunction)NavAgentManager_deactivateAllAgents, METH_NOARGS, NavAgentManager_deactivateAllAgents_doc},
        {"reactivateAllAgents", (PyCFunction)NavAgentManager_reactivateAllAgents, METH_NOARGS, NavAgentManager_reactivateAllAgents_doc},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_NavAgentManager = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.NavAgentManager", /* tp_name */
        sizeof(PyObject_NavAgentManager),                          /* tp_basicsize */
        0,                                                         /* tp_itemsize */
        (destructor)NavAgentManager_dealloc,                       /* tp_dealloc */
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
        (reprfunc)NavAgentManager_str,                             /* tp_str */
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
        NavAgentManager_methods,                                   /* tp_methods */
        0,                                                         /* tp_members */
        NavAgentManager_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                                   /* tp_base */
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
