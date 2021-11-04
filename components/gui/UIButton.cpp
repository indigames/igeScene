#include "components/gui/UIButton.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"

#include "event/EventContext.h"

#include "components/tween/Tween.h"
#include "components/tween/Tweener.h"

namespace fs = ghc::filesystem;

NS_IGE_SCENE_BEGIN

UIButton::UIButton(SceneObject& owner, const std::string& texture, const Vec2& size, const bool isSliced, const Vec4& border) :
	UIImage(owner, texture, size, isSliced, border),
    m_pressedTexture(nullptr), m_selectedTexture(nullptr), m_disabledTexture(nullptr),
	m_normalColor(1.0f, 1.0f, 1.0f, 1.0f), m_pressedColor(0.78f, 0.78f, 0.78f, 1.0f), 
    m_selectedColor(0.98f, 0.98f, 0.98f, 1.0f), m_disableColor(0.78f, 0.78f, 0.78f, 0.5f),
	m_fadeDuration(0.1f)
{
    m_bIsInteractable = true;
    m_btnState = ButtonState::NORMAL;
    m_transitionMode = TransitionMode::ColorTint;
    getOwner()->setIsInteractable(m_bIsInteractable);
    getOwner()->setIsRaycastTarget(m_bIsInteractable);
    
    getOwner()->addEventListener((int)EventType::TouchBegin, std::bind(&UIButton::_onTouchPress, this, std::placeholders::_1), m_instanceId);
    getOwner()->addEventListener((int)EventType::TouchEnd, std::bind(&UIButton::_onTouchRelease, this, std::placeholders::_1), m_instanceId);
    getOwner()->addEventListener((int)EventType::Click, std::bind(&UIButton::_onClick, this, std::placeholders::_1), m_instanceId);

}

UIButton::~UIButton()
{
    getOwner()->removeEventListener((int)EventType::TouchBegin, m_instanceId);
    getOwner()->removeEventListener((int)EventType::TouchEnd, m_instanceId);
    getOwner()->removeEventListener((int)EventType::Click, m_instanceId);

    if (m_pressedTexture) {
        m_pressedTexture->DecReference();
        m_pressedTexture = nullptr;
    }

    if (m_selectedTexture) {
        m_selectedTexture->DecReference();
        m_selectedTexture = nullptr;
    }

    if (m_disabledTexture) {
        m_disabledTexture->DecReference();
        m_disabledTexture = nullptr;
    }

    if (tween) {
        tween->kill();
        tween = nullptr;
    }
}

void UIButton::_onTouchPress(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    auto inputContext = dynamic_cast<InputEventContext*>(context);
    if (inputContext == nullptr) return;
    inputContext->captureTouch();
    changeState(ButtonState::PRESSED);
    getOnPressedEvent().invoke();
}

void UIButton::_onTouchRelease(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    changeState(ButtonState::NORMAL);
    getOnReleasedEvent().invoke();
}

void UIButton::_onClick(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    onClick();
    getOnClickedEvent().invoke();
}

void UIButton::_onSelected(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    getOnSelectedEvent().invoke();
}

void UIButton::_onExit(EventContext* context)
{
    changeState(ButtonState::NORMAL);
}

void UIButton::onClick() {
    
}

void UIButton::changeState(ButtonState state, bool forced)
{
    if (m_btnState != state || forced)
    {
        m_btnState = state;
        switch (m_transitionMode) {
        case TransitionMode::ColorTint:
        default:
            {
                auto col = m_sprite->getColor();
                auto t_col = getColorByState(m_btnState);
                if (col != t_col) {
                    if (tween != nullptr) {
                        tween->kill();
                        tween = nullptr;
                    }

                    tween = Tween::tween(col, t_col, m_fadeDuration);
                    tween->onUpdate([this](Tweener* tweener) {
                        m_sprite->setColor(tweener->value.getVec4());
                        })->onComplete([this]() { });
                    
                }
                else if (col != m_normalColor) {
                    m_sprite->setColor(m_normalColor);
                }
            }
            break;
        case TransitionMode::SpriteSwap:
            {
                auto tex = m_sprite->getTexture();
                m_sprite->setColor(m_normalColor, false);
                auto t_tex = getTextureByState(m_btnState);
                if (tex != t_tex && t_tex != nullptr) {
                    m_sprite->setTexture(t_tex);
                }
            }
            break;
        }
    }
}

