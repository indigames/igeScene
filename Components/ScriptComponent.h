#pragma once

#include "components/Component.h"
#include "event/Event.h"

//! PyObject declaration
struct _object;

namespace ige::scene
{
    //! ScriptComponent
    class ScriptComponent : public Component
    {
    public:
        //! Constructor
        ScriptComponent(const std::shared_ptr<SceneObject>& owner, const std::string& path = "");

        //! Destructor
        virtual ~ScriptComponent();

        //! Get component name
        virtual std::string getName() const override { return "ScriptComponent"; }

        //! Awake
        virtual void onAwake();

        //! Start
        virtual void onStart();

        //! Enable
        virtual void onEnable();

        //! Disable
        virtual void onDisable();

        //! Update functions
        virtual void onUpdate(float dt);
        virtual void onFixedUpdate(float dt);
        virtual void onLateUpdate(float dt);

        //! Render
        virtual void onRender();

        //! Destroyed
        virtual void onDestroy();

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize 
        virtual void from_json(const json& j);

        //! Path
        void setPath(const std::string& path);
        const std::string& getPath() { return m_path; }

        //! Script changed event
        Event<const std::string&>& getOnScriptChangedEvent() { return m_onScriptChangedEvent; }

    protected:
        void loadPyModule();
        void unloadPyModule();

        //! Path to figure file
        std::string m_path;
        bool m_bPathDirty = false;

        //! Events
        Event<const std::string&> m_onScriptChangedEvent;

        //! PyObject module
        _object* m_pModule;
    };
}
