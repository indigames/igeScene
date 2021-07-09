#ifndef __UI_SLIDER_H__
#define __UI_SLIDER_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "components/gui/UIImage.h"
#include "components/gui/RectTransform.h"
#include "event/Event.h"

USING_NS_PYXIE

NS_IGE_SCENE_BEGIN

class UIEventContext;
class EventContext;
class UIButton;
class Tweener;

class UISlider : public Component
{
public:
	enum class Direction {
		LeftToRight,
		RightToLeft,
		BottomToTop,
		TopToBottom,
	};

	UISlider(SceneObject& owner);
	virtual ~UISlider();

	virtual std::string getName() const override { return "UISlider"; }

	//! Returns the type of the component
	virtual Type getType() const override { return Type::UISlider; }

	float getMin() const { return m_min; }
	void setMin(float value);

	float getMax() const { return m_max; }
	void setMax(float value);

	float getValue() const { return m_value; }
	void setValue(float value);

	bool getWholeNumbers() const { return m_wholeNumbers; }
	void setWholeNumbers(bool value);

	void setFillObject(std::shared_ptr<SceneObject> obj);

	void setHandleObject(std::shared_ptr<SceneObject> obj);

	//! NormalColor
	void setColor(float r, float g, float b, float a = 1);
	void setColor(const Vec4& value);
	const Vec4 getColor()  const { return m_normalColor; }

	//! PresssedColor
	void setPressedColor(float r, float g, float b, float a = 1);
	void setPressedColor(const Vec4& value);
	const Vec4 getPressedColor()  const { return m_pressedColor; }

	//! DisabledColor
	void setDisabledColor(float r, float g, float b, float a = 1);
	void setDisabledColor(const Vec4& value);
	const Vec4 getDisabledColor()  const { return m_disableColor; }

	void setFadeDuration(float value) { m_fadeDuration = value; }
	const float getFadeDuration() const { return m_fadeDuration; }

	void setInteractable(bool value);
	const bool isInteractable() const { return m_bIsInteractable; }

	void setDirection(int value);
	void setDirection(Direction value);
	const Direction& getDirection() const { return m_direction; }

	virtual void onSerializeFinished(Scene* scene) override;

	//! Update property by key value
	virtual void setProperty(const std::string& key, const json& val) override;

protected:
	bool updateWithPercent(float percent, bool manual);

	void _onTouchPress(EventContext* context);
	void _onTouchDrag(EventContext* context);
	void _onTouchRelease(EventContext* context);
	
	void _update();

	void init();
	void clear();

	Vec4 getColorByState(int state) const;
	void changeState(int state, bool forced = false);

	void onChangeDirection();
	void onRotateDirection(SceneObject* obj);

	//! Serialize
	virtual void to_json(json& j) const override;

	//! Deserialize
	virtual void from_json(const json& j) override;

	

protected:
	bool m_bIsInit;

	float m_min;
	float m_max;
	float m_value;
	bool m_wholeNumbers;
	bool m_bIsInteractable;
	int m_handleState;

	float m_barMaxSize;
	float m_barMaxSizeDelta;
	float m_barStart;

	bool m_bIsHorizontal;
	bool m_bIsRevert;

	Vec2 m_clickPos;
		
	std::shared_ptr<RectTransform> m_rectFill;
	std::shared_ptr<RectTransform> m_rectHandle;
	std::shared_ptr<UIImage> m_imgHandle;

	std::string m_fillUUID;
	std::string m_handleUUID;
	bool m_dirtySetObj;

	//! Color Tint
	Vec4 m_normalColor;
	Vec4 m_pressedColor;
	Vec4 m_disableColor;
	float m_fadeDuration;

	Direction m_direction;

	std::shared_ptr<Tweener> m_tween = nullptr;

};

NS_IGE_SCENE_END

#endif