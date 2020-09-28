#include <Python.h>

#include "components/ScriptComponent.h"
#include "python/pySceneObject.h"
#include "python/pyScript.h"

#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include "utils/PyxieHeaders.h"
extern std::shared_ptr<Application> gApp;

namespace ige::scene
{
    //! Constructor
    ScriptComponent::ScriptComponent(const std::shared_ptr<SceneObject>& owner, const std::string& path)
        : Component(owner), m_path(path), m_pyModule(nullptr), m_pyInstance(nullptr)
    {
        m_bPathDirty = true;
    }

    //! Destructor
    ScriptComponent::~ScriptComponent()
    {
        unloadPyModule();
    }

    void ScriptComponent::loadPyModule()
    {
        unloadPyModule();

        if(!m_path.empty())
        {
            // Load the module from python source file
            m_pyModule = PyImport_ImportModule(m_path.c_str());

            // Reload from source
            if (m_pyModule)
            {
                auto module = PyImport_ReloadModule(m_pyModule);
                Py_DECREF(m_pyModule);
                m_pyModule = module;
            }

            // Return if the module was not loaded
            if (m_pyModule == nullptr)
            {
                PyErr_Clear();
                return;
            }

            // fetch the module's dictionary
            auto dict = PyModule_GetDict(m_pyModule);
            if (dict == nullptr)
            {
                PyErr_Clear();
                return;
            }

            // Builds the name of a callable class
            PyObject* key = nullptr,  *value = nullptr, *pyClass = nullptr;
            Py_ssize_t pos = 0;
            while (PyDict_Next(dict, &pos, &key, &value))
            {
                if(PyObject_HasAttrString(value, "onStart") && PyObject_IsSubclass(value, (PyObject*)&PyTypeObject_Script))
                {
                    pyClass = value;
                    break;
                }
            }

            if (pyClass == nullptr) {
                PyErr_Clear();
                return;
            }

            // Creates an instance of the class
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = getOwner();
            auto arglist = Py_BuildValue("(O)", obj);
            PyObject* pyConstruct = PyInstanceMethod_New(pyClass);
            m_pyInstance = PyObject_CallObject(pyConstruct, arglist);
            Py_DECREF(arglist);
            Py_DECREF(pyConstruct);

            PyErr_Clear();

            // Initialized, call invoke onAwake()
            onAwake();

            // Enable call onStart() next frame
            m_bOnStartCalled = false;
        }
    }

    //! Unload PyModule
    void ScriptComponent::unloadPyModule()
    {
        m_pyInstance = nullptr;

        if(m_pyModule != nullptr)
        {
            Py_XDECREF(m_pyModule);
            m_pyModule = nullptr;
        }
    }

    //! Awake
    void ScriptComponent::onAwake()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onAwake"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onAwake", nullptr);
            Py_XDECREF(ret);
        }        
    }

    //! Start
    void ScriptComponent::onStart()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onStart"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onStart", nullptr);
            Py_XDECREF(ret);
        }
    }

    //! Enable
    void ScriptComponent::onEnable()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onEnable"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onEnable", nullptr);
            Py_XDECREF(ret);
        }
    }

    //! Disable
    void ScriptComponent::onDisable()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onEnable"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onDisable", nullptr);
            Py_XDECREF(ret);
        }
    }

    //! Update functions
    void ScriptComponent::onUpdate(float dt)
    {
        if (SceneManager::getInstance()->isEditor())
            return;

        // Check reload script
        if (m_bPathDirty)
        {
            loadPyModule();
            m_bPathDirty = false;
        }
        
        // Check call onStart()
        if (!m_bOnStartCalled)
        {
            onStart();
            m_bOnStartCalled = true;
        }

        // Check call onUpdate()
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onUpdate", "(f)", dt);
            Py_XDECREF(ret);
        }
    }

    void ScriptComponent::onFixedUpdate(float dt)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onFixedUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onFixedUpdate", "f", dt);
            Py_XDECREF(ret);
        }
    }

    void ScriptComponent::onLateUpdate(float dt)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onLateUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onLateUpdate", "f", dt);
            Py_XDECREF(ret);
        }
    }

    //! Render
    void ScriptComponent::onRender()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onRender"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onRender", nullptr);
            Py_XDECREF(ret);
        }
    }

    //! Destroyed
    void ScriptComponent::onDestroy()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onDestroy"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onDestroy", nullptr);
            Py_XDECREF(ret);
        }
    }

    //! Click
    void ScriptComponent::onClick()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onClick"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onClick", nullptr);
            Py_XDECREF(ret);
        }
    }

    //! Suspend
    void ScriptComponent::onSuspend()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onSuspend"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onSuspend", nullptr);
            Py_XDECREF(ret);
        }
    }

    //! Resume
    void ScriptComponent::onResume()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onResume"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onResume", nullptr);
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
