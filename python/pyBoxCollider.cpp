#include "python/pyBoxCollider.h"
#include "python/pyBoxCollider_doc_en.h"

#include "components/physic/BoxCollider.h"

#include "utils/PhysicHelper.h"
#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void BoxCollider_dealloc(PyObject_BoxCollider *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *BoxCollider_str(PyObject_BoxCollider *self)
    {
        return PyUnicode_FromString("C++ BoxCollider object");
    }

    //! size
    PyObject *BoxCollider_getSize(PyObject_BoxCollider *self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec3Obj = PyObject_New(vec_obj, _Vec3Type);
        vmath_cpy(std::dynamic_pointer_cast<BoxCollider>(self->component.lock())->getSize().P(), 3, vec3Obj->v);
        vec3Obj->d = 3;
        return (PyObject *)vec3Obj;
    }

    int BoxCollider_setSize(PyObject_BoxCollider *self, PyObject *value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<BoxCollider>(self->component.lock())->setSize(*((Vec3 *)v));
        return 0;
    }

    // Get/set
    PyGetSetDef BoxCollider_getsets[] = {
        {"size", (getter)BoxCollider_getSize, (setter)BoxCollider_setSize, BoxCollider_size_doc, NULL},
        {NULL, NULL},
    };

    PyTypeObject PyTypeObject_BoxCollider = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.BoxCollider", /* tp_name */
        sizeof(PyObject_BoxCollider),                          /* tp_basicsize */
        0,                                                   /* tp_itemsize */
        (destructor)BoxCollider_dealloc,                       /* tp_dealloc */
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
        (reprfunc)BoxCollider_str,                           /* tp_str */
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
        BoxCollider_getsets,                                 /* tp_getset */
        &PyTypeObject_Component,                             /* tp_base */
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
