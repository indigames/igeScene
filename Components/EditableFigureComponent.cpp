#include "components/EditableFigureComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    EditableFigureComponent::EditableFigureComponent(std::shared_ptr<SceneObject> owner, const std::string& path, EditableFigure* figure)
        : Component(owner)
    {
        m_figure = figure != nullptr ? figure : ResourceCreator::Instance().NewEditableFigure(path.c_str());
        m_figure->WaitInitialize();
    }

    //! Destructor
    EditableFigureComponent::~EditableFigureComponent() 
    {
        if(m_figure)
            m_figure->DecReference();
        m_figure = nullptr;
    }

    //! Update
    void EditableFigureComponent::onUpdate(float dt)
    {
        if (m_figure == nullptr) return;

        std::shared_ptr<TransformComponent> transCmp = nullptr;

        if(getOwner() != nullptr)
        {
            // Update transform from transform component
            transCmp = getOwner()->getComponent<TransformComponent>();
            if(transCmp != nullptr)
            {
                m_figure->SetPosition(transCmp->getWorldPosition());
                m_figure->SetRotation(transCmp->getWorldRotation());
                m_figure->SetScale(transCmp->getWorldScale());
            }
        }

        m_figure->Pose();
    }

    //! Update
    void EditableFigureComponent::onRender()
    {
        if (m_figure == nullptr) return;

        m_figure->Render();
    }

    //! Serialize
    void EditableFigureComponent::to_json(json& j, const Component& obj)
    {

    }

    //! Deserialize
    void EditableFigureComponent::from_json(const json& j, Component& obj)
    {

    }
}
