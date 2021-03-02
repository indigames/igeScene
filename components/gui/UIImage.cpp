#include "components/gui/UIImage.h"
#include "components/gui/RectTransform.h"
#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    UIImage::UIImage(SceneObject &owner, const std::string &path, const Vec2 &size)
        : SpriteComponent(owner, path, size)
    {
    }

    //! Destructor
    UIImage::~UIImage()
    {
    }

    //! Update
    void UIImage::onUpdate(float dt)
    {
        // Sync size from transform
        auto rectTranform = getOwner()->getRectTransform();
        if (m_sprite && rectTranform)
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

    //! Set Fill Method
    void UIImage::setFillMethod(int value)
    {
        m_sprite->setFillMethod((FillMethod)value);
    }

    //! Set Fill Origin
    void UIImage::setFillOrigin(int value)
    {
        m_sprite->setFillOrigin((FillOrigin)value);
    }

    //! Set Fill Amount
    void UIImage::setFillAmount(float value)
    {
        m_sprite->setFillAmount(value);
    }

    //! Set Clockwise
    void UIImage::setClockwise(bool value)
    {
        m_sprite->setClockwise(value);
    }

    //! Serialize
    void UIImage::to_json(json &j) const
    {
        Component::to_json(j);
        j["path"] = getPath();
        j["size"] = getSize();
        j["fillmethod"] = getFillMethod();
        j["fillorigin"] = getFillOrigin();
        j["fillamount"] = getFillAmount();
    }

    //! Deserialize
    void UIImage::from_json(const json &j)
    {
        setSize(j.at("size"));
        setFillMethod(j.at("fillmethod"));
        setFillOrigin(j.at("fillorigin"));
        setFillAmount(j.at("fillamount"));
        setPath(j.at("path"));
        Component::from_json(j);
    }
} // namespace ige::scene
