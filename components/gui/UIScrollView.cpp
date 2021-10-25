#include "components/gui/UIScrollView.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"

#include "event/EventContext.h"

#include "components/tween/Tween.h"
#include "components/tween/Tweener.h"

#include "components/gui/UIScrollBar.h"

namespace fs = ghc::filesystem;

NS_IGE_SCENE_BEGIN

static inline float sp_EaseFunc(float t, float d)
{
    t = t / d - 1;
    return t * t * t + 1; 
}

static inline Vec2 lerp(Vec2& target, const Vec2& other, float alpha)
{
    return target * (1.f - alpha) + other * alpha;
}

static inline float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float deltaTime)
{
    // Based on Game Programming Gems 4 Chapter 1.10
    smoothTime = MATH_MAX(0.0001F, smoothTime);
    float omega = 2.0f / smoothTime;

    float x = omega * deltaTime;
    float exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float originalTo = target;

    // Clamp maximum speed
    //float maxChange = maxSpeed * smoothTime;
    //change = MATH_CLAMP(change, -maxChange, maxChange);
    target = current - change;

    float temp = (currentVelocity + omega * change) * deltaTime;
    currentVelocity = (currentVelocity - omega * temp) * exp;
    float output = target + (change + temp) * exp;

    // Prevent overshooting
    if (originalTo - current > 0.0F == output > originalTo)
    {
        output = originalTo;
        currentVelocity = (output - originalTo) / deltaTime;
    }

    return output;
}

static const float TWEEN_TIME_DEFAULT = 0.3f;
static const float TWEEN_SCALE_VELOCITY_DEFAULT = 0.8133f;
static const float NEARLY_ZERO = 0.001f;

UIScrollView::UIScrollView(SceneObject& owner, const std::string& texture, const Vec2& size, const bool isSliced, const Vec4& border) :
	UIImage(owner, texture, size, isSliced, border), 
    m_dirtySetObj(false), m_rectContent(nullptr), m_rectViewport(nullptr),
    m_bVertical(true), m_bHorizontal(true), m_bIsDragging(false), 
    m_movementType(MovementType::Elastic), m_elasticity(0.1f) , m_elasticExtra(Vec2(100, 100)),
    m_bInertia(true), m_decelerationRate(0.135f),
    m_horizontalScrollBar(nullptr), m_verticalScrollBar(nullptr), 
    m_eventHorizontalScrollBarId(-1) , m_eventVerticalScrollBarId(-1), 
    m_tweening(0), m_deltaTime(0.f), m_tweenStart(Vec2(0,0)), m_tweenChange(Vec2(0,0))
{
    m_deltaPos[0] = 0;
    m_deltaPos[1] = 0;

    m_currentContentSize[0] = 0;
    m_currentContentSize[1] = 0;
    m_currentViewportSize[0] = 0;
    m_currentViewportSize[1] = 0;

    m_velocity[0] = 0;
    m_velocity[1] = 0;

    m_bIsInteractable = true;
    getOwner()->setIsInteractable(m_bIsInteractable);
    getOwner()->setIsRaycastTarget(m_bIsInteractable);
    
    getOwner()->addEventListener((int)EventType::TouchBegin, std::bind(&UIScrollView::_onTouchPress, this, std::placeholders::_1), m_instanceId);
    getOwner()->addEventListener((int)EventType::TouchEnd, std::bind(&UIScrollView::_onTouchRelease, this, std::placeholders::_1), m_instanceId);
    getOwner()->addEventListener((int)EventType::TouchMove, std::bind(&UIScrollView::_onTouchDrag, this, std::placeholders::_1), m_instanceId);

    
}

