#include "python/pyPhysicManager.h"
#include "python/pyPhysicManager_doc_en.h"
#include "python/pySceneObject.h"

#include "physic/PhysicManager.h"
#include "utils/PhysicHelper.h"

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    // Deallocation
    void PhysicManager_dealloc(PyObject_PhysicManager *self)
    {
        if (self)
        {
            self->physicManager = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    // String representation
    PyObject *PhysicManager_str(PyObject_PhysicManager *self)
    {
        return PyUnicode_FromString("C++ PhysicManager object");
    }

    // Get singleton instance
    PyObject *PhysicManager_getInstance()
    {
        auto *self = PyObject_New(PyObject_PhysicManager, &PyTypeObject_PhysicManager);
        self->physicManager = PhysicManager::getInstance().get();
        return (PyObject *)self;
    }

    // Initialize
    PyObject *PhysicManager_initialize(PyObject_PhysicManager *self, PyObject *value)
    {
        int numIter = 4, deformable = 0;
        if (PyArg_ParseTuple(value, "ii", &numIter, &deformable))
        {
            self->physicManager->initialize(numIter, deformable);
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }

    // Clear
    PyObject *PhysicManager_clear(PyObject_PhysicManager *self)
    {
        self->physicManager->clear();
        Py_RETURN_NONE;
    }

    // Get deformable
    PyObject *PhysicManager_isDeformable(PyObject_PhysicManager *self)
    {
        return PyBool_FromLong(self->physicManager->isDeformable());
    }

    // Raytest closest
    PyObject *PhysicManager_rayTestClosest(PyObject_PhysicManager *self, PyObject *value)
    {
        PyObject *startObj;
        PyObject *endObj;
        int mask = -1;
        int group = -1;
        if (!PyArg_ParseTuple(value, "OO|ii", &startObj, &endObj, &mask, &group))
            return NULL;

        int d;
        float buff[4];
        auto v = pyObjToFloat(startObj, buff, d);
        if (!v)
            return NULL;
        auto start = PhysicHelper::to_btVector3(*((Vec3 *)v));

        v = pyObjToFloat(endObj, buff, d);
        if (!v)
            return NULL;
        auto end = PhysicHelper::to_btVector3(*((Vec3 *)v));

        auto hit = self->physicManager->rayTestClosest(start, end, group, mask);
        if (hit.m_object == nullptr)
            Py_RETURN_NONE;

        auto hitObj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
        hitObj->sceneObject = hit.m_object;

        auto hitPos = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(hit.m_position).P(), 3, hitPos->v);
        hitPos->d = 3;

        auto hitNorm = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(hit.m_normal).P(), 3, hitNorm->v);
        hitNorm->d = 3;

        PyObject *res = Py_BuildValue("{s:O,s:O,s:O,s:i}",
                                      "hitObject", hitObj,
                                      "hitPosition", hitPos,
                                      "hitNormal", hitNorm);
        Py_XDECREF(hitObj);
        Py_XDECREF(hitPos);
        Py_XDECREF(hitNorm);
        return res;
    }

    // Raytest all
    PyObject *PhysicManager_rayTestAll(PyObject_PhysicManager *self, PyObject *value)
    {
        PyObject *startObj;
        PyObject *endObj;
        int mask = -1;
        int group = -1;
        if (!PyArg_ParseTuple(value, "OO|ii", &startObj, &endObj, &mask, &group))
            return NULL;

        int d;
        float buff[4];
        auto v = pyObjToFloat(startObj, buff, d);
        if (!v)
            return NULL;
        auto start = PhysicHelper::to_btVector3(*((Vec3 *)v));

        v = pyObjToFloat(endObj, buff, d);
        if (!v)
            return NULL;
        auto end = PhysicHelper::to_btVector3(*((Vec3 *)v));

        auto hits = self->physicManager->rayTestAll(start, end, group, mask);
        PyObject *res = PyTuple_New(hits.size());

        for (int i = 0; i < hits.size(); ++i)
        {
            auto hit = self->physicManager->rayTestClosest(start, end, group, mask);
            if (hit.m_object == nullptr)
                Py_RETURN_NONE;

            auto hitObj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            hitObj->sceneObject = hit.m_object;

            auto hitPos = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(hit.m_position).P(), 3, hitPos->v);
            hitPos->d = 3;

            auto hitNorm = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(hit.m_normal).P(), 3, hitNorm->v);
            hitNorm->d = 3;

            PyObject *hitTuple = Py_BuildValue("{s:O,s:O,s:O,s:i}",
                                        "hitObject", hitObj,
                                        "hitPosition", hitPos,
                                        "hitNormal", hitNorm);

            PyTuple_SetItem(res, i, hitTuple);

            Py_XDECREF(hitObj);
            Py_XDECREF(hitPos);
            Py_XDECREF(hitNorm);
        }
        return res;
    }

    // Get gravity
    PyObject *PhysicManager_getGravity(PyObject_PhysicManager *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->physicManager->getGravity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    // Set gravity
    int PhysicManager_setGravity(PyObject_PhysicManager *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->physicManager->setGravity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    // Get number of iteration
    PyObject *PhysicManager_getNumIteration(PyObject_PhysicManager *self)
    {
        return PyLong_FromLong(self->physicManager->getNumIteration());
    }

    // Set number of iteration
    int PhysicManager_setNumIteration(PyObject_PhysicManager *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->physicManager->setNumIteration(val);
            return 0;
        }
        return -1;
    }

    // Get frame update ratio (speedup/slower effects)
    PyObject *PhysicManager_getFrameUpdateRatio(PyObject_PhysicManager *self)
    {
        return PyFloat_FromDouble(self->physicManager->getFrameUpdateRatio());
    }

    // Set frame update ratio
    int PhysicManager_setFrameUpdateRatio(PyObject_PhysicManager *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->physicManager->setFrameUpdateRatio(val);
            return 0;
        }
        return -1;
    }

    // Get frame max simulation sub step
    PyObject *PhysicManager_getFrameMaxSubStep(PyObject_PhysicManager *self)
    {
        return PyLong_FromLong(self->physicManager->getFrameMaxSubStep());
    }

    // Set frame max simulation sub step
    int PhysicManager_setFrameMaxSubStep(PyObject_PhysicManager *self, PyObject *value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->physicManager->setFrameMaxSubStep(val);
            return 0;
        }
        return -1;
    }

    // Get fixed time steps
    PyObject *PhysicManager_getFixedTimeStep(PyObject_PhysicManager *self)
    {
        return PyFloat_FromDouble(self->physicManager->getFixedTimeStep());
    }

    // Set fixed time steps
    int PhysicManager_setFixedTimeStep(PyObject_PhysicManager *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->physicManager->setFixedTimeStep(val);
            return 0;
        }
        return -1;
    }

    // Methods
    PyMethodDef PhysicManager_methods[] = {
        {"getInstance", (PyCFunction)PhysicManager_getInstance, METH_NOARGS | METH_STATIC, PhysicManager_getInstance_doc},
        {"initialize", (PyCFunction)PhysicManager_initialize, METH_VARARGS, PhysicManager_initialize_doc},
        {"clear", (PyCFunction)PhysicManager_clear, METH_NOARGS, PhysicManager_clear_doc},
        {"isDeformable", (PyCFunction)PhysicManager_isDeformable, METH_NOARGS, PhysicManager_isDeformable_doc},
        {"rayTestClosest", (PyCFunction)PhysicManager_rayTestClosest, METH_VARARGS, PhysicManager_rayTestClosest_doc},
        {"rayTestAll", (PyCFunction)PhysicManager_rayTestAll, METH_VARARGS, PhysicManager_rayTestAll_doc},
        {NULL, NULL}};

    // Get/Set
    PyGetSetDef PhysicManager_getsets[] = {
        {"gravity", (getter)PhysicManager_getGravity, (setter)PhysicManager_setGravity, PhysicManager_gravity_doc, NULL},
        {"numIteration", (getter)PhysicManager_getNumIteration, (setter)PhysicManager_setNumIteration, PhysicManager_numIteration_doc, NULL},
        {"frameUpdateRatio", (getter)PhysicManager_getFrameUpdateRatio, (setter)PhysicManager_setFrameUpdateRatio, PhysicManager_frameUpdateRatio_doc, NULL},
        {"frameMaxSubStep", (getter)PhysicManager_getFrameMaxSubStep, (setter)PhysicManager_setFrameMaxSubStep, PhysicManager_frameMaxSubStep_doc, NULL},
        {"fixedTimeStep", (getter)PhysicManager_getFixedTimeStep, (setter)PhysicManager_setFixedTimeStep, PhysicManager_fixedTimeStep_doc, NULL},
        {NULL, NULL}};

    // Type declaration
    PyTypeObject PyTypeObject_PhysicManager = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.PhysicManager", /* tp_name */
        sizeof(PyObject_PhysicManager),                          /* tp_basicsize */
        0,                                                       /* tp_itemsize */
        (destructor)PhysicManager_dealloc,                       /* tp_dealloc */
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
        (reprfunc)PhysicManager_str,                             /* tp_str */
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
        PhysicManager_methods,                                   /* tp_methods */
        0,                                                       /* tp_members */
        PhysicManager_getsets,                                   /* tp_getset */
        0,                                                       /* tp_base */
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
