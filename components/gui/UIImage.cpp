#include "components/gui/UIImage.h"
#include "components/gui/RectTransform.h"
#include "components/gui/UIMask.h"
#include "scene/SceneObject.h"
#include "event/EventContext.h"

namespace ige::scene
{
    //! Constructor
    UIImage::UIImage(SceneObject& owner, const std::string &path, const Vec2 &size, const bool isSliced, const Vec4& border)
        : SpriteComponent(owner, path, size, false), UIMaskable(), m_bIsInteractable(false)
    {
        setSpriteType(isSliced ? (int)SpriteType::Sliced : (int)SpriteType::Simple);
        setBorder(border);
        getOwner()->addEventListener((int)EventType::SetParent, std::bind(&UIImage::onSetParent, this, std::placeholders::_1), m_instanceId);
        setStencilMask(getOwner()->isInMask() ? 1 : -1);
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

    void UIImage::onResourceAdded(Resource* res) {
        SpriteComponent::onResourceAdded(res);
        if (res != nullptr) {
            setStencilMask(getOwner()->isInMask() ? 1 : -1);
        }
    }

    //! Serialize
    void UIImage::to_json(json &j) const
    {
        SpriteComponent::to_json(j);
        j["interactable"] = isInteractable();
    }

    //! Deserialize
    void UIImage::from_json(const json &j)
    {
        SpriteComponent::from_json(j);
        setInteractable(j.value("interactable", false));
    }

    //! Update property by key value
    void UIImage::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("interactable") == 0) {
            setInteractable(val);
        }
        else {
            SpriteComponent::setProperty(key, val);
        }
    }
} // namespace ige::scene
