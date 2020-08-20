#include <Python.h>

#include "components/ScriptComponent.h"
#include "python/pySceneObject.h"
#include "python/pyScript.h"

#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ige::scene
{
    //! Constructor
    ScriptComponent::ScriptComponent(const std::shared_ptr<SceneObject>& owner, const std::string& path)
        : Component(owner), m_pyModule(nullptr), m_pyOwner(nullptr), m_path(path), m_pyClass(nullptr), m_pyInstance(nullptr)
    {
        if (!m_path.empty())
            m_bPathDirty = true;
    }

    //! Destructor
    ScriptComponent::~ScriptComponent()
    {
        unloadPyModule();
        if (m_pyOwner)
        {
            SceneObject_dealloc((PyObject_SceneObject*)m_pyOwner);
            m_pyOwner = nullptr;
        }
    }

    void ScriptComponent::loadPyModule()
    {
        unloadPyModule();

        if(!m_path.empty())
        {
            // Load the module from python source file
            m_pyModule = PyImport_ImportModule(m_path.c_str());
            if(m_pyModule) // Reload from source
                m_pyModule = PyImport_ReloadModule(m_pyModule);

            // Return if the module was not loaded
            if(m_pyModule == nullptr) return;

            // fetch the module's dictionary
            auto dict = PyModule_GetDict(m_pyModule);
            if (dict == nullptr) return;

            // Builds the name of a callable class
            PyObject *key = nullptr, *value = nullptr;
            Py_ssize_t pos = 0;
            while (PyDict_Next(dict, &pos, &key, &value))
            {
                if(PyObject_HasAttrString(value, "onStart") && PyObject_IsSubclass(value, (PyObject*)&PyTypeObject_Script))
                {
                    m_pyClass = value;
                    break;
                }
            }
            Py_DECREF(dict);

            if (m_pyClass == nullptr) {
                return;
            }

            // Creates an instance of the class
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = SceneManager::getInstance()->getCurrentScene()->findObjectById(getOwner()->getId()).get();
            m_pyOwner = (PyObject*)obj;
            auto arglist = Py_BuildValue("(O)", m_pyOwner);
            PyObject* pyConstruct = PyInstanceMethod_New(m_pyClass);
            m_pyInstance = PyObject_CallObject(pyConstruct, arglist);
            Py_DECREF(arglist);
            Py_DECREF(pyConstruct);
        }

        // Clear error states
        PyErr_Clear();
    }

    //! Unload PyModule
    void ScriptComponent::unloadPyModule()
    {
        if(m_pyInstance != nullptr)
        {
            Py_DECREF(m_pyInstance);
            m_pyInstance = nullptr;
        }

        if(m_pyClass != nullptr)
        {
            Py_DECREF(m_pyClass);
            m_pyClass = nullptr;
        }

        if(m_pyModule != nullptr)
        {
            Py_DECREF(m_pyModule);
            m_pyModule = nullptr;
        }

        // Clear error states
        PyErr_Clear();
    }

    //! Awake
    void ScriptComponent::onAwake()
    {
        if (m_pyInstance)
        {
            if (PyObject_HasAttrString(m_pyInstance, "onAwake"))
            {
                auto ret = PyObject_CallMethod(m_pyInstance, "onAwake", nullptr);
                Py_XDECREF(ret);
            }
        }
    }

    //! Start
    void ScriptComponent::onStart()
    {
        if (m_pyInstance)
        {
            if (PyObject_HasAttrString(m_pyInstance, "onStart"))
            {
                auto ret = PyObject_CallMethod(m_pyInstance, "onStart", nullptr);
                Py_XDECREF(ret);
            }
        }
    }

    //! Enable
    void ScriptComponent::onEnable()
    {
        if (m_pyInstance)
        {
            if (PyObject_HasAttrString(m_pyInstance, "onEnable"))
            {
                auto ret = PyObject_CallMethod(m_pyInstance, "onEnable", nullptr);
                Py_XDECREF(ret);
            }
        }
    }

    //! Disable
    void ScriptComponent::onDisable()
    {
        if (m_pyInstance)
        {
            if (PyObject_HasAttrString(m_pyInstance, "onEnable"))
            {
                auto ret = PyObject_CallMethod(m_pyInstance, "onDisable", nullptr);
                Py_XDECREF(ret);
            }
        }
    }

    //! Update functions
    void ScriptComponent::onUpdate(float dt)
    {
        if (m_bPathDirty)
        {
            loadPyModule();

            onStart();

            m_bPathDirty = false;
        }

        if (m_pyInstance)
        {
            // Clear error states
            PyErr_Clear();

            if(PyObject_HasAttrString(m_pyInstance, "onUpdate"))
            {
                auto ret = PyObject_CallMethod(m_pyInstance, "onUpdate", "(f)", dt);
                Py_XDECREF(ret);
            }
        }
    }

    void ScriptComponent::onFixedUpdate(float dt)
    {
        if (PyObject_HasAttrString(m_pyInstance, "onFixedUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onFixedUpdate", "f", dt);
            Py_XDECREF(ret);
        }
    }

    void ScriptComponent::onLateUpdate(float dt)
    {
        if (PyObject_HasAttrString(m_pyInstance, "onLateUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onLateUpdate", "f", dt);
            Py_XDECREF(ret);
        }
    }

    //! Render
    void ScriptComponent::onRender()
    {
        if (PyObject_HasAttrString(m_pyInstance, "onRender"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onRender", nullptr);
            Py_XDECREF(ret);
        }
    }

    //! Destroyed
    void ScriptComponent::onDestroy()
    {
        if (PyObject_HasAttrString(m_pyInstance, "onDestroy"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onDestroy", nullptr);
            Py_XDECREF(ret);
        }
    }

    //! Serialize
    void ScriptComponent::to_json(json& j) const
    {
        j = json {
            {"path", m_path}
        };
    }

    //! Deserialize
    void ScriptComponent::from_json(const json& j)
    {
        setPath(j.at("path"));
    }

    //! Path
    void ScriptComponent::setPath(const std::string& path)
    {
        auto scriptName = fs::path(path).stem().string();

        if (strcmp(m_path.c_str(), scriptName.c_str()) != 0)
        {
            m_path = scriptName;
            m_bPathDirty = true;
        }
    }
}
