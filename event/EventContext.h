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
	PullDown = 32,
	PullUp = 33,
	PullDownRelease = 34,
	PullUpRelease = 35,
	DragStart = 36,
	DragMove = 37,
	DragEnd = 38,
	Drop = 39,

	SetParent		= 99,
	AddChild		= 100,
	RemoveChild		= 101,
	RemoveChildren	= 102,

	RunEditor		= 990,
	PauseEditor		= 991,
	StopEditor		= 992,

	Delete	= 999,
};

class EventContext
{
public:
	EventContext();
	virtual ~EventContext();

	int getType() const { return m_type; }
	SceneObject* getSender() const { return m_sender; }
	void stopPropagation() { m_bIsStopped = true; }
	void preventDefault() { m_defaultPrevented = true; }
	bool isDefaultPrevented() { return m_defaultPrevented; }

	const Value& getDataValue() const { return m_dataValue; }

protected:
	SceneObject* m_sender;
	Value m_dataValue;

	bool m_bIsStopped;
	bool m_defaultPrevented;
	int m_type;

	friend class SceneObject;
};

typedef std::function<void(EventContext* context)> EventCallback;

NS_IGE_SCENE_END

#endif