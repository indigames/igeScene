#include "python/pySceneManager.h"
#include "python/pySceneManager_doc_en.h"
#include "python/pyScene.h"

#include "scene/SceneManager.h"
#include "scene/Scene.h"

namespace ige::scene
{
    // Deallocation
    void  SceneManager_dealloc(PyObject_SceneManager *self)
    {
        if(self) {
            self->sceneManager = nullptr;
            Py_TYPE(self)->tp_free(self);
        }
    }

    // String representation
    PyObject* SceneManager_str(PyObject_SceneManager *self)
    {
        return PyUnicode_FromString("C++ SceneManager object");
    }

    // Get singleton instance
    PyObject* SceneManager_getInstance()
    {
        auto* self = (PyObject_SceneManager*)(&PyTypeObject_SceneManager)->tp_alloc(&PyTypeObject_SceneManager, 0);
        self->sceneManager = SceneManager::getInstance().get();
        return (PyObject*)self;
    }

    // Create scene
    PyObject* SceneManager_createScene(PyObject_SceneManager* self, PyObject* value)
    {
        if (!self->sceneManager) Py_RETURN_NONE;
        char* name;
        if (PyArg_ParseTuple(value, "s", &name)) {
            if(name) {
                auto scene = self->sceneManager->createScene(std::string((const char*)name));
                auto *obj = (PyObject_Scene*)(&PyTypeObject_Scene)->tp_alloc(&PyTypeObject_Scene, 0);
                obj->scene = scene;
                return (PyObject*)obj;
            }
        }
        Py_RETURN_NONE;
    }

    // Load scene
    PyObject* SceneManager_loadScene(PyObject_SceneManager* self, PyObject* value)
    {
        if (!self->sceneManager) Py_RETURN_NONE;
        char* path;
        if (PyArg_ParseTuple(value, "s", &path)) {
            if(path) {
                auto scene = self->sceneManager->createScene();
                auto success = self->sceneManager->loadScene(scene, std::string(path));
                if (success) {
                    auto* obj = (PyObject_Scene*)(&PyTypeObject_Scene)->tp_alloc(&PyTypeObject_Scene, 0);
                    obj->scene = scene;
                    return (PyObject*)obj;
                }
            }
        }
        Py_RETURN_NONE;
    }

    // Unload scene
    PyObject* SceneManager_unloadScene(PyObject_SceneManager* self, PyObject* value)
    {
        if (!self->sceneManager) Py_RETURN_NONE;
        PyObject* obj = nullptr;
		if (PyArg_ParseTuple(value, "|O", &obj))
        {
            if (obj)
            {
                if (PyUnicode_Check(obj))
                {
                    const char* name = PyUnicode_AsUTF8(obj);
                    self->sceneManager->unloadScene(name);
                    Py_RETURN_TRUE;
                }
                else if(obj->ob_type == &PyTypeObject_Scene)
                {
                    auto sceneObj = (PyObject_Scene*)obj;
                    if (!sceneObj->scene.expired()) {
                        self->sceneManager->unloadScene(sceneObj->scene.lock()->getName());
                        Py_RETURN_TRUE;
                    }
                }
            }
            else {
                self->sceneManager->unloadScene(self->sceneManager->getCurrentScene());
            }
        }
        Py_RETURN_FALSE;
    }

    // Reload scene
    PyObject* SceneManager_reloadScene(PyObject_SceneManager* self, PyObject* value)
    {
        if (!self->sceneManager) Py_RETURN_FALSE;
        self->sceneManager->reloadScene();
        Py_RETURN_TRUE;
    }

    // Save scene
    PyObject* SceneManager_saveScene(PyObject_SceneManager* self, PyObject* value)
    {
        if (!self->sceneManager) Py_RETURN_NONE;
        char* path;
        if (PyArg_ParseTuple(value, "s", &path)) {
            if(path) {
                auto *obj = (PyObject_Scene*)(&PyTypeObject_Scene)->tp_alloc(&PyTypeObject_Scene, 0);
                self->sceneManager->saveScene(std::string(path));
                Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    // Get current scene
    PyObject* SceneManager_getCurrentScene(PyObject_SceneManager* self)
    {
        if (!self->sceneManager) Py_RETURN_NONE;
        auto *obj = (PyObject_Scene*)(&PyTypeObject_Scene)->tp_alloc(&PyTypeObject_Scene, 0);
        obj->scene = self->sceneManager->getCurrentScene();
        return (PyObject*)obj;
    }

    // Set current scene
    int SceneManager_setCurrentScene(PyObject_SceneManager* self, PyObject* value)
    {
        if (!self->sceneManager) return -1;
        if (value && value->ob_type == &PyTypeObject_Scene) {
            auto sceneObj = (PyObject_Scene*)value;
            if (!sceneObj->scene.expired()) {
                self->sceneManager->setCurrentScene(sceneObj->scene.lock()->getName());
                return 0;
            }
        }
        return -1;
    }

    // Methods
    PyMethodDef SceneManager_methods[] = {
        { "getInstance", (PyCFunction)SceneManager_getInstance, METH_NOARGS | METH_STATIC, SceneManager_getInstance_doc },
        { "createScene", (PyCFunction)SceneManager_createScene, METH_VARARGS, SceneManager_createScene_doc },
        { "loadScene", (PyCFunction)SceneManager_loadScene, METH_VARARGS, SceneManager_loadScene_doc },
        { "unloadScene", (PyCFunction)SceneManager_unloadScene, METH_VARARGS, SceneManager_unloadScene_doc },
        { "reloadScene", (PyCFunction)SceneManager_reloadScene, METH_VARARGS, SceneManager_reloadScene_doc },
        { "saveScene", (PyCFunction)SceneManager_saveScene, METH_VARARGS, SceneManager_saveScene_doc },
        { NULL, NULL }
    };

    // Get/Set
    PyGetSetDef SceneManager_getsets[] = {
        { "currentScene", (getter)SceneManager_getCurrentScene, (setter)SceneManager_setCurrentScene, SceneManager_currentScene_doc, NULL },
        { NULL, NULL }
    };

    // Type declaration
    PyTypeObject PyTypeObject_SceneManager = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.SceneManager",            /* tp_name */
        sizeof(PyObject_SceneManager),      /* tp_basicsize */
        0,                                  /* tp_itemsize */
        (destructor)SceneManager_dealloc,   /* tp_dealloc */
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
        (reprfunc)SceneManager_str,         /* tp_str */
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
        SceneManager_methods,               /* tp_methods */
        0,                                  /* tp_members */
        SceneManager_getsets,               /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        0,                                  /* tp_new */ // [IGE]: singleton
        0,                                  /* tp_free */
    };
}
