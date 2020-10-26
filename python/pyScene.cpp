#include "python/pyScene.h"
#include "python/pyScene_doc_en.h"

#include "python/pySceneObject.h"
#include "python/pyCameraComponent.h"

#include "scene/SceneManager.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include <pythonResource.h>

namespace ige::scene
{
    PyObject* Scene_new(PyTypeObject* type, PyObject* args, PyObject* kw)
    {
        char* name;
        PyObject_Scene* self = nullptr;
        if (PyArg_ParseTuple(args, "s", &name)) {
            self = (PyObject_Scene*)type->tp_alloc(type, 0);
            self->scene = SceneManager::getInstance()->createScene(std::string(name)).get();
        }
        return (PyObject*)self;
    }

    void  Scene_dealloc(PyObject_Scene *self)
    {
        if(self && self->scene)
        {
            self->scene = nullptr;
        }
        Py_TYPE(self)->tp_free(self);
    }

    PyObject* Scene_str(PyObject_Scene *self)
    {
        return PyUnicode_FromString("C++ Scene object");
    }

    // Get name
    PyObject* Scene_getName(PyObject_Scene* self)
    {
        const char* str = self->scene ? self->scene->getName().c_str() : "";
        return PyUnicode_FromString(str);
    }

    // Set name
    int Scene_setName(PyObject_Scene* self, PyObject* value)
    {
        char* name = NULL;
        if (PyArg_ParseTuple(value, "s", &name)) {
            if(name != NULL)
                self->scene->setName(std::string(name));
        }
        return 0;
    }

    // Get active camera
    PyObject* Scene_getActiveCamera(PyObject_Scene* self)
    {
        auto camera = self->scene->getActiveCamera();
        if (camera)
        {
           auto* obj = PyObject_New(PyObject_CameraComponent, &PyTypeObject_CameraComponent);
           obj->component = camera;
           obj->super.component = obj->component;
           return (PyObject*)obj;
        }
        Py_RETURN_NONE;
    }

    // Set active camera
    int Scene_setActiveCamera(PyObject_Scene* self, PyObject* value)
    {
        PyObject* camera;
        if (PyArg_ParseTuple(value, "O", &camera)) {
           self->scene->setActiveCamera(((PyObject_CameraComponent*)camera)->component);
        }
        return 0;
    }

    // Create object
    PyObject* Scene_createObject(PyObject_Scene *self, PyObject* args)
    {
        char* name;
        PyObject* parentObj;

        if (PyArg_ParseTuple(args, "sO", &name, &parentObj)) {
            auto *obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            auto parent = parentObj ? SceneManager::getInstance()->getCurrentScene()->findObjectById(((PyObject_SceneObject*)parentObj)->sceneObject->getId()) : nullptr;
            obj->sceneObject = self->scene->createObject(name, parent).get();
            return (PyObject*)obj;
        }
        Py_RETURN_NONE;
    }

    // Remove object
    PyObject* Scene_removeObject(PyObject_Scene *self, PyObject* args)
    {
        PyObject* obj = nullptr;
		if (PyArg_ParseTuple(args, "O", &obj))
        {
            if(obj)
            {
                if(PyUnicode_Check(obj))
                {
                    const char* name = PyUnicode_AsUTF8(obj);
                    auto sceneObject = self->scene->findObjectByName(std::string(name));
                    if(sceneObject)
                    {
                        if(self->scene->removeObject(sceneObject))
                        {
                            Py_RETURN_TRUE;
                        }
                    }
                }
                else if(PyNumber_Check(obj))
                {
                    uint64_t id = PyLong_AsUnsignedLongLong(obj);
                    if(self->scene->removeObjectById(id))
                    {
                        Py_RETURN_TRUE;
                    }
                }
                else if (obj->ob_type == &PyTypeObject_SceneObject)
                {
                    auto pySceneObject = (PyObject_SceneObject*)(obj);
                    if(self->scene->removeObjectById(pySceneObject->sceneObject->getId()))
                    {
                        Py_RETURN_TRUE;
                    }
                }
            }
        }
        Py_RETURN_FALSE;
    }

    // Find object
    PyObject* Scene_findObject(PyObject_Scene *self, PyObject* args)
    {
        PyObject* obj = nullptr;
		if (PyArg_ParseTuple(args, "O", &obj))
        {
            if (obj)
            {
                if (PyUnicode_Check(obj))
                {
                    const char* name = PyUnicode_AsUTF8(obj);
                    auto sceneObject = self->scene->findObjectByName(std::string(name));
                    if (sceneObject)
                    {
                        auto* obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
                        obj->sceneObject = sceneObject.get();
                        return (PyObject*)obj;
                    }
                }
                else if (PyNumber_Check(obj))
                {
                    uint64_t id = PyLong_AsUnsignedLongLong(obj);
                    auto sceneObject = self->scene->findObjectById(id);
                    if (sceneObject)
                    {
                        auto* obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
                        obj->sceneObject = sceneObject.get();
                        return (PyObject*)obj;
                    }
                }
            }
        }
        Py_RETURN_NONE;
    }

