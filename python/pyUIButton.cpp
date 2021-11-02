#include "python/pyUIButton.h"
#include "python/pyUIButton_doc_en.h"

#include "components/gui/UIButton.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UIButton_dealloc(PyObject_UIButton *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject* UIButton_str(PyObject_UIButton *self)
    {
        return PyUnicode_FromString("C++ UIButton object");
    }

    // Get path
    PyObject* UIButton_getPath(PyObject_UIButton* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyUnicode_FromString(self->component->getPath().c_str());
    }

    // Set path
    int UIButton_setPath(PyObject_UIButton* self, PyObject* value)
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

    // Get press path
    PyObject* UIButton_getPressPath(PyObject_UIButton* self) 
    {
        if (!self->component) Py_RETURN_NONE;
        return PyUnicode_FromString(self->component->getPressedPath().c_str());
    }

    // Set press path
    int UIButton_setPressPath(PyObject_UIButton* self, PyObject* value) 
    {
        if (!self->component) return -1;
        if (PyUnicode_Check(value))
        {
            const char* name = PyUnicode_AsUTF8(value);
            self->component->setTexturePath(std::string(name), ButtonState::PRESSED);
            return 0;
        }
        return -1;
    }

    // Get selected path
    PyObject* UIButton_getSelectedPath(PyObject_UIButton* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyUnicode_FromString(self->component->getSelectedPath().c_str());
    }

    // Set selected path
    int UIButton_setSelectedPath(PyObject_UIButton* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyUnicode_Check(value))
        {
            const char* name = PyUnicode_AsUTF8(value);
            self->component->setTexturePath(std::string(name), ButtonState::SELECTED);
            return 0;
        }
        return -1;
    }

    // Get disable path
    PyObject* UIButton_getDisablePath(PyObject_UIButton* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyUnicode_FromString(self->component->getDisabledPath().c_str());
    }

    // Set disable path
    int UIButton_setDisablePath(PyObject_UIButton* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyUnicode_Check(value))
        {
            const char* name = PyUnicode_AsUTF8(value);
            self->component->setTexturePath(std::string(name), ButtonState::DISABLE);
            return 0;
        }
        return -1;
    }


    // Get color
    PyObject* UIButton_getColor(PyObject_UIButton* self)
    {
        if (!self->component) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set color
    int UIButton_setColor(PyObject_UIButton* self, PyObject* value)
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

    // Get Press color
    PyObject* UIButton_getPressColor(PyObject_UIButton* self) {
        if (!self->component) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getPressedColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set Press color
    int UIButton_setPressColor(PyObject_UIButton* self, PyObject* value) {
        if (!self->component) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        self->component->setPressedColor(*((Vec4*)v));
        return 0;
    }

    // Get Selected color
    PyObject* UIButton_getSelectedColor(PyObject_UIButton* self) {
        if (!self->component) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getSelectedColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set Selected color
    int UIButton_setSelectedColor(PyObject_UIButton* self, PyObject* value) {
        if (!self->component) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        self->component->setSelectedColor(*((Vec4*)v));
        return 0;
    }

    // Get Disable color
    PyObject* UIButton_getDisableColor(PyObject_UIButton* self) {
        if (!self->component) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getDisabledColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set Disable color
    int UIButton_setDisableColor(PyObject_UIButton* self, PyObject* value) {
        if (!self->component) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        self->component->setDisabledColor(*((Vec4*)v));
        return 0;
    }

    // Get Transition Mode
    PyObject* UIButton_getTransitionMode(PyObject_UIButton* self) {
        if (!self->component) Py_RETURN_NONE;
        return PyLong_FromLong((int)self->component->getTransitionMode());
    }

    // Set Transition Mode
    int UIButton_setTransitionMode(PyObject_UIButton* self, PyObject* value) {
        if (!self->component) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setTransitionMode((TransitionMode)val);
            return 0;
        }
        return -1;
    }

    // Get Fade Duration
    PyObject* UIButton_getFadeDuration(PyObject_UIButton* self) {
        if (!self->component) Py_RETURN_NONE;
        return PyFloat_FromDouble(self->component->getFadeDuration());
    }

    // Set Fade Duration
    int UIButton_setFadeDuration(PyObject_UIButton* self, PyObject* value) {
        if (!self->component) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setFadeDuration(val);
            return 0;
        }
        return -1;
    }

    // Methods
    PyMethodDef UIButton_methods[] = {
        {NULL, NULL} };

    // Get/Set
    PyGetSetDef UIButton_getsets[] = {
        {"image", (getter)UIButton_getPath, (setter)UIButton_setPath, NULL, NULL},
        {"pressImage", (getter)UIButton_getPressPath, (setter)UIButton_setPressPath, NULL, NULL},
        {"selectedImage", (getter)UIButton_getSelectedPath, (setter)UIButton_setSelectedPath, NULL, NULL},
        {"disableImage", (getter)UIButton_getDisablePath, (setter)UIButton_setDisablePath, NULL, NULL},
        {"color", (getter)UIButton_getColor, (setter)UIButton_setColor, NULL, NULL},
        {"pressColor", (getter)UIButton_getPressColor, (setter)UIButton_setPressColor, NULL, NULL},
        {"selectedColor", (getter)UIButton_getSelectedColor, (setter)UIButton_setSelectedColor, NULL, NULL},
        {"disableColor", (getter)UIButton_getDisableColor, (setter)UIButton_setDisableColor, NULL, NULL},
        {"interactable", (getter)UIImage_getInteractable, (setter)UIImage_setInteractable, NULL, NULL},
        {"transitionMode", (getter)UIButton_getTransitionMode, (setter)UIButton_setTransitionMode, NULL, NULL},
        {"fadeDuration", (getter)UIButton_getFadeDuration, (setter)UIButton_setFadeDuration, NULL, NULL},
        {NULL, NULL} };

    PyTypeObject PyTypeObject_UIButton = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.UIButton",                          /* tp_name */
        sizeof(PyObject_UIButton),                    /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)UIButton_dealloc,                 /* tp_dealloc */
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
        (reprfunc)UIButton_str,                       /* tp_str */
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
        UIButton_methods,                            /* tp_methods */  //! [IGE]: no methods
        0,                                           /* tp_members */
        UIButton_getsets,                            /* tp_getset */
        & PyTypeObject_UIImage,                      /* tp_base */  //! [IGE]: inheritance
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
