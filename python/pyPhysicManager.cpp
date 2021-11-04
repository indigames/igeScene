#include "python/pyPhysicManager.h"
#include "python/pyPhysicManager_doc_en.h"
#include "python/pySceneObject.h"
#include "python/pyPhysicObject.h"

#include "components/physic/PhysicManager.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "utils/PhysicHelper.h"

#include "pyRigidBody.h"
using namespace ige::bullet;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    // Deallocation
    void PhysicManager_dealloc(PyObject_PhysicManager *self)
    {
        if (self) {
            self->component.reset();
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
        if (SceneManager::getInstance()->getCurrentScene())
        {
            auto physicManager = SceneManager::getInstance()->getCurrentScene()->getRoot()->getComponent<PhysicManager>();
            if (physicManager) {
                auto* self = (PyObject_PhysicManager*)(&PyTypeObject_PhysicManager)->tp_alloc(&PyTypeObject_PhysicManager, 0);
                self->component = physicManager;
                return (PyObject*)self;
            }
        }
        Py_RETURN_NONE;
    }

    // Clear
    PyObject *PhysicManager_clear(PyObject_PhysicManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->clear();
        Py_RETURN_NONE;
    }

    // Get deformable
    PyObject *PhysicManager_isDeformable(PyObject_PhysicManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->isDeformable());
    }

    // Raytest closest
    PyObject *PhysicManager_rayTestClosest(PyObject_PhysicManager *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
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

        auto hit = std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->rayTestClosest(start, end, group, mask);
        if (hit.object == nullptr)
            Py_RETURN_NONE;

        auto hitObj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
        hitObj->sceneObject = reinterpret_cast<PhysicObject *>(hit.object->getUserPointer())->getOwner()->getSharedPtr();

        auto hitPos = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(hit.position).P(), 3, hitPos->v);
        hitPos->d = 3;

        auto hitNorm = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(hit.normal).P(), 3, hitNorm->v);
        hitNorm->d = 3;

        PyObject *res = Py_BuildValue("{s:O,s:O,s:O}",
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
        if (self->component.expired()) Py_RETURN_NONE;
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

        auto hits = std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->rayTestAll(start, end, group, mask);
        PyObject *res = PyTuple_New(hits.size());

        for (int i = 0; i < hits.size(); ++i)
        {
            auto hit = std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->rayTestClosest(start, end, group, mask);
            if (hit.object == nullptr)
                Py_RETURN_NONE;

            auto hitObj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            hitObj->sceneObject = reinterpret_cast<PhysicObject *>(hits[i].object->getUserPointer())->getOwner()->getSharedPtr();

            auto hitPos = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(hits[i].position).P(), 3, hitPos->v);
            hitPos->d = 3;

            auto hitNorm = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(hits[i].normal).P(), 3, hitNorm->v);
            hitNorm->d = 3;

            PyObject *hitTuple = Py_BuildValue("{s:O,s:O,s:O}",
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

    // Contact test
    PyObject *PhysicManager_contactTest(PyObject_PhysicManager *self, PyObject *args)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject *obj;
        int group = 1;
        int mask = -1;

        btCollisionObject *object = nullptr;

        if (!PyArg_ParseTuple(args, "O|ii", &obj, &mask, &group))
        {
            PyErr_SetString(PyExc_TypeError, "[contactTest] Parameter error!");
            return NULL;
        }

        if (obj->ob_type == &PyTypeObject_SceneObject)
        {
            auto sceneObj = (PyObject_SceneObject*)obj;
            auto physicComp = sceneObj->sceneObject.lock()->getComponent<PhysicObject>();
            if (physicComp && physicComp->getBody())
            {
                object = physicComp->getBody();
            }
        }
        else if (obj->ob_type == &PyTypeObject_PhysicObject)
        {
            auto physicObjectObj = (PyObject_PhysicObject*)obj;
            object = std::dynamic_pointer_cast<PhysicObject>(physicObjectObj->component.lock())->getBody();
        }
        else if (obj->ob_type == &RigidBodyType)
        {
            auto rigidBodyObject = (rigidbody_obj*)obj;
            if (rigidBodyObject->btbody)
                object = (btCollisionObject*)rigidBodyObject->btbody;
        }

        if (object == nullptr)
            return PyTuple_New(0);

        auto results = std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->contactTest(object, group, mask);
        PyObject *pyResults = PyTuple_New(results.size());
        for (int i = 0; i < results.size(); ++i)
        {
            const auto &result = results[i];

            auto objectA = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            objectA->sceneObject = reinterpret_cast<PhysicObject *>(result.objectA->getUserPointer())->getOwner()->getSharedPtr();

            auto objectB = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            objectB->sceneObject = reinterpret_cast<PhysicObject *>(result.objectB->getUserPointer())->getOwner()->getSharedPtr();

            auto localPosA = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.localPosA).P(), 3, localPosA->v);
            localPosA->d = 3;

            auto localPosB = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.localPosB).P(), 3, localPosB->v);
            localPosB->d = 3;

            auto worldPosA = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.worldPosA).P(), 3, worldPosA->v);
            worldPosA->d = 3;

            auto worldPosB = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.worldPosB).P(), 3, worldPosB->v);
            worldPosB->d = 3;

            auto normalB = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.normalB).P(), 3, normalB->v);
            normalB->d = 3;

            PyObject *pyResult = Py_BuildValue("{s:O,s:O,s:O,s:O,s:O,s:O,s:O}",
                                               "objectA", objectA,
                                               "objectB", objectB,
                                               "localPosA", localPosA,
                                               "localPosB", localPosB,
                                               "worldPosA", worldPosA,
                                               "worldPosB", worldPosB,
                                               "normalB", normalB);

            PyTuple_SetItem(pyResults, i, pyResult);
        }
        return pyResults;
    }

    // Contact pair test
    PyObject *PhysicManager_contactPairTest(PyObject_PhysicManager *self, PyObject *args)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject *objA;
        PyObject *objB;
        int group = 1;
        int mask = -1;

        btCollisionObject *objectA = nullptr;
        btCollisionObject *objectB = nullptr;

        if (!PyArg_ParseTuple(args, "OO|ii", &objA, &objB, &mask, &group))
        {
            PyErr_SetString(PyExc_TypeError, "[contactPairTest] Parameter error!");
            return NULL;
        }

        if (objA->ob_type == &PyTypeObject_SceneObject)
        {
            auto sceneObj = (PyObject_SceneObject*)objA;
            auto physicComp = sceneObj->sceneObject.lock()->getComponent<PhysicObject>();
            if (physicComp && physicComp->getBody())
            {
                objectA = physicComp->getBody();
            }
        }
        else if (objA->ob_type == &PyTypeObject_PhysicObject)
        {
            auto physicObjectObj = (PyObject_PhysicObject*)objA;
            objectA = std::dynamic_pointer_cast<PhysicObject>(physicObjectObj->component.lock())->getBody();
        }
        else if (objA->ob_type == &RigidBodyType)
        {
            auto rigidBodyObject = (rigidbody_obj*)objA;
            if (rigidBodyObject->btbody)
                objectA = (btCollisionObject*)rigidBodyObject->btbody;
        }

        if (objB->ob_type == &PyTypeObject_SceneObject)
        {
            auto sceneObj = (PyObject_SceneObject*)objB;
            auto physicComp = sceneObj->sceneObject.lock()->getComponent<PhysicObject>();
            if (physicComp && physicComp->getBody())
            {
                objectB = physicComp->getBody();
            }
        }
        else if (objB->ob_type == &PyTypeObject_PhysicObject)
        {
            auto physicObjectObj = (PyObject_PhysicObject*)objB;
            objectB = std::dynamic_pointer_cast<PhysicObject>(physicObjectObj->component.lock())->getBody();            
        }
        else if (objB->ob_type == &RigidBodyType)
        {
            auto rigidBodyObject = (rigidbody_obj*)objB;
            if (rigidBodyObject->btbody)
                objectB = (btCollisionObject*)rigidBodyObject->btbody;
        }

        if (objectA == nullptr || objectB == nullptr)
            return PyTuple_New(0);

        auto results = std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->contactPairTest(objectA, objectB, group, mask);

        PyObject *pyResults = PyTuple_New(results.size());
        for (int i = 0; i < results.size(); ++i)
        {
            const auto &result = results[i];

            auto objectA = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            objectA->sceneObject = reinterpret_cast<PhysicObject *>(result.objectA->getUserPointer())->getOwner()->getSharedPtr();

            auto objectB = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            objectB->sceneObject = reinterpret_cast<PhysicObject *>(result.objectB->getUserPointer())->getOwner()->getSharedPtr();

            auto localPosA = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.localPosA).P(), 3, localPosA->v);
            localPosA->d = 3;

            auto localPosB = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.localPosB).P(), 3, localPosB->v);
            localPosB->d = 3;

            auto worldPosA = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.worldPosA).P(), 3, worldPosA->v);
            worldPosA->d = 3;

            auto worldPosB = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.worldPosB).P(), 3, worldPosB->v);
            worldPosB->d = 3;

            auto normalB = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(result.normalB).P(), 3, normalB->v);
            normalB->d = 3;

            PyObject *pyResult = Py_BuildValue("{s:O,s:O,s:O,s:O,s:O,s:O,s:O}",
                                               "objectA", objectA,
                                               "objectB", objectB,
                                               "localPosA", localPosA,
                                               "localPosB", localPosB,
                                               "worldPosA", worldPosA,
                                               "worldPosB", worldPosB,
                                               "normalB", normalB);

            PyTuple_SetItem(pyResults, i, pyResult);
        }
        return pyResults;
    }

    // Get gravity
    PyObject *PhysicManager_getGravity(PyObject_PhysicManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->getGravity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    // Set gravity
    int PhysicManager_setGravity(PyObject_PhysicManager *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->setGravity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    // Get number of iteration
    PyObject *PhysicManager_getNumIteration(PyObject_PhysicManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->getNumIteration());
    }

    // Set number of iteration
    int PhysicManager_setNumIteration(PyObject_PhysicManager *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->setNumIteration(val);
            return 0;
        }
        return -1;
    }

    // Get frame update ratio (speedup/slower effects)
    PyObject *PhysicManager_getFrameUpdateRatio(PyObject_PhysicManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->getFrameUpdateRatio());
    }

    // Set frame update ratio
    int PhysicManager_setFrameUpdateRatio(PyObject_PhysicManager *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->setFrameUpdateRatio(val);
            return 0;
        }
        return -1;
    }

    // Get frame max simulation sub step
    PyObject *PhysicManager_getFrameMaxSubStep(PyObject_PhysicManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->getFrameMaxSubStep());
    }

    // Set frame max simulation sub step
    int PhysicManager_setFrameMaxSubStep(PyObject_PhysicManager *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->setFrameMaxSubStep(val);
            return 0;
        }
        return -1;
    }

    // Get fixed time steps
    PyObject *PhysicManager_getFixedTimeStep(PyObject_PhysicManager *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->getFixedTimeStep());
    }

    // Set fixed time steps
    int PhysicManager_setFixedTimeStep(PyObject_PhysicManager *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<PhysicManager>(self->component.lock())->setFixedTimeStep(val);
            return 0;
        }
        return -1;
    }

    // Methods
    PyMethodDef PhysicManager_methods[] = {
        {"getInstance", (PyCFunction)PhysicManager_getInstance, METH_NOARGS | METH_STATIC, PhysicManager_getInstance_doc},
        {"clear", (PyCFunction)PhysicManager_clear, METH_NOARGS, PhysicManager_clear_doc},
        {"isDeformable", (PyCFunction)PhysicManager_isDeformable, METH_NOARGS, PhysicManager_isDeformable_doc},
        {"rayTestClosest", (PyCFunction)PhysicManager_rayTestClosest, METH_VARARGS, PhysicManager_rayTestClosest_doc},
        {"rayTestAll", (PyCFunction)PhysicManager_rayTestAll, METH_VARARGS, PhysicManager_rayTestAll_doc},
        {"contactTest", (PyCFunction)PhysicManager_contactTest, METH_VARARGS, PhysicManager_contactTest_doc},
        {"contactPairTest", (PyCFunction)PhysicManager_contactPairTest, METH_VARARGS, PhysicManager_contactPairTest_doc},
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
        &PyTypeObject_Component,                                 /* tp_base */
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
