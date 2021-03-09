#include "components/LightComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{
    //! Constructor
    LightComponent::LightComponent(SceneObject& owner)
        : Component(owner)
    {
        owner.setLockedFrameAABB(true);
    }

    //! Destructor
    LightComponent::~LightComponent()
    {
    }

} // namespace ige::scene