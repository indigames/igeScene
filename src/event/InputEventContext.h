#ifndef __UI_INPUT_EVENT_CONTEXT_H__
#define __UI_INPUT_EVENT_CONTEXT_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "core/Value.h"

#include "event/InputEvent.h"
#include "event/EventContext.h"

USING_NS_PYXIE

NS_IGE_SCENE_BEGIN

class SceneObject;

class InputEventContext : public EventContext
{
public:
	InputEventContext();
	virtual ~InputEventContext();

	InputEvent* getInput() const { return m_inputEvent; }
	/*void stopPropagation() { m_bIsStopped = true; }
	void preventDefault() { m_defaultPrevented = true; }
	bool isDefaultPrevented() { return m_defaultPrevented; }*/
	void captureTouch() { m_touchCapture = 1; }
	void uncaptureTouch() { m_touchCapture = 2; }
	bool isCaptureTouch() { return m_touchCapture == 1;}
	bool isUnCaptureTouch() { return m_touchCapture == 2; }
protected:
	InputEvent* m_inputEvent;
	/*bool m_bIsStopped;
	bool m_defaultPrevented;*/
	int m_touchCapture;
	//int m_type;

	friend class SceneObject;
};

NS_IGE_SCENE_END

#endif // __UI_INPUT_EVENT_CONTEXT_H__