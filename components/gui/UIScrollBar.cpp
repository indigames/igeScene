#include "components/gui/UIScrollBar.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"

#include "event/EventContext.h"

#include "components/tween/Tween.h"
#include "components/tween/Tweener.h"

#include "components/TransformComponent.h"
#include "components/gui/RectTransform.h"
#include "components/gui/RectTransformUtility.h"

namespace fs = ghc::filesystem;

NS_IGE_SCENE_BEGIN

static inline Vec2 GetTransposed(Vec2 input) 
{
    return Vec2(input[1], input[0]);
}

UIScrollBar::UIScrollBar(SceneObject& owner, const std::string& texture, const Vec2& size, const bool isSliced, const Vec4& border) :
	UIImage(owner, texture, size, isSliced, border), 
    m_dirtySetObj(false), m_rectHandle(nullptr), m_rectArea(nullptr), m_imgHandle(nullptr),
    m_bIsDragging(false), m_size(0.5f), m_bIsCaptureTouch(false),
    m_deltaPos(Vec2(0,0)), m_offsetPos(Vec2(0, 0)), m_tween(nullptr),
    m_handleState(0), m_normalColor(1.0f, 1.0f, 1.0f, 1.0f), m_pressedColor(0.78f, 0.78f, 0.78f, 1.0f), m_disableColor(0.78f, 0.78f, 0.78f, 0.5f), m_fadeDuration(0.1f)
{
    m_bIsInteractable = true;
    getOwner()->setIsInteractable(m_bIsInteractable);
    getOwner()->setIsRaycastTarget(m_bIsInteractable);
    
    getOwner()->addEventListener((int)EventType::TouchBegin, std::bind(&UIScrollBar::_onTouchPress, this, std::placeholders::_1), m_instanceId);
    getOwner()->addEventListener((int)EventType::TouchEnd, std::bind(&UIScrollBar::_onTouchRelease, this, std::placeholders::_1), m_instanceId);
    getOwner()->addEventListener((int)EventType::TouchMove, std::bind(&UIScrollBar::_onTouchDrag, this, std::placeholders::_1), m_instanceId);
    getOwner()->addEventListener((int)EventType::Click, std::bind(&UIScrollBar::_onClick, this, std::placeholders::_1), m_instanceId);
    
    m_rect = getOwner()->getRectTransform();
}

UIScrollBar::~UIScrollBar()
{
    getOwner()->removeEventListener((int)EventType::TouchBegin, m_instanceId);
    getOwner()->removeEventListener((int)EventType::TouchEnd, m_instanceId);
    getOwner()->removeEventListener((int)EventType::TouchMove, m_instanceId);
    getOwner()->removeEventListener((int)EventType::Click, m_instanceId);
    
    if (m_imgHandle) {
        m_imgHandle = nullptr;
    }

    if (m_rectHandle) {
        if (m_rectHandle->getOwner() != nullptr)
            m_rectHandle->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
        m_rectHandle = nullptr;
    }

    if (m_rectArea) {
        m_rectArea = nullptr;
    }

    if (m_tween) {
        m_tween->kill();
        m_tween = nullptr;
    }

    getOnValueChanged().removeAllListeners();

    m_rect = nullptr;
}

