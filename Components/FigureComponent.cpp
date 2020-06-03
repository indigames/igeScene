#include "FigureComponent.h"

namespace ige::scene
{
    //! Constructor
    FigureComponent::FigureComponent(std::shared_ptr<SceneObject> owner)
        : Component(owner) 
    {}

    //! Destructor
    FigureComponent::~FigureComponent() {}

    //! Load figure
    bool FigureComponent::loadFigure(const std::string& path)
    {
        m_figure = std::make_shared<Figure>(path.c_str());
        if (m_figure)
        {
            m_figure->Build();
            m_figure->Initialize();
            return true;
        }
        return false;
    }

    //! Load figure from other figure
    bool FigureComponent::loadFromFigure(const Figure& figure)
    {
        m_figure = std::make_shared<Figure>((Figure*) &figure);
        if (m_figure)
        {
            m_figure->Build();
            m_figure->Initialize();
            return true;
        }
        return false;
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
        if (m_figure)
        {
            m_figure->Step(dt);
            m_figure->Render();
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