UIScrollView::~UIScrollView()
{
    getOwner()->removeEventListener((int)EventType::TouchBegin, m_instanceId);
    getOwner()->removeEventListener((int)EventType::TouchEnd, m_instanceId);
    getOwner()->removeEventListener((int)EventType::TouchMove, m_instanceId);
    
    if (m_rectContent) {
        if (m_rectContent->getOwner() != nullptr)
            m_rectContent->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
        m_rectContent = nullptr;
    }

    if (m_rectViewport) {
        if (m_rectViewport->getOwner() != nullptr)
            m_rectViewport->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
        m_rectViewport = nullptr;
    }

    if (m_horizontalScrollBar) 
    {
        if (m_horizontalScrollBar->getOwner() != nullptr)
        {
            m_horizontalScrollBar->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
        }
        if (m_eventHorizontalScrollBarId != -1) {
            m_horizontalScrollBar->getOnValueChanged().removeListener(m_eventHorizontalScrollBarId);
            m_eventHorizontalScrollBarId = -1;
        }
        m_horizontalScrollBar = nullptr;
    }

    if (m_verticalScrollBar)
    {
        if (m_verticalScrollBar->getOwner() != nullptr)
        {
            m_verticalScrollBar->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
        }
        if (m_eventVerticalScrollBarId != -1) {
            m_verticalScrollBar->getOnValueChanged().removeListener(m_eventVerticalScrollBarId);
            m_eventVerticalScrollBarId = -1;
        }
        m_verticalScrollBar = nullptr;
    }

}

void UIScrollView::_onTouchPress(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    auto inputContext = dynamic_cast<InputEventContext*>(context);
    if (inputContext == nullptr) return;
    inputContext->captureTouch();

    if (m_tweening != 0)
    {
        _killTween();
    }


    _updateSize();
    
    m_clickedPos = inputContext->getInput()->getPosition();

    m_velocity[0] = 0;
    m_velocity[1] = 0;
    m_lastMoveTime = clock();
    getOnPressedEvent().invoke();
}

void UIScrollView::_onTouchRelease(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    bool flag = false;
    if (m_deltaPos[0] < m_boundContent[0] || m_deltaPos[0] > m_boundContent[2])
    {
        flag = true;
    }
    else if (m_deltaPos[1] < m_boundContent[1] || m_deltaPos[1] > m_boundContent[3])
    {
        flag = true;
    }
    m_tweenStart = m_deltaPos;

    if (m_bInertia)
    {
        Vec2 endPos = m_tweenStart;
        float elapsed = (clock() - m_lastMoveTime) / (double)CLOCKS_PER_SEC;
        elapsed = elapsed * 60 - 1;
        if (elapsed > 1)
            m_velocity = m_velocity * pow(TWEEN_SCALE_VELOCITY_DEFAULT, elapsed);

        endPos[0] = _updateInertiaDuration(m_tweenStart[0], 0);
        endPos[1] = _updateInertiaDuration(m_tweenStart[1], 1);

        m_tweenChange = endPos - m_tweenStart;

        if (m_velocity[0] != 0 || m_velocity[1] != 0)
            _startTween(2);
        else if (flag)
            _startTween(3);
    }
    else if (flag)
    {
        _startTween(3);
    }

    getOnReleasedEvent().invoke();
}

void UIScrollView::_onTouchDrag(EventContext* context)
{
    if (!isInteractable()) return;
    context->stopPropagation();
    auto inputContext = dynamic_cast<InputEventContext*>(context);
    if (inputContext == nullptr) return;
    if (inputContext->getInput() == nullptr) return;
    if (m_rectContent == nullptr) return;
    auto size = m_rectContent->getSize();
    auto clickPoint = inputContext->getInput()->getPosition();
    auto point = m_rectContent->globalToLocal(Vec3(clickPoint[0], clickPoint[1], m_rectContent->getPosition()[3]));

    Vec2 delta = clickPoint - m_clickedPos;

    _scroll(delta);

    m_clickedPos = clickPoint;

    auto deltaTime = m_deltaTime;
    clock_t lastTime = clock();
    float elapsed = (lastTime - m_lastMoveTime) / (double)CLOCKS_PER_SEC;
    elapsed = elapsed * 60 - 1;
    if (elapsed > 1)
        m_velocity = m_velocity * pow(TWEEN_SCALE_VELOCITY_DEFAULT, elapsed);
    m_velocity = lerp(m_velocity, delta / (deltaTime * 2), deltaTime * 10);
    m_lastMoveTime = lastTime;
    m_bIsDragging = true;
    getOnDragEvent().invoke();
}

