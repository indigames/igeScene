#ifndef __INPUT_EVENT_H__
#define __INPUT_EVENT_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

NS_IGE_SCENE_BEGIN

class SceneObject;
class InputProcessor;

class InputEvent
{
public:
	InputEvent();
	~InputEvent();

	std::shared_ptr<SceneObject> getTarget() const { return m_target; }
	const Vec2 getPosition() const { return m_position; }
	int getTouchId() const { return m_touchId; }
	KeyCode getKeyCode() const { return m_keyCode; }
	InputProcessor* getProcessor() const { return m_inputProcessor; }

private:
	std::shared_ptr<SceneObject> m_target;
	Vec2 m_position;
	int m_touchId;
	int m_clickCount;
	KeyCode m_keyCode;
	InputProcessor* m_inputProcessor;
	
	friend class InputProcessor;
};


NS_IGE_SCENE_END


#endif //__INPUT_EVENT_H__