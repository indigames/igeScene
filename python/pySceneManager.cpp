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
        if(self)
        {
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
        auto* self = PyObject_New(PyObject_SceneManager, &PyTypeObject_SceneManager);
        self->sceneManager = SceneManager::getInstance().get();
        return (PyObject*)self;
    }

    // Load scene
    PyObject* SceneManager_loadScene(PyObject_SceneManager* self, PyObject* value)
    {
        char* path;
        if (PyArg_ParseTuple(value, "s", &path)) {
            if(path) {
                if(self->sceneManager->loadScene(std::string(path))){
                    Py_RETURN_TRUE;
                }
            }
        }
        Py_RETURN_FALSE;
    }   

    // Get current scene
    PyObject* SceneManager_getCurrentScene(PyObject_SceneManager* self)
    {
        auto *obj = PyObject_New(PyObject_Scene, &PyTypeObject_Scene);
        obj->scene = self->sceneManager->getCurrentScene();
        return (PyObject*)obj;
    }

    // Set current scene
    PyObject* SceneManager_setCurrentScene(PyObject_SceneManager* self, PyObject* value)
    {
        PyObject* obj;
        if (PyArg_ParseTuple(value, "O", &obj)) {
            if(obj && obj->ob_type == &PyTypeObject_Scene) {
                auto sceneObj = (PyObject_Scene*)obj;
                self->sceneManager->setCurrentScene(sceneObj->scene);
                Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }

    // Methods
    PyMethodDef SceneManager_methods[] = {
        { "getInstance", (PyCFunction)SceneManager_getInstance, METH_NOARGS | METH_STATIC, SceneManager_getInstance_doc },
        { "loadScene", (PyCFunction)SceneManager_loadScene, METH_VARARGS, SceneManager_loadScene_doc },
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
