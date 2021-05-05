#pragma once

#include <memory>
#include <vector>
#include <string>

#include "utils/Serialize.h"

namespace ige::scene
{
    class Scene;
    class SceneObject;

    /**
    * Component is the base class of any component
    */
    class Component
    {
    public:
        //! Constructor
        Component(SceneObject &owner);

        //! Destructor
        virtual ~Component();

        //! Get owner object
        virtual SceneObject *getOwner() const { return &m_owner; }

        //! Returns the name of the component
        virtual std::string getName() const = 0;

        //! Enable/Disable
        bool isEnabled() const { return m_bIsEnabled; }
        virtual void setEnabled(bool enable = true);

        //! Skip serialize
        bool isSkipSerialize() const { return m_bSkipSerialize; }
        void setSkipSerialize(bool skip = true) { m_bSkipSerialize = skip; }

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

        //! Click
        virtual void onClick();

        //! Suspend
        virtual void onSuspend();

        //! Resume
        virtual void onResume();

        //! Serialize
        friend void to_json(json &j, const Component &obj);

        //! Deserialize
        friend void from_json(const json &j, Component &obj);

        uint64_t getInstanceId() { return m_instanceId; }

    protected:
        //! Serialize
        virtual void to_json(json &j) const;

        //! Deserialize
        virtual void from_json(const json &j);

        //! Serialize finished event
        virtual void onSerializeFinished(Scene* scene);

        //! Reference to owner object
        SceneObject &m_owner;

        //! Enable/Disable
        bool m_bIsEnabled = true;

        //! Skip serialize (editor properties)
        bool m_bSkipSerialize = false;

        //! Cache serialize event id
        uint64_t m_serializeEventId = (uint64_t)(-1);
        
        uint64_t m_instanceId;
        static std::atomic<uint64_t> s_instanceID;
    };

    //! Serialize
    void to_json(json &j, const Component &obj);

    //! Deserialize
    void from_json(const json &j, Component &obj);
} // namespace ige::scene
