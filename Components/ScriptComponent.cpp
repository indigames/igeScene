#include <Python.h>

#include "components/ScriptComponent.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ige::scene
{
    //! Constructor
    ScriptComponent::ScriptComponent(const std::shared_ptr<SceneObject>& owner, const std::string& path)
        : Component(owner), m_pModule(nullptr), m_path(path)
    {
        if (!m_path.empty())
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
            auto mod =  PyUnicode_FromString(m_path.c_str());
            if (mod != nullptr) {
                m_pModule = PyImport_Import(mod);
                Py_DECREF(mod);
            }
        }
    }

    //! Unload PyModule
    void ScriptComponent::unloadPyModule()
    {
        if(m_pModule != nullptr)
        {
            Py_DECREF(m_pModule);
            m_pModule = nullptr;
        }

        // Clear error states
        PyErr_Clear();
    }

    //! Awake
    void ScriptComponent::onAwake()
    {
        if (m_pModule)
        {
            auto func = PyObject_GetAttrString(m_pModule, "Awake");
            if(func)
            {
                auto ret = PyObject_CallObject(func, NULL);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Start
    void ScriptComponent::onStart()
    {
        if (m_pModule)
        {
            auto func = PyObject_GetAttrString(m_pModule, "Start");
            if(func)
            {
                auto ret = PyObject_CallObject(func, NULL);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Enable
    void ScriptComponent::onEnable()
    {
        if (m_pModule)
        {
            auto func = PyObject_GetAttrString(m_pModule, "Enable");
            if(func)
            {
                auto ret = PyObject_CallObject(func, NULL);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Disable
    void ScriptComponent::onDisable()
    {
        if (m_pModule)
        {
            auto func = PyObject_GetAttrString(m_pModule, "Disable");
            if(func)
            {
                auto ret = PyObject_CallObject(func, NULL);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Update functions
    void ScriptComponent::onUpdate(float dt)
    {
        if (m_bPathDirty)
        {
            loadPyModule();
            m_bPathDirty = false;
        }

        if (m_pModule)
        {
            auto func = PyObject_GetAttrString(m_pModule, "Update");
            if(func)
            {
                auto arglist = Py_BuildValue("(f)", dt);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    void ScriptComponent::onFixedUpdate(float dt)
    {
        if (m_pModule)
        {
            auto func = PyObject_GetAttrString(m_pModule, "FixedUpdate");
            if(func)
            {
                auto arglist = Py_BuildValue("(f)", dt);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    void ScriptComponent::onLateUpdate(float dt)
    {
        if (m_pModule)
        {
            auto func = PyObject_GetAttrString(m_pModule, "LateUpdate");
            if(func)
            {
                auto arglist = Py_BuildValue("(f)", dt);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Render
    void ScriptComponent::onRender()
    {
        if (m_pModule)
        {
            auto func = PyObject_GetAttrString(m_pModule, "Render");
            if(func)
            {
                auto ret = PyObject_CallObject(func, NULL);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Destroyed
    void ScriptComponent::onDestroy()
    {
        if (m_pModule)
        {
            auto func = PyObject_GetAttrString(m_pModule, "Destroy");
            if(func)
            {
                auto ret = PyObject_CallObject(func, NULL);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
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

        if (!scriptName.empty() && strcmp(m_path.c_str(), scriptName.c_str()) != 0)
        {
            m_path = scriptName;
            m_bPathDirty = true;
        }
    }
}
