#include "event/InputProcessor.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

NS_IGE_SCENE_BEGIN

class TouchInfo
{
public:
    TouchInfo();
    ~TouchInfo();

    void reset();

    Vec2 pos;
    int touchId;
    int clickCount;
    Vec2 downPos;
    bool began;
    bool clickCancelled;
    clock_t lastClickTime;
    std::vector<uint64_t> downTargets;
    std::vector<uint64_t> touchMonitors;
};

TouchInfo::TouchInfo() :
    touchId(-1),
    clickCount(0),
    began(false),
    lastClickTime(0),
    clickCancelled(false)
{
}

TouchInfo::~TouchInfo()
{
    touchMonitors.clear();
}

void TouchInfo::reset()
{
    touchId = -1;
    pos.X(0);
    pos.Y(0);
    downPos.X(0);
    downPos.Y(0);
    clickCount = 0;
    lastClickTime = 0;
    began = false;
    clickCancelled = false;
    touchMonitors.clear();
}


//!============================================================
//!============================================================
//!============================================================

bool InputProcessor::m_touchOnUI = false;
InputProcessor* InputProcessor::m_activeProcessor = nullptr;

InputProcessor::InputProcessor()  : m_captureCallback(nullptr)
{
    m_recentInput.m_inputProcessor = this;

}

InputProcessor::~InputProcessor()
{
    for (auto& ti : m_touches)
        delete ti;
}

Vec2 InputProcessor::getTouchPosition(int touchId)
{
    for (auto& ti : m_touches)
    {
        if (ti->touchId == touchId)
            return ti->pos;
    }
    return m_recentInput.getPosition();
}

void InputProcessor::addTouchMonitor(int touchId, SceneObject* target)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (!ti)
        return;

    auto it = std::find(ti->touchMonitors.cbegin(), ti->touchMonitors.cend(), target->getId());
    if (it == ti->touchMonitors.cend())
        ti->touchMonitors.push_back(target->getId());
}

void InputProcessor::removeTouchMonitor(SceneObject* target)
{
    for (auto it = m_touches.cbegin(); it != m_touches.cend(); ++it)
    {
        auto it2 = std::find((*it)->touchMonitors.begin(), (*it)->touchMonitors.end(), target->getId());
        if (it2 != (*it)->touchMonitors.end())
            *it2 = 0;
    }
}

void InputProcessor::cancelClick(int touchId)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (ti)
        ti->clickCancelled = true;
}

TouchInfo* InputProcessor::getTouch(int touchId, bool createIfNotExists)
{
    TouchInfo* ret = nullptr;
    for (auto& ti : m_touches)
    {
        if (ti->touchId == touchId)
            return ti;
        else if (ti->touchId == -1)
            ret = ti;
    }

    if (!ret)
    {
        if (!createIfNotExists)
            return nullptr;

        ret = new TouchInfo();
        m_touches.push_back(ret);
    }
    ret->touchId = touchId;
    return ret;
}

void InputProcessor::updateRecentInput(TouchInfo* ti, SceneObject* target)
{
    //Cast int to get round
    m_recentInput.m_position.X((int)ti->pos.X());
    m_recentInput.m_position.Y((int)ti->pos.Y());
    m_recentInput.m_target = target;
    m_recentInput.m_clickCount = ti->clickCount;
    m_recentInput.m_touchId = ti->touchId;
}

void InputProcessor::handleRollOver(TouchInfo* touch, SceneObject* target)
{
    
}

void InputProcessor::setBegin(TouchInfo* touch, SceneObject* target)
{
    touch->began = true;
    touch->clickCancelled = false;
    touch->downPos = touch->pos;

    touch->downTargets.clear();
    SceneObject* obj = target;
    while (obj != nullptr)
    {
        touch->downTargets.push_back(obj->getId());
        obj = obj->getParent();
    }
}

void InputProcessor::setEnd(TouchInfo* touch, SceneObject* target)
{
    touch->began = false;

    auto now = clock();
    float elapsed = (now - touch->lastClickTime) / (double)CLOCKS_PER_SEC;

    if (elapsed < 0.45f)
    {
        if (touch->clickCount == 2)
            touch->clickCount = 1;
        else
            touch->clickCount++;
    }
    else
        touch->clickCount = 1;
    touch->lastClickTime = now;
}

void InputProcessor::touchDown(int touchId, float x, float y)
{
    onTouchBegan(touchId, x, y);
}

void InputProcessor::touchMove(int touchId, float x, float y)
{
    onTouchMoved(touchId, x, y);
}

void InputProcessor::touchUp(int touchId, float x, float y)
{
    onTouchEnded(touchId, x, y);
}

