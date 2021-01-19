#include "components/LightComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    LightComponent::LightComponent(SceneObject& owner)
        : Component(owner)
    {
        auto transform = owner.getComponent<TransformComponent>();
        if (transform) {
            transform->m_bLockFrameAABB = true;
        }
    }

    //! Destructor
    LightComponent::~LightComponent()
    {
    }

} // namespace ige::scene