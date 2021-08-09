#include "event/EventContext.h"

NS_IGE_SCENE_BEGIN

EventContext::EventContext() :
    m_sender(nullptr),
    m_bIsStopped(false),
    m_defaultPrevented(false),
    m_type(0)
{
}

EventContext::~EventContext()
{

}

NS_IGE_SCENE_END