void UIScrollBar::_onTouchPress(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    auto inputContext = dynamic_cast<InputEventContext*>(context);
    if (inputContext == nullptr) return;
    inputContext->captureTouch();
    if (m_rectHandle == nullptr) return;
    
    m_offsetPos[0] = 0;
    m_offsetPos[1] = 0;
    m_deltaPos[0] = 0;
    m_deltaPos[1] = 0;

    auto clickedPos = inputContext->getInput()->getPosition();
    if (RectTransformUtility::RectangleContainsPoint(m_rectHandle.get(), clickedPos))
    {
        Vec2 localPoint = RectTransformUtility::WorldToLocalPointInRectable(m_rectHandle.get(), clickedPos);
        auto ssrect = m_rectHandle->getRect();
        Vec2 center((ssrect[2] * 0.5f + ssrect[0]), (ssrect[1] + ssrect[3] * 0.5f));
        float parentSize = (int)getAxisBar() == 0 ? m_rectArea->getSize()[0] : m_rectArea->getSize()[1];
        float remainingSize = parentSize * (1 - getSize());
        Vec2 sizeDelta(m_rectArea->getSize()[0], m_rectArea->getSize()[1]);

        float off =  remainingSize;
        switch (m_direction)
        {
        case Direction::LeftToRight:
        case Direction::BottomToTop:
            off = (1 - m_value) * off;
            break;
        case Direction::RightToLeft:
        case Direction::TopToBottom:
            off *= m_value;
            break;
        }

        int axis = (int)getAxisBar();
        m_deltaPos[axis] = off;
        m_deltaPos += (m_rectArea->getSize() - sizeDelta) * 0.5f;
        m_offsetPos = localPoint - center - m_deltaPos;
        m_bIsCaptureTouch = true;

        changeState(1);
    }

    getOnPressedEvent().invoke();
}

void UIScrollBar::_onTouchRelease(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    m_bIsDragging = false;
    m_bIsCaptureTouch = false;
    changeState(0);
    getOnReleasedEvent().invoke();
}

void UIScrollBar::_onTouchDrag(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    auto inputContext = dynamic_cast<InputEventContext*>(context);
    if (inputContext == nullptr) return;
    if (inputContext->getInput() == nullptr) return;
    
    if (!m_bIsCaptureTouch) return;
    auto clickPoint = inputContext->getInput()->getPosition();
    _drag(clickPoint);
    getOnDragEvent().invoke();
}

void UIScrollBar::_onClick(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    auto inputContext = dynamic_cast<InputEventContext*>(context);
    if (inputContext == nullptr) return;
    if (inputContext->getInput() == nullptr) return;
    if (!m_bIsDragging)
    {
        auto clickPoint = inputContext->getInput()->getPosition();
        _clickRepeat(clickPoint);
    }

    getOnClickedEvent().invoke();
}

//! Update
void UIScrollBar::onUpdate(float dt)
{
    UIImage::onUpdate(dt);
    
}

//! Render
void UIScrollBar::onRender()
{
    UIImage::onRender();
}

void UIScrollBar::_drag(Vec2 clickPoint)
{
    if (m_rectHandle == nullptr || m_rect == nullptr || m_rectArea == nullptr) return;

    m_bIsDragging = true;
    auto point = RectTransformUtility::WorldToLocalPointInRectable(m_rectArea.get(), clickPoint);
    Vec2 pos(m_rectArea->getLocalPosition()[0], m_rectArea->getLocalPosition()[1]);
    Vec2 handleCenter = point - m_offsetPos - pos;

    Vec2 sizeDelta(m_rectArea->getSize()[0], m_rectArea->getSize()[1]);
    Vec2 handleCorner = handleCenter - (m_rectArea->getSize() - sizeDelta) * 0.5f;
    float parentSize = (int)getAxisBar() == 0 ? m_rectArea->getSize()[0] : m_rectArea->getSize()[1];
    float remainingSize = parentSize * (1 - getSize());

    if (remainingSize <= 0) return;
    _updateDrag(handleCorner, remainingSize);
}

void UIScrollBar::_clickRepeat(Vec2 clickPoint)
{
    if (m_rectHandle == nullptr || m_rect == nullptr || m_rectArea == nullptr) return;

    if (!RectTransformUtility::RectangleContainsPoint(m_rectHandle.get(), clickPoint))
    {
        Vec2 localPoint = RectTransformUtility::WorldToLocalPointInRectable(m_rectHandle.get(), clickPoint);
        auto axisCoordinate = (int)getAxisBar() == 0 ? localPoint[0] : localPoint[1];
        float change = axisCoordinate < 0 ? m_size : -m_size;
        float value = m_value;
        value += (isReverseValue() ? -change : change);
        setValue(value);
    }

}

