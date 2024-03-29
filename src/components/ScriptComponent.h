#pragma once

#include "components/RuntimeComponent.h"
#include "event/Event.h"
#include "core/Value.h"

//! PyObject declaration
struct _object;
namespace ige::scene
{
    class EventContext;
    //! ScriptComponent
    class ScriptComponent : public RuntimeComponent
    {
    public:
        //! Constructor
        ScriptComponent(SceneObject& owner, const std::string &path = "");

        //! Destructor
        virtual ~ScriptComponent();

        //! Get component name
        virtual std::string getName() const override { return "Script"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Script; }

        //! Allow multiple scripts attached to one object
        virtual bool canMultiInstance() override { return true; }

        //! Check and clear error
        void PyErr_CheckAndClear();

        //! Reload script
        void Reload();

        //! Active when start runtime
        virtual void Initialize() override;

        //! Active when stop runtime
        virtual void Clear() override;

        //! Awake
        virtual void onAwake();

        //! Start
        virtual void onStart();

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Update functions
        virtual void onRuntimeUpdate(float dt) override;
        virtual void onRuntimeFixedUpdate(float dt) override;
        virtual void onRuntimeLateUpdate(float dt) override;

        //! Render
        virtual void onRender() override;
        virtual void onRenderUI() override;

        //! Destroyed
        virtual void onDestroy() override;

        //! Click
        virtual void onClick() override;
        virtual void onTouchBegin();
        virtual void onTouchEnd();

        //! Suspend
        virtual void onSuspend() override;

        //! Resume
        virtual void onResume() override;

        //! Change Value
        virtual void onChangedValue(Value value);

        //! Trigger events
        virtual void onTriggerStart(SceneObject &other);
        virtual void onTriggerStay(SceneObject &other);
        virtual void onTriggerStop(SceneObject &other);

        //! Collision events
        virtual void onCollisionStart(SceneObject &other);
        virtual void onCollisionStay(SceneObject &other);
        virtual void onCollisionStop(SceneObject &other);

        //! Particle events
        virtual void onParticleFrameEnded();

        //! Path
        void setPath(const std::string &path, bool forceReload = false);
        const std::string &getPath() const { return m_path; }

        //! Class
        const std::string& getClassName() const { return m_className; }

        //! Script changed event
        Event<const std::string &> &getOnScriptChangedEvent() { return m_onScriptChangedEvent; }

        //! Access to members
        const std::unordered_map<std::string, json>& getMembers() { return m_members; }

        //! Member value changed
        virtual void onMemberValueChanged(const std::string& key, json value);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

        //! Invoke Function 
        virtual _object* Invoke(const std::string &functionName, const Value& value);
        virtual _object* Invoke(const std::string& functionName, _object* pyObj);

        //! PyObject: instance of Python class
        _object* getPyInstance() { return m_pyInstance; }

        //! Serialize finished event
        virtual void onSerializeFinished() override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Load/unload modules
        void loadPyModule();
        void unloadPyModule();

        //! Register/unregister events
        void registerEvents();
        void unregisterEvents();

        void onClickEvent(EventContext* context);
        void onTouchBeginEvent(EventContext* context);
        void onTouchEndEvent(EventContext* context);

        void onChangedValueEvent(EventContext* context);

    protected:

        //! Path to figure file
        std::string m_path;

        //! Cache the class name
        std::string m_className;

        //! Flags for onAwake() event
        bool m_bOnAwakeCalled = false;

        //! Flags for onStart() event
        bool m_bOnStartCalled = false;

        //! Events
        Event<const std::string &> m_onScriptChangedEvent;

        //! PyObject: module
        _object *m_pyModule;

        //! PyObject: instance of Python class
        _object *m_pyInstance;

        //! Python class members
        std::unordered_map<std::string, json> m_members;
    };
} // namespace ige::scene
