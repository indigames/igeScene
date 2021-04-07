#include "components/gui/UIImage.h"
#include "components/gui/RectTransform.h"
#include "scene/SceneObject.h"
#include "event/EventContext.h"

namespace ige::scene
{
    //! Constructor
    UIImage::UIImage(SceneObject &owner, const std::string &path, const Vec2 &size, const bool isSliced, const Vec4& border)
        : SpriteComponent(owner, path, size, false, true), 
        m_bIsInteractable(false)
    {
        getOwner()->setIsRaycastTarget(true);
        if (m_sprite) {
            m_sprite->setIsScaleBorder(true);
        }
        if (isSliced)
            setSpriteType(1);
        setBorder(border);
    }

    //! Destructor
    UIImage::~UIImage()
    {
        m_onClickedEvent.removeAllListeners();
        m_onPressedEvent.removeAllListeners();
        m_onDragEvent.removeAllListeners();
        m_onReleasedEvent.removeAllListeners();
        m_onSelectedEvent.removeAllListeners();
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

    //! Set Interactable
    void UIImage::setInteractable(bool value) {
        m_bIsInteractable = value;
        getOwner()->setIsInteractable(m_bIsInteractable);
    }


    //==================================

    void UIImage::_onTouchPress(EventContext* context)
    {
        if (!isInteractable()) return;
        getOnPressedEvent().invoke();
    }

    void UIImage::_onTouchRelease(EventContext* context)
    {
        if (!isInteractable()) return;
        getOnReleasedEvent().invoke();
    }

    void UIImage::_onTouchDrag(EventContext* context)
    {
        if (!isInteractable()) return;
        getOnDragEvent().invoke();
    }

    void UIImage::_onClick(EventContext* context)
    {
        if (!isInteractable()) return;
        getOnClickedEvent().invoke();
    }

    void UIImage::_onSelected(EventContext* context)
    {
        if (!isInteractable()) return;
        getOnSelectedEvent().invoke();
    }

    void UIImage::_onExit(EventContext* context)
    {
        
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
        j["color"] = getColor();
        j["interactable"] = isInteractable();
        j["spritetype"] = (int)getSpriteType();
        j["border"] = getBorder();
    }

    //! Deserialize
    void UIImage::from_json(const json &j)
    {
        setSize(j.at("size"));
        setFillMethod(j.at("fillmethod"));
        setFillOrigin(j.at("fillorigin"));
        setFillAmount(j.at("fillamount"));
        setColor(j.at("color"));
        setInteractable(j.at("interactable"));
        setSpriteType(j.at("spritetype"));
        setBorder(j.at("border"));
        setPath(j.at("path"));
        Component::from_json(j);
    }
} // namespace ige::scene
