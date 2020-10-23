#include "components/SpriteComponent.h"
#include "components/TransformComponent.h"
#include "components/gui/RectTransform.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    SpriteComponent::SpriteComponent(SceneObject &owner, const std::string &path, const Vec2 &size, bool isBillboard)
        : Component(owner)
    {
        m_sprite = std::make_shared<Sprite>(path, size);
        if (m_sprite->getFigure())
            getOwner()->getScene()->getResourceAddedEvent().invoke(m_sprite->getFigure());        
        setBillboard(isBillboard);
    }

    //! Destructor
    SpriteComponent::~SpriteComponent()
    {
        if (m_sprite && m_sprite->getFigure())
            if (getOwner()->getScene())
                getOwner()->getScene()->getResourceRemovedEvent().invoke(m_sprite->getFigure());
        m_sprite = nullptr;
    }

    //! Update
    void SpriteComponent::onUpdate(float dt)
    {
        if (getFigure() == nullptr)
            return;

        // Update transform from transform component
        auto transform = getOwner()->getTransform();
        getFigure()->SetPosition(transform->getWorldPosition());
        getFigure()->SetRotation(transform->getWorldRotation());
        getFigure()->SetScale(transform->getWorldScale());

        // Update
        getFigure()->Pose();
    }

    //! Update
    void SpriteComponent::onRender()
    {
        if (getFigure() == nullptr)
            return;
        getFigure()->Render();
    }

    //! Set path
    void SpriteComponent::setPath(const std::string &path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        auto oldFigure = m_sprite->getFigure();
        m_sprite->setPath(relPath);
        auto newFigure = m_sprite->getFigure();

        if (oldFigure == nullptr && newFigure)
            getOwner()->getScene()->getResourceAddedEvent().invoke(newFigure);

        if (newFigure)
        {
            auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
            shaderDesc->SetValue(newFigure->GetShaderName(0));
            shaderDesc->SetBillboard(m_bIsBillboard);
            newFigure->SetShaderName(0, shaderDesc->GetValue());
        }
    }

    //! Set size
    void SpriteComponent::setSize(const Vec2 &size)
    {
        m_sprite->setSize(size);
    }

    //! Set billboard
    void SpriteComponent::setBillboard(bool isBillboard)
    {
        if (m_bIsBillboard != isBillboard)
        {
            m_bIsBillboard = isBillboard;
            auto figure = m_sprite->getFigure();
            if (figure)
            {
                auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                shaderDesc->SetValue(figure->GetShaderName(0));
                shaderDesc->SetBillboard(m_bIsBillboard);
                figure->SetShaderName(0, shaderDesc->GetValue());
            }
        }
    }

    //! Serialize
    void SpriteComponent::to_json(json &j) const
    {
        j = json{
            {"path", getPath()},
            {"size", getSize()}};
    }

    //! Deserialize
    void SpriteComponent::from_json(const json &j)
    {
        setSize(j.at("size"));
        setPath(j.at("path"));
    }
} // namespace ige::scene
