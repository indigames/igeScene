#include "EnvironmentComponent.h"
#include "TransformComponent.h"
#include "SceneObject.h"

namespace ige::scene
{
    //! Constructor
    EnvironmentComponent::EnvironmentComponent(std::shared_ptr<SceneObject> owner, const std::string& name, Figure* parentFigure)
        : Component(owner)
    {
        m_environment = std::make_shared<Environment>(ResourceCreator::Instance().NewEnvironmentSet(name.c_str(), parentFigure));
    }

    //! Destructor
    EnvironmentComponent::~EnvironmentComponent() 
    {
        m_environment = nullptr;
    }
    
    //! Update
    void EnvironmentComponent::onUpdate(float dt)
    {
        if (m_environment == nullptr) return;

        // Render
        m_environment->Render();
    }

    //! Serialize
    void EnvironmentComponent::to_json(json& j, const Component& obj)
    {

    }

    //! Deserialize
    void EnvironmentComponent::from_json(const json& j, Component& obj)
    {

    }
}
