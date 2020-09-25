#include "python/pyPhysicBox.h"
#include "python/pyPhysicBox_doc_en.h"

#include "components/physic/PhysicBox.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void PhysicBox_dealloc(PyObject_PhysicBox *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *PhysicBox_str(PyObject_PhysicBox *self)
    {
        return PyUnicode_FromString("C++ PhysicBox object");
    }

    //! size
    PyObject *PhysicBox_getSize(PyObject_PhysicBox *self)
    {
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(self->component->getSize().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int PhysicBox_setSize(PyObject_PhysicBox *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setSize(*((Vec3 *)v));
        return 0;
    }

    // Get/set
    PyGetSetDef PhysicBox_getsets[] = {
        {"size", (getter)PhysicBox_getSize, (setter)PhysicBox_setSize, PhysicBox_size_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_PhysicBox = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.PhysicBox", /* tp_name */
        sizeof(PyObject_PhysicBox),                          /* tp_basicsize */
        0,                                                   /* tp_itemsize */
        (destructor)PhysicBox_dealloc,                       /* tp_dealloc */
        0,                                                   /* tp_print */
        0,                                                   /* tp_getattr */
        0,                                                   /* tp_setattr */
        0,                                                   /* tp_reserved */
        0,                                                   /* tp_repr */
        0,                                                   /* tp_as_number */
        0,                                                   /* tp_as_sequence */
        0,                                                   /* tp_as_mapping */
        0,                                                   /* tp_hash */
        0,                                                   /* tp_call */
        (reprfunc)PhysicBox_str,                             /* tp_str */
        0,                                                   /* tp_getattro */
        0,                                                   /* tp_setattro */
        0,                                                   /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                                  /* tp_flags */
        0,                                                   /* tp_doc */
        0,                                                   /* tp_traverse */
        0,                                                   /* tp_clear */
        0,                                                   /* tp_richcompare */
        0,                                                   /* tp_weaklistoffset */
        0,                                                   /* tp_iter */
        0,                                                   /* tp_iternext */
        0,                                                   /* tp_methods */
        0,                                                   /* tp_members */
        PhysicBox_getsets,                                   /* tp_getset */
        &PyTypeObject_PhysicBase,                            /* tp_base */
        0,                                                   /* tp_dict */
        0,                                                   /* tp_descr_get */
        0,                                                   /* tp_descr_set */
        0,                                                   /* tp_dictoffset */
        0,                                                   /* tp_init */
        0,                                                   /* tp_alloc */
        0,                                                   /* tp_new */
        0,                                                   /* tp_free */
    };
} // namespace ige::scene