void UIScrollBar::_update()
{
    if (m_rectHandle == nullptr)
        return;
    Vec2 anchorMin(0, 0);
    Vec2 anchorMax(1, 1);
    auto offset = m_rectHandle->getOffset();
    float movement = MATH_CLAMP(1 - m_value, 0, 1.0f) * (1 - m_size);
    AxisBar axis = getAxisBar();
    if (isReverseValue())
    {
        anchorMin[(int)axis] = 1 - movement - m_size;
        anchorMax[(int)axis] = 1 - movement;
    }
    else
    {
        anchorMin[(int)axis] = movement;
        anchorMax[(int)axis] = movement + m_size;
    }
    m_rectHandle->setAnchor(Vec4(anchorMin[0], anchorMin[1], anchorMax[0], anchorMax[1]));
    m_rectHandle->setOffset(offset);
}

void UIScrollBar::_updateDrag(Vec2 handleCorner, float remainingSize)
{
    if (remainingSize == 0) {
        setValue(0);
        return;
    }
    
    switch (m_direction)
    {
    case Direction::LeftToRight:
        setValue(MATH_CLAMP(1.f - (handleCorner[0] / remainingSize), 0, 1));
        break;
    case Direction::RightToLeft:
        setValue(MATH_CLAMP((handleCorner[0] / remainingSize), 0, 1));
        break;
    case Direction::BottomToTop:
        setValue(MATH_CLAMP(1.f - (handleCorner[1] / remainingSize), 0, 1));
        break;
    case Direction::TopToBottom:
        setValue(MATH_CLAMP((handleCorner[1] / remainingSize), 0, 1));
        break;
    }
}

void UIScrollBar::_flipLayoutAxes(std::shared_ptr<RectTransform> rect, bool keepPositioning, bool recursive)
{
    if (rect == nullptr) return;

    if (recursive) 
    {
        auto children = rect->getOwner()->getChildren();
        int childCount = children.size();
        if (childCount > 0)
        {
            for (int i = 0; i < childCount; i++)
            {
                if (!children[i].expired()) {
                    auto childRect = children[i].lock()->getRectTransform();
                    if (childRect)
                    {
                        _flipLayoutAxes(childRect, false, true);
                    }
                }
            }
        }
    }

    rect->setPivot(GetTransposed(rect->getPivot()));
    rect->setSize(GetTransposed(rect->getSize()));
    if (keepPositioning)
        return;
    rect->setAnchoredPosition(GetTransposed(rect->getAnchoredPosition()));
    auto anchor = rect->getAnchor();
    Vec4 newAnchor(anchor[1], anchor[0], anchor[3], anchor[2]);
    rect->setAnchor(newAnchor);
}

void UIScrollBar::_flipLayoutOnAxis(std::shared_ptr<RectTransform> rect, int axis, bool keepPositioning, bool recursive)
{
    if (rect == nullptr) return;

    if (recursive)
    {
        auto children = rect->getOwner()->getChildren();
        int childCount = children.size();
        if (childCount > 0)
        {
            for (int i = 0; i < childCount; i++)
            {
                if (!children[i].expired()) {
                    auto childRect = children[i].lock()->getRectTransform();
                    if (childRect)
                    {
                        _flipLayoutOnAxis(childRect, axis, false, true);
                    }
                }
            }
        }
    }

    auto pivot = rect->getPivot();
    pivot[axis] = 1.0f - pivot[axis];
    rect->setPivot(pivot);

    if (keepPositioning)
        return;
    
    auto anchoredPos = rect->getAnchoredPosition();
    anchoredPos[axis] = -anchoredPos[axis];
    rect->setAnchoredPosition(anchoredPos);

    auto anchor = rect->getAnchor();
    float temp = anchor[axis];
    anchor[axis] = 1 - anchor[2 + axis];
    anchor[2 + axis] = 1 - temp; //! Anchor Max
    rect->setAnchor(anchor);

}

