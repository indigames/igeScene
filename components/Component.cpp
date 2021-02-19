#include "components/Component.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    std::atomic<uint64_t> Component::s_instanceID;

    //! Constructor
    Component::Component(SceneObject &owner)
        : m_owner(owner) , m_instanceId(++s_instanceID)
    {
        m_serializeEventId = getOwner()->getScene()->getSerializeFinishedEvent().addListener(std::bind(&Component::onSerializeFinished, this, std::placeholders::_1));
    }

    //! Destructor
    Component::~Component()
    {
        getOwner()->getScene()->getSerializeFinishedEvent().removeListener(m_serializeEventId);

        if (getOwner()->isActive())
        {
            onDisable();
            onDestroy();
        }
    }

    void Component::setEnabled(bool enable)
    {
        m_bIsEnabled = enable;

        // Invoke enable/disable events
        if(m_bIsEnabled)
            onEnable();
        else
            onDisable();
    }

    //! Enable
    void Component::onEnable() {}

    //! Disable
    void Component::onDisable() {}

    //! Update functions
    void Component::onUpdate(float dt) {}

    void Component::onFixedUpdate(float dt) {}

    void Component::onLateUpdate(float dt) {}

    //! Render
    void Component::onRender() {}

    //! Destroyed
    void Component::onDestroy() {}

    //! Click
    void Component::onClick() {}

    //! Suspend
    void Component::onSuspend() {}

    //! Resume
    void Component::onResume() {}

    //! Serialize
    void Component::to_json(json &j) const
    {
        j = json{
            {"enabled", isEnabled()},
        };
    }

    //! Deserialize
    void Component::from_json(const json &j)
    {
        setEnabled(j.value("enabled", true));
    }

    //! Serialize component
    void to_json(json &j, const Component &obj)
    {
        if (!obj.isSkipSerialize())
            obj.to_json(j);
    }

    //! Deserialize component
    void from_json(const json &j, Component &obj)
    {
        obj.from_json(j);
    }
} // namespace ige::scene