//! Update
void UIScrollView::onUpdate(float dt)
{
    UIImage::onUpdate(dt);

    _updateSize();
    
    m_deltaTime = dt;
    if (m_tweening > 0) 
    {
        _tweenUpdate(dt);
    }
}

//! Render
void UIScrollView::onRender()
{
    UIImage::onRender();
}

void UIScrollView::setContent(std::shared_ptr<SceneObject> obj)
{
    if (obj) 
    {
        
        if (m_rectContent) {
            if (m_rectContent->getOwner() != nullptr)
                m_rectContent->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
            m_rectContent = nullptr;
        }

        m_rectContent = std::dynamic_pointer_cast<RectTransform>(obj->getTransform());
        obj->addEventListener((int)EventType::Delete, [this](auto val) {
            m_rectContent = nullptr;
            }, m_instanceId);
        _updateSize();
    }
    else 
    {
        if (m_rectContent) {
            if (m_rectContent->getOwner() != nullptr)
                m_rectContent->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
            m_rectContent = nullptr;
        }
    }
}

void UIScrollView::setViewport(std::shared_ptr<SceneObject> obj)
{
    if (obj)
    {

        if (m_rectViewport) {
            if (m_rectViewport->getOwner() != nullptr)
                m_rectViewport->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
            m_rectViewport = nullptr;
        }

        m_rectViewport = std::dynamic_pointer_cast<RectTransform>(obj->getTransform());
        obj->addEventListener((int)EventType::Delete, [this](auto val) {
            m_rectViewport = nullptr;
            }, m_instanceId);
        _updateSize();
    }
    else
    {
        if (m_rectViewport) {
            if (m_rectViewport->getOwner() != nullptr)
                m_rectViewport->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
            m_rectViewport = nullptr;
        }
    }
}

void UIScrollView::_updateSize(bool forced)
{
    bool flag = false;
    if (m_rectViewport)
    {
        auto size = m_rectViewport->getSize();
        if (m_currentViewportSize != size || forced)
        {
            m_currentViewportSize = size;
            flag = true;
        }
    }

    if (m_rectContent)
    {
        auto size = m_rectContent->getSize();
        if (m_currentContentSize != size || forced || flag)
        {
            flag = true;
            m_currentContentSize = size;
            auto pivot = m_rectContent->getPivot();
            auto pos = m_rectContent->getLocalPosition();
            auto rSize = m_currentViewportSize;
            if (rSize[0] == 0 && rSize[1] == 0) {
                auto rect = getOwner()->getRectTransform();
                if (rect == nullptr) return;
                rSize = rect->getSize();
            }

            Vec2 excess = rSize - size;
            if (excess[0] > 0)
            {
                size[0] = rSize[0];
            }

            if (excess[1] > 0)
            {
                size[1] = rSize[1];
            }

            m_boundContent[0] = MATH_MIN((rSize[0] - size[0]), 0);
            m_boundContent[1] = 0;
            m_boundContent[2] = 0;
            m_boundContent[3] = MATH_MAX((size[1] - rSize[1]), 0);
        }
    }

    if(flag || forced)
        _updateScrollBars(Vec2(0, 0));
}

void UIScrollView::_scroll(Vec2 delta)
{
    Vec2 _delta = m_deltaPos;
    _delta[0] += delta[0];
    _delta[1] += delta[1];
    scrollTo(_delta);
}