void UIScrollBar::setHandle(std::shared_ptr<SceneObject> obj)
{
    if (obj) 
    {
        if (m_rectHandle) {
            if (m_rectHandle->getOwner() != nullptr)
                m_rectHandle->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
            m_rectHandle = nullptr;
            m_imgHandle = nullptr;
        }

        m_rectHandle = std::dynamic_pointer_cast<RectTransform>(obj->getTransform());
        m_imgHandle = obj->getComponent<UIImage>();
        obj->addEventListener((int)EventType::Delete, [this](auto val) {
            m_rectHandle = nullptr;
            m_imgHandle = nullptr;
            }, m_instanceId);

        if (obj->getParent() != nullptr) {
            m_rectArea = std::dynamic_pointer_cast<RectTransform>(obj->getParent()->getTransform());
        }
    }
    else
    {
        if (m_rectHandle) {
            if (m_rectHandle->getOwner() != nullptr)
                m_rectHandle->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
            m_rectHandle = nullptr;
        }
        m_imgHandle = nullptr;

        if (m_rectArea) {
            m_rectArea = nullptr;
        }
    }
}

void UIScrollBar::setValue(float value) {
    float _value = MATH_CLAMP(value, 0, 1.0f);
    if (_value != m_value) {
        m_value = _value;
        _update();
        getOwner()->dispatchEvent((int)EventType::Changed, Value(m_value));
        getOnValueChanged().invoke(m_value);
    }
}

void UIScrollBar::setValueWithoutNotify(float value)
{
    float _value = MATH_CLAMP(value, 0, 1.0f);
    if (_value != m_value) {
        m_value = _value;
        _update();
        getOwner()->dispatchEvent((int)EventType::Changed, Value(m_value));
    }
}

void UIScrollBar::setSize(float value)
{
    float _value = MATH_CLAMP(value, 0, 1.0f);
    if (_value != m_size) {
        m_size = _value;
        _update();
    }
}

void UIScrollBar::setDirection(Direction value, bool flip)
{
    if (m_direction != value) 
    {
        AxisBar oldAxis = getAxisBar();
        bool oldReverse = isReverseValue();

        m_direction = value;

        if (flip)
        {
            if (oldAxis != getAxisBar())
                _flipLayoutAxes(m_rectHandle, true, true);

            if (oldReverse != isReverseValue())
                _flipLayoutOnAxis(m_rectHandle, (int)getAxisBar(), true, true);
        }
        _update();
    }
}

void UIScrollBar::setDirection(int value, bool flip)
{
    setDirection((Direction)value, flip);
}

UIScrollBar::AxisBar UIScrollBar::getAxisBar() const
{
    return ((m_direction == Direction::LeftToRight || m_direction == Direction::RightToLeft) ? AxisBar::Horizontal : AxisBar::Vertical);
}

bool UIScrollBar::isReverseValue() const
{
    return m_direction == Direction::RightToLeft || m_direction == Direction::TopToBottom;
}

//! Set Normal Color
void UIScrollBar::setNormalColor(const Vec4& value)
{
    if (m_normalColor != value) {
        m_normalColor = value;
        if (m_imgHandle != nullptr)
            m_imgHandle->setColor(m_normalColor);
    }
}

void UIScrollBar::setNormalColor(float r, float g, float b, float a) {
    Vec4 value(r, g, b, a);
    value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
    value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
    value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
    value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
    setNormalColor(value);
}

//! Set Pressed Color
void UIScrollBar::setPressedColor(const Vec4& value)
{
    if (m_pressedColor != value) {
        m_pressedColor = value;
    }
}