    // Get roots
    PyObject* Scene_getObjects(PyObject_Scene *self)
    {
        auto roots = self->scene->getObjects();
        PyObject* pyList = PyList_New(0);
        for(int i = 0; i < roots.size(); ++i)
        {
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = roots[i].get();
            PyList_Append(pyList, (PyObject*)obj);
        }
        return (PyObject*)pyList;
    }

    // Get root object
    PyObject* Scene_getRoot(PyObject_Scene *self)
    {
        auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
        obj->sceneObject = self->scene->getRoot().get();
        return (PyObject*)obj;
    }

    // Get path
    PyObject* Scene_getPath(PyObject_Scene *self)
    {
        return PyUnicode_FromString(self->scene->getPath().c_str());
    }

    // Get showcase
    PyObject* Scene_getShowcase(PyObject_Scene *self)
    {
        auto *obj = PyObject_New(showcase_obj, &ShowcaseType);
        obj->showcase = self->scene->getShowcase();
        return (PyObject *)obj;
    }

    // Get environment
    PyObject* Scene_getEnvironment(PyObject_Scene *self)
    {
        auto *obj = PyObject_New(environment_obj, &EnvironmentType);
        obj->envSet = self->scene->getEnvironment();
        return (PyObject *)obj;
    }

    // Compare function
    static PyObject* Scene_richcompare(PyObject* self, PyObject* other, int op)
    {
        if (op == Py_LT || op == Py_LE || op == Py_GT || op == Py_GE)
        {
            return Py_NotImplemented;
        }

        if (self != Py_None && other != Py_None)
        {
            if (other->ob_type == &PyTypeObject_Scene)
            {
                auto selfCmp = (PyObject_Scene*)(self);
                auto otherCmp = (PyObject_Scene*)(other);
                bool eq = (selfCmp->scene == otherCmp->scene);
                if (op == Py_NE)
                    eq = !eq;
                return eq ? Py_True : Py_False;
            }
            else
            {
                return (op == Py_EQ) ? Py_False : Py_True;
            }
        }
        else if (self == Py_None && other == Py_None)
        {
            return (op == Py_EQ) ? Py_True : Py_False;
        }
        else
        {
            return (op == Py_EQ) ? Py_False : Py_True;
        }
    }

    // Methods definition
    PyMethodDef Scene_methods[] = {
        { "createObject", (PyCFunction)Scene_createObject, METH_VARARGS, Scene_createObject_doc },
        { "removeObject", (PyCFunction)Scene_removeObject, METH_VARARGS, Scene_removeObject_doc },
        { "findObject", (PyCFunction)Scene_findObject, METH_VARARGS, Scene_findObject_doc },
        { "getObjects", (PyCFunction)Scene_getObjects, METH_NOARGS, Scene_getObjects_doc },
        { "getRoot", (PyCFunction)Scene_getRoot, METH_NOARGS, Scene_getRoot_doc },
        { "getPath", (PyCFunction)Scene_getPath, METH_NOARGS, Scene_getPath_doc },
        { "getShowcase", (PyCFunction)Scene_getShowcase, METH_NOARGS, Scene_getShowcase_doc },
        { "getEnvironment", (PyCFunction)Scene_getEnvironment, METH_NOARGS, Scene_getEnvironment_doc },
        { NULL, NULL }
    };

    // Variable definition
    PyGetSetDef Scene_getsets[] = {
        { "name", (getter)Scene_getName, (setter)Scene_setName, Scene_name_doc, NULL },
        { "activeCamera", (getter)Scene_getActiveCamera, (setter)Scene_setActiveCamera, Scene_activeCamera_doc, NULL },
        { NULL, NULL }
    };

    // Type definition
    PyTypeObject PyTypeObject_Scene = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "igeScene.Scene",                   /* tp_name */
        sizeof(PyObject_Scene),             /* tp_basicsize */
        0,                                  /* tp_itemsize */
        (destructor)Scene_dealloc,          /* tp_dealloc */
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
        (reprfunc)Scene_str,                /* tp_str */
        0,                                  /* tp_getattro */
        0,                                  /* tp_setattro */
        0,                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                 /* tp_flags */
        0,                                  /* tp_doc */
        0,                                  /* tp_traverse */
        0,                                  /* tp_clear */
        Scene_richcompare,                  /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,                                  /* tp_iter */
        0,                                  /* tp_iternext */
        Scene_methods,                      /* tp_methods */
        0,                                  /* tp_members */
        Scene_getsets,                      /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        0,                                  /* tp_init */
        0,                                  /* tp_alloc */
        Scene_new,                          /* tp_new */
        0,                                  /* tp_free */
    };
}
