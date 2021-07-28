#include <Python.h>
#include <algorithm>

#include "components/ScriptComponent.h"
#include "components/physic/PhysicObject.h"

#include "python/pySceneObject.h"
#include "python/pyScript.h"

#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "event/EventContext.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include "utils/PyxieHeaders.h"
extern std::shared_ptr<Application> gApp;

namespace ige::scene
{
    static PyObject* parseObject(const Value& value)
    {
        switch (value.getType()) {
        case Value::Type::NONE:
        default:
        {
        }
        break;
        case Value::Type::BYTE:
        {
            return Py_BuildValue("(b)", value.asByte());
        }
        break;
        case Value::Type::INTEGER:
        {
            return Py_BuildValue("(i)", value.asInt());
        }
        break;
        case Value::Type::UNSIGNED:
        {
            return Py_BuildValue("(I)", value.asUnsignedInt());
        }
        break;
        case Value::Type::FLOAT:
        {
            return Py_BuildValue("(f)", value.asFloat());
        }
        break;
        case Value::Type::DOUBLE:
        {
            return Py_BuildValue("(d)", value.asDouble());
        }
        break;
        case Value::Type::BOOLEAN:
        {
            return Py_BuildValue("(O)", value.asBool() ? Py_True : Py_False);
        }
        break;
        case Value::Type::STRING:
        {
            return Py_BuildValue("(s)", value.asString().c_str());
        }
        break;
        case Value::Type::VECTOR:
        {

            auto valueArray = value.asValueVector();
            int listSize = valueArray.size();
            if (listSize > 0)
            {
                PyObject* list = PyList_New(listSize);
                for (int i = 0; i < listSize; i++)
                {
                    PyList_SetItem(list, i, parseObject(valueArray[i]));
                }
                return Py_BuildValue("O", list);
            }
        }
        break;
        }
        return nullptr;
    }

    //! Constructor
    ScriptComponent::ScriptComponent(SceneObject &owner, const std::string &path)
        : RuntimeComponent(owner), m_path(path), m_pyModule(nullptr), m_pyInstance(nullptr)
    {
        
    }

    //! Destructor
    ScriptComponent::~ScriptComponent()
    {
        
    }

    void ScriptComponent::Initialize()
    {
        RuntimeComponent::Initialize();
        getOwner()->addEventListener((int)EventType::Click, std::bind(&ScriptComponent::onClickEvent, this, std::placeholders::_1), m_instanceId);
        getOwner()->addEventListener((int)EventType::Changed, std::bind(&ScriptComponent::onChangedValueEvent, this, std::placeholders::_1), m_instanceId);

        loadPyModule();
    }

    void ScriptComponent::Clear()
    {
        RuntimeComponent::Clear();
        unloadPyModule();
        m_members.clear();
        getOwner()->removeEventListener((int)EventType::Click, m_instanceId);
        getOwner()->removeEventListener((int)EventType::Changed, m_instanceId);
    }

    void ScriptComponent::onClickEvent(EventContext* context)
    {
        this->onClick();
    }

    void ScriptComponent::onChangedValueEvent(EventContext* context)
    {
        this->onChangedValue(context->getDataValue());
    }

    void ScriptComponent::loadPyModule()
    {
        unloadPyModule();

        if (!m_path.empty())
        {
            // Load the module from python source file
            auto path = fs::absolute(fs::path(m_path));
            auto appendCmd = std::string("import sys\nsys.path.append('") + path.parent_path().string() + "'\n)";
            std::replace(appendCmd.begin(), appendCmd.end(), '\\', '/');
            PyRun_SimpleString(appendCmd.c_str());

            auto module = path.stem().string();
            m_pyModule = PyImport_ImportModule(module.c_str());

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
                PyObject* ptype, * pvalue, * ptraceback;
                PyErr_Fetch(&ptype, &pvalue, &ptraceback);
                auto pStrErrorMessage = PyUnicode_AsUTF8(PyObject_Str(pvalue));
                if (pStrErrorMessage != nullptr)
                    pyxie_printf(pStrErrorMessage);
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
            PyObject *key = nullptr, *value = nullptr, *pyClass = nullptr;
            Py_ssize_t pos = 0;
            while (PyDict_Next(dict, &pos, &key, &value))
            {
                if (PyObject_HasAttrString(value, "onStart") && PyObject_IsSubclass(value, (PyObject *)&PyTypeObject_Script))
                {
                    pyClass = value;
                    break;
                }
            }

            if (pyClass == nullptr)
            {
                PyErr_Clear();
                return;
            }

            // Creates an instance of the class
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = getOwner();
            auto arglist = Py_BuildValue("(O)", obj);
            PyObject *pyConstruct = PyInstanceMethod_New(pyClass);
            m_pyInstance = PyObject_CallObject(pyConstruct, arglist);
            Py_DECREF(arglist);
            Py_DECREF(pyConstruct);

            PyErr_Clear();

            key = nullptr;
            value = nullptr;
            pos = 0;
            dict = PyObject_GenericGetDict(m_pyInstance, nullptr);
            while (PyDict_Next(dict, &pos, &key, &value))
            {
                auto key_str = std::string(PyUnicode_AsUTF8(key));
                if (!PyObject_IsInstance(value, (PyObject*)&PyFunction_Type))
                {
                    if (m_members.count(key_str) > 0)
                    {
                        onMemberValueChanged(key_str, m_members[key_str]);
                    }
                    else
                    {
                        if (PyUnicode_Check(value))
                        {
                            m_members[key_str] = std::string(PyUnicode_AsUTF8(value));
                        }
                        else if (PyBool_Check(value))
                        {
                            m_members[key_str] = (bool)(PyLong_AsLong(value));
                        }
                        else if (PyLong_Check(value))
                        {
                            m_members[key_str] = (int)PyLong_AsLong(value);
                        }
                        else if (PyFloat_Check(value))
                        {
                            m_members[key_str] = PyFloat_AsDouble(value);
                        }
                        else
                        {
                            m_members[key_str] = nullptr;
                        }
                    }
                }
            }

            // Enable call onAwake() next frame
            m_bOnAwakeCalled = false;
        }
    }

