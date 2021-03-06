#include "components/FigureComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ige::scene
{
    //! Constructor
    FigureComponent::FigureComponent(const std::shared_ptr<SceneObject>& owner, const std::string& path)
        : Component(owner), m_figure(nullptr), m_path(path)
    {
        if (!m_path.empty())
        {
            m_figure = ResourceCreator::Instance().NewFigure(m_path.c_str());
            m_figure->WaitBuild();
            getOwner()->getRoot()->getResourceAddedEvent().invoke(m_figure);
        }
    }

    //! Destructor
    FigureComponent::~FigureComponent()
    {
        if (m_figure)
        {
            if (hasOwner()) getOwner()->getRoot()->getResourceRemovedEvent().invoke(m_figure);
            m_figure->DecReference();
            m_figure = nullptr;
        }
        ResourceManager::Instance().DeleteDaemon();
    }

    //! Update
    void FigureComponent::onUpdate(float dt)
    {
        if (m_figure == nullptr) return;

        // Update transform from transform component
        auto transCmp = getOwner()->getTransform();
        m_figure->SetPosition(transCmp->getWorldPosition());
        m_figure->SetRotation(transCmp->getWorldRotation());
        m_figure->SetScale(transCmp->getWorldScale());

        // Update
        m_figure->Step(dt);

        // Update transform back to transform component
        transCmp->setWorldPosition(m_figure->GetPosition());
        transCmp->setWorldRotation(m_figure->GetRotation());
        transCmp->setWorldScale(m_figure->GetScale());
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
                ResourceManager::Instance().DeleteDaemon();
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