void UIScrollBar::setPressedColor(float r, float g, float b, float a) {
    Vec4 value(r, g, b, a);
    value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
    value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
    value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
    value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
    setPressedColor(value);
}

//! Set Disabled Color
void UIScrollBar::setDisabledColor(const Vec4& value)
{
    if (m_disableColor != value) {
        m_disableColor = value;
    }
}

void UIScrollBar::setDisabledColor(float r, float g, float b, float a) {
    Vec4 value(r, g, b, a);
    value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
    value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
    value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
    value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
    setDisabledColor(value);
}

Vec4 UIScrollBar::getColorByState(int state) const
{
    switch (state) {
    case 0:
    default:
        return m_normalColor;
    case 1:
        return m_pressedColor;
    case 2:
        return m_disableColor;
    }
}

void UIScrollBar::changeState(int state, bool forced)
{
    if (m_imgHandle == nullptr) return;

    if (m_handleState != state || forced)
    {
        m_handleState = state;
        auto col = m_imgHandle->getColor();
        auto t_col = getColorByState(m_handleState);
        if (col != t_col) {
            if (m_tween != nullptr) {
                m_tween->kill();
                m_tween = nullptr;
            }

            m_tween = Tween::tween(col, t_col, m_fadeDuration);
            m_tween->onUpdate([this](Tweener* tweener) {
                m_imgHandle->setColor(tweener->value.getVec4());
                })->onComplete([this]() {});

        }
        else if (col != m_normalColor) {
            m_imgHandle->setColor(m_normalColor);
        }
    }
}

//! Json 
void UIScrollBar::to_json(json& j) const
{
    UIImage::to_json(j);
    j["content"] = m_rectHandle ? m_rectHandle->getOwner()->getUUID() : "";
    j["directionbar"] = (int)getDirection();
    j["valuebar"] = getValue();
    j["sizebar"] = getSize();
    j["normalcolor"] = getNormalColor();
    j["pressedcolor"] = getPressedColor();
    j["disabledcolor"] = getDisabledColor();
    j["fadeduration"] = getFadeDuration();
}

//! Deserialize
void UIScrollBar::from_json(const json& j)
{
    UIImage::from_json(j);
    m_contentUUID = j.value("content", "");
    m_direction = (Direction)j.value("directionbar", 0);
    m_value = j.value("valuebar", 1.0f);
    m_size = j.value("sizebar", 0.2f);
    setNormalColor(j.at("normalcolor"));
    setPressedColor(j.at("pressedcolor"));
    setDisabledColor(j.at("disabledcolor"));
    setFadeDuration(j.at("fadeduration"));
    m_dirtySetObj = true;
    _update();
}

void UIScrollBar::onSerializeFinished(Scene* scene)
{
    if (m_dirtySetObj) {
        m_dirtySetObj = false;
        if (!m_contentUUID.empty()) {
            auto obj = scene->findObjectByUUID(m_contentUUID);
            if (obj) {
                setHandle(obj);
            }
        }
    }
}

//! Update property by key value
void UIScrollBar::setProperty(const std::string& key, const json& val)
{
    if (key.compare("content") == 0)
    {
        m_contentUUID = val;
    }
    else if (key.compare("directionbar") == 0)
    {
        m_direction = val;
    }
    else if (key.compare("valuebar") == 0)
    {
        m_value = val;
    }
    else if (key.compare("sizebar") == 0)
    {
        m_size = val;
    }
    else if (key.compare("normalcolor") == 0)
    {
        setNormalColor(val);
    }
    else if (key.compare("pressedcolor") == 0)
    {
        setPressedColor(val);
    }
    else if (key.compare("disabledcolor") == 0)
    {
        setDisabledColor(val);
    }
    else if (key.compare("fadeduration") == 0)
    {
        setFadeDuration(val);
    }
    else
    {
        UIImage::setProperty(key, val);
    }

    m_dirtySetObj = true;
    _update();
}

NS_IGE_SCENE_END
