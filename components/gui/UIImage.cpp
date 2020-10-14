#include "components/gui/UIImage.h"
#include "components/gui/RectTransform.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    UIImage::UIImage(const std::shared_ptr<SceneObject>& owner, const std::string& path, const Vec2& size)
        : SpriteComponent(owner, path, size)
    {}

    //! Destructor
    UIImage::~UIImage()
    {}

    //! Update
    void UIImage::onUpdate(float dt)
    {
        // Sync size from transform
        auto rectTranform = getOwner()->getRectTransform();
        if(m_sprite && rectTranform)
        {
            m_sprite->setSize(rectTranform->getSize());
        }

        SpriteComponent::onUpdate(dt);
    }

    //! Update
    void UIImage::onRender()
    {
        SpriteComponent::onRender();
    }

    //! Serialize
    void UIImage::to_json(json& j) const
    {
        j = json{
            {"path", getPath()},
            {"size", getSize()}
        };
    }

    //! Deserialize
    void UIImage::from_json(const json& j)
    {
        setSize(j.at("size"));
        setPath(j.at("path"));
    }
}
