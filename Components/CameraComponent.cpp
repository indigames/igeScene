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
    
    //! Pan (Y-axis)
    void CameraComponent::setPan(float pan)
    {
        if (m_camera == nullptr) return;
        m_camera->SetPan(pan);
        m_camera->Step(0.f);

        if (getOwner() != nullptr)
        {
            // Update transform from transform component
            auto transCmp = getOwner()->getComponent<TransformComponent>();
            if (transCmp != nullptr)
            {
                transCmp->setWorldPosition(m_camera->GetPosition());
                transCmp->setWorldRotation(m_camera->GetRotation());
            }
        }
    }

    //! Tilt (X-axis)
    void CameraComponent::setTilt(float tilt)
    {
        if (m_camera == nullptr) return;
        m_camera->SetTilt(tilt);
        m_camera->Step(0.f);

        if (getOwner() != nullptr)
        {
            // Update transform from transform component
            auto transCmp = getOwner()->getComponent<TransformComponent>();
            if (transCmp != nullptr)
            {
                transCmp->setWorldPosition(m_camera->GetPosition());
                transCmp->setWorldRotation(m_camera->GetRotation());
            }
        }
    }

    //! Roll (Z-axis)
    void CameraComponent::setRoll(float roll)
    {
        if (m_camera == nullptr) return;
        m_camera->SetRoll(roll);
        m_camera->Step(0.f);

        if (getOwner() != nullptr)
        {
            // Update transform from transform component
            auto transCmp = getOwner()->getComponent<TransformComponent>();
            if (transCmp != nullptr)
            {
                transCmp->setWorldPosition(m_camera->GetPosition());
                transCmp->setWorldRotation(m_camera->GetRotation());
            }
        }
    }

    // Set lock target vector
    void CameraComponent::setTarget(const Vec3& tar)
    {
        if (m_camera == nullptr) return;
        m_camera->SetTarget(tar);
        m_camera->Step(0.f);

        if (getOwner() != nullptr)
        {
            // Update transform from transform component
            auto transCmp = getOwner()->getComponent<TransformComponent>();
            if (transCmp != nullptr)
            {
                transCmp->setWorldPosition(m_camera->GetPosition());
                transCmp->setWorldRotation(m_camera->GetRotation());
            }
        }
    }

    // Set lock target
    void CameraComponent::lockonTarget(bool lockon) 
    {
        if (m_camera == nullptr) return;        
        m_camera->LockonTarget(lockon);
        m_camera->Step(0.f);

        if (getOwner() != nullptr)
        {
            // Update transform from transform component
            auto transCmp = getOwner()->getComponent<TransformComponent>();
            if (transCmp != nullptr)
            {
                transCmp->setWorldPosition(m_camera->GetPosition());
                transCmp->setWorldRotation(m_camera->GetRotation());
            }
        }        
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
                m_camera->SetPosition(transCmp->getWorldPosition());
                m_camera->SetRotation(transCmp->getWorldRotation());
            }
        }

        // Update
        m_camera->Step(dt);

        if (getOwner() != nullptr)
        {
            // Update transform from transform component
            transCmp = getOwner()->getComponent<TransformComponent>();
            if (transCmp != nullptr)
            {
                transCmp->setWorldPosition(m_camera->GetPosition());
                transCmp->setWorldRotation(m_camera->GetRotation());
            }
        }
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