void UIScrollView::scrollTo(Vec2 delta)
{
    Vec2 _delta(0,0);
    switch (m_movementType)
    {
    case MovementType::Clamped:
        {
            _delta[0] = MATH_CLAMP(delta[0], m_boundContent[0], m_boundContent[2]);
            _delta[1] = MATH_CLAMP(delta[1], m_boundContent[1], m_boundContent[3]);
        }
        break;
    case MovementType::Elastic:
        {
            _delta[0] = MATH_CLAMP(delta[0], m_boundContent[0] - m_elasticExtra[0], m_boundContent[2] + m_elasticExtra[0]);
            _delta[1] = MATH_CLAMP(delta[1], m_boundContent[1] - m_elasticExtra[1], m_boundContent[3] + m_elasticExtra[1]);
        }
        break;
    }

    _scrollTo(_delta);
}

void UIScrollView::_scrollTo(Vec2 delta, bool updateScrollBar)
{
    if (m_rectContent == nullptr) return;
    Vec2 fixDelta((int)delta[0], (int)delta[1]);
    if (fixDelta == m_deltaPos) return;
    auto anchoredPos = m_rectContent->getAnchoredPosition();

    if (m_bHorizontal)
    {
        auto m = fixDelta[0] - m_deltaPos[0];
        anchoredPos[0] += m;
        m_deltaPos[0] = fixDelta[0];
    }

    if (m_bVertical)
    {
        auto m = fixDelta[1] - m_deltaPos[1];
        anchoredPos[1] += m;
        m_deltaPos[1] = fixDelta[1];
    }

    m_rectContent->setAnchoredPosition(anchoredPos);
    if(updateScrollBar) 
        _updateScrollBars(Vec2(0, 0));
}

float UIScrollView::_updateInertiaDuration(float pos, int axis)
{
    float v = m_velocity[axis];
    float duration = 0;
    float v2 = std::abs(v);
    float ratio = 0;

    if(v2 > 50) ratio = pow(v2, 2);
    
    if (ratio != 0 && m_decelerationRate > 0)
    {
        if (std::abs(m_velocity[axis]) < 1)
            m_velocity[axis] = 0;
        float change = (int)(m_velocity[axis] * m_deltaTime * 10);
        pos += change;
    }
    else
        m_velocity[axis] = 0;


    return pos;
}

void UIScrollView::_updateScrollBars(Vec2 offset) 
{
    if (m_horizontalScrollBar)
    {
        auto sizeW = m_boundContent[2] - m_boundContent[0];
        if (sizeW > 0)
        {
            float size = MATH_CLAMP((m_currentViewportSize[0] - std::abs(offset[0])) / m_currentContentSize[0], 0.f, 1.0f);
            m_horizontalScrollBar->setSize(size);
        }
        else
            m_horizontalScrollBar->setSize(1);
        float h = _getHorizontalNormalizedPosition();
        m_horizontalScrollBar->setValueWithoutNotify(h);
    }

    if (m_verticalScrollBar)
    {
        auto sizeH = m_boundContent[3] - m_boundContent[1];
        if (sizeH > 0)
        {
            float size = MATH_CLAMP((m_currentViewportSize[1] - std::abs(offset[1])) / m_currentContentSize[1], 0.f, 1.0f);
            m_verticalScrollBar->setSize(size);
        }
        else
            m_verticalScrollBar->setSize(1);
        float v = _getVerticalNormalizedPosition();
        m_verticalScrollBar->setValueWithoutNotify(v);
    }
}

void UIScrollView::_startTween(int type)
{
    m_tweening = type;
}

void UIScrollView::_killTween()
{
    if (m_tweening == 1)
    {
        Vec2 t = m_tweenStart + m_tweenChange;
        scrollTo(t);
    }
    m_tweening = 0;
    m_velocity[0] = 0;
    m_velocity[1] = 0;
}

void UIScrollView::_tweenUpdate(float dt)
{
    float nx = _runTween(0, dt);
    float ny = _runTween(1, dt);
    
    scrollTo(Vec2(nx, ny));

    if (m_tweenChange[0] == 0 && m_tweenChange[1] == 0)
    {
        m_tweening = 0;             
    }
}

