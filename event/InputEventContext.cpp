#include "event/InputEventContext.h"

NS_IGE_SCENE_BEGIN

InputEventContext::InputEventContext() : 
    EventContext(),
    m_inputEvent(nullptr),
    m_touchCapture(0)
{
    
}

InputEventContext::~InputEventContext()
{

}

NS_IGE_SCENE_END