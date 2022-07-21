#include "python/pyUIScrollView.h"
#include "python/pyUIScrollView_doc_en.h"
#include "python/pySceneObject.h"
#include "python/pyUIScrollBar.h"

#include "components/gui/UIScrollView.h"
#include "scene/SceneManager.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include <pyVectorMath.h>
#include <pythonResource.h>

namespace ige::scene
{
    void UIScrollView_dealloc(PyObject_UIScrollView *self)
    {
        if (self) {
            self->component.reset();
            Py_TYPE(self)->tp_free(self);
        }
    }

    PyObject* UIScrollView_str(PyObject_UIScrollView *self)
    {
        return PyUnicode_FromString("C++ UIScrollView object");
    }

    //Methods
    PyObject* UIScrollView_setContent(PyObject_UIScrollView* self, PyObject* value)
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
                            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setContent(sObj);
                            Py_RETURN_TRUE;
                        }
                    }
                }
            }
        }
        Py_RETURN_FALSE;
    }

    PyObject* UIScrollView_setViewport(PyObject_UIScrollView* self, PyObject* value)
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
                            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setViewport(sObj);
                            Py_RETURN_TRUE;
                        }
                    }
                }
            }
        }
        Py_RETURN_FALSE;
    }

    PyObject* UIScrollView_scrollTo(PyObject_UIScrollView* self, PyObject* value)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        PyObject* obj;
        if (PyArg_ParseTuple(value, "O", &obj)) {
            int d;
            float buff[4];
            auto v = pyObjToFloat((PyObject*)value, buff, d);
            if (v) {
                std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->scrollTo(*((Vec2*)v));
                Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    //Get set
    PyObject* UIScrollView_getEnableHorizontal(PyObject_UIScrollView* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->enableHorizontal());
    }

    int UIScrollView_setEnableHorizontal(PyObject_UIScrollView* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setEnableHorizontal(val);
            return 0;
        }
        return -1;
    }

    PyObject* UIScrollView_getEnableVertical(PyObject_UIScrollView* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->enableVertical());
    }

    int UIScrollView_setEnableVertical(PyObject_UIScrollView* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setEnableVertical(val);
            return 0;
        }
        return -1;
    }

    PyObject* UIScrollView_getHorizontalScrollBar(PyObject_UIScrollView* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto bar = std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->getHorizontalScrollBar();
        if (bar) {
            auto* obj = (PyObject_UIScrollBar*)(&PyTypeObject_UIScrollBar)->tp_alloc(&PyTypeObject_UIScrollBar, 0);;
            obj->component = bar;
            return (PyObject*)obj;
        }
        Py_RETURN_NONE;
    }
    int UIScrollView_setHorizontalScrollBar(PyObject_UIScrollView* self, PyObject* value) {
        if (self->component.expired()) return -1;
        if (value != nullptr) {
            if (value->ob_type == &PyTypeObject_UIScrollBar) {
                auto val = (PyObject_UIScrollBar*)(value);
                if (val && !val->component.expired()) {
                    auto owner = val->component.lock()->getOwner();
                    auto component = owner->getComponent<UIScrollBar>();
                    std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setHorizontalScrollBar(component);
                    component = nullptr;
                    return 0;
                }
            }
            return -1;
        }
        else {
            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setHorizontalScrollBar(nullptr);
            return 0;
        }
    }

    PyObject* UIScrollView_getVerticalScrollBar(PyObject_UIScrollView* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto bar = std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->getVerticalScrollBar();
        if (bar) {
            auto* obj = (PyObject_UIScrollBar*)(&PyTypeObject_UIScrollBar)->tp_alloc(&PyTypeObject_UIScrollBar, 0);;
            obj->component = bar;
            return (PyObject*)obj;
        }
        Py_RETURN_NONE;
    }

    int UIScrollView_setVerticalScrollBar(PyObject_UIScrollView* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (value != nullptr) {
            if (value->ob_type == &PyTypeObject_UIScrollBar) {
                auto val = (PyObject_UIScrollBar*)(value);
                if (val &&! val->component.expired()) {
                    auto owner = val->component.lock()->getOwner();
                    auto component = owner->getComponent<UIScrollBar>();
                    std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setVerticalScrollBar(component);
                    component = nullptr;
                    return 0;
                }
            }
            return -1;
        }
        else {
            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setHorizontalScrollBar(nullptr);
            return 0;
        }
    }

    PyObject* UIScrollView_getMovementType(PyObject_UIScrollView* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyLong_FromLong((int)std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->getMovementType());
    }

    int UIScrollView_setMovementType(PyObject_UIScrollView* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value);
            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setMovementType(val);
            return 0;
        }
        return -1;
    }

    PyObject* UIScrollView_getElasticExtra(PyObject_UIScrollView* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        auto vec2Obj = PyObject_New(vec_obj, _Vec2Type);
        vmath_cpy(std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->getElasticExtra().P(), 2, vec2Obj->v);
        vec2Obj->d = 2;
        return (PyObject*)vec2Obj;
    }

    int UIScrollView_setElasticExtra(PyObject_UIScrollView* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        int d;
        float buff[4];
        auto v = pyObjToFloat((PyObject*)value, buff, d);
        if (!v) return -1;
        std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setElasticExtra(*((Vec2*)v));
        return 0;
    }

    PyObject* UIScrollView_getElasticity(PyObject_UIScrollView* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->getElasticity());
    }

    int UIScrollView_setElasticity(PyObject_UIScrollView* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setElasticity(val);
            return 0;
        }
        return -1;
    }

    PyObject* UIScrollView_getIsInertia(PyObject_UIScrollView* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyBool_FromLong(std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->isInertia());
    }

    int UIScrollView_setInertia(PyObject_UIScrollView* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyLong_Check(value))
        {
            auto val = (uint32_t)PyLong_AsLong(value) != 0;
            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setInertia(val);
            return 0;
        }
        return -1;
    }

    PyObject* UIScrollView_getDecelerationRate(PyObject_UIScrollView* self)
    {
        if (self->component.expired()) Py_RETURN_NONE;
        return PyFloat_FromDouble(std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->getDecelerationRate());
    }

    int UIScrollView_setDecelerationRate(PyObject_UIScrollView* self, PyObject* value)
    {
        if (self->component.expired()) return -1;
        if (PyFloat_Check(value))
        {
            float val = (float)PyFloat_AsDouble(value);
            std::dynamic_pointer_cast<UIScrollView>(self->component.lock())->setDecelerationRate(val);
            return 0;
        }
        return -1;
    }

    // Methods definition
    PyMethodDef UIScrollView_methods[] = {
        { "setContent", (PyCFunction)UIScrollView_setContent, METH_VARARGS, NULL },
        { "setViewport", (PyCFunction)UIScrollView_setViewport, METH_VARARGS, NULL },
        { "scrollTo", (PyCFunction)UIScrollView_scrollTo, METH_VARARGS, NULL },
        { NULL, NULL }
    };

    // Get/Set
    PyGetSetDef UIScrollView_getsets[] = {
        {"enableHorizontal", (getter)UIScrollView_getEnableHorizontal, (setter)UIScrollView_setEnableHorizontal, NULL, NULL},
        {"enableVertical", (getter)UIScrollView_getEnableVertical, (setter)UIScrollView_setEnableVertical, NULL, NULL},
        {"horizontalScrollBar", (getter)UIScrollView_getHorizontalScrollBar, (setter)UIScrollView_setHorizontalScrollBar, NULL, NULL},
        {"verticalScrollBar", (getter)UIScrollView_getVerticalScrollBar, (setter)UIScrollView_setVerticalScrollBar, NULL, NULL},
        {"movementType", (getter)UIScrollView_getMovementType, (setter)UIScrollView_setMovementType, NULL, NULL},
        {"elasticExtra", (getter)UIScrollView_getElasticExtra, (setter)UIScrollView_setElasticExtra, NULL, NULL},
        {"elasticity", (getter)UIScrollView_getElasticity, (setter)UIScrollView_setElasticity, NULL, NULL},
        {"isInertia", (getter)UIScrollView_getIsInertia, (setter)UIScrollView_setInertia, NULL, NULL},
        {"decelerationRate", (getter)UIScrollView_getDecelerationRate, (setter)UIScrollView_setDecelerationRate, NULL, NULL},
        {"interactable", (getter)UIImage_getInteractable, (setter)UIImage_setInteractable, NULL, NULL},
        {"image", (getter)UIImage_getPath, (setter)UIImage_setPath, NULL, NULL},
        {"color", (getter)UIImage_getColor, (setter)UIImage_setColor, NULL, NULL},
        {NULL, NULL} };


    PyTypeObject PyTypeObject_UIScrollView = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.UIScrollView",                     /* tp_name */
        sizeof(PyObject_UIScrollView),               /* tp_basicsize */
        0,                                           /* tp_itemsize */
        (destructor)UIScrollView_dealloc,            /* tp_dealloc */
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
        (reprfunc)UIScrollView_str,                  /* tp_str */
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
        0,                                           /* tp_methods */  //! [IGE]: no methods
        0,                                           /* tp_members */
        0,                                           /* tp_getset */
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