float UIScrollView::_runTween(int axis, float dt)
{
    float value = m_deltaPos[axis];

    float threshold1 = m_boundContent[axis];
    float threshold2 = m_boundContent[2 + axis];

    if (m_velocity[axis] != 0)
    {
        if (m_tweening == 3) 
        {
            float speed = m_velocity[axis];
            float smoothTime = m_elasticity;
            float target = MATH_CLAMP(value, threshold1, threshold2);
            if (target != value) {
                m_tweenChange[axis] = SmoothDamp(value, target, speed, smoothTime, m_deltaTime);
                if (std::abs(speed) < 1)
                    speed = 0;
                m_velocity[axis] = speed;
                value = m_tweenChange[axis];
            }
        }
        else
        {
            m_velocity[axis] *= pow(m_decelerationRate, m_deltaTime);
            if (std::abs(m_velocity[axis]) < 1)
                m_velocity[axis] = 0;
            m_tweenChange[axis] += m_velocity[axis] * m_deltaTime;
            value = m_tweenStart[axis] + m_tweenChange[axis];
        }
        

        if (m_tweening == 2)
        {
            if ((value < threshold1 && m_tweenChange[axis] < 0) || (value < threshold1 && m_tweenChange[axis] == 0))
            {
                m_tweenChange[axis] = threshold1 - value;
                m_tweenStart[axis] = value;
                m_velocity[axis] = 0;
                value = m_tweenStart[axis] + m_tweenChange[axis];
            }
            else if ((value > threshold2 && m_tweenChange[axis] > 0) || (value > threshold2 && m_tweenChange[axis] == 0))
            {
                m_tweenChange[axis] = threshold2 - value;
                m_tweenStart[axis] = value;
                m_velocity[axis] = 0;
                value = m_tweenStart[axis] + m_tweenChange[axis];
            }
        }
        else
        {
            if (value <= threshold1)
            {
                value = threshold1;
                m_tweenChange[axis] = 0;
            }
            else if (value >= threshold2)
            {
                value = threshold2;
                m_tweenChange[axis] = 0;
            }
        }
    }
    else
    {
        if (value < threshold1 || value > threshold2)
        {
            float speed = m_velocity[axis];
            float smoothTime = m_elasticity;
            float target = MATH_CLAMP(value, threshold1, threshold2);
            if (target != value) {
                m_tweenChange[axis] = SmoothDamp(value, target, speed, smoothTime, m_deltaTime);
                if (std::abs(speed) < 1)
                    speed = 0;
                m_velocity[axis] = speed;
                value = m_tweenChange[axis];
            }
        }
        else
            value = m_deltaPos[axis];
    }
    return value;
}

void UIScrollView::setEnableHorizontal(bool value)
{
    if (m_bHorizontal != value) 
    {
        //! Update Drag Pos
        if (!value) 
        {
            Vec2 delta = m_deltaPos;
            delta[0] = 0;
            scrollTo(delta);
        }
        m_bHorizontal = value;
    }
}

void UIScrollView::setEnableVertical(bool value)
{
    if (m_bVertical != value) 
    {
        //! Update Drag Pos
        if (!value)
        {
            Vec2 delta = m_deltaPos;
            delta[1] = 0;
            scrollTo(delta);
        }
        m_bVertical = value;
        
    }
}

void UIScrollView::setMovementType(MovementType type)
{
    if (type != m_movementType) {
        m_movementType = type;
        scrollTo(m_deltaPos);
    }
}

void UIScrollView::setMovementType(int type)
{
    setMovementType((MovementType)type);
}

void UIScrollView::setElasticExtra(Vec2 value)
{
    m_elasticExtra = value;
    _updateSize(true);
}

void UIScrollView::setElasticity(float value)
{
    m_elasticity = value > 0 ? value : 0;
}

void UIScrollView::setDecelerationRate(float value)
{
    m_decelerationRate = value > 0 ? value : 0;
}