    //! Unload PyModule
    void ScriptComponent::unloadPyModule()
    {
        m_pyInstance = nullptr;

        if (m_pyModule != nullptr)
        {
            Py_XDECREF(m_pyModule);
            m_pyModule = nullptr;
        }

        unregisterPhysicEvents();
    }

    //! Register physic events
    void ScriptComponent::registerPhysicEvents()
    {
        if (getOwner() == nullptr)
            return;

        auto physicComp = getOwner()->getComponent<PhysicObject>();
        if (physicComp != nullptr)
        {
            physicComp->getTriggerStartEvent().addListener([this](auto other) {
                auto otherObject = other->getOwner();
                onTriggerStart(*otherObject);
            });

            physicComp->getTriggerStayEvent().addListener([this](auto other) {
                auto otherObject = other->getOwner();
                onTriggerStay(*otherObject);
            });

            physicComp->getTriggerStopEvent().addListener([this](auto other) {
                auto otherObject = other->getOwner();
                onTriggerStop(*otherObject);
            });

            physicComp->getCollisionStartEvent().addListener([this](auto other) {
                auto otherObject = other->getOwner();
                onCollisionStart(*otherObject);
            });

            physicComp->getCollisionStayEvent().addListener([this](auto other) {
                auto otherObject = other->getOwner();
                onCollisionStay(*otherObject);
            });

            physicComp->getCollisionStayEvent().addListener([this](auto other) {
                auto otherObject = other->getOwner();
                onCollisionStop(*otherObject);
            });
        }
    }

    //! Unregister physic events
    void ScriptComponent::unregisterPhysicEvents()
    {
        if (getOwner() == nullptr)
            return;

        auto physicComp = getOwner()->getComponent<PhysicObject>();
        if (physicComp != nullptr)
        {
            physicComp->getTriggerStartEvent().removeAllListeners();
            physicComp->getTriggerStayEvent().removeAllListeners();
            physicComp->getTriggerStopEvent().removeAllListeners();

            physicComp->getCollisionStartEvent().removeAllListeners();
            physicComp->getCollisionStayEvent().removeAllListeners();
            physicComp->getCollisionStayEvent().removeAllListeners();
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
        if (SceneManager::getInstance()->isEditor() || !isRunning())
            return;

        if (!m_bOnAwakeCalled)
        {
            // Register physic events
            registerPhysicEvents();

            // Initialized, call invoke onAwake()
            onAwake();

            m_bOnAwakeCalled = true;
            m_bOnStartCalled = false;
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
        if (SceneManager::getInstance()->isEditor())
            return;
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onFixedUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onFixedUpdate", "f", dt);
            Py_XDECREF(ret);
        }
    }

    void ScriptComponent::onLateUpdate(float dt)
    {
        if (SceneManager::getInstance()->isEditor())
            return;
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onLateUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onLateUpdate", "f", dt);
            Py_XDECREF(ret);
        }
    }

