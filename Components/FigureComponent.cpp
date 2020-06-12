#include "components/FigureComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    FigureComponent::FigureComponent(std::shared_ptr<SceneObject> owner, const std::string& path)
        : Component(owner)
    {
        m_figure = ResourceCreator::Instance().NewFigure(path.c_str());
        m_figure->WaitInitialize();
    }

    //! Destructor
    FigureComponent::~FigureComponent() 
    {
        if(m_figure)
            m_figure->DecReference();
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

    //! Serialize
    void FigureComponent::to_json(json& j, const Component& obj)
    {

    }

    //! Deserialize
    void FigureComponent::from_json(const json& j, Component& obj)
    {

    }
}
