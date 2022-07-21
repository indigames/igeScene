#include "python/pyUIScrollBar.h"
#include "python/pyUIScrollBar_doc_en.h"
#include "python/pySceneObject.h"

#include "components/gui/UIScrollBar.h"
#include "scene/SceneManager.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UIScrollBar_dealloc(PyObject_UIScrollBar *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* UIScrollBar_str(PyObject_UIScrollBar *self)
    {
        return PyUnicode_FromString("C++ UIScrollBar object");
    }

    //Methods
    PyObject* UIScrollBar_setHandle(PyObject_UIScrollBar* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* obj;
        if (PyArg_ParseTuple(value, "O", &obj)) {
            if (obj->ob_type == &PyTypeObject_SceneObject)
            {
                auto pySceneObject = (PyObject_SceneObject*)(obj);
                if (!pySceneObject->sceneObject.expired()) {
                    auto id = pySceneObject->sceneObject.lock()->getUUID();
                    if (SceneManager::getInstance() != nullptr && SceneManager::getInstance()->getCurrentScene() != nullptr) {
                        auto sObj = SceneManager::getInstance()->getCurrentScene()->findObjectByUUID(id);
                        if (sObj != nullptr)
                        {
                            std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->setHandle(sObj);
                            Py_RETURN_TRUE;
                        }
                    }
                }
            }
        }
        Py_RETURN_FALSE;
    }


    //Get Set
    PyObject* UIScrollBar_getValue(PyObject_UIScrollBar* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->getValue());
    }

    int UIScrollBar_setValue(PyObject_UIScrollBar* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->setValue(val);
            return 0;
        }
        return -1;
    }

    PyObject* UIScrollBar_getSize(PyObject_UIScrollBar* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->getSize());
    }

    int UIScrollBar_setSize(PyObject_UIScrollBar* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->setSize(val);
            return 0;
        }
        return -1;
    }

    // Set Direction
    PyObject* UIScrollBar_getDirection(PyObject_UIScrollBar* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong((int)std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->getDirection());
    }

    // Get Direction
    int UIScrollBar_setDirection(PyObject_UIScrollBar* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->setDirection(val);
            return 0;
        }
        return -1;
    }

    // Get color
    PyObject* UIScrollBar_getColor(PyObject_UIScrollBar* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->getColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set color
    int UIScrollBar_setColor(PyObject_UIScrollBar* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->setColor(*((Vec4*)v));
        return 0;
    }

    // Get Press color
    PyObject* UIScrollBar_getPressColor(PyObject_UIScrollBar* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->getPressedColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set Press color
    int UIScrollBar_setPressColor(PyObject_UIScrollBar* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->setPressedColor(*((Vec4*)v));
        return 0;
    }

    // Get Disable color
    PyObject* UIScrollBar_getDisableColor(PyObject_UIScrollBar* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec4Obj = PyObject_New(vec_obj, _Vec4Type);
        vmath_cpy(std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->getDisabledColor().P(), 4, vec4Obj->v);
        vec4Obj->d = 4;
        return (PyObject*)vec4Obj;
    }

    // Set Disable color
    int UIScrollBar_setDisableColor(PyObject_UIScrollBar* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v)
            return -1;
        std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->setDisabledColor(*((Vec4*)v));
        return 0;
    }

    // Set Fade Duration
    PyObject* UIScrollBar_getFadeDuration(PyObject_UIScrollBar* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->getFadeDuration());
    }

    // Get Fade Duration
    int UIScrollBar_setFadeDuration(PyObject_UIScrollBar* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<UIScrollBar>(self->component.lock())->setFadeDuration(val);
            return 0;
        }
        return -1;
    }

    // Methods definition
    PyMethodDef UIScrollBar_methods[] = {
        { "setHandle", (PyCFunction)UIScrollBar_setHandle, METH_VARARGS, NULL },
        { NULL, NULL }
    };

    // Get/Set
    PyGetSetDef UIScrollBar_getsets[] = {
        {"value", (getter)UIScrollBar_getValue, (setter)UIScrollBar_setValue, NULL, NULL},
        {"size", (getter)UIScrollBar_getSize, (setter)UIScrollBar_setSize, NULL, NULL},
        {"direction", (getter)UIScrollBar_getDirection, (setter)UIScrollBar_setDirection, NULL, NULL},
        {"color", (getter)UIScrollBar_getColor, (setter)UIScrollBar_setColor, NULL, NULL},
        {"pressColor", (getter)UIScrollBar_getPressColor, (setter)UIScrollBar_setPressColor, NULL, NULL},
        {"disableColor", (getter)UIScrollBar_getDisableColor, (setter)UIScrollBar_setDisableColor, NULL, NULL},
        {"interactable", (getter)UIImage_getInteractable, (setter)UIImage_setInteractable, NULL, NULL},
        {"fadeDuration", (getter)UIScrollBar_getFadeDuration, (setter)UIScrollBar_setFadeDuration, NULL, NULL},
        {"image", (getter)UIImage_getPath, (setter)UIImage_setPath, NULL, NULL},
        {NULL, NULL} };

    PyTypeObject PyTypeObject_UIScrollBar = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.UIScrollBar",                     /* tp_name */
        sizeof(PyObject_UIScrollBar),               /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)UIScrollBar_dealloc,            /* tp_dealloc */
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
        (reprfunc)UIScrollBar_str,                  /* tp_str */
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
        UIScrollBar_methods,                         /* tp_methods */  //! [IGE]: no methods
        0,                                           /* tp_members */
        UIScrollBar_getsets,                         /* tp_getset */
        &PyTypeObject_UIImage,                       /* tp_base */  //! [IGE]: inheritance
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
