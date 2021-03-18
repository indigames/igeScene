#ifndef __UI_EVENT_CONTEXT_H__
#define __UI_EVENT_CONTEXT_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "core/Value.h"

#include "event/InputEvent.h"

USING_NS_PYXIE

NS_IGE_SCENE_BEGIN

class SceneObject;

enum class EventType {
	Enter = 0,
	Exit = 1,
	Changed = 2,
	Submit = 3,

	TouchBegin = 10,
	TouchMove = 11,
	TouchEnd = 12,
	Click = 13,
	RollOver = 14,
	RollOut = 15,

	PositionChanged = 20,
	SizeChanged = 21,

	KeyDown = 30,
	KeyUp = 31,

	PullDown = 40,
	PullUp = 41,
	PullDownRelease = 42,
	PullUpRelease = 43,

	DragStart = 50,
	DragMove = 51,
	DragEnd = 52,
	Drop = 53,
};

class EventContext
{
public:
	EventContext();
	virtual ~EventContext();

	int getType() const { return m_type; }
	std::shared_ptr<SceneObject> getSender() const { return m_sender; }
	InputEvent* getInput() const { return m_inputEvent; }
	void stopPropagation() { m_bIsStopped = true; }
	void preventDefault() { m_defaultPrevented = true; }
	bool isDefaultPrevented() { return m_defaultPrevented; }
	void captureTouch() { m_touchCapture = 1; }
	void uncaptureTouch() { m_touchCapture = 2; }

	const Value& getDataValue() const { return m_dataValue; }

protected:
	std::shared_ptr<SceneObject> m_sender;
	Value m_dataValue;

	InputEvent* m_inputEvent;
	bool m_bIsStopped;
	bool m_defaultPrevented;
	int m_touchCapture;
	int m_type;

	friend class SceneObject;
};

typedef std::function<void(EventContext* context)> EventCallback;

NS_IGE_SCENE_END

#endif