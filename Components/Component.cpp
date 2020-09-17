#include "components/Component.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    Component::Component(const std::shared_ptr<SceneObject>& owner): m_owner(owner.get()) {}
    
    //! Destructor
    Component::~Component() 
    {
        if (m_owner && m_owner->isActive())
        {
            onDisable();
            onDestroy();
        }
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
    void to_json(json& j, const Component& obj)
    {
        obj.to_json(j);
    }

    //! Deserialize 
    void from_json(const json& j, Component& obj)
    {
        obj.from_json(j);
    }
}