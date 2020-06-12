#include "components/EnvironmentComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    EnvironmentComponent::EnvironmentComponent(std::shared_ptr<SceneObject> owner, const std::string& name, Figure* parentFigure)
        : Component(owner)
    {
        m_environment = ResourceCreator::Instance().NewEnvironmentSet(name.c_str(), parentFigure);
        m_environment->IncReference();
    }

    //! Destructor
    EnvironmentComponent::~EnvironmentComponent() 
    {
        if (m_environment)
            m_environment->DecReference();
        m_environment = nullptr;
    }
    
    //! Update
    void EnvironmentComponent::onUpdate(float dt)
    {
        if (m_environment == nullptr) return;
    }

    //! Render
    void EnvironmentComponent::onRender()
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
