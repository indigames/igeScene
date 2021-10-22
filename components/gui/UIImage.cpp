#include "components/gui/UIImage.h"
#include "components/gui/RectTransform.h"
#include "components/gui/UIMask.h"
#include "scene/SceneObject.h"
#include "event/EventContext.h"

namespace ige::scene
{
    //! Constructor
    UIImage::UIImage(SceneObject &owner, const std::string &path, const Vec2 &size, const bool isSliced, const Vec4& border)
        : SpriteComponent(owner, path, size, false), UIMaskable(), m_bIsInteractable(false)
    {
        setSpriteType(isSliced ? (int)SpriteType::Sliced : (int)SpriteType::Simple);
        setBorder(border);
        setStencilMask(getOwner()->isInMask() ? 1 : -1);
        getOwner()->addEventListener((int)EventType::SetParent, std::bind(&UIImage::onSetParent, this, std::placeholders::_1), m_instanceId);
    }

    //! Destructor
    UIImage::~UIImage()
    {
        getOwner()->removeEventListener((int)EventType::SetParent, m_instanceId);

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

    //! Enable
    void UIImage::onEnable()
    {
        SpriteComponent::onEnable();
    }

    //! Disable
    void UIImage::onDisable()
    {
        SpriteComponent::onDisable();
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
        getOwner()->setIsRaycastTarget(m_bIsInteractable);
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

    EditableFigure* UIImage::getCurrentFigure() { return getFigure(); }
    SceneObject* UIImage::getSceneObjectOwner() { return getOwner(); }

    //! Serialize
    void UIImage::to_json(json &j) const
    {
        Component::to_json(j);
        j["path"] = getPath();
        j["size"] = getSize();
        j["fillmethod"] = getFillMethod();
        j["fillorigin"] = getFillOrigin();
        j["fillamount"] = getFillAmount();
        j["clockwise"] = getClockwise();
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
        setClockwise(j.at("clockwise"));
        setColor(j.at("color"));
        setInteractable(j.at("interactable"));
        setSpriteType(j.at("spritetype"));
        setBorder(j.at("border"));
        setPath(j.at("path"));
        Component::from_json(j);
    }

    //! Update property by key value
    void UIImage::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("size") == 0)
        {
            setSize(val);
        }
        else if (key.compare("fillmethod") == 0)
        {
            setFillMethod(val);
        }
        else if (key.compare("fillorigin") == 0)
        {
            setFillOrigin(val);
        }
        else if (key.compare("fillamount") == 0)
        {
            setFillAmount(val);
        }
        else if (key.compare("clockwise") == 0)
        {
            setClockwise(val);
        }
        else if (key.compare("color") == 0)
        {
            setColor(val);
        }
        else if (key.compare("interactable") == 0)
        {
            setInteractable(val);
        }
        else if (key.compare("spritetype") == 0)
        {
            setSpriteType(val);
        }
        else if (key.compare("border") == 0)
        {
            setBorder(val);
        }
        else if (key.compare("path") == 0)
        {
            setPath(val);
        }
        else
        {
            SpriteComponent::setProperty(key, val);
        }
    }
} // namespace ige::scene