Texture* UIButton::getTextureByState(ButtonState state) const {
    switch (state) {
    case ButtonState::NORMAL:
        return getTexture();
    case ButtonState::PRESSED:
        return m_pressedTexture;
    case ButtonState::SELECTED:
        return m_selectedTexture;
    case ButtonState::DISABLE:
        return m_disabledTexture;
    }
    return nullptr;
}

Vec4 UIButton::getColorByState(ButtonState state) const
{
    switch (state) {
    case ButtonState::NORMAL:
        return m_normalColor;
    case ButtonState::PRESSED:
        return m_pressedColor;
    case ButtonState::SELECTED:
        return m_selectedColor;
    case ButtonState::DISABLE:
        return m_disableColor;
    }
	return m_normalColor;
}

void UIButton::setTransitionMode(TransitionMode value)
{
    if (m_transitionMode != value) 
    {
        m_transitionMode = value;
    }
}

void UIButton::setAnimationMode(AnimationMode value)
{
    if (m_animationMode != value)
    {
        m_animationMode = value;
    }
}

void UIButton::setTexturePath(const std::string& path, ButtonState setState)
{
    auto fsPath = fs::path(path);
    auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
    std::replace(relPath.begin(), relPath.end(), '\\', '/');
    switch (setState)
    {
    case ButtonState::NORMAL:
        setPath(path);
        break;
    case ButtonState::PRESSED:
        if (m_pressedTexture) {
            m_pressedTexture->DecReference();
            m_pressedTexture = nullptr;
        }
        m_pressedPath = relPath;
        if (relPath.length() > 0)
            m_pressedTexture = ResourceCreator::Instance().NewTexture(relPath.c_str());
        break;
    case ButtonState::SELECTED:
        if (m_selectedTexture) {
            m_selectedTexture->DecReference();
            m_selectedTexture = nullptr;
        }
        m_selectedPath = relPath;
        if (relPath.length() > 0)
            m_selectedTexture = ResourceCreator::Instance().NewTexture(relPath.c_str());
        break;
    case ButtonState::DISABLE:
        if (m_disabledTexture) {
            m_disabledTexture->DecReference();
            m_disabledTexture = nullptr;
        }
        m_disabledPath = relPath;
        if (relPath.length() > 0)
            m_disabledTexture = ResourceCreator::Instance().NewTexture(relPath.c_str());
        break;
    }

    if (getFigure() && setState == ButtonState::NORMAL)
        changeState(ButtonState::NORMAL, true);
}

//! Set Normal Color
void UIButton::setColor(const Vec4& value)
{
    if (m_normalColor != value) {
        m_normalColor = value;
        m_sprite->setColor(m_normalColor);
    }
}

void UIButton::setColor(float r, float g, float b, float a) {
    Vec4 value (r, g, b, a);
    value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
    value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
    value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
    value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
    setColor(value);
}

//! Set Pressed Color
void UIButton::setPressedColor(const Vec4& value)
{
    if (m_selectedColor != value) {
        m_selectedColor = value;
    }
}

void UIButton::setPressedColor(float r, float g, float b, float a) {
    Vec4 value(r, g, b, a);
    value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
    value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
    value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
    value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
    setPressedColor(value);
}

//! Set Selected Color
void UIButton::setSelectedColor(const Vec4& value)
{
    if (m_selectedColor != value) {
        m_selectedColor = value;
    }
}

