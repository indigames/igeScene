#include "python/pySceneObject.h"
#include "python/pySceneObject_doc_en.h"
#include "python/pyScene.h"

#include "python/pyComponent.h"
#include "python/pyTransformComponent.h"
#include "python/pyCameraComponent.h"
#include "python/pyFigureComponent.h"
#include "python/pyEnvironmentComponent.h"
#include "python/pySpriteComponent.h"

#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

namespace ige::scene
{
    // Deallocation
    void  SceneObject_dealloc(PyObject_SceneObject *self)
    {
        if(self)
        {
            self->sceneObject = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    // String representation
    PyObject* SceneObject_str(PyObject_SceneObject *self)
    {
        return PyUnicode_FromString("C++ SceneObject object");
    }

    // Get ID
    PyObject* SceneObject_getId(PyObject_SceneObject* self)
    {
        return PyLong_FromUnsignedLongLong(self->sceneObject->getId());
    }

    // Get name
    PyObject* SceneObject_getName(PyObject_SceneObject* self)
    {
        return PyUnicode_FromString(self->sceneObject->getName().c_str());
    }

    // Set name
    int SceneObject_setName(PyObject_SceneObject* self, PyObject* value)
    {
        char* name = nullptr;
        if (PyArg_ParseTuple(value, "s", &name))
        {
            self->sceneObject->setName(std::string(name));
        }
        return 0;
    }

    // Get active
    PyObject* SceneObject_getActive(PyObject_SceneObject* self)
    {
        return PyBool_FromLong(self->sceneObject->isActive());
    }

    // Set active
    int SceneObject_setActive(PyObject_SceneObject* self, PyObject* value)
    {
        if (PyLong_Check(value))
        {
			auto isActive = (uint32_t)PyLong_AsLong(value) != 0;
            self->sceneObject->setActive(isActive);
            return 0;
		}
		return -1;
    }

    // Get parent
    PyObject* SceneObject_getParent(PyObject_SceneObject* self)
    {
        if(self->sceneObject->hasParent())
        {
            auto *obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = std::make_shared<SceneObject>(self->sceneObject->getParent());
            return (PyObject*)obj;
        }
        Py_RETURN_NONE;
    }

    // Set parent
    int SceneObject_setParent(PyObject_SceneObject* self, PyObject* value)
    {
        PyObject* obj;
        if (PyArg_ParseTuple(value, "O", &obj))
        {
            if(obj && obj->ob_type == &PyTypeObject_SceneObject) {
                auto sceneObj = (PyObject_SceneObject*)obj;
                self->sceneObject->setParent(sceneObj->sceneObject.get());
                return 0;
            }
        }
        return -1;
    }

    // Add child
    PyObject* SceneObject_addChild(PyObject_SceneObject* self, PyObject* value)
    {
        PyObject* obj;
        if (PyArg_ParseTuple(value, "O", &obj))
        {
            if(obj) 
            {
                if(PyUnicode_Check(obj))
                {
                    const char* name = PyUnicode_AsUTF8(obj);
                    auto sceneObj = SceneManager::getInstance()->getCurrentScene()->findObjectByName(std::string(name));
                    if(sceneObj)
                    {
                        self->sceneObject->addChild(sceneObj);
                        Py_RETURN_TRUE;
                    }
                }
                else if(PyNumber_Check(obj))
                {
                    uint64_t id = PyLong_AsUnsignedLongLong(obj);
                    auto sceneObj = SceneManager::getInstance()->getCurrentScene()->findObjectById(id);
                    if(sceneObj)
                    {
                        self->sceneObject->addChild(sceneObj);
                        Py_RETURN_TRUE;
                    }
                }
                else if(obj->ob_type == &PyTypeObject_SceneObject) 
                {
                    auto sceneObj = (PyObject_SceneObject*)obj;
                    self->sceneObject->addChild(sceneObj->sceneObject);
                    Py_RETURN_TRUE;
                }
            }
        }
        Py_RETURN_FALSE;
    }

    // Remove child
    PyObject* SceneObject_removeChild(PyObject_SceneObject* self, PyObject* value)
    {
        PyObject* obj;
        if (PyArg_ParseTuple(value, "O", &obj))
        {
            if(obj) 
            {
                if(PyUnicode_Check(obj))
                {
                    const char* name = PyUnicode_AsUTF8(obj);
                    auto sceneObj = SceneManager::getInstance()->getCurrentScene()->findObjectByName(std::string(name));
                    if(sceneObj)
                    {
                        self->sceneObject->removeChild(sceneObj);
                        Py_RETURN_TRUE;
                    }
                }
                else if(PyNumber_Check(obj))
                {
                    uint64_t id = PyLong_AsUnsignedLongLong(obj);
                    auto sceneObj = SceneManager::getInstance()->getCurrentScene()->findObjectById(id);
                    if(sceneObj)
                    {
                        self->sceneObject->removeChild(sceneObj);
                        Py_RETURN_TRUE;
                    }
                }
                else if(obj->ob_type == &PyTypeObject_SceneObject) 
                {
                    auto sceneObj = (PyObject_SceneObject*)obj;
                    self->sceneObject->removeChild(sceneObj->sceneObject);
                    Py_RETURN_TRUE;
                }
            }
        }
        Py_RETURN_FALSE;
    }

    // Get children
    PyObject* SceneObject_getChildren(PyObject_SceneObject* self)
    {
        auto len = self->sceneObject->getChildrenCount();
        if(len > 0)
        {
            auto children = self->sceneObject->getChildren();
            auto childrenTuple = PyTuple_New(len);
            for(int i = 0; i < len; ++i)
            {
                auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
                obj->sceneObject = children[i];
                PyTuple_SetItem(childrenTuple, i, (PyObject*)obj);
                Py_XDECREF(obj);
            }
            return (PyObject*)childrenTuple;
        }
        Py_RETURN_NONE;
    }

    // Remove children
    PyObject* SceneObject_removeChildren(PyObject_SceneObject* self)
    {
        if(self->sceneObject)
        {
            self->sceneObject->removeChildren();
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }

    // Add component
    PyObject* SceneObject_addComponent(PyObject_SceneObject* self, PyObject* value)
    {
        PyObject* obj = nullptr;
        if (PyArg_ParseTuple(value, "O", &obj) && obj)
        {
            if(PyUnicode_Check(obj))
            {
                auto type = std::string(PyUnicode_AsUTF8(obj));
                if(type == "TransformComponent")
                {
                    auto comp = self->sceneObject->addComponent<TransformComponent>();
                    if(comp)
                    {
                        auto *compObj = PyObject_New(PyObject_TransformComponent, &PyTypeObject_TransformComponent);
                        compObj->component = comp;
                        compObj->super.component = compObj->component;
                        return (PyObject*)compObj;
                    }
                }
                else if(type == "CameraComponent")
                {
                    auto comp = self->sceneObject->addComponent<CameraComponent>();                    
                    if(comp)
                    {
                        auto *compObj = PyObject_New(PyObject_CameraComponent, &PyTypeObject_CameraComponent);
                        compObj->component = comp;
                        compObj->super.component = compObj->component;
                        return (PyObject*)compObj;
                    }
                }
                else if(type == "EnvironmentComponent")
                {
                    auto comp = self->sceneObject->addComponent<EnvironmentComponent>();
                    if(comp)
                    {
                        auto *compObj = PyObject_New(PyObject_EnvironmentComponent, &PyTypeObject_EnvironmentComponent);
                        compObj->component = comp;
                        compObj->super.component = compObj->component;
                        return (PyObject*)compObj;
                    }
                }
                else if(type == "FigureComponent")
                {
                    auto comp = self->sceneObject->addComponent<FigureComponent>();
                    if(comp)
                    {
                        auto *compObj = PyObject_New(PyObject_FigureComponent, &PyTypeObject_FigureComponent);
                        compObj->component = comp;
                        compObj->super.component = compObj->component;
                        return (PyObject*)compObj;
                    }
                }
                else if(type == "SpriteComponent")
                {
                    auto comp = self->sceneObject->addComponent<SpriteComponent>();
                    if(comp)
                    {
                        auto *compObj = PyObject_New(PyObject_SpriteComponent, &PyTypeObject_SpriteComponent);
                        compObj->component = comp;
                        compObj->super.component = compObj->component;
                        return (PyObject*)compObj;
                    }
                }
            }
            else if(obj->ob_type == &PyTypeObject_Component
                    || obj->ob_type == &PyTypeObject_TransformComponent
                    || obj->ob_type == &PyTypeObject_CameraComponent
                    || obj->ob_type == &PyTypeObject_EnvironmentComponent
                    || obj->ob_type == &PyTypeObject_FigureComponent
                    || obj->ob_type == &PyTypeObject_SpriteComponent
                )
            {
                auto componentObj = (PyObject_Component*)obj;
                self->sceneObject->addComponent(componentObj->component);
                return obj;
            }            
        }
        Py_RETURN_NONE;
    }

    // Remove component
    PyObject* SceneObject_removeComponent(PyObject_SceneObject* self, PyObject* value)
    {
        PyObject* obj = nullptr;
        if (PyArg_ParseTuple(value, "O", &obj) && obj)
        {
            if(PyUnicode_Check(obj))
            {
                std::shared_ptr<Component> comp = nullptr;
                auto type = std::string(PyUnicode_AsUTF8(obj));
                if(type == "TransformComponent")
                {
                    comp = self->sceneObject->getComponent<TransformComponent>();
                }
                else if(type == "CameraComponent")
                {
                    comp = self->sceneObject->getComponent<CameraComponent>();
                }
                else if(type == "EnvironmentComponent")
                {
                    comp = self->sceneObject->getComponent<EnvironmentComponent>();
                }
                else if(type == "FigureComponent")
                {
                    comp = self->sceneObject->getComponent<FigureComponent>();
                }
                else if(type == "SpriteComponent")
                {
                    comp = self->sceneObject->getComponent<SpriteComponent>();
                }

                if(comp)
                {
                    self->sceneObject->removeComponent(comp);
                    Py_RETURN_TRUE;
                }                
            }
            else if(obj->ob_type == &PyTypeObject_Component
                    || obj->ob_type == &PyTypeObject_TransformComponent
                    || obj->ob_type == &PyTypeObject_CameraComponent
                    || obj->ob_type == &PyTypeObject_EnvironmentComponent
                    || obj->ob_type == &PyTypeObject_FigureComponent
                    || obj->ob_type == &PyTypeObject_SpriteComponent
                )
            {
                auto componentObj = (PyObject_Component*)obj;
                if(self->sceneObject) self->sceneObject->removeComponent(componentObj->component);
                Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    // Get component by type
    PyObject* SceneObject_getComponent(PyObject_SceneObject* self, PyObject* value)
    {
        char* type = nullptr;
        if (PyArg_ParseTuple(value, "s", &type))
        {
            if(type == "TransformComponent")
            {
                auto comp = self->sceneObject->getComponent<TransformComponent>();
                if(comp)
                {
                    auto *compObj = PyObject_New(PyObject_TransformComponent, &PyTypeObject_TransformComponent);
                    compObj->component = comp;
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if(type == "CameraComponent")
            {
                auto comp = self->sceneObject->getComponent<CameraComponent>();
                if(comp)
                {
                    auto *compObj = PyObject_New(PyObject_CameraComponent, &PyTypeObject_CameraComponent);
                    compObj->component = comp;
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if(type == "EnvironmentComponent")
            {
                auto comp = self->sceneObject->getComponent<EnvironmentComponent>();
                if(comp)
                {
                    auto *compObj = PyObject_New(PyObject_EnvironmentComponent, &PyTypeObject_EnvironmentComponent);
                    compObj->component = comp;
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if(type == "FigureComponent")
            {
                auto comp = self->sceneObject->getComponent<FigureComponent>();
                if(comp)
                {
                    auto *compObj = PyObject_New(PyObject_FigureComponent, &PyTypeObject_FigureComponent);
                    compObj->component = comp;
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
            else if(type == "SpriteComponent")
            {
                auto comp = self->sceneObject->getComponent<SpriteComponent>();
                if(comp)
                {
                    auto *compObj = PyObject_New(PyObject_SpriteComponent, &PyTypeObject_SpriteComponent);
                    compObj->component = comp;
                    compObj->super.component = compObj->component;
                    return (PyObject*)compObj;
                }
            }
        }
        Py_RETURN_NONE;
    }

    // Get components
    PyObject* SceneObject_getComponents(PyObject_SceneObject* self)
    {
        auto len = self->sceneObject->getComponentsCount();
        if(len > 0)
        {
            auto components = self->sceneObject->getComponents();
            auto compTuple = PyTuple_New(len);
            for(int i = 0; i < len; ++i)
            {
                auto obj = PyObject_New(PyObject_Component, &PyTypeObject_Component);
                obj->component = components[i];
                PyTuple_SetItem(compTuple, i, (PyObject*)obj);
                Py_XDECREF(obj);
            }
            return (PyObject*)compTuple;
        }
        Py_RETURN_NONE;
    }

    // Remove components
    PyObject* SceneObject_removeComponents(PyObject_SceneObject* self)
    {
        self->sceneObject->removeAllComponents();
        Py_RETURN_TRUE;
    }
    
    // Methods
    PyMethodDef SceneObject_methods[] = {
        { "addChild", (PyCFunction)SceneObject_addChild, METH_VARARGS, SceneObject_addChild_doc },
        { "removeChild", (PyCFunction)SceneObject_removeChild, METH_VARARGS, SceneObject_removeChild_doc },
        { "getChildren", (PyCFunction)SceneObject_getChildren, METH_VARARGS, SceneObject_getChildren_doc },
        { "removeChildren", (PyCFunction)SceneObject_removeChildren, METH_VARARGS, SceneObject_removeChildren_doc },
        { "addComponent", (PyCFunction)SceneObject_addComponent, METH_VARARGS, SceneObject_addComponent_doc },
        { "removeComponent", (PyCFunction)SceneObject_removeComponent, METH_VARARGS, SceneObject_removeComponent_doc },
        { "getComponent", (PyCFunction)SceneObject_getComponent, METH_VARARGS, SceneObject_getComponent_doc },
        { "getComponents", (PyCFunction)SceneObject_getComponents, METH_VARARGS, SceneObject_getComponents_doc },
        { "removeComponents", (PyCFunction)SceneObject_removeComponents, METH_VARARGS, SceneObject_removeComponents_doc },
        { NULL, NULL }
    };

    // Get/Set
    PyGetSetDef SceneObject_getsets[] = {
        { "id", (getter)SceneObject_getId, NULL, SceneObject_id_doc, NULL },
        { "name", (getter)SceneObject_getName, (setter)SceneObject_setName, SceneObject_name_doc, NULL },
        { "active", (getter)SceneObject_getActive, (setter)SceneObject_setActive, SceneObject_active_doc, NULL },
        { "parent", (getter)SceneObject_getParent, (setter)SceneObject_setParent, SceneObject_parent_doc, NULL },
        { NULL, NULL }
    };

    // Type declaration
    PyTypeObject PyTypeObject_SceneObject = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.SceneObject",             /* tp_name */
        sizeof(PyObject_SceneObject),       /* tp_basicsize */
        0,                                  /* tp_itemsize */
        (destructor)SceneObject_dealloc,    /* tp_dealloc */
        0,                                  /* tp_print */
        0,                                  /* tp_getattr */
        0,                                  /* tp_setattr */
        0,                                  /* tp_reserved */
        0,                                  /* tp_repr */
        0,                                  /* tp_as_number */
        0,                                  /* tp_as_sequence */
        0,                                  /* tp_as_mapping */
        0,                                  /* tp_hash */
        0,                                  /* tp_call */
        (reprfunc)SceneObject_str,          /* tp_str */
        0,                                  /* tp_getattro */
        0,                                  /* tp_setattro */
        0,                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                 /* tp_flags */
        0,                                  /* tp_doc */
        0,                                  /* tp_traverse */
        0,                                  /* tp_clear */
        0,                                  /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,                                  /* tp_iter */
        0,                                  /* tp_iternext */
        SceneObject_methods,                /* tp_methods */
        0,                                  /* tp_members */
        SceneObject_getsets,                /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        0,                                  /* tp_new */
        0,                                  /* tp_free */
    };
}
