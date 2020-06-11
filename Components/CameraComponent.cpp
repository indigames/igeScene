#include "CameraComponent.h"
#include "TransformComponent.h"
#include "SceneObject.h"

namespace ige::scene
{
    //! Constructor
    CameraComponent::CameraComponent(std::shared_ptr<SceneObject> owner, const std::string& name)
        : Component(owner)
    {
        m_camera = std::make_shared<Camera>(name.c_str());
    }

    //! Destructor
    CameraComponent::~CameraComponent() 
    {
        m_camera = nullptr;
    }
    
    //! Update
    void CameraComponent::onUpdate(float dt)
    {
        if (m_camera == nullptr) return;

        std::shared_ptr<TransformComponent> transCmp = nullptr;

        if(getOwner() != nullptr)
        {
            // Update transform from transform component
            transCmp = getOwner()->getComponent<TransformComponent>();
            if(transCmp != nullptr)
            {
                m_camera->SetPosition(transCmp->getPosition());
                m_camera->SetRotation(transCmp->getRotation());
            }
        }

        // Update        
        m_camera->Step(dt);        

        if(transCmp != nullptr)
        {
            // Sync transform to transform component
            transCmp->setPosition(m_camera->GetPosition());
            transCmp->setRotation(m_camera->GetRotation());            
        }

        // Render
        m_camera->Render();
    }

    //! Serialize
    void CameraComponent::to_json(json& j, const Component& obj)
    {

    }

    //! Deserialize
    void CameraComponent::from_json(const json& j, Component& obj)
    {

    }
}
