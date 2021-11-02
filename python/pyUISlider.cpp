#include "python/pyUISlider.h"
#include "python/pyUISlider_doc_en.h"
#include "python/pySceneObject.h"
#include "components/gui/UISlider.h"

#include "scene/SceneManager.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UISlider_dealloc(PyObject_UISlider *self)
    {
        if(self && self->component)
        {
            self->component = nullptr;
        }
        PyObject_Del(self);
    }

    PyObject* UISlider_str(PyObject_UISlider*self)
    {
        return PyUnicode_FromString("C++ UISlider object");
    }


    //Methods
    PyObject* UISlider_setFillObject(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) Py_RETURN_NONE;
        PyObject* obj;
        if (PyArg_ParseTuple(value, "O", &obj)) {
            if (obj->ob_type == &PyTypeObject_SceneObject)
            {
                auto pySceneObject = (PyObject_SceneObject*)(obj);
                auto id = pySceneObject->sceneObject->getUUID();
                if (SceneManager::getInstance() != nullptr && SceneManager::getInstance()->getCurrentScene() != nullptr) {
                    auto sObj = SceneManager::getInstance()->getCurrentScene()->findObjectByUUID(id);
                    if (sObj != nullptr)
                    {
                        self->component->setFillObject(sObj);
                        Py_RETURN_TRUE;
                    }
                }
            }
        }
        Py_RETURN_FALSE;
    }

    PyObject* UISlider_setHandleObject(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) Py_RETURN_NONE;
        PyObject* obj;
        if (PyArg_ParseTuple(value, "O", &obj)) {
            if (obj->ob_type == &PyTypeObject_SceneObject)
            {
                auto pySceneObject = (PyObject_SceneObject*)(obj);
                auto id = pySceneObject->sceneObject->getUUID();
                if (SceneManager::getInstance() != nullptr && SceneManager::getInstance()->getCurrentScene() != nullptr) {
                    auto sObj = SceneManager::getInstance()->getCurrentScene()->findObjectByUUID(id);
                    if (sObj != nullptr)
                    {
                        self->component->setHandleObject(sObj);
                        Py_RETURN_TRUE;
                    }
                }
            }
        }
        Py_RETURN_FALSE;
    }

    //Get set
    PyObject* UISlider_getMin(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyFloat_FromDouble(self->component->getMin());
    }

    int UISlider_setMin(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setMin(val);
            return 0;
        }
        return -1;
    }

    PyObject* UISlider_getMax(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyFloat_FromDouble(self->component->getMax());
    }

    int UISlider_setMax(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setMax(val);
            return 0;
        }
        return -1;
    }

    PyObject* UISlider_getValue(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyFloat_FromDouble(self->component->getValue());
    }

    int UISlider_setValue(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setValue(val);
            return 0;
        }
        return -1;
    }

    PyObject* UISlider_getWholeNumbers(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->getWholeNumbers());
    }

    int UISlider_setWholeNumbers(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            self->component->setWholeNumbers(val);
            return 0;
        }
        return -1;
    }

    // Get color
    PyObject* UISlider_getColor(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set color
    int UISlider_setColor(PyObject_UISlider* self, PyObject* value)
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
    PyObject* UISlider_getPressColor(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getPressedColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set Press color
    int UISlider_setPressColor(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        self->component->setPressedColor(*((Vec4*)v));
        return 0;
    }

    // Get Disable color
    PyObject* UISlider_getDisableColor(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(self->component->getDisabledColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set Disable color
    int UISlider_setDisableColor(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        self->component->setDisabledColor(*((Vec4*)v));
        return 0;
    }

    // Get Interactable
    PyObject* UISlider_getInteractable(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyBool_FromLong(self->component->isInteractable());
    }

    // Set Interactable
    int UISlider_setInteractable(PyObject_UISlider* self, PyObject* value)
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

    // Get Direction
    PyObject* UISlider_getDirection(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyLong_FromLong((int)self->component->getDirection());
    }

    // Set Direction
    int UISlider_setDirection(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            self->component->setDirection(val);
            return 0;
        }
        return -1;
    }

    // Set Fade Duration
    PyObject* UISlider_getFadeDuration(PyObject_UISlider* self)
    {
        if (!self->component) Py_RETURN_NONE;
        return PyFloat_FromDouble(self->component->getFadeDuration());
    }

    // Get Fade Duration
    int UISlider_setFadeDuration(PyObject_UISlider* self, PyObject* value)
    {
        if (!self->component) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            self->component->setFadeDuration(val);
            return 0;
        }
        return -1;
    }

    // Methods definition
    PyMethodDef UISlider_methods[] = {
        { "setFill", (PyCFunction)UISlider_setFillObject, METH_VARARGS, NULL },
        { "setHandle", (PyCFunction)UISlider_setHandleObject, METH_VARARGS, NULL },
        { NULL, NULL }
    };

    // Get/Set
    PyGetSetDef UISlider_getsets[] = {
        {"min", (getter)UISlider_getMin, (setter)UISlider_setMin, NULL, NULL},
        {"max", (getter)UISlider_getMax, (setter)UISlider_setMax, NULL, NULL},
        {"value", (getter)UISlider_getValue, (setter)UISlider_setValue, NULL, NULL},
        {"wholeNumbers", (getter)UISlider_getColor, (setter)UISlider_setColor, NULL, NULL},
        {"color", (getter)UISlider_getColor, (setter)UISlider_setColor, NULL, NULL},
        {"pressColor", (getter)UISlider_getPressColor, (setter)UISlider_setPressColor, NULL, NULL},
        {"disableColor", (getter)UISlider_getDisableColor, (setter)UISlider_setDisableColor, NULL, NULL},
        {"direction", (getter)UISlider_getDirection, (setter)UISlider_setDirection, NULL, NULL},
        {"interactable", (getter)UISlider_getInteractable, (setter)UISlider_setInteractable, NULL, NULL},
        {"fadeDuration", (getter)UISlider_getFadeDuration, (setter)UISlider_setFadeDuration, NULL, NULL},
        {NULL, NULL} };

    PyTypeObject PyTypeObject_UISlider = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.UISlider",                          /* tp_name */
        sizeof(PyObject_UISlider),                    /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)UISlider_dealloc,                 /* tp_dealloc */
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
        (reprfunc)UISlider_str,                       /* tp_str */
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
        UISlider_methods,                            /* tp_methods */  //! [IGE]: no methods
        0,                                           /* tp_members */
        UISlider_getsets,                            /* tp_getset */
        & PyTypeObject_Component,                    /* tp_base */  //! [IGE]: inheritance
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