void UIScrollView::setHorizontalScrollBar(std::shared_ptr<UIScrollBar> obj)
{
    if (obj != m_horizontalScrollBar)
    {
        if (m_horizontalScrollBar)
        {
            if (m_horizontalScrollBar->getOwner() != nullptr)
            {
                m_horizontalScrollBar->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
            }

            if (m_eventHorizontalScrollBarId != -1) {
                m_horizontalScrollBar->getOnValueChanged().removeListener(m_eventHorizontalScrollBarId);
                m_eventHorizontalScrollBarId = -1;
            }

            m_horizontalScrollBar = nullptr;
        }

        m_horizontalScrollBar = obj;
        if (m_horizontalScrollBar) {
            obj->getOwner()->addEventListener((int)EventType::Delete, [this](auto val) {
                m_horizontalScrollBar = nullptr;
                }, m_instanceId);

            m_eventHorizontalScrollBarId = m_horizontalScrollBar->getOnValueChanged().addListener(std::bind(&UIScrollView::_setHorizontalNormalizedPosition, this, std::placeholders::_1));
        }
        if (m_bHorizontal)
            _updateSize(true);
    }
}

void UIScrollView::setVerticalScrollBar(std::shared_ptr<UIScrollBar> obj)
{
    if (obj != m_verticalScrollBar)
    {
        if (m_verticalScrollBar)
        {
            if (m_verticalScrollBar->getOwner() != nullptr)
            {
                m_verticalScrollBar->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
            }

            if (m_eventVerticalScrollBarId != -1) {
                m_verticalScrollBar->getOnValueChanged().removeListener(m_eventVerticalScrollBarId);
                m_eventVerticalScrollBarId = -1;
            }
            m_verticalScrollBar = nullptr;
        }

        m_verticalScrollBar = obj;
        if (m_verticalScrollBar) {
            obj->getOwner()->addEventListener((int)EventType::Delete, [this](auto val) {
                m_verticalScrollBar = nullptr;
                }, m_instanceId);
            m_eventVerticalScrollBarId = m_verticalScrollBar->getOnValueChanged().addListener(std::bind(&UIScrollView::_setVerticalNormalizedPosition, this, std::placeholders::_1));
        }
        if (m_bVertical)
            _updateSize(true);
    }
}

float UIScrollView::_getHorizontalNormalizedPosition()
{
    if (m_rectContent == nullptr) return 1;
    
    if (m_boundContent[2] <= m_boundContent[0] || std::abs(m_boundContent[2] - m_boundContent[0]) <= NEARLY_ZERO) 
        return (m_boundView[0] > m_boundContent[0]) ? 1 : 0;
    
    return (m_deltaPos[0] - m_boundContent[0]) / (m_boundContent[2] - m_boundContent[0]);
}

void UIScrollView::_setHorizontalNormalizedPosition(float value)
{
    _setNormalizedPosition(value, 0);
}

float UIScrollView::_getVerticalNormalizedPosition()
{
    if (m_rectContent == nullptr) return 1;
    
    if (m_boundContent[3] <= m_boundContent[1] || std::abs(m_boundContent[3] - m_boundContent[1]) <= NEARLY_ZERO) 
        return (m_boundView[2] > m_boundContent[2]) ? 1 : 0;

    return (m_deltaPos[1] - m_boundContent[1]) / (m_boundContent[3] - m_boundContent[1]);
}

void UIScrollView::_setVerticalNormalizedPosition(float value)
{
    _setNormalizedPosition(value, 1);
}

void UIScrollView::_setNormalizedPosition(float value, int axis)
{
    if (m_rectContent == nullptr) return;

    if (m_tweening != 0)
    {
        _killTween();
    }

    if (m_boundContent[2 + axis] <= m_boundContent[axis] || std::abs(m_boundContent[2 + axis] - m_boundContent[axis]) <= NEARLY_ZERO)
    {
        auto delta = m_deltaPos;
        delta[axis] = 0;
        _scrollTo(delta, false);
        return;
    }

    float s = value * (m_boundContent[2 + axis] - m_boundContent[axis]) + m_boundContent[axis];
    auto delta = m_deltaPos;
    delta[axis] = s;
    _scrollTo(delta, false);
}

