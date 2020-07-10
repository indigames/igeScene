#include "components/FigureComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ige::scene
{
    //! Constructor
    FigureComponent::FigureComponent(std::shared_ptr<SceneObject> owner, const std::string& path)
        : Component(owner), m_figure(nullptr), m_path(path)
    {
        if (!m_path.empty())
        {
            m_figure = ResourceCreator::Instance().NewFigure(m_path.c_str());
            m_figure->WaitBuild();
        }
    }

    //! Destructor
    FigureComponent::~FigureComponent() 
    {
        if (m_figure)
        {
             m_figure->DecReference();
             m_figure = nullptr;
        }
        ResourceManager::Instance().DeleteDaemon();
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
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0)
        {
            m_path = relPath;

            if (m_figure != nullptr)
            {
                getOnFigureDestroyedEvent().invoke(m_figure);
                m_figure->DecReference();
                m_figure = nullptr;
                ResourceManager::Instance().DeleteDaemon();
            }

            m_figure = ResourceCreator::Instance().NewFigure(m_path.c_str());
            m_figure->WaitInitialize();
            getOnFigureCreatedEvent().invoke(m_figure);
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
