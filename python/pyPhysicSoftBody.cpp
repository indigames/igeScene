#include "python/pyPhysicSoftBody.h"
#include "python/pyPhysicSoftBody_doc_en.h"
#include "python/pySceneObject.h"

#include "components/physic/PhysicSoftBody.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "pyRigidBody.h"
using namespace ige::bullet;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void PhysicSoftBody_dealloc(PyObject_PhysicSoftBody *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject *PhysicSoftBody_str(PyObject_PhysicSoftBody *self)
    {
        return PyUnicode_FromString("C++ PhysicSoftBody object");
    }

    //! Apply Repulsion Force
    PyObject *PhysicSoftBody_applyRepulsionForce(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        float timeStep = 0.f;
        int applySpringForce = 0;
        if (PyArg_ParseTuple(value, "fi", &timeStep, &applySpringForce))
            self->component->applyRepulsionForce(timeStep, applySpringForce);
        Py_RETURN_NONE;
    }

    //! Add Velocity
    PyObject *PhysicSoftBody_addVelocity(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        PyObject *velocityObj;
        int nodeIdx = -1;
        if (PyArg_ParseTuple(value, "O|i", &velocityObj, &nodeIdx))
        {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject *)velocityObj, buff, d);
            self->component->addVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)), nodeIdx);
        }
        Py_RETURN_NONE;
    }

    //! Get Volume
    PyObject *PhysicSoftBody_getVolume(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        return PyFloat_FromDouble(self->component->getVolume());
    }

    //! Get Center Of Mass
    PyObject *PhysicSoftBody_getCenterOfMass(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getCenterOfMass()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    //! Get nearest node
    PyObject *PhysicSoftBody_getNearestNodeIndex(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (v)
        {
            return PyLong_FromLong(self->component->getNearestNodeIndex(PhysicHelper::to_btVector3(*((Vec3 *)v))));
        }
        return PyLong_FromLong(-1);
    }

    //! Get node position
    PyObject *PhysicSoftBody_getNodePosition(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        int nodeIdx = -1;
        if (PyArg_ParseTuple(value, "i", &nodeIdx))
        {
            auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(self->component->getNodePosition(nodeIdx)).P(), 3, vec3Obj->v);
            vec3Obj->d = 3;
            return (PyObject *)vec3Obj;
        }
        PyErr_SetString(PyExc_TypeError, "Parameter error: 'nodeIdx' was not set.");
        return NULL;
    }

    //! Get node normal
    PyObject *PhysicSoftBody_getNodeNormal(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        int nodeIdx = -1;
        if (PyArg_ParseTuple(value, "i", &nodeIdx))
        {
            auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(self->component->getNodeNormal(nodeIdx)).P(), 3, vec3Obj->v);
            vec3Obj->d = 3;
            return (PyObject *)vec3Obj;
        }
        PyErr_SetString(PyExc_TypeError, "Parameter error: 'nodeIdx' was not set.");
        return NULL;
    }

    //! Append deformable anchor
    PyObject *PhysicSoftBody_appendDeformableAnchor(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        int nodeIdx = -1;
        PyObject *obj = nullptr;
        if (PyArg_ParseTuple(value, "iO", &nodeIdx, &obj) && nodeIdx >= 0 && obj)
        {
            if (obj->ob_type == &PyTypeObject_SceneObject)
            {
                auto sceneObj = (PyObject_SceneObject *)obj;
                auto physicComp = sceneObj->sceneObject->getComponent<PhysicObject>();
                if (physicComp && physicComp->getBody())
                {
                    self->component->appendDeformableAnchor(nodeIdx, physicComp->getBody());
                }
            }
            else if (obj->ob_type == &PyTypeObject_PhysicObject)
            {
                auto physicObjectObj = (PyObject_PhysicObject *)obj;
                if (physicObjectObj->component->getBody())
                {
                    self->component->appendDeformableAnchor(nodeIdx, physicObjectObj->component->getBody());
                }
            }
            else if (obj->ob_type == &RigidBodyType)
            {
                auto rigidBodyObject = (rigidbody_obj *)obj;
                if (rigidBodyObject->btbody)
                {
                    self->component->appendDeformableAnchor(nodeIdx, (btRigidBody *)(rigidBodyObject->btbody));
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Append anchor
    PyObject *PhysicSoftBody_appendAnchor(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        int nodeIdx = -1;
        PyObject *obj = nullptr;
        int disableLinkedCollission = 0;
        float influent = 1.f;

        if (PyArg_ParseTuple(value, "iO|if", &nodeIdx, &obj, &disableLinkedCollission, &influent) && nodeIdx >= 0 && obj)
        {
            if (obj->ob_type == &PyTypeObject_SceneObject)
            {
                auto sceneObj = (PyObject_SceneObject *)obj;
                auto physicComp = sceneObj->sceneObject->getComponent<PhysicObject>();
                if (physicComp && physicComp->getBody())
                {
                    self->component->appendAnchor(nodeIdx, physicComp->getBody(), disableLinkedCollission, influent);
                }
            }
            else if (obj->ob_type == &PyTypeObject_PhysicObject)
            {
                auto physicObjectObj = (PyObject_PhysicObject *)obj;
                if (physicObjectObj->component->getBody())
                {
                    self->component->appendAnchor(nodeIdx, physicObjectObj->component->getBody(), disableLinkedCollission, influent);
                }
            }
            else if (obj->ob_type == &RigidBodyType)
            {
                auto rigidBodyObject = (rigidbody_obj *)obj;
                if (rigidBodyObject->btbody)
                {
                    self->component->appendAnchor(nodeIdx, (btRigidBody *)(rigidBodyObject->btbody), disableLinkedCollission, influent);
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Remove anchor
    PyObject *PhysicSoftBody_removeAnchor(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        int nodeIdx = -1;
        if (PyArg_ParseTuple(value, "i", &nodeIdx))
        {
            self->component->removeAnchor(nodeIdx);
            Py_RETURN_NONE;
        }
        PyErr_SetString(PyExc_TypeError, "Parameter error: 'nodeIdx' was not set.");
        return NULL;
    }

    //! Damping coefficient
    PyObject *PhysicSoftBody_getDampingCoeff(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getDampingCoeff());
    }

    int PhysicSoftBody_setDampingCoeff(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setDampingCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Pressure coefficient
    PyObject *PhysicSoftBody_getPressureCoeff(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getPressureCoeff());
    }

    int PhysicSoftBody_setPressureCoeff(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPressureCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Volume conversation coefficient
    PyObject *PhysicSoftBody_getVolumeConvCoeff(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getVolumeConvCoeff());
    }

    int PhysicSoftBody_setVolumeConvCoeff(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setVolumeConvCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Dynamic friction coefficient
    PyObject *PhysicSoftBody_getDynamicFrictionCoeff(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getDynamicFrictionCoeff());
    }

    int PhysicSoftBody_setDynamicFrictionCoeff(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setDynamicFrictionCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Pose matching coefficient
    PyObject *PhysicSoftBody_getPoseMatchCoeff(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getPoseMatchCoeff());
    }

    int PhysicSoftBody_setPoseMatchCoeff(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            self->component->setPoseMatchCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Repulsion Stiffness
    PyObject *PhysicSoftBody_getRepulsionStiffness(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getRepulsionStiffness());
    }

    int PhysicSoftBody_setRepulsionStiffness(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setRepulsionStiffness(val);
            return 0;
        }
        return -1;
    }

    //! Sleeping Threshold
    PyObject *PhysicSoftBody_getSleepingThreshold(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getSleepingThreshold());
    }

    int PhysicSoftBody_setSleepingThreshold(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setSleepingThreshold(val);
            return 0;
        }
        return -1;
    }

    //! Rest Length Scale
    PyObject *PhysicSoftBody_getRestLengthScale(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getRestLengthScale());
    }

    int PhysicSoftBody_setRestLengthScale(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setRestLengthScale(val);
            return 0;
        }
        return -1;
    }

    //! Gravity factor
    PyObject *PhysicSoftBody_getGravityFactor(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getGravityFactor());
    }

    int PhysicSoftBody_setGravityFactor(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setGravityFactor(val);
            return 0;
        }
        return -1;
    }

    //! Velocities correction factor
    PyObject *PhysicSoftBody_getVelocityFactor(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getVelocityFactor());
    }

    int PhysicSoftBody_setVelocityFactor(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setVelocityFactor(val);
            return 0;
        }
        return -1;
    }

    //! Rigid contacts hardness
    PyObject *PhysicSoftBody_getRigidContactHardness(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getRigidContactHardness());
    }

    int PhysicSoftBody_setRigidContactHardness(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setRigidContactHardness(val);
            return 0;
        }
        return -1;
    }

    //! Kinetic contacts hardness
    PyObject *PhysicSoftBody_getKineticContactHardness(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getKineticContactHardness());
    }

    int PhysicSoftBody_setKineticContactHardness(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setKineticContactHardness(val);
            return 0;
        }
        return -1;
    }

    //! Soft contacts hardness
    PyObject *PhysicSoftBody_getSoftContactHardness(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getSoftContactHardness());
    }

    int PhysicSoftBody_setSoftContactHardness(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setSoftContactHardness(val);
            return 0;
        }
        return -1;
    }

    //! Anchor hardness
    PyObject *PhysicSoftBody_getAnchorHardness(PyObject_PhysicSoftBody *self)
    {
        return PyFloat_FromDouble(self->component->getAnchorHardness());
    }

    int PhysicSoftBody_setAnchorHardness(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setAnchorHardness(val);
            return 0;
        }
        return -1;
    }

    //! Anchor hardness
    PyObject *PhysicSoftBody_getPosIterationNumber(PyObject_PhysicSoftBody *self)
    {
        return PyLong_FromLong(self->component->getPosIterationNumber());
    }

    int PhysicSoftBody_setPosIterationNumber(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setPosIterationNumber(val);
            return 0;
        }
        return -1;
    }

    //! Aero Model
    PyObject *PhysicSoftBody_getAeroModel(PyObject_PhysicSoftBody *self)
    {
        return PyLong_FromLong(self->component->getAeroModel());
    }

    int PhysicSoftBody_setAeroModel(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setAeroModel(val);
            return 0;
        }
        return -1;
    }

    //! Self collision
    PyObject *PhysicSoftBody_isSelfCollision(PyObject_PhysicSoftBody *self)
    {
        return PyBool_FromLong(self->component->isSelfCollision());
    }

    int PhysicSoftBody_setSelfCollision(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setSelfCollision(val);
            return 0;
        }
        return -1;
    }

    //! Soft soft-collision
    PyObject *PhysicSoftBody_isSoftSoftCollision(PyObject_PhysicSoftBody *self)
    {
        return PyBool_FromLong(self->component->isSoftSoftCollision());
    }

    int PhysicSoftBody_setSoftSoftCollision(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setSoftSoftCollision(val);
            return 0;
        }
        return -1;
    }

    //! Wind velocity
    PyObject *PhysicSoftBody_getWindVelocity(PyObject_PhysicSoftBody *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(self->component->getWindVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicSoftBody_setWindVelocity(PyObject_PhysicSoftBody *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setWindVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    // Methods definition
    PyMethodDef PhysicSoftBody_methods[] = {
        {"applyRepulsionForce", (PyCFunction)PhysicSoftBody_applyRepulsionForce, METH_VARARGS, PhysicSoftBody_applyRepulsionForce_doc},
        {"addVelocity", (PyCFunction)PhysicSoftBody_addVelocity, METH_VARARGS, PhysicSoftBody_addVelocity_doc},
        {"getVolume", (PyCFunction)PhysicSoftBody_getVolume, METH_NOARGS, PhysicSoftBody_getVolume_doc},
        {"getCenterOfMass", (PyCFunction)PhysicSoftBody_getCenterOfMass, METH_NOARGS, PhysicSoftBody_getCenterOfMass_doc},
        {"getNearestNodeIndex", (PyCFunction)PhysicSoftBody_getNearestNodeIndex, METH_VARARGS, PhysicSoftBody_getNearestNodeIndex_doc},
        {"getNodePosition", (PyCFunction)PhysicSoftBody_getNodePosition, METH_VARARGS, PhysicSoftBody_getNodePosition_doc},
        {"getNodeNormal", (PyCFunction)PhysicSoftBody_getNodeNormal, METH_VARARGS, PhysicSoftBody_getNodeNormal_doc},
        {"appendDeformableAnchor", (PyCFunction)PhysicSoftBody_appendDeformableAnchor, METH_VARARGS, PhysicSoftBody_appendDeformableAnchor_doc},
        {"appendAnchor", (PyCFunction)PhysicSoftBody_appendAnchor, METH_VARARGS, PhysicSoftBody_appendAnchor_doc},
        {"removeAnchor", (PyCFunction)PhysicSoftBody_removeAnchor, METH_VARARGS, PhysicSoftBody_removeAnchor_doc},
        {NULL, NULL},
    };

    PyGetSetDef PhysicSoftBody_getsets[] = {
        {"dampingCoeff", (getter)PhysicSoftBody_getDampingCoeff, (setter)PhysicSoftBody_setDampingCoeff, PhysicSoftBody_dampingCoeff_doc, NULL},
        {"linearStiffness", (getter)PhysicSoftBody_getRepulsionStiffness, (setter)PhysicSoftBody_setRepulsionStiffness, PhysicSoftBody_linearStiffness_doc, NULL},
        {"pressureCoeff", (getter)PhysicSoftBody_getPressureCoeff, (setter)PhysicSoftBody_setPressureCoeff, PhysicSoftBody_pressureCoeff_doc, NULL},
        {"volumeConvCoeff", (getter)PhysicSoftBody_getVolumeConvCoeff, (setter)PhysicSoftBody_setVolumeConvCoeff, PhysicSoftBody_volumeConvCoeff_doc, NULL},
        {"dynamicFrictionCoeff", (getter)PhysicSoftBody_getDynamicFrictionCoeff, (setter)PhysicSoftBody_setDynamicFrictionCoeff, PhysicSoftBody_dynamicFrictionCoeff_doc, NULL},
        {"poseMatchCoeff", (getter)PhysicSoftBody_getPoseMatchCoeff, (setter)PhysicSoftBody_setPoseMatchCoeff, PhysicSoftBody_poseMatchCoeff_doc, NULL},
        {"velocityFactor", (getter)PhysicSoftBody_getVelocityFactor, (setter)PhysicSoftBody_setVelocityFactor, PhysicSoftBody_velocityFactor_doc, NULL},
        {"gravityFactor", (getter)PhysicSoftBody_getGravityFactor, (setter)PhysicSoftBody_setGravityFactor, PhysicSoftBody_gravityFactor_doc, NULL},
        {"rigidContactHardness", (getter)PhysicSoftBody_getRigidContactHardness, (setter)PhysicSoftBody_setRigidContactHardness, PhysicSoftBody_rigidContactHardness_doc, NULL},
        {"kineticContactHardness", (getter)PhysicSoftBody_getKineticContactHardness, (setter)PhysicSoftBody_setKineticContactHardness, PhysicSoftBody_kineticContactHardness_doc, NULL},
        {"softContactHardness", (getter)PhysicSoftBody_getSoftContactHardness, (setter)PhysicSoftBody_setSoftContactHardness, PhysicSoftBody_softContactHardness_doc, NULL},
        {"anchorHardness", (getter)PhysicSoftBody_getAnchorHardness, (setter)PhysicSoftBody_setAnchorHardness, PhysicSoftBody_anchorHardness_doc, NULL},
        {"sleepingThreshold", (getter)PhysicSoftBody_getSleepingThreshold, (setter)PhysicSoftBody_setSleepingThreshold, PhysicSoftBody_sleepingThreshold_doc, NULL},
        {"restLengthScale", (getter)PhysicSoftBody_getRestLengthScale, (setter)PhysicSoftBody_setRestLengthScale, PhysicSoftBody_restLengthScale_doc, NULL},
        {"positionIterNumber", (getter)PhysicSoftBody_getPosIterationNumber, (setter)PhysicSoftBody_setPosIterationNumber, PhysicSoftBody_positionIterNumber_doc, NULL},
        {"aeroModel", (getter)PhysicSoftBody_getAeroModel, (setter)PhysicSoftBody_setAeroModel, PhysicSoftBody_aeroModel_doc, NULL},
        {"windVelocity", (getter)PhysicSoftBody_getWindVelocity, (setter)PhysicSoftBody_setWindVelocity, PhysicSoftBody_windVelocity_doc, NULL},
        {"selfCollision", (getter)PhysicSoftBody_isSelfCollision, (setter)PhysicSoftBody_setSelfCollision, PhysicSoftBody_selfCollision_doc, NULL},
        {"softCollision", (getter)PhysicSoftBody_isSoftSoftCollision, (setter)PhysicSoftBody_setSoftSoftCollision, PhysicSoftBody_softCollision_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_PhysicSoftBody = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.PhysicSoftBody", /* tp_name */
        sizeof(PyObject_PhysicSoftBody),                          /* tp_basicsize */
        0,                                                        /* tp_itemsize */
        (destructor)PhysicSoftBody_dealloc,                       /* tp_dealloc */
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
        (reprfunc)PhysicSoftBody_str,                             /* tp_str */
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
        PhysicSoftBody_methods,                                   /* tp_methods */
        0,                                                        /* tp_members */
        PhysicSoftBody_getsets,                                   /* tp_getset */
        &PyTypeObject_PhysicObject,                               /* tp_base */
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
