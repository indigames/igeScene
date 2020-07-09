#include "components/SpriteComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/GraphicsHelper.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ige::scene
{
    //! Constructor
    SpriteComponent::SpriteComponent(std::shared_ptr<SceneObject> owner, const Vec2& size, const std::string& path)
        : Component(owner), m_figure(nullptr), m_size(size), m_path(path)
    {
        if (!m_path.empty())
        {
            m_figure = GraphicsHelper::getInstance()->createSprite(m_size, m_path.c_str());
            m_figure->WaitInitialize();
        }
    }

    //! Destructor
    SpriteComponent::~SpriteComponent() 
    {
        if(m_figure)
            m_figure->DecReference();
        m_figure = nullptr;
    }

    //! Update
    void SpriteComponent::onUpdate(float dt)
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
    void SpriteComponent::onRender()
    {
        if (m_figure == nullptr) return;
        m_figure->Render();
    }

    //! Set path
    void SpriteComponent::setPath(const std::string& path)
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
            }

            if (m_path != "")
            {
                m_figure = GraphicsHelper::getInstance()->createSprite(m_size, m_path.c_str());
                m_figure->WaitInitialize();
                getOnFigureCreatedEvent().invoke(m_figure);
            }
        }
    }

    //! Set size
    void SpriteComponent::setSize(const Vec2& size)
    {
        if (m_size != size)
        {
            m_size = size;

            if (m_figure != nullptr)
            {
                getOnFigureDestroyedEvent().invoke(m_figure);
                m_figure->DecReference();
                m_figure = nullptr;
            }
            
            if (m_path != "")
            {
                m_figure = GraphicsHelper::getInstance()->createSprite(m_size, m_path.c_str());
                m_figure->WaitInitialize();
                getOnFigureCreatedEvent().invoke(m_figure);
            }
        }
    }

    //! Serialize
    void SpriteComponent::to_json(json& j) const
    {
        j = json {
            {"path", m_path},
            {"size", m_size}
        };
    }

    //! Deserialize
    void SpriteComponent::from_json(const json& j)
    {
        j.at("size").get_to(m_size);
        setPath(j.at("path"));
    }
}
