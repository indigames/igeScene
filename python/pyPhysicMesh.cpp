#include "python/pyPhysicMesh.h"
#include "python/pyPhysicMesh_doc_en.h"

#include "components/physic/PhysicMesh.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void PhysicMesh_dealloc(PyObject_PhysicMesh *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *PhysicMesh_str(PyObject_PhysicMesh *self)
    {
        return PyUnicode_FromString("C++ PhysicMesh object");
    }

    //! Path
    PyObject *PhysicMesh_getPath(PyObject_PhysicMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyUnicode_FromString(std::dynamic_pointer_cast<PhysicMesh>(self->component.lock())->getPath().c_str());
    }

    int PhysicMesh_setPath(PyObject_PhysicMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyUnicode_Check(value)) {
            const char* val = PyUnicode_AsUTF8(value);
            if (val != NULL) {
                std::dynamic_pointer_cast<PhysicMesh>(self->component.lock())->setPath(std::string(val));
                return 0;
            }
        }
        return -1;
    }

    //! Convex
    PyObject *PhysicMesh_isConvex(PyObject_PhysicMesh *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<PhysicMesh>(self->component.lock())->isConvex());
    }

    int PhysicMesh_setConvex(PyObject_PhysicMesh *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<PhysicMesh>(self->component.lock())->setConvex(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef PhysicMesh_getsets[] = {
        {"meshPath", (getter)PhysicMesh_getPath, (setter)PhysicMesh_setPath, PhysicMesh_meshPath_doc, NULL},
        {"isConvex", (getter)PhysicMesh_isConvex, (setter)PhysicMesh_setConvex, PhysicMesh_isConvex_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_PhysicMesh = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.PhysicMesh", /* tp_name */
        sizeof(PyObject_PhysicMesh),                          /* tp_basicsize */
        0,                                                    /* tp_itemsize */
        (destructor)PhysicMesh_dealloc,                       /* tp_dealloc */
        0,                                                    /* tp_print */
        0,                                                    /* tp_getattr */
        0,                                                    /* tp_setattr */
        0,                                                    /* tp_reserved */
        0,                                                    /* tp_repr */
        0,                                                    /* tp_as_number */
        0,                                                    /* tp_as_sequence */
        0,                                                    /* tp_as_mapping */
        0,                                                    /* tp_hash */
        0,                                                    /* tp_call */
        (reprfunc)PhysicMesh_str,                             /* tp_str */
        0,                                                    /* tp_getattro */
        0,                                                    /* tp_setattro */
        0,                                                    /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                   /* tp_flags */
        0,                                                    /* tp_doc */
        0,                                                    /* tp_traverse */
        0,                                                    /* tp_clear */
        0,                                                    /* tp_richcompare */
        0,                                                    /* tp_weaklistoffset */
        0,                                                    /* tp_iter */
        0,                                                    /* tp_iternext */
        0,                                                    /* tp_methods */
        0,                                                    /* tp_members */
        PhysicMesh_getsets,                                   /* tp_getset */
        &PyTypeObject_PhysicObject,                           /* tp_base */
        0,                                                    /* tp_dict */
        0,                                                    /* tp_descr_get */
        0,                                                    /* tp_descr_set */
        0,                                                    /* tp_dictoffset */
        0,                                                    /* tp_init */
        0,                                                    /* tp_alloc */
        0,                                                    /* tp_new */
        0,                                                    /* tp_free */
    };
} // namespace ige::scene
