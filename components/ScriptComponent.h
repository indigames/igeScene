#pragma once

#include "components/Component.h"
#include "event/Event.h"

//! PyObject declaration
struct _object;

namespace ige::scene
{
    class EventContext;
    //! ScriptComponent
    class ScriptComponent : public Component
    {
    public:
        //! Constructor
        ScriptComponent(SceneObject &owner, const std::string &path = "");

        //! Destructor
        virtual ~ScriptComponent();

        //! Get component name
        virtual std::string getName() const override { return "ScriptComponent"; }

        //! Awake
        virtual void onAwake();

        //! Start
        virtual void onStart();

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Update functions
        virtual void onUpdate(float dt) override;
        virtual void onFixedUpdate(float dt) override;
        virtual void onLateUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Destroyed
        virtual void onDestroy() override;

        //! Click
        virtual void onClick() override;

        //! Suspend
        virtual void onSuspend() override;

        //! Resume
        virtual void onResume() override;

        //! Trigger events
        virtual void onTriggerStart(SceneObject &other);
        virtual void onTriggerStay(SceneObject &other);
        virtual void onTriggerStop(SceneObject &other);

        //! Collision events
        virtual void onCollisionStart(SceneObject &other);
        virtual void onCollisionStay(SceneObject &other);
        virtual void onCollisionStop(SceneObject &other);

        //! Path
        void setPath(const std::string &path);
        const std::string &getPath() { return m_path; }

        //! Script changed event
        Event<const std::string &> &getOnScriptChangedEvent() { return m_onScriptChangedEvent; }

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        void loadPyModule();
        void unloadPyModule();

        void registerPhysicEvents();
        void unregisterPhysicEvents();

        void onClickEvent(EventContext* context);

    protected:

        //! Path to figure file
        std::string m_path;
        bool m_bPathDirty = false;

        //! Flags for onStart() event
        bool m_bOnStartCalled = false;

        //! Events
        Event<const std::string &> m_onScriptChangedEvent;

        //! PyObject: module
        _object *m_pyModule;

        //! PyObject: instance of Python class
        _object *m_pyInstance;
    };
} // namespace ige::scene
