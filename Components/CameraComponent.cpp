#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    CameraComponent::CameraComponent(std::shared_ptr<SceneObject> owner, const std::string& name, Figure* parentFigure)
        : Component(owner)
    {
        m_camera = ResourceCreator::Instance().NewCamera(name.c_str(), parentFigure);
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
    }

    //! Render
    void CameraComponent::onRender()
    {
        if (m_camera == nullptr) return;

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
