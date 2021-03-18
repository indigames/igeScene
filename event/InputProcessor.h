#ifndef __INPUT_PROCESSOR_H__
#define __INPUT_PROCESSOR_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"
#include "scene/SceneObject.h"
#include "event/InputEvent.h"

NS_IGE_SCENE_BEGIN

class TouchInfo;

class InputProcessor
{
public :
	typedef std::function<void(int eventType)> CaptureEventCallback;

	static InputProcessor* getInstance() { return m_activeProcessor; }

	InputEvent* getRecentInput() { return &m_recentInput; }
	static bool isTouchOnUI();

	InputProcessor();
	~InputProcessor();

	Vec2 getTouchPosition(int touchId);

	void addTouchMonitor(int touchId, std::shared_ptr<SceneObject> target);
	void removeTouchMonitor(std::shared_ptr<SceneObject> target);

	void cancelClick(int touchId);

	void setCaptureCallback(CaptureEventCallback value) { m_captureCallback = value; }

	void touchDown(int touchId, float x, float y);
	void touchMove(int touchId, float x, float y);
	void touchUp(int touchId, float x, float y);

private:
	void onTouchBegan(int touchId, float x, float y);
	void onTouchMoved(int touchId, float x, float y);
	void onTouchEnded(int touchId, float x, float y);
	void onTouchCancelled(int touchId, float x, float y);


	TouchInfo* getTouch(int touchId, bool createIfNotExists = true);
	void updateRecentInput(TouchInfo* touch, SceneObject* target);
	void handleRollOver(TouchInfo* touch, SceneObject* target);
	void setBegin(TouchInfo* touch, SceneObject* target);
	void setEnd(TouchInfo* touch, SceneObject* target);

	//! use to check which UI SceneObject will be hit by raycast
	std::pair<SceneObject*, Vec3> hitTestUI(float touchX, float touchY);

	//! use to get which UI SceneObject will active click 
	//! (** example: cast UIText but UIButton will be clicked)
	SceneObject* clickTestUI(TouchInfo* touch, SceneObject* target);

private:

	CaptureEventCallback m_captureCallback;
	InputEvent m_recentInput;
	std::vector<TouchInfo*> m_touches;
	static bool m_touchOnUI;
	static InputProcessor* m_activeProcessor;
};


NS_IGE_SCENE_END


#endif //__INPUT_PROCESSOR_H__