#include "event/InputEvent.h"
#include "scene/SceneObject.h"
#include "event/InputProcessor.h"

NS_IGE_SCENE_BEGIN

InputEvent::InputEvent() :
    m_target(nullptr),
    m_inputProcessor(nullptr),
    m_touchId(-1),
    m_clickCount(0),
    m_keyCode(KeyCode::KEY_0)
{
}

InputEvent::~InputEvent()
{

}

NS_IGE_SCENE_END