//! Json 
void UIScrollView::to_json(json& j) const
{
    UIImage::to_json(j);
    j["enablehorizontal"] = enableHorizontal();
    j["enablevertical"] = enableVertical();
    j["content"] = m_rectContent ? m_rectContent->getOwner()->getUUID() : "";
    j["viewport"] = m_rectViewport ? m_rectViewport->getOwner()->getUUID() : "";
    j["horizontalscrollbar"] = m_horizontalScrollBar ? m_horizontalScrollBar->getOwner()->getUUID() : "";
    j["verticalscrollbar"] = m_verticalScrollBar ? m_verticalScrollBar->getOwner()->getUUID() : "";
    j["movementtype"] = (int)getMovementType();
    j["elasticextra"] = getElasticExtra();
    j["elasticity"] = getElasticity();
    j["inertia"] = isInertia();
    j["decelerationrate"] = getDecelerationRate();


}

//! Deserialize
void UIScrollView::from_json(const json& j)
{
    UIImage::from_json(j);
    m_contentUUID = j.value("content", "");
    m_viewportUUID = j.value("viewport", "");
    m_horizontalUUID = j.value("horizontalscrollbar", "");
    m_verticalUUID = j.value("verticalscrollbar", "");
    setEnableHorizontal(j.at("enablehorizontal"));
    setEnableVertical(j.at("enablevertical"));
    setMovementType((int)j.at("movementtype"));
    setElasticExtra(j.at("elasticextra"));
    setElasticity(j.at("elasticity"));
    setInertia(j.at("inertia"));
    setDecelerationRate(j.at("decelerationrate"));
    m_dirtySetObj = true;
}

void UIScrollView::onSerializeFinished(Scene* scene)
{
    if (m_dirtySetObj) {
        m_dirtySetObj = false;
        if (!m_contentUUID.empty()) 
        {
            auto obj = scene->findObjectByUUID(m_contentUUID);
            if (obj) {
                setContent(obj);
            }
        }

        if (!m_viewportUUID.empty())
        {
            auto obj = scene->findObjectByUUID(m_viewportUUID);
            if (obj) {
                setViewport(obj);
            }
        }

        if (!m_horizontalUUID.empty())
        {
            auto obj = scene->findObjectByUUID(m_horizontalUUID);
            if (obj) {
                auto uiHorizontalBar = obj->getComponent<UIScrollBar>();
                if(uiHorizontalBar)
                    setHorizontalScrollBar(uiHorizontalBar);
            }
        }

        if (!m_verticalUUID.empty())
        {
            auto obj = scene->findObjectByUUID(m_verticalUUID);
            if (obj) {
                auto uiVerticalBar = obj->getComponent<UIScrollBar>();
                if(uiVerticalBar)
                    setVerticalScrollBar(uiVerticalBar);
            }
        }
    }
}

//! Update property by key value
void UIScrollView::setProperty(const std::string& key, const json& val)
{
    if (key.compare("content") == 0)
    {
        m_contentUUID = val;
    }
    else if (key.compare("viewport") == 0)
    {
        m_viewportUUID = val;
    }
    else if (key.compare("horizontalscrollbar") == 0)
    {
        m_horizontalUUID = val;
    }
    else if (key.compare("verticalscrollbar") == 0)
    {
        m_verticalUUID = val;
    }
    else if (key.compare("enablehorizontal") == 0)
    {
        setEnableHorizontal(val);
    }
    else if (key.compare("enablevertical") == 0)
    {
        setEnableVertical(val);
    }
    else if (key.compare("movementtype") == 0)
    {
        setMovementType((int)val);
    }
    else if (key.compare("elasticextra") == 0)
    {
        setElasticExtra(val);
    }
    else if (key.compare("elasticity") == 0)
    {
        setElasticity(val);
    }
    else if (key.compare("inertia") == 0)
    {
        setInertia(val);
    }
    else if (key.compare("decelerationrate") == 0)
    {
        setDecelerationRate(val);
    }
    else
    {
        UIImage::setProperty(key, val);
    }

    m_dirtySetObj = true;
}

NS_IGE_SCENE_END