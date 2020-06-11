#include "FigureComponent.h"
#include "TransformComponent.h"
#include "SceneObject.h"

namespace ige::scene
{
    //! Constructor
    FigureComponent::FigureComponent(std::shared_ptr<SceneObject> owner, const std::string& path)
        : Component(owner) 
    {
        m_figure = std::make_shared<Figure>(ResourceCreator::Instance().NewFigure(path.c_str()));
    }

    //! Destructor
    FigureComponent::~FigureComponent() 
    {
        m_figure = nullptr;
    }

    //! Bind animation
    void FigureComponent::bindAnim(Figure::AnimatorSlot slot, const std::string& name)
    {
        if (m_figure)
        {
            m_figure->BindAnimator(slot, name.c_str());
        }
    }

    //! Set animation time
    void FigureComponent::setAnimTime(float time)
    {
        if (m_figure)
        {
            m_figure->SetEvalTime(time);
        }
    }

    //! Set animation blending weight
    void FigureComponent::setAnimBlendWeight(uint32_t part, float weight)
    {
        if (m_figure)
        {
            m_figure->SetBlendingWeight(part, weight);
        }
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
                m_figure->SetPosition(transCmp->getPosition());
                m_figure->SetRotation(transCmp->getRotation());
                m_figure->SetScale(transCmp->getScale());
            }
        }

        // Update        
        m_figure->Step(dt);        

        if(transCmp != nullptr)
        {
            // Sync transform to transform component
            transCmp->setPosition(m_figure->GetPosition());
            transCmp->setRotation(m_figure->GetRotation());
            transCmp->setScale(m_figure->GetScale());
        }

        // Render
        m_figure->Render();
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
