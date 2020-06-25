#include "components/EditableFigureComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    EditableFigureComponent::EditableFigureComponent(std::shared_ptr<SceneObject> owner, const std::string& path, EditableFigure* efig)
        : Component(owner), m_figure(nullptr), m_path(path)
    {
        if (efig != nullptr)
        {
            m_figure = efig;
            m_figure->IncReference();
            m_figure->WaitInitialize();
        }
        else if (m_path != "")
        {
            m_figure = ResourceCreator::Instance().NewEditableFigure(path.c_str());
            m_figure->WaitInitialize();
        }
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

    //! Set path
    void EditableFigureComponent::setPath(const std::string& path)
    {
        if (m_path != path)
        {
            if (m_figure != nullptr)
            {
                getOnFigureDestroyedEvent().invoke(m_figure);
                m_figure->DecReference();
                m_figure = nullptr;
            }

            m_figure = ResourceCreator::Instance().NewEditableFigure(path.c_str());
            m_figure->WaitInitialize();
            getOnFigureCreatedEvent().invoke(m_figure);
        }
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
