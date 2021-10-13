#include "python/pyNavAgent.h"
#include "python/pyNavAgent_doc_en.h"

#include "components/navigation/NavAgent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void NavAgent_dealloc(PyObject_NavAgent *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *NavAgent_str(PyObject_NavAgent *self)
    {
        return PyUnicode_FromString("C++ NavAgent object");
    }

    //! ID
    PyObject *NavAgent_getAgentId(PyObject_NavAgent *self)
    {
        return PyLong_FromLong(self->component->getAgentId());
    }

    int NavAgent_setAgentId(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setAgentId(val);
            return 0;
        }
        return -1;
    }

    // Radius
    PyObject *NavAgent_getRadius(PyObject_NavAgent *self)
    {
        return PyFloat_FromDouble(self->component->getRadius());
    }

    int NavAgent_setRadius(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setRadius(val);
            return 0;
        }
        return -1;
    }

    // Height
    PyObject *NavAgent_getHeight(PyObject_NavAgent *self)
    {
        return PyFloat_FromDouble(self->component->getHeight());
    }

    int NavAgent_setHeight(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setHeight(val);
            return 0;
        }
        return -1;
    }

    //! Max speed
    PyObject *NavAgent_getMaxSpeed(PyObject_NavAgent *self)
    {
        return PyFloat_FromDouble(self->component->getMaxSpeed());
    }

    int NavAgent_setMaxSpeed(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setMaxSpeed(val);
            return 0;
        }
        return -1;
    }

    //! Max acceleration
    PyObject *NavAgent_getMaxAcceleration(PyObject_NavAgent *self)
    {
        return PyFloat_FromDouble(self->component->getMaxAcceleration());
    }

    int NavAgent_setMaxAcceleration(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setMaxAcceleration(val);
            return 0;
        }
        return -1;
    }

    //! Target position
    PyObject *NavAgent_getTargetPosition(PyObject_NavAgent *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getTargetPosition().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int NavAgent_setTargetPosition(PyObject_NavAgent *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setTargetPosition(*((Vec3 *)v));
        return 0;
    }

    //! Auto Update Position
    PyObject *NavAgent_isUpdateNodePosition(PyObject_NavAgent *self)
    {
        return PyBool_FromLong(self->component->isUpdateNodePosition());
    }

    int NavAgent_setUpdateNodePosition(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setUpdateNodePosition(val);
            return 0;
        }
        return -1;
    }

    //! Query filter type
    PyObject *NavAgent_getQueryFilterType(PyObject_NavAgent *self)
    {
        return PyLong_FromLong(self->component->getQueryFilterType());
    }

    int NavAgent_setQueryFilterType(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setQueryFilterType(val);
            return 0;
        }
        return -1;
    }

    //! Obstacle avoidance type
    PyObject *NavAgent_getObstacleAvoidanceType(PyObject_NavAgent *self)
    {
        return PyLong_FromLong(self->component->getObstacleAvoidanceType());
    }

    int NavAgent_setObstacleAvoidanceType(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setObstacleAvoidanceType(val);
            return 0;
        }
        return -1;
    }

    //! Navigation quality
    PyObject *NavAgent_getNavigationQuality(PyObject_NavAgent *self)
    {
        return PyLong_FromLong((int)self->component->getObstacleAvoidanceType());
    }

    int NavAgent_setNavigationQuality(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setObstacleAvoidanceType(val);
            return 0;
        }
        return -1;
    }

    //! Navigation quality
    PyObject *NavAgent_getNavigationPushiness(PyObject_NavAgent *self)
    {
        return PyLong_FromLong((int)self->component->getNavigationPushiness());
    }

    int NavAgent_setNavigationPushiness(PyObject_NavAgent *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setObstacleAvoidanceType(val);
            return 0;
        }
        return -1;
    }

    //! Is in crowd
    PyObject *NavAgent_isInCrowd(PyObject_NavAgent *self)
    {
        return PyBool_FromLong(self->component->isInCrowd());
    }

    //! Has target
    PyObject *NavAgent_hasTarget(PyObject_NavAgent *self)
    {
        return PyBool_FromLong(self->component->isInCrowd());
    }

    //! Reset target
    PyObject *NavAgent_resetTarget(PyObject_NavAgent *self)
    {
        self->component->resetTarget();
        Py_RETURN_NONE;
    }

    //! Has arrived
    PyObject *NavAgent_hasArrived(PyObject_NavAgent *self)
    {
        return PyBool_FromLong(self->component->hasArrived());
    }

    //! Get position
    PyObject *NavAgent_getPosition(PyObject_NavAgent *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getPosition().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    //! Get velocity
    PyObject *NavAgent_getVelocity(PyObject_NavAgent *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getVelocity().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    PyGetSetDef NavAgent_getsets[] = {
        {"id", (getter)NavAgent_getAgentId, (setter)NavAgent_setAgentId, NavAgent_id_doc, NULL},
        {"radius", (getter)NavAgent_getRadius, (setter)NavAgent_setRadius, NavAgent_radius_doc, NULL},
        {"height", (getter)NavAgent_getHeight, (setter)NavAgent_setHeight, NavAgent_height_doc, NULL},
        {"maxSpeed", (getter)NavAgent_getMaxSpeed, (setter)NavAgent_setMaxSpeed, NavAgent_maxSpeed_doc, NULL},
        {"maxAcceleration", (getter)NavAgent_getMaxAcceleration, (setter)NavAgent_setMaxAcceleration, NavAgent_maxAcceleration_doc, NULL},
        {"targetPosition", (getter)NavAgent_getTargetPosition, (setter)NavAgent_setTargetPosition, NavAgent_targetPosition_doc, NULL},
        {"autoUpdatePosition", (getter)NavAgent_isUpdateNodePosition, (setter)NavAgent_setUpdateNodePosition, NavAgent_autoUpdatePosition_doc, NULL},
        {"queryFilterType", (getter)NavAgent_getQueryFilterType, (setter)NavAgent_setQueryFilterType, NavAgent_queryFilterType_doc, NULL},
        {"obstacleAvoidanceType", (getter)NavAgent_getObstacleAvoidanceType, (setter)NavAgent_setObstacleAvoidanceType, NavAgent_obstacleAvoidanceType_doc, NULL},
        {"navigationPushiness", (getter)NavAgent_getNavigationPushiness, (setter)NavAgent_setNavigationPushiness, NavAgent_navigationPushiness_doc, NULL},
        {NULL, NULL},
    };

    // Methods
    PyMethodDef NavAgent_methods[] = {
        {"isInCrowd", (PyCFunction)NavAgent_isInCrowd, METH_NOARGS, NavAgent_isInCrowd_doc},
        {"hasTarget", (PyCFunction)NavAgent_hasTarget, METH_NOARGS, NavAgent_hasTarget_doc},
        {"resetTarget", (PyCFunction)NavAgent_resetTarget, METH_NOARGS, NavAgent_resetTarget_doc},
        {"hasArrived", (PyCFunction)NavAgent_hasArrived, METH_NOARGS, NavAgent_hasArrived_doc},
        {"getPosition", (PyCFunction)NavAgent_getPosition, METH_NOARGS, NavAgent_getPosition_doc},
        {"getVelocity", (PyCFunction)NavAgent_getVelocity, METH_NOARGS, NavAgent_getVelocity_doc},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_NavAgent = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.NavAgent", /* tp_name */
        sizeof(PyObject_NavAgent),                          /* tp_basicsize */
        0,                                                  /* tp_itemsize */
        (destructor)NavAgent_dealloc,                       /* tp_dealloc */
        0,                                                  /* tp_print */
        0,                                                  /* tp_getattr */
        0,                                                  /* tp_setattr */
        0,                                                  /* tp_reserved */
        0,                                                  /* tp_repr */
        0,                                                  /* tp_as_number */
        0,                                                  /* tp_as_sequence */
        0,                                                  /* tp_as_mapping */
        0,                                                  /* tp_hash */
        0,                                                  /* tp_call */
        (reprfunc)NavAgent_str,                             /* tp_str */
        0,                                                  /* tp_getattro */
        0,                                                  /* tp_setattro */
        0,                                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                 /* tp_flags */
        0,                                                  /* tp_doc */
        0,                                                  /* tp_traverse */
        0,                                                  /* tp_clear */
        0,                                                  /* tp_richcompare */
        0,                                                  /* tp_weaklistoffset */
        0,                                                  /* tp_iter */
        0,                                                  /* tp_iternext */
        NavAgent_methods,                                   /* tp_methods */
        0,                                                  /* tp_members */
        NavAgent_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                            /* tp_base */
        0,                                                  /* tp_dict */
        0,                                                  /* tp_descr_get */
        0,                                                  /* tp_descr_set */
        0,                                                  /* tp_dictoffset */
        0,                                                  /* tp_init */
        0,                                                  /* tp_alloc */
        0,                                                  /* tp_new */
        0,                                                  /* tp_free */
    };
} // namespace ige::scene
