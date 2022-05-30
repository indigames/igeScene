#include "python/pyMeshCollider.h"
#include "python/pyMeshCollider_doc_en.h"

#include "components/physic/collider/MeshCollider.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void MeshCollider_dealloc(PyObject_MeshCollider *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *MeshCollider_str(PyObject_MeshCollider *self)
    {
        return PyUnicode_FromString("C++ MeshCollider object");
    }

    //! Convex
    PyObject *MeshCollider_isConvex(PyObject_MeshCollider *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<MeshCollider>(self->component.lock())->isConvex());
    }

    int MeshCollider_setConvex(PyObject_MeshCollider *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value)) {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<MeshCollider>(self->component.lock())->setConvex(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef MeshCollider_getsets[] = {
        {"isConvex", (getter)MeshCollider_isConvex, (setter)MeshCollider_setConvex, MeshCollider_isConvex_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_MeshCollider = {
        PyVarObject_HEAD_INIT(NULL, 1) "igeScene.MeshCollider", /* tp_name */
        sizeof(PyObject_MeshCollider),                          /* tp_basicsize */
        0,                                                    /* tp_itemsize */
        (destructor)MeshCollider_dealloc,                       /* tp_dealloc */
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
        (reprfunc)MeshCollider_str,                           /* tp_str */
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
        MeshCollider_getsets,                                 /* tp_getset */
        &PyTypeObject_Component,                              /* tp_base */
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
