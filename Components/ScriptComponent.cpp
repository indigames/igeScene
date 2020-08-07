#include <Python.h>

#include "components/ScriptComponent.h"
#include "python/pySceneObject.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ige::scene
{
    //! Constructor
    ScriptComponent::ScriptComponent(const std::shared_ptr<SceneObject>& owner, const std::string& path)
        : Component(owner), m_pyModule(nullptr), m_pyOwner(nullptr), m_path(path)
    {
        if (!m_path.empty())
            m_bPathDirty = true;

        auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
        obj->sceneObject = owner;
        m_pyOwner = (PyObject*)obj;
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
            auto mod =  PyUnicode_FromString(m_path.c_str());
            if (mod != nullptr) {
                m_pyModule = PyImport_Import(mod);
                Py_DECREF(mod);
            }
        }
    }

    //! Unload PyModule
    void ScriptComponent::unloadPyModule()
    {
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
        if (m_pyModule)
        {
            auto func = PyObject_GetAttrString(m_pyModule, "Awake");
            if (func)
            {
                auto arglist = Py_BuildValue("(O)", m_pyOwner);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Start
    void ScriptComponent::onStart()
    {
        if (m_pyModule)
        {
            auto func = PyObject_GetAttrString(m_pyModule, "Start");
            if(func)
            {
                auto arglist = Py_BuildValue("(O)", m_pyOwner);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Enable
    void ScriptComponent::onEnable()
    {
        if (m_pyModule)
        {
            auto func = PyObject_GetAttrString(m_pyModule, "Enable");
            if(func)
            {
                auto arglist = Py_BuildValue("(O)", m_pyOwner);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Disable
    void ScriptComponent::onDisable()
    {
        if (m_pyModule)
        {
            auto func = PyObject_GetAttrString(m_pyModule, "Disable");
            if(func)
            {
                auto arglist = Py_BuildValue("(O)", m_pyOwner);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
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

        if (m_pyModule)
        {
            auto func = PyObject_GetAttrString(m_pyModule, "Update");
            if(func)
            {
                auto arglist = Py_BuildValue("(Of)", m_pyOwner, dt);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    void ScriptComponent::onFixedUpdate(float dt)
    {
        if (m_pyModule)
        {
            auto func = PyObject_GetAttrString(m_pyModule, "FixedUpdate");
            if(func)
            {
                auto arglist = Py_BuildValue("(Of)", m_pyOwner, dt);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    void ScriptComponent::onLateUpdate(float dt)
    {
        if (m_pyModule)
        {
            auto func = PyObject_GetAttrString(m_pyModule, "LateUpdate");
            if(func)
            {
                auto arglist = Py_BuildValue("(Of)", m_pyOwner, dt);
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
        if (m_pyModule)
        {
            auto func = PyObject_GetAttrString(m_pyModule, "Render");
            if(func)
            {
                auto arglist = Py_BuildValue("(O)", m_pyOwner);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
                Py_XDECREF(ret);
                Py_DECREF(func);
            }
        }
    }

    //! Destroyed
    void ScriptComponent::onDestroy()
    {
        if (m_pyModule)
        {
            auto func = PyObject_GetAttrString(m_pyModule, "Destroy");
            if(func)
            {
                auto arglist = Py_BuildValue("(O)", m_pyOwner);
                auto ret = PyObject_CallObject(func, arglist);
                Py_DECREF(arglist);
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

        if (strcmp(m_path.c_str(), scriptName.c_str()) != 0)
        {
            m_path = scriptName;
            m_bPathDirty = true;
        }
    }
}