void UIButton::setSelectedColor(float r, float g, float b, float a) {
    Vec4 value(r, g, b, a);
    value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
    value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
    value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
    value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
    setSelectedColor(value);
}

//! Set Disabled Color
void UIButton::setDisabledColor(const Vec4& value)
{
    if (m_disableColor != value) {
        m_disableColor = value;
    }
}

void UIButton::setDisabledColor(float r, float g, float b, float a) {
    Vec4 value(r, g, b, a);
    value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
    value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
    value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
    value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
    setDisabledColor(value);
}

//! Json 
void UIButton::to_json(json& j) const
{
    Component::to_json(j);
    j["path"] = getPath();
    j["pressedpath"] = getPressedPath();
    j["seletecpath"] = getSelectedPath();
    j["disabledpath"] = getDisabledPath();
    j["size"] = getSize();
    j["fillmethod"] = getFillMethod();
    j["fillorigin"] = getFillOrigin();
    j["fillamount"] = getFillAmount();
    j["color"] = getColor();
    j["pressedcolor"] = getPressedColor();
    j["selectedcolor"] = getSelectedColor();
    j["disabledcolor"] = getDisabledColor();
    j["transitionmode"] = (int)getTransitionMode();
    j["fadeduration"] = getFadeDuration();
    j["spritetype"] = (int)getSpriteType();
    j["border"] = getBorder();
    j["interactable"] = isInteractable();
}

//! Deserialize
void UIButton::from_json(const json& j)
{
    setSize(j.at("size"));
    setFillMethod(j.at("fillmethod"));
    setFillOrigin(j.at("fillorigin"));
    setFillAmount(j.at("fillamount"));
    setColor(j.at("color"));
    setPressedColor(j.at("pressedcolor"));
    setSelectedColor(j.at("selectedcolor"));
    setDisabledColor(j.at("disabledcolor"));
    setTexturePath(j.at("pressedpath"), ButtonState::PRESSED);
    setTexturePath(j.at("seletecpath"), ButtonState::SELECTED);
    setTexturePath(j.at("disabledpath"), ButtonState::DISABLE);
    setTexturePath(j.at("path"), ButtonState::NORMAL);
    setTransitionMode((TransitionMode)j.at("transitionmode"));
    setFadeDuration(j.at("fadeduration"));
    setSpriteType(j.at("spritetype"));
    setBorder(j.at("border"));
    setInteractable(j.value("interactable", true));
    Component::from_json(j);
}

//! Update property by key value
void UIButton::setProperty(const std::string& key, const json& val)
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
    else if (key.compare("color") == 0)
    {
        setColor(val);
    }
    else if (key.compare("pressedcolor") == 0)
    {
        setPressedColor(val);
    }
    else if (key.compare("selectedcolor") == 0)
    {
        setSelectedColor(val);
    }
    else if (key.compare("disabledcolor") == 0)
    {
        setDisabledColor(val);
    }
    else if (key.compare("pressedpath") == 0)
    {
        setTexturePath(val, ButtonState::PRESSED);
    }
    else if (key.compare("seletecpath") == 0)
    {
        setTexturePath(val, ButtonState::SELECTED);
    }
    else if (key.compare("disabledpath") == 0)
    {
        setTexturePath(val, ButtonState::DISABLE);
    }
    else if (key.compare("path") == 0)
    {
        setTexturePath(val, ButtonState::NORMAL);
    }
    else if (key.compare("transitionmode") == 0)
    {
        setTransitionMode((TransitionMode)val);
    }
    else if (key.compare("fadeduration") == 0)
    {
        setFadeDuration(val);
    }
    else if (key.compare("spritetype") == 0)
    {
        setSpriteType(val);
    }
    else if (key.compare("border") == 0)
    {
        setBorder(val);
    }
    else if (key.compare("interactable") == 0)
    {
        setInteractable(val);
    }
    else
    {
        UIImage::setProperty(key, val);
    }
}

NS_IGE_SCENE_END
