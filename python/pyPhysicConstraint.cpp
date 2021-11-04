#include "python/pyPhysicConstraint.h"
#include "python/pyPhysicConstraint_doc_en.h"
#include "python/pyPhysicObject.h"

#include "components/physic/PhysicConstraint.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    void PhysicConstraint_dealloc(PyObject_PhysicConstraint *self)
    {
        if (self) {
            self->constraint = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *PhysicConstraint_str(PyObject_PhysicConstraint *self)
    {
        return PyUnicode_FromString("C++ PhysicConstraint object");
    }

    // Compare function
    static PyObject *PhysicConstraint_richcompare(PyObject *self, PyObject *other, int op)
    {
        if (op == Py_LT || op == Py_LE || op == Py_GT || op == Py_GE)
        {
            return Py_NotImplemented;
        }

        if (self != Py_None && other != Py_None)
        {
            if (other->ob_type == &PyTypeObject_PhysicConstraint)
            {
                auto selfConstraint = (PyObject_PhysicConstraint *)(self);
                auto otherConstraint = (PyObject_PhysicConstraint *)(other);
                bool eq = (selfConstraint->constraint == otherConstraint->constraint);
                if (op == Py_NE)
                    eq = !eq;
                return eq ? Py_True : Py_False;
            }
            else
            {
                return (op == Py_EQ) ? Py_False : Py_True;
            }
        }
        else if (self == Py_None && other == Py_None)
        {
            return (op == Py_EQ) ? Py_True : Py_False;
        }
        else
        {
            return (op == Py_EQ) ? Py_False : Py_True;
        }
    }

    // Get name
    PyObject *PhysicConstraint_getType(PyObject_PhysicConstraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        return PyLong_FromLong((int)self->constraint->getType());
    }

    // Get owner
    PyObject *PhysicConstraint_getOwner(PyObject_PhysicConstraint *self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        auto *obj = (PyObject_PhysicObject*)(&PyTypeObject_PhysicObject)->tp_alloc(&PyTypeObject_PhysicObject, 0);
        obj->component = self->constraint->getOther()->getOwner()->getComponent(self->constraint->getOther()->getInstanceId());
        return (PyObject *)obj;
    }

    // other
    PyObject* PhysicConstraint_getOther(PyObject_PhysicConstraint* self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        if(self->constraint->getOther())
        {
            auto *obj = (PyObject_PhysicObject*)(&PyTypeObject_PhysicObject)->tp_alloc(&PyTypeObject_PhysicObject, 0);
            obj->component = self->constraint->getOther()->getOwner()->getComponent(self->constraint->getOther()->getInstanceId());
            return (PyObject *)obj;
        }
        Py_RETURN_NONE;
    }

    int PhysicConstraint_setOther(PyObject_PhysicConstraint* self, PyObject* value)
    {
        if (!self->constraint) return -1;
        if (value->ob_type != &PyTypeObject_PhysicObject)
            return -1;

        auto other = (PyObject_PhysicObject *)(value);
        if(!other->component.expired())
            self->constraint->setOther(std::dynamic_pointer_cast<PhysicObject>(other->component.lock()).get());
        return 1;
    }

    // isEnabled
    PyObject* PhysicConstraint_isEnabled(PyObject_PhysicConstraint* self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        return PyBool_FromLong(self->constraint->isEnabled());
    }

    int PhysicConstraint_setEnabled(PyObject_PhysicConstraint* self, PyObject* value)
    {
        if (PyLong_Check(value) && self->constraint)
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->constraint->setEnabled(val);
            return 0;
        }
        return -1;
    }

    // isBodiesCollisionEnabled
    PyObject* PhysicConstraint_isBodiesCollisionEnabled(PyObject_PhysicConstraint* self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        return PyBool_FromLong(self->constraint->isEnableCollisionBetweenBodies());
    }

    int PhysicConstraint_setBodiesCollisionEnabled(PyObject_PhysicConstraint* self, PyObject* value)
    {
        if (PyLong_Check(value) && self->constraint)
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->constraint->setEnableCollisionBetweenBodies(val);
            return 0;
        }
        return -1;
    }

    // breakingImpulse
    PyObject* PhysicConstraint_getBreakingImpulse(PyObject_PhysicConstraint* self)
    {
        if (!self->constraint) Py_RETURN_NONE;
        return PyFloat_FromDouble(self->constraint->getBreakingImpulseThreshold());
    }

    int PhysicConstraint_setBreakingImpulse(PyObject_PhysicConstraint* self, PyObject* value)
    {
        if (PyFloat_Check(value) && self->constraint)
        {
            float val = (float)PyFloat_AsDouble(value);
            self->constraint->setBreakingImpulseThreshold(val);
            return 0;
        }
        return -1;
    }

    // Variable definition
    PyGetSetDef PhysicConstraint_getsets[] = {
        {"type", (getter)PhysicConstraint_getType, NULL, PhysicConstraint_type_doc, NULL},
        {"owner", (getter)PhysicConstraint_getOwner, NULL, PhysicConstraint_owner_doc, NULL},
        {"other", (getter)PhysicConstraint_getOther, (setter)PhysicConstraint_setOther, PhysicConstraint_other_doc, NULL},
        {"isEnabled", (getter)PhysicConstraint_isEnabled, (setter)PhysicConstraint_setEnabled, PhysicConstraint_isEnabled_doc, NULL},
        {"isBodiesCollisionEnabled", (getter)PhysicConstraint_isBodiesCollisionEnabled, (setter)PhysicConstraint_setBodiesCollisionEnabled, PhysicConstraint_isBodiesCollisionEnabled_doc, NULL},
        {"breakingImpulse", (getter)PhysicConstraint_getBreakingImpulse, (setter)PhysicConstraint_setBreakingImpulse, PhysicConstraint_breakingImpulse_doc, NULL},
        {NULL, NULL}};

    // Type definition
    PyTypeObject PyTypeObject_PhysicConstraint = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.PhysicConstraint", /* tp_name */
        sizeof(PyObject_PhysicConstraint),                          /* tp_basicsize */
        0,                                                          /* tp_itemsize */
        (destructor)PhysicConstraint_dealloc,                       /* tp_dealloc */
        0,                                                          /* tp_print */
        0,                                                          /* tp_getattr */
        0,                                                          /* tp_setattr */
        0,                                                          /* tp_reserved */
        0,                                                          /* tp_repr */
        0,                                                          /* tp_as_number */
        0,                                                          /* tp_as_sequence */
        0,                                                          /* tp_as_mapping */
        0,                                                          /* tp_hash */
        0,                                                          /* tp_call */
        (reprfunc)PhysicConstraint_str,                             /* tp_str */
        0,                                                          /* tp_getattro */
        0,                                                          /* tp_setattro */
        0,                                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                   /* tp_flags */
        0,                                                          /* tp_doc */
        0,                                                          /* tp_traverse */
        0,                                                          /* tp_clear */
        PhysicConstraint_richcompare,                               /* tp_richcompare */
        0,                                                          /* tp_weaklistoffset */
        0,                                                          /* tp_iter */
        0,                                                          /* tp_iternext */
        0,                                                          /* tp_methods */
        0,                                                          /* tp_members */
        PhysicConstraint_getsets,                                   /* tp_getset */
        0,                                                          /* tp_base */
        0,                                                          /* tp_dict */
        0,                                                          /* tp_descr_get */
        0,                                                          /* tp_descr_set */
        0,                                                          /* tp_dictoffset */
        0,                                                          /* tp_init */
        0,                                                          /* tp_alloc */
        0,                                                          /* tp_new */
        0,                                                          /* tp_free */
    };
} // namespace ige::scene
