#pragma once

#include <Python.h>

#include "components/Component.h"
#include "event/Event.h"

namespace ige::scene
{
    //! ScriptComponent
    class ScriptComponent : public Component
    {
    public:
        //! Constructor
        ScriptComponent(std::shared_ptr<SceneObject> owner, const std::string& path = "");

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
        void to_json(json& j, const Component& obj);

        //! Deserialize 
        void from_json(const json& j, Component& obj);

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
        PyObject* m_pModule;
    };
}
