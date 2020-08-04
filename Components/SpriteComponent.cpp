#include "components/SpriteComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"

#include "utils/GraphicsHelper.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ige::scene
{
    //! Constructor
    SpriteComponent::SpriteComponent(const std::shared_ptr<SceneObject>& owner, const Vec2& size, const std::string& path)
        : Component(owner)
    {
        m_sprite = std::make_shared<Sprite>(path, size);
        if (m_sprite->getFigure())
            getOnFigureCreatedEvent().invoke(m_sprite->getFigure());
    }

    //! Destructor
    SpriteComponent::~SpriteComponent()
    {
        getOnFigureDestroyedEvent().invoke(m_sprite->getFigure());
        m_sprite = nullptr;
    }

    //! Update
    void SpriteComponent::onUpdate(float dt)
    {
        if (getFigure() == nullptr) return;

        // Update transform from transform component
        auto transCmp = getOwner()->getTransform();
        getFigure()->SetPosition(transCmp->getWorldPosition());
        getFigure()->SetRotation(transCmp->getWorldRotation());
        getFigure()->SetScale(transCmp->getWorldScale());

        // Update
        getFigure()->Pose();
    }

    //! Update
    void SpriteComponent::onRender()
    {
        if (getFigure() == nullptr) return;
        getFigure()->Render();
    }

    //! Set path
    void SpriteComponent::setPath(const std::string& path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        auto figure = m_sprite->getFigure();
        m_sprite->setPath(relPath);
        auto figureAfter = m_sprite->getFigure();
        if (figure == nullptr && figureAfter)
        {
            getOnFigureCreatedEvent().invoke(figureAfter);
        }
    }

    //! Set size
    void SpriteComponent::setSize(const Vec2& size)
    {
        m_sprite->setSize(size);
    }

    //! Serialize
    void SpriteComponent::to_json(json& j) const
    {
        j = json {
            {"path", getPath()},
            {"size", getSize()}
        };
    }

    //! Deserialize
    void SpriteComponent::from_json(const json& j)
    {
        setSize(j.at("size"));
        setPath(j.at("path"));
    }
}