void InputProcessor::onTouchBegan(int touchId, float x, float y)
{
    auto hit = hitTestUI(x, y);
    if (hit.first == nullptr) {
        auto scene = SceneManager::getInstance()->getCurrentScene();
        if (scene)
            hit.first = scene->getRootUI().get();
    }
    auto target = hit.first;
    if (target == nullptr) return;

    TouchInfo* ti = getTouch(touchId);
    ti->pos = Vec2(hit.second.X(), hit.second.Y());
    ti->touchId = touchId;
    setBegin(ti, target);

    updateRecentInput(ti, target);
    m_activeProcessor = this;
    if (m_captureCallback)
        m_captureCallback((int)EventType::TouchBegin);

    if(target->isInteractable())
        target->bubbleInputEvent((int)EventType::TouchBegin);

    handleRollOver(ti, target);

    m_activeProcessor = nullptr;
}

void InputProcessor::onTouchMoved(int touchId, float x, float y)
{
    auto hit = hitTestUI(x, y);
    if (hit.first == nullptr) {
        auto scene = SceneManager::getInstance()->getCurrentScene();
        if (scene) {
            hit.first = scene->getRootUI().get();
        }
    }
    auto target = hit.first;
    if (target == nullptr) return;

    TouchInfo* ti = getTouch(touchId);
    ti->pos = Vec2(hit.second.X(), hit.second.Y());
    ti->touchId = touchId;
    
    updateRecentInput(ti, target);
    m_activeProcessor = this;
    
    if (m_captureCallback)
        m_captureCallback((int)EventType::TouchMove);

    handleRollOver(ti, target);

    if (ti->began)
    {
        bool done = false;
        auto scene = SceneManager::getInstance()->getCurrentScene();
        size_t cnt = ti->touchMonitors.size();
        if (cnt > 0)
        {
            for (size_t i = 0; i < cnt; i++)
            {
                auto mm = scene->findObjectById(ti->touchMonitors[i]);
                if (!mm)
                    continue;

                mm->dispatchInputEvent((int)EventType::TouchMove);
                if (mm == scene->getRootUI())
                    done = true;
            }
        }
        if (!done)
            scene->getRootUI()->dispatchInputEvent((int)EventType::TouchMove);
    }

    m_activeProcessor = nullptr;
}

void InputProcessor::onTouchEnded(int touchId, float x, float y)
{
    auto hit = hitTestUI(x, y);
    auto scene = SceneManager::getInstance()->getCurrentScene();
    if (hit.first == nullptr) {
        if(scene)
            hit.first = scene->getRootUI().get();
    }
    auto target = hit.first;
    if (target == nullptr) return;
    
    TouchInfo * ti = getTouch(touchId);
    ti->pos = Vec2(hit.second.X(), hit.second.Y());
    ti->touchId = touchId;
    setEnd(ti, target);
    updateRecentInput(ti, target);
    m_activeProcessor = this;

    if (m_captureCallback)
        m_captureCallback((int)EventType::TouchEnd);

    size_t cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            auto mm = scene->findObjectById(ti->touchMonitors[i]);
            if (!mm)
                continue;

            if (mm.get() != target)
                mm->dispatchInputEvent((int)EventType::TouchEnd);
        }
        ti->touchMonitors.clear();
    }

    if (target)
    {
        target->bubbleInputEvent((int)EventType::TouchEnd);
    }

    target = clickTestUI(ti, target);
    if (target) {
        updateRecentInput(ti, target);
        target->bubbleInputEvent((int)EventType::Click);
    }

    handleRollOver(ti, target);

    ti->touchId = -1;

    m_activeProcessor = nullptr;
}

std::pair<SceneObject*, Vec3> InputProcessor::hitTestUI(float touchX, float touchY) {
    auto scene = SceneManager::getInstance()->getCurrentScene();
    if (scene) {
        auto hit = scene->raycastUI(Vec2(touchX, touchY));
        return hit;
    }
    std::pair<SceneObject*, Vec3> hitNull(nullptr, Vec3());
    return hitNull;
}

SceneObject* InputProcessor::clickTestUI(TouchInfo* touch, SceneObject* target) {
    if (touch->downTargets.empty()
        || touch->clickCancelled
        || std::abs(touch->pos.X() - touch->downPos.X()) > 50 || std::abs(touch->pos.Y() - touch->downPos.Y()) > 50)
        return nullptr;

    auto scene = SceneManager::getInstance()->getCurrentScene();

    auto obj = scene->findObjectById(touch->downTargets[0]);
    if (obj && obj->isActive())
        return obj.get();

    auto current = target;
    while (current != nullptr)
    {
        auto it = std::find(touch->downTargets.cbegin(), touch->downTargets.cend(), obj->getId());
        if (it != touch->downTargets.cend())
        {
            auto itObj = scene->findObjectById(*it);
            if (itObj->isActive()) {
                current = itObj.get();
                break;
            }
        }

        current = current->getParent();
    }
    return current;
}

bool InputProcessor::isTouchOnUI()
{
    return m_touchOnUI;
}

NS_IGE_SCENE_END