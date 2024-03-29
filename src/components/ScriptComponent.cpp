#include <Python.h>
#include <algorithm>

#include <pythonResource.h>
#include <pyVectorMath.h>

#include "components/ScriptComponent.h"
#include "components/physic/Rigidbody.h"
#include "components/particle/Particle.h"

#include "python/pySceneObject.h"
#include "python/pyScript.h"

#include "scene/SceneManager.h"
#include "scene/Scene.h"

#include "event/EventContext.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include "utils/PyxieHeaders.h"
using namespace pyxie;


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
    ScriptComponent::ScriptComponent(SceneObject& owner, const std::string &path)
        : RuntimeComponent(owner), m_path(path), m_pyModule(nullptr), m_pyInstance(nullptr)
    {
        loadPyModule();
    }

    //! Destructor
    ScriptComponent::~ScriptComponent()
    {
        Clear();
        unloadPyModule();
        m_members.clear();
    }

    void ScriptComponent::PyErr_CheckAndClear() {
        if (PyErr_Occurred()) {
            PyErr_PrintEx(1);
            PyRun_SimpleString("import traceback\ntraceback.print_exception(sys.last_type, sys.last_value, sys.last_traceback)");
            PyErr_Clear();
        }
    }

    void ScriptComponent::Reload()
    {
        bool isLoad = isInitialized();
        Clear();
        unloadPyModule();
        loadPyModule();
        if (isLoad)
            Initialize();
    }

    void ScriptComponent::Initialize()
    {
        RuntimeComponent::Initialize();
        getOwner()->addEventListener((int)EventType::Click, std::bind(&ScriptComponent::onClickEvent, this, std::placeholders::_1), m_instanceId);
        getOwner()->addEventListener((int)EventType::TouchBegin, std::bind(&ScriptComponent::onTouchBeginEvent, this, std::placeholders::_1), m_instanceId);
        getOwner()->addEventListener((int)EventType::TouchEnd, std::bind(&ScriptComponent::onTouchEndEvent, this, std::placeholders::_1), m_instanceId);
        getOwner()->addEventListener((int)EventType::Changed, std::bind(&ScriptComponent::onChangedValueEvent, this, std::placeholders::_1), m_instanceId);
    }

    void ScriptComponent::Clear()
    {
        RuntimeComponent::Clear();
        getOwner()->removeEventListener((int)EventType::Click, m_instanceId);
        getOwner()->removeEventListener((int)EventType::TouchBegin, m_instanceId);
        getOwner()->removeEventListener((int)EventType::TouchEnd, m_instanceId);
        getOwner()->removeEventListener((int)EventType::Changed, m_instanceId);
    }

    void ScriptComponent::onClickEvent(EventContext* context)
    {
        onClick();
    }

    void ScriptComponent::onTouchBeginEvent(EventContext* context)
    {
        onTouchBegin();
    }

    void ScriptComponent::onTouchEndEvent(EventContext* context)
    {
        onTouchEnd();
    }

    void ScriptComponent::onChangedValueEvent(EventContext* context)
    {
        onChangedValue(context->getDataValue());
    }

    void ScriptComponent::loadPyModule()
    {
        unloadPyModule();

        if (!m_path.empty())
        {
            // Load the module from python source file
            auto path = fs::relative(fs::path(m_path));
            auto appendCmd = std::string("import sys\nsys.path.append('") + path.parent_path().string() + "')";
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
                // Check for script error.
                if (PyErr_Occurred()) {
                    PyObject* ptype, *pvalue, *ptraceback;
                    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
                    auto pStrErrorMessage = PyUnicode_AsUTF8(PyObject_Str(pvalue));
                    if (pStrErrorMessage != nullptr)
                        pyxie_printf("[PYTHON] ERROR in '%s': %s", getPath().c_str(), pStrErrorMessage);
                    PyErr_CheckAndClear();
                }
                return;
            }

            // fetch the module's dictionary
            auto dict = PyModule_GetDict(m_pyModule);
            if (dict == nullptr)
            {
                PyErr_CheckAndClear();
                return;
            }

            // Builds the name of a callable class
            PyObject *key = nullptr, *value = nullptr, *pyClass = nullptr;
            Py_ssize_t pos = 0;
            while (PyDict_Next(dict, &pos, &key, &value))
            {   
                if ((PyObject_HasAttrString(value, "onUpdate") || PyObject_HasAttrString(value, "onStart")) 
                    && PyObject_IsSubclass(value, (PyObject *)&PyTypeObject_Script))
                {
                    pyClass = value;
                    m_className = std::string(PyUnicode_AsUTF8(PyObject_GetAttrString(value, "__name__")));
                    break;
                }
            }

            if (pyClass == nullptr)
            {
                PyErr_CheckAndClear();
                return;
            }

            // Creates an instance of the class
            auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            auto owner = getOwner()->getSharedPtr();
            obj->sceneObject = std::weak_ptr<SceneObject>(owner);
            auto arglist = Py_BuildValue("(O)", obj);
            PyObject *pyConstruct = PyInstanceMethod_New(pyClass);
            m_pyInstance = PyObject_CallObject(pyConstruct, arglist);
            if (m_pyInstance == nullptr) {
                PyErr_CheckAndClear();
                return;
            }
            Py_DECREF(arglist);
            Py_DECREF(pyConstruct);

            key = nullptr;
            value = nullptr;
            pos = 0;
            dict = PyObject_GenericGetDict(m_pyInstance, nullptr);
            std::unordered_map<std::string, json> members;
            while (PyDict_Next(dict, &pos, &key, &value))
            {
                auto key_str = std::string(PyUnicode_AsUTF8(key));
                if (key_str.size() > 0 && key_str[0] != '_' 
                    && !PyObject_IsInstance(value, (PyObject*)&PyFunction_Type)
                    && !PyObject_IsInstance(value, (PyObject*)&PyModule_Type)) {
                    if (m_members.count(key_str) > 0) {
                        members[key_str] = m_members[key_str];
                    }
                    else {
                        if (PyUnicode_Check(value)) {
                            members[key_str] = std::string(PyUnicode_AsUTF8(value));
                        }
                        else if (PyBool_Check(value)) {
                            members[key_str] = (bool)(PyLong_AsLong(value));
                        }
                        else if (PyLong_Check(value)) {
                            members[key_str] = (int)PyLong_AsLong(value);
                        }
                        else if (PyFloat_Check(value)) {
                            members[key_str] = PyFloat_AsDouble(value);
                        }
                        else if (value->ob_type == _Vec2Type || value->ob_type == _Vec3Type || value->ob_type == _Vec4Type || value->ob_type == _QuatType) {
                            int d;
                            float buff[4];
                            float* v = pyObjToFloat((PyObject*)value, buff, d);
                            if (v != nullptr) {
                                if (d == 2) members[key_str] = *((Vec2*)v);
                                else if (d == 3) members[key_str] = *((Vec3*)v);
                                else if (d > 3) members[key_str] = *((Vec4*)v);
                            }
                        }
                        else {
                            members[key_str] = std::string();
                        }
                    }
                }
            }
            m_members.clear();
            m_members = members;

            // Enable call onAwake() next frame
            m_bOnAwakeCalled = false;
        }        
    }

    //! Unload PyModule
    void ScriptComponent::unloadPyModule()
    {
        m_pyInstance = nullptr;
        m_className.clear();

        if (m_pyModule != nullptr)
        {
            Py_XDECREF(m_pyModule);
            m_pyModule = nullptr;
        }

        unregisterEvents();
    }

    //! Register physic events
    void ScriptComponent::registerEvents()
    {
        if (getOwner() == nullptr)
            return;

        auto physicComp = getOwner()->getComponent<Rigidbody>();
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

        auto particleComp = getOwner()->getComponent<Particle>();
        if (particleComp) {
            particleComp->getFrameEndedEvent().addListener([this]() {
                onParticleFrameEnded();
            });
        }
    }

    //! Unregister physic events
    void ScriptComponent::unregisterEvents()
    {
        if (getOwner() == nullptr)
            return;

        auto physicComp = getOwner()->getComponent<Rigidbody>();
        if (physicComp != nullptr)
        {
            physicComp->getTriggerStartEvent().removeAllListeners();
            physicComp->getTriggerStayEvent().removeAllListeners();
            physicComp->getTriggerStopEvent().removeAllListeners();

            physicComp->getCollisionStartEvent().removeAllListeners();
            physicComp->getCollisionStayEvent().removeAllListeners();
            physicComp->getCollisionStayEvent().removeAllListeners();
        }

        auto particleComp = getOwner()->getComponent<Particle>();
        if (particleComp) {
            particleComp->getFrameEndedEvent().removeAllListeners();
        }
    }

    //! Awake
    void ScriptComponent::onAwake()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onAwake"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onAwake", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Start
    void ScriptComponent::onStart()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onStart"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onStart", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Enable
    void ScriptComponent::onEnable()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onEnable"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onEnable", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Disable
    void ScriptComponent::onDisable()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onEnable"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onDisable", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Update functions
    void ScriptComponent::onRuntimeUpdate(float dt)
    {
        // Check for script error.
        PyErr_CheckAndClear();

        if (!m_bOnAwakeCalled)
        {
            // Register physic events
            registerEvents();

            // Initialized, call invoke onAwake()
            onAwake();

            m_bOnAwakeCalled = true;
            m_bOnStartCalled = false;
            return;
        }

        // Check call onStart()
        if (!m_bOnStartCalled)
        {
            onStart();
            m_bOnStartCalled = true;
            return;
        }

        // Check call onUpdate()
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onUpdate", "(f)", dt);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    void ScriptComponent::onRuntimeFixedUpdate(float dt)
    {
        if (!isInitialized()) return;
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onFixedUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onFixedUpdate", "(f)", dt);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    void ScriptComponent::onRuntimeLateUpdate(float dt)
    {
        if (!isInitialized()) return;
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onLateUpdate"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onLateUpdate", "(f)", dt);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Render
    void ScriptComponent::onRender()
    {
        if (!isInitialized()) return;
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onRender"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onRender", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Render
    void ScriptComponent::onRenderUI()
    {
        if (!isInitialized()) return;
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onRenderUI"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onRenderUI", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Destroyed
    void ScriptComponent::onDestroy()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onDestroy"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onDestroy", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Click
    void ScriptComponent::onClick()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onClick"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onClick", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    void ScriptComponent::onTouchBegin()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onTouchBegin"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onTouchBegin", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    void ScriptComponent::onTouchEnd()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onTouchEnd"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onTouchEnd", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Suspend
    void ScriptComponent::onSuspend()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onSuspend"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onSuspend", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Resume
    void ScriptComponent::onResume()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onResume"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onResume", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Change Value
    void ScriptComponent::onChangedValue(Value value)
    {
        Invoke("onValueChanged", value);
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

            case json::value_t::array:
                {
                    if (value.size() == 2) {
                    #ifdef __cpp_exceptions
                        try {
                    #endif
                            Vec2 vec;
                            value.get_to<Vec2>(vec);
                            auto vecObj = PyObject_New(vec_obj, _Vec2Type);
                            if (vecObj) {
                                vmath_cpy(vec.P(), 2, vecObj->v);
                                vecObj->d = 2;
                                pyValue = (PyObject*)vecObj;
                            }
                    #ifdef __cpp_exceptions
                        }
                        catch (std::exception ex) {}
                    #endif
                    }
                    else if (value.size() == 3) {
                    #ifdef __cpp_exceptions
                        try {
                    #endif
                            Vec3 vec;
                            value.get_to<Vec3>(vec);
                            auto vecObj = PyObject_New(vec_obj, _Vec3Type);
                            if (vecObj) {
                                vmath_cpy(vec.P(), 3, vecObj->v);
                                vecObj->d = 3;
                                pyValue = (PyObject*)vecObj;
                            }
                    #ifdef __cpp_exceptions
                        }
                        catch (std::exception ex) {}
                    #endif
                    }
                    else if (value.size() > 3) {
                    #ifdef __cpp_exceptions
                        try {
                    #endif
                            Vec4 vec;
                            value.get_to<Vec4>(vec);
                            auto vecObj = PyObject_New(vec_obj, _Vec4Type);
                            if (vecObj) {
                                vmath_cpy(vec.P(), 4, vecObj->v);
                                vecObj->d = 4;
                                pyValue = (PyObject*)vecObj;
                            }
                    #ifdef __cpp_exceptions
                        }
                        catch (std::exception ex) {}
                    #endif
                    }
                }
                break;

            case json::value_t::string:
            case json::value_t::object:
                {
                    if (value.type() == json::value_t::string) {
                        auto str = value.get<std::string>();
                        if(!str.empty()) pyValue = PyUnicode_FromString(str.c_str());
                    }
                    else {
                        auto uuid = value.value("uuid", "");
                        auto obj = getOwner()->getScene()->findObjectByUUID(uuid);
                        if (obj) {
                            auto compName = value.value("comp", "");
                            if (compName.empty() || compName.compare("") == 0 || compName.compare("SceneObject") == 0) {
                                auto pyObj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
                                pyObj->sceneObject = obj;
                                pyValue = (PyObject*)pyObj;
                            }
                            else {
                                pyValue = pySceneObject_getComponent(obj, compName);
                            }
                        }
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
            auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            obj->sceneObject = other.getSharedPtr();

            auto ret = PyObject_CallMethod(m_pyInstance, "onTriggerStart", "(O)", obj);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    void ScriptComponent::onTriggerStay(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onTriggerStay"))
        {
            auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            obj->sceneObject = other.getSharedPtr();

            auto ret = PyObject_CallMethod(m_pyInstance, "onTriggerStay", "(O)", obj);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    void ScriptComponent::onTriggerStop(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onTriggerStop"))
        {
            auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            obj->sceneObject = other.getSharedPtr();

            auto ret = PyObject_CallMethod(m_pyInstance, "onTriggerStop", "(O)", obj);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    //! Collision events
    void ScriptComponent::onCollisionStart(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onCollisionStart"))
        {
            auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            obj->sceneObject = other.getSharedPtr();

            auto ret = PyObject_CallMethod(m_pyInstance, "onCollisionStart", "(O)", obj);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    void ScriptComponent::onCollisionStay(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onCollisionStay"))
        {
            auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            obj->sceneObject = other.getSharedPtr();

            auto ret = PyObject_CallMethod(m_pyInstance, "onCollisionStay", "(O)", obj);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    void ScriptComponent::onCollisionStop(SceneObject &other)
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onCollisionStop"))
        {
            auto obj = (PyObject_SceneObject*)(&PyTypeObject_SceneObject)->tp_alloc(&PyTypeObject_SceneObject, 0);
            obj->sceneObject = other.getSharedPtr();

            auto ret = PyObject_CallMethod(m_pyInstance, "onCollisionStop", "(O)", obj);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    void ScriptComponent::onParticleFrameEnded()
    {
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, "onParticleFrameEnded"))
        {
            auto ret = PyObject_CallMethod(m_pyInstance, "onParticleFrameEnded", NULL);
            Py_XDECREF(ret);
        }
        PyErr_CheckAndClear();
    }

    _object* ScriptComponent::Invoke(const std::string& functionName, const Value& value)
    {
        auto m_fncName = functionName.c_str();
        if (m_pyInstance && PyObject_HasAttrString(m_pyInstance, m_fncName))
        {
            switch (value.getType()) 
            {
                case Value::Type::NONE:
                default:
                {
                    return PyObject_CallMethod(m_pyInstance, m_fncName, nullptr);
                }
                break;
                case Value::Type::BYTE:
                {
                    return PyObject_CallMethod(m_pyInstance, m_fncName, "(b)", value.asByte());
                }
                break;
                case Value::Type::INTEGER:
                {
                    return PyObject_CallMethod(m_pyInstance, m_fncName, "(i)", value.asInt());
                }
                break;
                case Value::Type::UNSIGNED:
                {
                    return PyObject_CallMethod(m_pyInstance, m_fncName, "(I)", value.asUnsignedInt());
                }
                break;
                case Value::Type::FLOAT:
                {
                    return PyObject_CallMethod(m_pyInstance, m_fncName, "(f)", value.asFloat());
                }
                break;
                case Value::Type::DOUBLE:
                {
                    return PyObject_CallMethod(m_pyInstance, m_fncName, "(d)", value.asDouble());
                }
                break;
                case Value::Type::BOOLEAN:
                {
                    return PyObject_CallMethod(m_pyInstance, m_fncName, "(i)", value.asBool());
                }
                break;
                case Value::Type::STRING:
                {
                    return PyObject_CallMethod(m_pyInstance, m_fncName, "(s)", value.asString().c_str());
                }
                break;
                case Value::Type::VECTOR:
                {
                    return PyObject_CallMethod(m_pyInstance, m_fncName, "(O)", parseObject(value));
                }
                break;
            }
        }
        return nullptr;
    }

    _object* ScriptComponent::Invoke(const std::string& functionName, _object* pyObj)
    {
        auto m_fncName = functionName.c_str();
        if (pyObj == nullptr) 
        {
            return PyObject_CallMethod(m_pyInstance, m_fncName, nullptr);
        }
        else
        {
            PyTypeObject* type = pyObj->ob_type;
            std::string m_type(type->tp_name);
            if (m_type.compare("int") == 0) {
                return Invoke(functionName, Value((int)PyLong_AsLong(pyObj)));
            }
            else if (m_type.compare("float") == 0) {
                return Invoke(functionName, Value((float)PyFloat_AsDouble(pyObj)));
            }
            else if (m_type.compare("string") == 0) {
                return Invoke(functionName, Value(PyUnicode_AsUTF8(pyObj)));
            }
            else {
                return PyObject_CallMethod(m_pyInstance, m_fncName, "(O)", pyObj);
            }
        }
        return nullptr;
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
        auto jMembers = j.value("members", json::array());
        m_members = jMembers.get<std::unordered_map<std::string, json>>();

        setPath(j.at("path"), true);
        Component::from_json(j);
    }

    //! Serialize finished event
    void ScriptComponent::onSerializeFinished()
    {
        // Check and set object UUID to object reference once load done
        for (const auto& pair : m_members)
        {
            auto key = pair.first;
            auto value = pair.second;
            onMemberValueChanged(key, value);
        }
        Component::onSerializeFinished();
    }

    //! Path
    void ScriptComponent::setPath(const std::string &path, bool forceReload)
    {
        auto scriptName = fs::path(path).string();
        std::replace(scriptName.begin(), scriptName.end(), '\\', '/');

        if (forceReload || strcmp(m_path.c_str(), scriptName.c_str()) != 0)
        {
            const auto& components = getOwner()->getComponents();
            auto itr = std::find_if(components.begin(), components.end(), [scriptName](const auto& elem) {
                auto scriptComp = std::dynamic_pointer_cast<ScriptComponent>(elem);
                return (scriptComp && scriptComp->getPath().compare(scriptName) == 0);
            });

            // DON'T allow duplicated Script with the same path
            if (itr == components.end()) {
                m_path = scriptName;
                loadPyModule();
            }
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
