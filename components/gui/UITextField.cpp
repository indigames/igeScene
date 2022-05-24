#include "components/gui/UITextField.h"
#include "components/gui/RectTransform.h"

#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace pyxie
{
    extern void pop_back_utf8(std::string& utf8);
}

namespace ige::scene
{
    UITextField::UITextField(SceneObject &owner, const std::string &text)
        : UIText(owner)
    {
        auto texture = ResourceCreator::Instance().NewTexture("sprites/background");
        m_sprite = std::make_shared<Sprite>(texture);
        m_sprite->setSpriteType(SpriteType::Sliced);
        m_sprite->setBorder(20.f, 20.f, 20.f, 20.f);
        getOwner()->getScene()->getUIResourceAddedEvent().invoke(m_sprite->getFigure());

        setText(text);
        getOwner()->setIsInteractable(true);
        getOwner()->setIsRaycastTarget(true);
        getOwner()->addEventListener((int)EventType::Click, std::bind(&UITextField::onClick, this));
    }

    //! Destructor
    UITextField::~UITextField()
    {
        if (m_sprite && m_sprite->getFigure())
            getOwner()->getScene()->getUIResourceRemovedEvent().invoke(m_sprite->getFigure());
        m_sprite = nullptr;
    }

    //! Enable
    void UITextField::onEnable()
    {
        if (!getOwner()->isActive() || !isEnabled()) return;
        UIText::onEnable();
        if (m_sprite) onResourceAdded(m_sprite->getFigure());
    }

    //! Disable
    void UITextField::onDisable()
    {
        if (m_sprite) onResourceRemoved(m_sprite->getFigure());
        UIText::onDisable();
    }

    //! Update
    void UITextField::onUpdate(float dt)
    {
        if (m_sprite) {
            auto transform = getOwner()->getRectTransform();
            m_sprite->setSize(transform->getSize());
            const auto& pos = transform->getPosition();
            m_sprite->getFigure()->SetPosition({ pos[0], pos[1], pos[2] - 0.01f });
            m_sprite->getFigure()->SetRotation(transform->getRotation());
            m_sprite->getFigure()->SetScale(transform->getScale());
            m_sprite->getFigure()->Pose();
        }
                
        if (m_bIsInputing) {
            if (IsVirtualKeyboardShown()) {
                int size;
                auto str = std::string(GetInputText(&size));
                if (m_lastInput.compare(str) != 0) {
                    m_lastInput = str;
                    UIText::setText(str.append("|"));
                }
            }
            else {
                UIText::setText(m_lastInput);
                getOwner()->dispatchEvent((int)EventType::Changed, Value(m_lastInput));
                m_lastInput.clear();
                StopTextInput();
                m_bIsInputing = false;
            }
        }
        UIText::onUpdate(dt);
    }

    //! Click
    void UITextField::onClick() {
        if (!IsVirtualKeyboardShown()) {
            StartTextInput(getText().c_str());
            UIText::setText(getText() + "|");
            m_bIsInputing = true;
        }        
    }

    void UITextField::setBackgroundColor(const Vec4& color) {
        if (m_bgColor != color) {
            m_bgColor = color;
            m_sprite->setColor(m_bgColor);
        }
    }

    //! Serialize
    void UITextField::to_json(json &j) const {
        UIText::to_json(j);
        j["bgColor"] = m_bgColor;
    }

    //! Deserialize
    void UITextField::from_json(const json &j) {
        UIText::from_json(j);
        setBackgroundColor(j.value("bgColor", Vec4(1.f, 1.f, 1.f, 1.f)));
    }

    //! Update property by key value
    void UITextField::setProperty(const std::string& key, const json& val) {
        if (key.compare("bgColor") == 0) {
            setBackgroundColor(val);
        }
        else {
            UIText::setProperty(key, val);
        }
    }
} // namespace ige::scene