#include "components/RuntimeComponent.h"
#include "scene/SceneManager.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "event/EventContext.h"

namespace ige::scene
{
    //! Constructor
    RuntimeComponent::RuntimeComponent(SceneObject& owner)
        : Component(owner)
    {
        m_bIsInit = false;
    }

    //! Destructor
    RuntimeComponent::~RuntimeComponent()
    {
        //! Clear 
        Clear();
    }

    //! Update functions
    void RuntimeComponent::onUpdate(float dt) {
        if (!SceneManager::getInstance()->isPlaying()) return;
        if (!m_bIsInit) Initialize();
        onRuntimeUpdate(dt);
    }

    void RuntimeComponent::onFixedUpdate(float dt) {
        if (!isInitialized()) return;
        onRuntimeFixedUpdate(dt);
    }

    void RuntimeComponent::onLateUpdate(float dt) {
        if (!isInitialized()) return;
        onRuntimeLateUpdate(dt);
    }

    void RuntimeComponent::Initialize()
    {
        m_bIsInit = true;
    }

    void RuntimeComponent::Clear()
    {
        m_bIsInit = false;
    }
} // namespace ige::scene