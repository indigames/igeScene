#include "python/pySoftbody.h"
#include "python/pySoftbody_doc_en.h"
#include "python/pySceneObject.h"

#include "components/physic/Softbody.h"
#include "scene/SceneObject.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void Softbody_dealloc(PyObject_Softbody *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *Softbody_str(PyObject_Softbody *self)
    {
        return PyUnicode_FromString("C++ Softbody object");
    }

    //! Apply Repulsion Force
    PyObject *Softbody_applyRepulsionForce(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        float timeStep = 0.f;
        int applySpringForce = 0;
        if (PyArg_ParseTuple(value, "fi", &timeStep, &applySpringForce))
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->applyRepulsionForce(timeStep, applySpringForce);
        Py_RETURN_NONE;
    }

    //! Add Velocity
    PyObject *Softbody_addVelocity(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject *velocityObj;
        int nodeIdx = -1;
        if (PyArg_ParseTuple(value, "O|i", &velocityObj, &nodeIdx))
        {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject *)velocityObj, buff, d);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->addVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)), nodeIdx);
        }
        Py_RETURN_NONE;
    }

    //! Get Volume
    PyObject *Softbody_getVolume(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getVolume());
    }

    //! Get Center Of Mass
    PyObject *Softbody_getCenterOfMass(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getCenterOfMass()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    //! Get nearest node
    PyObject *Softbody_getNearestNodeIndex(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return PyLong_FromLong(-1);
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (v)
        {
            return PyLong_FromLong(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getNearestNodeIndex(PhysicHelper::to_btVector3(*((Vec3 *)v))));
        }
        return PyLong_FromLong(-1);
    }

    //! Get node position
    PyObject *Softbody_getNodePosition(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int nodeIdx = -1;
        if (PyArg_ParseTuple(value, "i", &nodeIdx))
        {
            auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getNodePosition(nodeIdx)).P(), 3, vec3Obj->v);
            vec3Obj->d = 3;
            return (PyObject *)vec3Obj;
        }
        PyErr_SetString(PyExc_TypeError, "Parameter error: 'nodeIdx' was not set.");
        return NULL;
    }

    //! Get node normal
    PyObject *Softbody_getNodeNormal(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int nodeIdx = -1;
        if (PyArg_ParseTuple(value, "i", &nodeIdx))
        {
            auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
            vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getNodeNormal(nodeIdx)).P(), 3, vec3Obj->v);
            vec3Obj->d = 3;
            return (PyObject *)vec3Obj;
        }
        PyErr_SetString(PyExc_TypeError, "Parameter error: 'nodeIdx' was not set.");
        return NULL;
    }

    //! Append deformable anchor
    PyObject *Softbody_appendDeformableAnchor(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int nodeIdx = -1;
        PyObject *obj = nullptr;
        if (PyArg_ParseTuple(value, "iO", &nodeIdx, &obj) && nodeIdx >= 0 && obj)
        {
            if (obj->ob_type == &PyTypeObject_SceneObject)
            {
                auto sceneObj = (PyObject_SceneObject *)obj;
                if (!sceneObj->sceneObject.expired()) {
                    auto physicComp = sceneObj->sceneObject.lock()->getComponent<Rigidbody>();
                    if (physicComp && physicComp->getBody()) {
                        std::dynamic_pointer_cast<Softbody>(self->component.lock())->appendDeformableAnchor(nodeIdx, physicComp->getBody());
                    }
                }
            }
            else if (obj->ob_type == &PyTypeObject_Rigidbody)
            {
                auto RigidbodyObj = (PyObject_Rigidbody *)obj;
                if (!RigidbodyObj->component.expired()) {
                    auto body = std::dynamic_pointer_cast<Rigidbody>(RigidbodyObj->component.lock())->getBody();
                    if (body) {
                        std::dynamic_pointer_cast<Softbody>(self->component.lock())->appendDeformableAnchor(nodeIdx, body);
                    }
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Append anchor
    PyObject *Softbody_appendAnchor(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int nodeIdx = -1;
        PyObject *obj = nullptr;
        int disableLinkedCollission = 0;
        float influent = 1.f;

        if (PyArg_ParseTuple(value, "iO|if", &nodeIdx, &obj, &disableLinkedCollission, &influent) && nodeIdx >= 0 && obj)
        {
            if (obj->ob_type == &PyTypeObject_SceneObject)
            {
                auto sceneObj = (PyObject_SceneObject *)obj;
                if (!sceneObj->sceneObject.expired()) {
                    auto physicComp = sceneObj->sceneObject.lock()->getComponent<Rigidbody>();
                    if (physicComp && physicComp->getBody()) {
                        std::dynamic_pointer_cast<Softbody>(self->component.lock())->appendAnchor(nodeIdx, physicComp->getBody(), disableLinkedCollission, influent);
                    }
                }
            }
            else if (obj->ob_type == &PyTypeObject_Rigidbody)
            {
                auto RigidbodyObj = (PyObject_Rigidbody *)obj;
                if (!RigidbodyObj->component.expired()) {
                    auto body = std::dynamic_pointer_cast<Rigidbody>(RigidbodyObj->component.lock())->getBody();
                    if (body) {
                        std::dynamic_pointer_cast<Softbody>(self->component.lock())->appendAnchor(nodeIdx, body, disableLinkedCollission, influent);
                    }
                }
            }
        }
        Py_RETURN_NONE;
    }

    //! Remove anchor
    PyObject *Softbody_removeAnchor(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        int nodeIdx = -1;
        if (PyArg_ParseTuple(value, "i", &nodeIdx))
        {
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->removeAnchor(nodeIdx);
            Py_RETURN_NONE;
        }
        PyErr_SetString(PyExc_TypeError, "Parameter error: 'nodeIdx' was not set.");
        return NULL;
    }

    //! Damping coefficient
    PyObject *Softbody_getDampingCoeff(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getDampingCoeff());
    }

    int Softbody_setDampingCoeff(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setDampingCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Pressure coefficient
    PyObject *Softbody_getPressureCoeff(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getPressureCoeff());
    }

    int Softbody_setPressureCoeff(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setPressureCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Volume conversation coefficient
    PyObject *Softbody_getVolumeConvCoeff(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getVolumeConvCoeff());
    }

    int Softbody_setVolumeConvCoeff(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setVolumeConvCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Dynamic friction coefficient
    PyObject *Softbody_getDynamicFrictionCoeff(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getDynamicFrictionCoeff());
    }

    int Softbody_setDynamicFrictionCoeff(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setDynamicFrictionCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Pose matching coefficient
    PyObject *Softbody_getPoseMatchCoeff(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getPoseMatchCoeff());
    }

    int Softbody_setPoseMatchCoeff(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        float val;
        if (PyArg_ParseTuple(value, "f", &val))
        {
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setPoseMatchCoeff(val);
            return 0;
        }
        return -1;
    }

    //! Repulsion Stiffness
    PyObject *Softbody_getRepulsionStiffness(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getRepulsionStiffness());
    }

    int Softbody_setRepulsionStiffness(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setRepulsionStiffness(val);
            return 0;
        }
        return -1;
    }

    //! Sleeping Threshold
    PyObject *Softbody_getSleepingThreshold(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getSleepingThreshold());
    }

    int Softbody_setSleepingThreshold(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setSleepingThreshold(val);
            return 0;
        }
        return -1;
    }

    //! Rest Length Scale
    PyObject *Softbody_getRestLengthScale(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getRestLengthScale());
    }

    int Softbody_setRestLengthScale(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setRestLengthScale(val);
            return 0;
        }
        return -1;
    }

    //! Gravity factor
    PyObject *Softbody_getGravityFactor(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getGravityFactor());
    }

    int Softbody_setGravityFactor(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setGravityFactor(val);
            return 0;
        }
        return -1;
    }

    //! Velocities correction factor
    PyObject *Softbody_getVelocityFactor(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getVelocityFactor());
    }

    int Softbody_setVelocityFactor(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setVelocityFactor(val);
            return 0;
        }
        return -1;
    }

    //! Rigid contacts hardness
    PyObject *Softbody_getRigidContactHardness(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getRigidContactHardness());
    }

    int Softbody_setRigidContactHardness(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setRigidContactHardness(val);
            return 0;
        }
        return -1;
    }

    //! Kinetic contacts hardness
    PyObject *Softbody_getKineticContactHardness(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getKineticContactHardness());
    }

    int Softbody_setKineticContactHardness(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setKineticContactHardness(val);
            return 0;
        }
        return -1;
    }

    //! Soft contacts hardness
    PyObject *Softbody_getSoftContactHardness(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getSoftContactHardness());
    }

    int Softbody_setSoftContactHardness(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setSoftContactHardness(val);
            return 0;
        }
        return -1;
    }

    //! Anchor hardness
    PyObject *Softbody_getAnchorHardness(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getAnchorHardness());
    }

    int Softbody_setAnchorHardness(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value)) {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setAnchorHardness(val);
            return 0;
        }
        return -1;
    }

    //! Anchor hardness
    PyObject *Softbody_getPosIterationNumber(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getPosIterationNumber());
    }

    int Softbody_setPosIterationNumber(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setPosIterationNumber(val);
            return 0;
        }
        return -1;
    }

    //! Aero Model
    PyObject *Softbody_getAeroModel(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getAeroModel());
    }

    int Softbody_setAeroModel(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setAeroModel(val);
            return 0;
        }
        return -1;
    }

    //! Self collision
    PyObject *Softbody_isSelfCollision(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Softbody>(self->component.lock())->isSelfCollision());
    }

    int Softbody_setSelfCollision(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setSelfCollision(val);
            return 0;
        }
        return -1;
    }

    //! Soft soft-collision
    PyObject *Softbody_isSoftSoftCollision(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<Softbody>(self->component.lock())->isSoftSoftCollision());
    }

    int Softbody_setSoftSoftCollision(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<Softbody>(self->component.lock())->setSoftSoftCollision(val);
            return 0;
        }
        return -1;
    }

    //! Wind velocity
    PyObject *Softbody_getWindVelocity(PyObject_Softbody *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(PhysicHelper::from_btVector3(std::dynamic_pointer_cast<Softbody>(self->component.lock())->getWindVelocity()).P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int Softbody_setWindVelocity(PyObject_Softbody *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<Softbody>(self->component.lock())->setWindVelocity(PhysicHelper::to_btVector3(*((Vec3 *)v)));
        return 0;
    }

    // Methods definition
    PyMethodDef Softbody_methods[] = {
        {"applyRepulsionForce", (PyCFunction)Softbody_applyRepulsionForce, METH_VARARGS, Softbody_applyRepulsionForce_doc},
        {"addVelocity", (PyCFunction)Softbody_addVelocity, METH_VARARGS, Softbody_addVelocity_doc},
        {"getVolume", (PyCFunction)Softbody_getVolume, METH_NOARGS, Softbody_getVolume_doc},
        {"getCenterOfMass", (PyCFunction)Softbody_getCenterOfMass, METH_NOARGS, Softbody_getCenterOfMass_doc},
        {"getNearestNodeIndex", (PyCFunction)Softbody_getNearestNodeIndex, METH_VARARGS, Softbody_getNearestNodeIndex_doc},
        {"getNodePosition", (PyCFunction)Softbody_getNodePosition, METH_VARARGS, Softbody_getNodePosition_doc},
        {"getNodeNormal", (PyCFunction)Softbody_getNodeNormal, METH_VARARGS, Softbody_getNodeNormal_doc},
        {"appendDeformableAnchor", (PyCFunction)Softbody_appendDeformableAnchor, METH_VARARGS, Softbody_appendDeformableAnchor_doc},
        {"appendAnchor", (PyCFunction)Softbody_appendAnchor, METH_VARARGS, Softbody_appendAnchor_doc},
        {"removeAnchor", (PyCFunction)Softbody_removeAnchor, METH_VARARGS, Softbody_removeAnchor_doc},
        {NULL, NULL},
    };

    PyGetSetDef Softbody_getsets[] = {
        {"dampingCoeff", (getter)Softbody_getDampingCoeff, (setter)Softbody_setDampingCoeff, Softbody_dampingCoeff_doc, NULL},
        {"linearStiffness", (getter)Softbody_getRepulsionStiffness, (setter)Softbody_setRepulsionStiffness, Softbody_linearStiffness_doc, NULL},
        {"pressureCoeff", (getter)Softbody_getPressureCoeff, (setter)Softbody_setPressureCoeff, Softbody_pressureCoeff_doc, NULL},
        {"volumeConvCoeff", (getter)Softbody_getVolumeConvCoeff, (setter)Softbody_setVolumeConvCoeff, Softbody_volumeConvCoeff_doc, NULL},
        {"dynamicFrictionCoeff", (getter)Softbody_getDynamicFrictionCoeff, (setter)Softbody_setDynamicFrictionCoeff, Softbody_dynamicFrictionCoeff_doc, NULL},
        {"poseMatchCoeff", (getter)Softbody_getPoseMatchCoeff, (setter)Softbody_setPoseMatchCoeff, Softbody_poseMatchCoeff_doc, NULL},
        {"velocityFactor", (getter)Softbody_getVelocityFactor, (setter)Softbody_setVelocityFactor, Softbody_velocityFactor_doc, NULL},
        {"gravityFactor", (getter)Softbody_getGravityFactor, (setter)Softbody_setGravityFactor, Softbody_gravityFactor_doc, NULL},
        {"rigidContactHardness", (getter)Softbody_getRigidContactHardness, (setter)Softbody_setRigidContactHardness, Softbody_rigidContactHardness_doc, NULL},
        {"kineticContactHardness", (getter)Softbody_getKineticContactHardness, (setter)Softbody_setKineticContactHardness, Softbody_kineticContactHardness_doc, NULL},
        {"softContactHardness", (getter)Softbody_getSoftContactHardness, (setter)Softbody_setSoftContactHardness, Softbody_softContactHardness_doc, NULL},
        {"anchorHardness", (getter)Softbody_getAnchorHardness, (setter)Softbody_setAnchorHardness, Softbody_anchorHardness_doc, NULL},
        {"sleepingThreshold", (getter)Softbody_getSleepingThreshold, (setter)Softbody_setSleepingThreshold, Softbody_sleepingThreshold_doc, NULL},
        {"restLengthScale", (getter)Softbody_getRestLengthScale, (setter)Softbody_setRestLengthScale, Softbody_restLengthScale_doc, NULL},
        {"positionIterNumber", (getter)Softbody_getPosIterationNumber, (setter)Softbody_setPosIterationNumber, Softbody_positionIterNumber_doc, NULL},
        {"aeroModel", (getter)Softbody_getAeroModel, (setter)Softbody_setAeroModel, Softbody_aeroModel_doc, NULL},
        {"windVelocity", (getter)Softbody_getWindVelocity, (setter)Softbody_setWindVelocity, Softbody_windVelocity_doc, NULL},
        {"selfCollision", (getter)Softbody_isSelfCollision, (setter)Softbody_setSelfCollision, Softbody_selfCollision_doc, NULL},
        {"softCollision", (getter)Softbody_isSoftSoftCollision, (setter)Softbody_setSoftSoftCollision, Softbody_softCollision_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_Softbody = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.Softbody", /* tp_name */
        sizeof(PyObject_Softbody),                          /* tp_basicsize */
        0,                                                        /* tp_itemsize */
        (destructor)Softbody_dealloc,                       /* tp_dealloc */
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
        (reprfunc)Softbody_str,                             /* tp_str */
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
        Softbody_methods,                                   /* tp_methods */
        0,                                                        /* tp_members */
        Softbody_getsets,                                   /* tp_getset */
        &PyTypeObject_Rigidbody,                               /* tp_base */
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