    //! Render
    void ScriptComponent::onRender()
    {
        if (SceneManager::getInstance()->isEditor())
            return;
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
        if (SceneManager::getInstance()->isEditor())
            return;
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

    //! Change Value
    void ScriptComponent::onChangedValue(Value value)
    {
        /*if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onChangedValue"))
        {
            switch (value.getType()) {
            case Value::Type::NONE:
            default:
                {
                    auto ret = PyObject_CallMethod(m_pyInstance, "onChangedValue", nullptr);
                    Py_XDECREF(ret);
                }
                break;
            case Value::Type::BYTE:
                {
                    auto ret = PyObject_CallMethod(m_pyInstance, "onChangedValue", "(b)", value.asByte());
                    Py_XDECREF(ret);
                }
                break;
            case Value::Type::INTEGER:
                {
                    auto ret = PyObject_CallMethod(m_pyInstance, "onChangedValue", "(i)", value.asInt());
                    Py_XDECREF(ret);
                }
                break;
            case Value::Type::UNSIGNED:
                {
                    auto ret = PyObject_CallMethod(m_pyInstance, "onChangedValue", "(I)", value.asUnsignedInt());
                    Py_XDECREF(ret);
                }
                break;
            case Value::Type::FLOAT:
                {
                    auto ret = PyObject_CallMethod(m_pyInstance, "onChangedValue", "(f)", value.asFloat());
                    Py_XDECREF(ret);
                }
                break;
            case Value::Type::DOUBLE:
                {
                    auto ret = PyObject_CallMethod(m_pyInstance, "onChangedValue", "(d)", value.asDouble());
                    Py_XDECREF(ret);
                }
                break;
            case Value::Type::BOOLEAN:
                {
                    auto ret = PyObject_CallMethod(m_pyInstance, "onChangedValue", "(i)", value.asBool());
                    Py_XDECREF(ret);
                }
                break;
            case Value::Type::STRING:
                {
                    auto ret = PyObject_CallMethod(m_pyInstance, "onChangedValue", "(s)", value.asString().c_str());
                    Py_XDECREF(ret);
                }
                break;
            }
        }*/

        Invoke("onChangedValue", value);
    }

    //! Member value changed
    void ScriptComponent::onMemberValueChanged(const std::string& key, json value)
    {
        // Change saved value
        m_members[key] = value;

        // Apply changes to script instance
        if (m_pyInstance)
        {
            PyObject* pyValue = Py_None;

            switch (value.type()) 
            {
            case json::value_t::number_integer:
            case json::value_t::number_unsigned:
                {
                    pyValue = PyLong_FromLong(value.get<int>());
                }
                break;

            case json::value_t::number_float:
                {
                    pyValue = PyFloat_FromDouble(value.get<double>());
                }
                break;

            case json::value_t::boolean:
                {
                    pyValue = PyBool_FromLong(value.get<bool>());
                }
                break;

            case json::value_t::string:
                {
                    auto jVal = json::parse(value.get<std::string>(), 0, false);
                    auto uuid = jVal.is_null() || jVal.is_discarded() ? "" : jVal.value("uuid", "");
                    auto obj = getOwner()->getScene()->findObjectByUUID(uuid);
                    if (obj) 
                    {
                        auto compName = jVal.is_null() || jVal.is_discarded() ? "" : jVal.value("comp", "");
                        if (compName.empty() || compName.compare("") == 0 || compName.compare("SceneObject") == 0)
                        {
                            auto pyObj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
                            pyObj->sceneObject = obj.get();
                            pyValue = (PyObject*)pyObj;
                        }
                        else
                        {
                            pyValue = pySceneObject_getComponent(obj.get(), compName);
                        }
                    }
                    else
                    {
                        pyValue = PyUnicode_FromString(value.get<std::string>().c_str());
                    }
                }
                break;
            }

            PyObject_SetAttrString(m_pyInstance, key.c_str(), pyValue);
            Py_XDECREF(pyValue);
        }
    }

    //! Trigger events
    void ScriptComponent::onTriggerStart(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onTriggerStart"))
        {
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = &other;

            auto ret = PyObject_CallMethod(m_pyInstance, "onTriggerStart", "(O)", obj);
            Py_XDECREF(ret);
        }
    }

    void ScriptComponent::onTriggerStay(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onTriggerStay"))
        {
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = &other;

            auto ret = PyObject_CallMethod(m_pyInstance, "onTriggerStay", "(O)", obj);
            Py_XDECREF(ret);
        }
    }

    void ScriptComponent::onTriggerStop(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onTriggerStop"))
        {
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = &other;

            auto ret = PyObject_CallMethod(m_pyInstance, "onTriggerStop", "(O)", obj);
            Py_XDECREF(ret);
        }
    }

    //! Collision events
    void ScriptComponent::onCollisionStart(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onCollisionStart"))
        {
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = &other;

            auto ret = PyObject_CallMethod(m_pyInstance, "onCollisionStart", "(O)", obj);
            Py_XDECREF(ret);
        }
    }

    void ScriptComponent::onCollisionStay(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onCollisionStay"))
        {
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = &other;

            auto ret = PyObject_CallMethod(m_pyInstance, "onCollisionStay", "(O)", obj);
            Py_XDECREF(ret);
        }
    }

    void ScriptComponent::onCollisionStop(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onCollisionStop"))
        {
            auto obj = PyObject_New(PyObject_SceneObject, &PyTypeObject_SceneObject);
            obj->sceneObject = &other;

            auto ret = PyObject_CallMethod(m_pyInstance, "onCollisionStop", "(O)", obj);
            Py_XDECREF(ret);
        }
    }

    void ScriptComponent::Invoke(const std::string& functionName, const Value& value)
    {
        auto m_fncName = functionName.c_str();
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, m_fncName))
        {
            switch (value.getType()) {
            case Value::Type::NONE:
            default:
            {
                auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, nullptr);
                Py_XDECREF(ret);
            }
            break;
            case Value::Type::BYTE:
            {
                auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, "(b)", value.asByte());
                Py_XDECREF(ret);
            }
            break;
            case Value::Type::INTEGER:
            {
                auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, "(i)", value.asInt());
                Py_XDECREF(ret);
            }
            break;
            case Value::Type::UNSIGNED:
            {
                auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, "(I)", value.asUnsignedInt());
                Py_XDECREF(ret);
            }
            break;
            case Value::Type::FLOAT:
            {
                auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, "(f)", value.asFloat());
                Py_XDECREF(ret);
            }
            break;
            case Value::Type::DOUBLE:
            {
                auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, "(d)", value.asDouble());
                Py_XDECREF(ret);
            }
            break;
            case Value::Type::BOOLEAN:
            {
                auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, "(i)", value.asBool());
                Py_XDECREF(ret);
            }
            break;
            case Value::Type::STRING:
            {
                auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, "(s)", value.asString().c_str());
                Py_XDECREF(ret);
            }
            break;
            case Value::Type::VECTOR:
            {
                auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, "(O)", parseObject(value));
                Py_XDECREF(ret);
            }
            break;
            }
        }
    }

    void ScriptComponent::Invoke(const std::string& functionName, void* pyObj)
    {
        auto m_fncName = functionName.c_str();
        PyObject* m_pyObj = static_cast<PyObject*>(pyObj);
        PyTypeObject* type = m_pyObj->ob_type;
        std::string m_type(type->tp_name);
        if (m_type.compare("int") == 0) {
            Invoke(functionName, Value((int)PyLong_AsLong(m_pyObj)));
        }
        else if (m_type.compare("float") == 0) {
            Invoke(functionName, Value((float)PyFloat_AsDouble(m_pyObj)));
        }
        else if (m_type.compare("string") == 0) {
            Invoke(functionName, Value(PyUnicode_AsUTF8(m_pyObj)));
        }
        else {
            //pyxie_printf("Recv Type %s\n", type->tp_name);
            auto ret = PyObject_CallMethod(m_pyInstance, m_fncName, "(O)", m_pyObj);
            Py_XDECREF(ret);
        }
    }

    //! Serialize
    void ScriptComponent::to_json(json &j) const
    {
        Component::to_json(j);
        j["path"] = m_path;
        j["members"] = m_members;
    }

    //! Deserialize
    void ScriptComponent::from_json(const json &j)
    {
        setPath(j.at("path"));

        auto jMembers = j.value("members", json::array());
        m_members = jMembers.get<std::unordered_map<std::string, json>>();
        Component::from_json(j);
    }

    //! Serialize finished event
    void ScriptComponent::onSerializeFinished(Scene* scene)
    {
        // Check and set object UUID to object reference once load done
        for (const auto& pair : m_members)
        {
            auto key = pair.first;
            auto value = pair.second;

            if (value.type() == json::value_t::string)
            {
                onMemberValueChanged(key, value);
            }
        }
        Component::onSerializeFinished(scene);
    }

    //! Path
    void ScriptComponent::setPath(const std::string &path, bool forceReload)
    {
        auto scriptName = fs::path(path).string();
        std::replace(scriptName.begin(), scriptName.end(), '\\', '/');

        if (forceReload || strcmp(m_path.c_str(), scriptName.c_str()) != 0)
        {
            m_path = scriptName;
        }
    }


    //! Update property by key value
    void ScriptComponent::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("path") == 0)
        {
            setPath(val, true);
        }
        else if (key.compare("members") == 0)
        {
            for (const auto& [key, val] : val.items())
            {
                if (m_members.find(key) != m_members.end())
                {
                    onMemberValueChanged(key, val);
                }
            }
        }
    }
} // namespace ige::scene
