#include "components/FigureComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    FigureComponent::FigureComponent(std::shared_ptr<SceneObject> owner)
        : Component(owner)
    {
        m_figure = nullptr;
    }

    //! Destructor
    FigureComponent::~FigureComponent() 
    {
        if (m_figure)
        {
             m_figure->DecReference();
        }
        m_figure = nullptr;
    }

    //! Update
    void FigureComponent::onUpdate(float dt)
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

        // Update
        m_figure->Step(dt);
    }

    //! Update
    void FigureComponent::onRender()
    {
        if (m_figure == nullptr) return;

        m_figure->Render();
    }


    void FigureComponent::setPath(const std::string& path)
    {
        if (m_path != path) 
        {
            if (m_figure != nullptr)
            {
                getOnFigureDestroyedEvent().invoke(m_figure);
                m_figure->DecReference();
                m_figure = nullptr;
            }

            m_figure = ResourceCreator::Instance().NewFigure(path.c_str());
            m_figure->WaitInitialize();
            getOnFigureCreatedEvent().invoke(m_figure);
        }
    }

    //! Serialize
    void FigureComponent::to_json(json& j, const Component& obj)
    {

    }

    //! Deserialize
    void FigureComponent::from_json(const json& j, Component& obj)
    {

    }
}
