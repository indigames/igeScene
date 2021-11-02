#include "python/pyUIImage.h"
#include "python/pyUIImage_doc_en.h"

#include "components/gui/UIImage.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UIImage_dealloc(PyObject_UIImage *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject* UIImage_str(PyObject_UIImage *self)
    {
        return PyUnicode_FromString("C++ UIImage object");
    }

    PyObject* UIImage_getFillMethod(PyObject_UIImage* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyLong_FromLong((int)self->component->getFillMethod());
    }

    int UIImage_setFillMethod(PyObject_UIImage* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setFillMethod(val);
            return 0;
        }
        return -1;
    }

    // Get path
    PyObject* UIImage_getPath(PyObject_UIImage* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyUnicode_FromString(self->component->getPath().c_str());
    }

    // Set path
    int UIImage_setPath(PyObject_UIImage* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyUnicode_Check(value))
        {
            const char* name = PyUnicode_AsUTF8(value);
            self->component->setPath(std::string(name));
            return 0;
        }
        return -1;
    }

    // Get color
    PyObject* UIImage_getColor(PyObject_UIImage* self)
    {
        if (!self->component) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set color
    int UIImage_setColor(PyObject_UIImage* self, PyObject* value)
    {
        if (!self->component) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        self->component->setColor(*((Vec4*)v));
        return 0;
    }

    // Get Fill Origin
    PyObject* UIImage_getFillOrigin(PyObject_UIImage* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyLong_FromLong((int)self->component->getFillOrigin());
    }

    // Set Fill Origin
    int UIImage_setFillOrigin(PyObject_UIImage* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setFillOrigin(val);
            return 0;
        }
        return -1;
    }

    // Get Fill Amount
    PyObject* UIImage_getFillAmount(PyObject_UIImage* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyFloat_FromDouble(self->component->getFillAmount());
    }

    // Set Fill Amount
    int UIImage_setFillAmount(PyObject_UIImage* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyFloat_Check(value))
        {
            auto val = (float)PyFloat_AsDouble(value);
            self->component->setFillAmount(val);
            return 0;
        }
        return -1;
    }

    // Get Fill Clockwise
    PyObject* UIImage_getClockwise(PyObject_UIImage* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->getClockwise());
    }

    // Set Fill Clockwise
    int UIImage_setClockwise(PyObject_UIImage* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setClockwise(val);
            return 0;
        }
        return -1;
    }

    // Set Interactable
    PyObject* UIImage_getInteractable(PyObject_UIImage* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->isInteractable());
    }

    // Set Interactable
    int UIImage_setInteractable(PyObject_UIImage* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setInteractable(val);
            return 0;
        }
        return -1;
    }

    // Methods
    PyMethodDef UIImage_methods[] = {
        {NULL, NULL} };

    // Get/Set
    PyGetSetDef UIImage_getsets[] = {
        {"image", (getter)UIImage_getPath, (setter)UIImage_setPath, NULL, NULL},
        {"color", (getter)UIImage_getColor, (setter)UIImage_setColor, NULL, NULL},
        {"fillMethod", (getter)UIImage_getFillMethod, (setter)UIImage_setFillMethod, NULL, NULL},
        {"fillOrigin", (getter)UIImage_getFillOrigin, (setter)UIImage_setFillOrigin, NULL, NULL},
        {"fillAmount", (getter)UIImage_getFillAmount, (setter)UIImage_setFillAmount, NULL, NULL},
        {"clockwise", (getter)UIImage_getClockwise, (setter)UIImage_setClockwise, NULL, NULL},
        {"interactable", (getter)UIImage_getInteractable, (setter)UIImage_setInteractable, NULL, NULL},
        {NULL, NULL} };

    PyTypeObject PyTypeObject_UIImage = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.UIImage",                          /* tp_name */
        sizeof(PyObject_UIImage),                    /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)UIImage_dealloc,                 /* tp_dealloc */
        0,                                           /* tp_print */
        0,                                           /* tp_getattr */
        0,                                           /* tp_setattr */
        0,                                           /* tp_reserved */
        0,                                           /* tp_repr */
        0,                                           /* tp_as_number */
        0,                                           /* tp_as_sequence */
        0,                                           /* tp_as_mapping */
        0,                                           /* tp_hash */
        0,                                           /* tp_call */
        (reprfunc)UIImage_str,                       /* tp_str */
        0,                                           /* tp_getattro */
        0,                                           /* tp_setattro */
        0,                                           /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                          /* tp_flags */
        0,                                           /* tp_doc */
        0,                                           /* tp_traverse */
        0,                                           /* tp_clear */
        0,                                           /* tp_richcompare */
        0,                                           /* tp_weaklistoffset */
        0,                                           /* tp_iter */
        0,                                           /* tp_iternext */
        UIImage_methods,                             /* tp_methods */  //! [IGE]: no methods
        0,                                           /* tp_members */
        UIImage_getsets,                             /* tp_getset */
        &PyTypeObject_SpriteComponent,               /* tp_base */  //! [IGE]: inheritance
        0,                                           /* tp_dict */
        0,                                           /* tp_descr_get */
        0,                                           /* tp_descr_set */
        0,                                           /* tp_dictoffset */
        0,                                           /* tp_init */
        0,                                           /* tp_alloc */
        0,                                           /* tp_new */  //! [IGE]: no new
        0,                                           /* tp_free */
    };
}
