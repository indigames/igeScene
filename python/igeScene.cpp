#include <Python.h>
#include <pythonResource.h>

#include "python/igeScene.h"
#include "python/igeScene_doc_en.h"

#include "python/pyScene.h"
#include "python/pySceneManager.h"
#include "python/pySceneObject.h"
#include "python/pyScript.h"

#include "python/pyComponent.h"
#include "python/pyTransformComponent.h"
#include "python/pyCameraComponent.h"
#include "python/pyEnvironmentComponent.h"
#include "python/pyFigureComponent.h"
#include "python/pySpriteComponent.h"
#include "python/pyRectTransform.h"
#include "python/pyCanvas.h"
#include "python/pyUIImage.h"
#include "python/pyUIText.h"
#include "python/pyUIButton.h"

using namespace ige::scene;

static PyObject* igeScene_getVersion(PyObject* self)
{
    return PyUnicode_FromString("V0.0.1");
}

static PyMethodDef igeScene_methods[] =
{
    {"getVersion", (PyCFunction)igeScene_getVersion, METH_NOARGS, getVersion_doc },
    { nullptr, nullptr, 0, nullptr }
};

static PyModuleDef igeScene_module =
{
    PyModuleDef_HEAD_INIT,
    "igeScene",
    "IGE Scene Module",
    0,
    igeScene_methods
};

PyMODINIT_FUNC PyInit_igeScene()
{
    if (!ImportVMath()) {
        PyErr_SetString(PyExc_TypeError, "pyvmath isn't installed. please  pip install pyvmath before.");
        return NULL;
    }

    PyObject *module = PyModule_Create(&igeScene_module);
    if (PyType_Ready(&PyTypeObject_Scene) < 0) return NULL;
    if (PyType_Ready(&PyTypeObject_SceneManager) < 0) return NULL;
    if (PyType_Ready(&PyTypeObject_SceneObject) < 0) return NULL;

    Py_INCREF(&PyTypeObject_Scene);
    PyModule_AddObject(module, "Scene", (PyObject*)&PyTypeObject_Scene);

    Py_INCREF(&PyTypeObject_SceneManager);
    PyModule_AddObject(module, "SceneManager", (PyObject*)&PyTypeObject_SceneManager);

    Py_INCREF(&PyTypeObject_SceneObject);
    PyModule_AddObject(module, "SceneObject", (PyObject*)&PyTypeObject_SceneObject);

    if (PyType_Ready(&PyTypeObject_Component) < 0) return NULL;
    if (PyType_Ready(&PyTypeObject_TransformComponent) < 0) return NULL;
    if (PyType_Ready(&PyTypeObject_CameraComponent) < 0) return NULL;
    if (PyType_Ready(&PyTypeObject_EnvironmentComponent) < 0) return NULL;
    if (PyType_Ready(&PyTypeObject_FigureComponent) < 0) return NULL;
    if (PyType_Ready(&PyTypeObject_SpriteComponent) < 0) return NULL;

    Py_INCREF(&PyTypeObject_Component);
    PyModule_AddObject(module, "Component", (PyObject*)&PyTypeObject_Component);

    Py_INCREF(&PyTypeObject_TransformComponent);
    PyModule_AddObject(module, "TransformComponent", (PyObject*)&PyTypeObject_TransformComponent);

    Py_INCREF(&PyTypeObject_CameraComponent);
    PyModule_AddObject(module, "CameraComponent", (PyObject*)&PyTypeObject_CameraComponent);

    Py_INCREF(&PyTypeObject_EnvironmentComponent);
    PyModule_AddObject(module, "EnvironmentComponent", (PyObject*)&PyTypeObject_EnvironmentComponent);

    Py_INCREF(&PyTypeObject_FigureComponent);
    PyModule_AddObject(module, "FigureComponent", (PyObject*)&PyTypeObject_FigureComponent);

    Py_INCREF(&PyTypeObject_SpriteComponent);
    PyModule_AddObject(module, "SpriteComponent", (PyObject*)&PyTypeObject_SpriteComponent);

    if (PyType_Ready(&PyTypeObject_RectTransform) < 0) return NULL;
    Py_INCREF(&PyTypeObject_RectTransform);
    PyModule_AddObject(module, "RectTransform", (PyObject*)&PyTypeObject_RectTransform);

    if (PyType_Ready(&PyTypeObject_Canvas) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Canvas);
    PyModule_AddObject(module, "Canvas", (PyObject*)&PyTypeObject_Canvas);

    if (PyType_Ready(&PyTypeObject_UIImage) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UIImage);
    PyModule_AddObject(module, "UIImage", (PyObject*)&PyTypeObject_UIImage);

    if (PyType_Ready(&PyTypeObject_UIText) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UIText);
    PyModule_AddObject(module, "UIText", (PyObject*)&PyTypeObject_UIText);

    if (PyType_Ready(&PyTypeObject_UIButton) < 0) return NULL;
    Py_INCREF(&PyTypeObject_UIButton);
    PyModule_AddObject(module, "UIButton", (PyObject*)&PyTypeObject_UIButton);

    if (PyType_Ready(&PyTypeObject_Script) < 0) return NULL;
    Py_INCREF(&PyTypeObject_Script);
    PyModule_AddObject(module, "Script", (PyObject*)&PyTypeObject_Script);

    return module;
}
