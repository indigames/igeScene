#include "python/pySpriteComponent.h"
#include "python/pySpriteComponent_doc_en.h"

#include "components/SpriteComponent.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void SpriteComponent_dealloc(PyObject_SpriteComponent *self)
    {
        if (self && self->component)
        {
            self->component = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject *SpriteComponent_str(PyObject_SpriteComponent *self)
    {
        return PyUnicode_FromString("C++ SpriteComponent object");
    }

    // Get path
    PyObject *SpriteComponent_getPath(PyObject_SpriteComponent *self)
    {
        return PyUnicode_FromString(self->component->getPath().c_str());
    }

    // Set path
    int SpriteComponent_setPath(PyObject_SpriteComponent *self, PyObject *value)
    {
        char *val = NULL;
        if (PyArg_ParseTuple(value, "s", &val))
        {
            if (val != NULL)
            {
                self->component->setPath(std::string(val));
                return 0;
            }
        }
        return -1;
    }

    // Get size
    PyObject *SpriteComponent_getSize(PyObject_SpriteComponent *self)
    {
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(self->component->getSize().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject *)vec2Obj;
    }

    // Set size
    int SpriteComponent_setSize(PyObject_SpriteComponent *self, PyObject *value)
    {
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject *)value, buff, d);
        if (!v)
            return -1;
        self->component->setSize(*((Vec2 *)v));
        return 0;
    }

    PyObject* SpriteComponent_isBillboard(PyObject_SpriteComponent* self)
    {
        return PyBool_FromLong(self->component->isBillboard());
    }

    int SpriteComponent_setBillboard(PyObject_SpriteComponent* self, PyObject* value)
    {
        int val;
        if (PyArg_ParseTuple(value, "i", &val))
        {
            self->component->setBillboard(val);
            return 0;
        }
        return -1;
    }

    PyGetSetDef SpriteComponent_getsets[] = {
        {"path", (getter)SpriteComponent_getPath, (setter)SpriteComponent_setPath, SpriteComponent_path_doc, NULL},
        {"size", (getter)SpriteComponent_getSize, (setter)SpriteComponent_setSize, SpriteComponent_size_doc, NULL},
        {"isBillboard", (getter)SpriteComponent_isBillboard, (setter)SpriteComponent_setBillboard, SpriteComponent_isBillboard_doc, NULL},
        {NULL, NULL}};

    PyTypeObject PyTypeObject_SpriteComponent = {
        PyVarObject_HEAD_INIT(NULL, 0) "igeScene.SpriteComponent", /* tp_name */
        sizeof(PyObject_SpriteComponent),                          /* tp_basicsize */
        0,                                                         /* tp_itemsize */
        (destructor)SpriteComponent_dealloc,                       /* tp_dealloc */
        0,                                                         /* tp_print */
        0,                                                         /* tp_getattr */
        0,                                                         /* tp_setattr */
        0,                                                         /* tp_reserved */
        0,                                                         /* tp_repr */
        0,                                                         /* tp_as_number */
        0,                                                         /* tp_as_sequence */
        0,                                                         /* tp_as_mapping */
        0,                                                         /* tp_hash */
        0,                                                         /* tp_call */
        (reprfunc)SpriteComponent_str,                             /* tp_str */
        0,                                                         /* tp_getattro */
        0,                                                         /* tp_setattro */
        0,                                                         /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                  /* tp_flags */
        0,                                                         /* tp_doc */
        0,                                                         /* tp_traverse */
        0,                                                         /* tp_clear */
        0,                                                         /* tp_richcompare */
        0,                                                         /* tp_weaklistoffset */
        0,                                                         /* tp_iter */
        0,                                                         /* tp_iternext */
        0,                                                         /* tp_methods */
        0,                                                         /* tp_members */
        SpriteComponent_getsets,                                   /* tp_getset */
        &PyTypeObject_Component,                                   /* tp_base */
        0,                                                         /* tp_dict */
        0,                                                         /* tp_descr_get */
        0,                                                         /* tp_descr_set */
        0,                                                         /* tp_dictoffset */
        0,                                                         /* tp_init */
        0,                                                         /* tp_alloc */
        0,                                                         /* tp_new */
        0,                                                         /* tp_free */
    };
} // namespace ige::scene
