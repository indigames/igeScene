#include "components/FigureComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ige::scene
{
    //! Constructor
    FigureComponent::FigureComponent(const std::shared_ptr<SceneObject>& owner, const std::string& path)
        : Component(owner), m_figure(nullptr)
    {
        setPath(path);
    }

    //! Destructor
    FigureComponent::~FigureComponent()
    {
        if (m_figure)
        {
            if (hasOwner() && getOwner()->getRoot())
                getOwner()->getRoot()->getResourceRemovedEvent().invoke(m_figure);
            m_figure->DecReference();
            m_figure = nullptr;
        }
    }

    //! Update
    void FigureComponent::onUpdate(float dt)
    {
        if (m_figure == nullptr) return;

        // Update transform from transform component
        auto transform = getOwner()->getTransform();
        m_figure->SetPosition(transform->getWorldPosition());
        m_figure->SetRotation(transform->getWorldRotation());
        m_figure->SetScale(transform->getWorldScale());

        // Update
        m_figure->Step(dt);

        // Update transform back to transform component
        transform->setWorldPosition(m_figure->GetPosition());
        transform->setWorldRotation(m_figure->GetRotation());
        transform->setWorldScale(m_figure->GetScale());
    }

    //! Update
    void FigureComponent::onRender()
    {
        if (m_figure == nullptr) return;

        m_figure->Render();
    }


    void FigureComponent::setPath(const std::string& path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0)
        {
            m_path = relPath;

            if (m_figure != nullptr)
            {
                getOwner()->getRoot()->getResourceRemovedEvent().invoke(m_figure);
                m_figure->DecReference();
                m_figure = nullptr;
            }

            m_figure = ResourceCreator::Instance().NewFigure(m_path.c_str());
            m_figure->WaitInitialize();
            getOwner()->getRoot()->getResourceAddedEvent().invoke(m_figure);
        }
    }

    //! Serialize
    void FigureComponent::to_json(json& j) const
    {
        j = json {
            {"path", m_path}
        };
    }

    //! Deserialize
    void FigureComponent::from_json(const json& j)
    {
        setPath(j.at("path"));
    }
}
