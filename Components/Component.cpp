#include "components/Component.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    Component::Component(std::shared_ptr<SceneObject> owner): m_owner(owner) {}
    
    //! Destructor
    Component::~Component() 
    {
        auto owner = m_owner.lock();
        if (owner && owner->isActive())
        {
            onDisable();
            onDestroy();
        }
    }
    
    //! Awake
    void Component::onAwake() {}

    //! Start
    void Component::onStart() {}

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