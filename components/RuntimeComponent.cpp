#include "components/RuntimeComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"
#include "event/EventContext.h"

namespace ige::scene
{
    //! Constructor
    RuntimeComponent::RuntimeComponent(SceneObject &owner)
        : Component(owner)
    {
        m_bIsInit = false;
#if EDITOR_MODE
        getOwner()->addEventListener((int)EventType::RunEditor, std::bind(&RuntimeComponent::onRunEditor, this, std::placeholders::_1), m_instanceId);
        getOwner()->addEventListener((int)EventType::PauseEditor, std::bind(&RuntimeComponent::onPauseEditor, this, std::placeholders::_1), m_instanceId);
        getOwner()->addEventListener((int)EventType::StopEditor, std::bind(&RuntimeComponent::onStopEditor, this, std::placeholders::_1), m_instanceId);
#else
        Initialize();
#endif
    }

    //! Destructor
    RuntimeComponent::~RuntimeComponent()
    {
        //! Clear 
        Clear();

#if EDITOR_MODE
        getOwner()->removeEventListener((int)EventType::RunEditor, m_instanceId);
        getOwner()->removeEventListener((int)EventType::PauseEditor, m_instanceId);
        getOwner()->removeEventListener((int)EventType::StopEditor, m_instanceId);
#endif
    }

    //! Enable
    void RuntimeComponent::onEnable() {}

    //! Disable
    void RuntimeComponent::onDisable() {}

    //! Update functions
    void RuntimeComponent::onUpdate(float dt) {}

    void RuntimeComponent::onFixedUpdate(float dt) {}

    void RuntimeComponent::onLateUpdate(float dt) {}

    //! Render
    void RuntimeComponent::onRender() {}

    //! Destroyed
    void RuntimeComponent::onDestroy() {}

    //! Click
    void RuntimeComponent::onClick() {}

    //! Suspend
    void RuntimeComponent::onSuspend() {}

    //! Resume
    void RuntimeComponent::onResume() {}

    void RuntimeComponent::Initialize()
    {
        m_bIsInit = true;
    }

    void RuntimeComponent::Clear()
    {
        m_bIsInit = false;
    }

    void RuntimeComponent::onRunEditor(EventContext* context)
    {
        this->Initialize();
    }

    void RuntimeComponent::onPauseEditor(EventContext* context)
    {
        
    }
    
    void RuntimeComponent::onStopEditor(EventContext* context)
    {
        this->Clear();
    }


} // namespace ige::scene