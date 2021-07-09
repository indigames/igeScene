#ifndef __UI_SCROLL_BAR_H__
#define __UI_SCROLL_BAR_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "components/gui/UIImage.h"
#include "components/gui/RectTransform.h"
#include "event/Event.h"

USING_NS_PYXIE

NS_IGE_SCENE_BEGIN

class UIEventContext;
class EventContext;
class Tweener;


class UIScrollBar : public UIImage
{
public:
	enum class Direction
	{
		LeftToRight,
		RightToLeft,
		BottomToTop,
		TopToBottom,
	};

protected:
	enum class AxisBar
	{
		Horizontal,
		Vertical,
	};

public:
	UIScrollBar(SceneObject& owner, const std::string& texture = "", const Vec2& size = { 256.f, 256.f }, const bool isSliced = false, const Vec4& border = { 0.f, 0.f, 0.f, 0.f });
	virtual ~UIScrollBar();

	//! Get component name
	virtual std::string getName() const override { return "UIScrollBar"; }

	//! Returns the type of the component
	virtual Type getType() const override { return Type::UIScrollBar; }

	//! Update
	virtual void onUpdate(float dt) override;

	//! Render
	virtual void onRender() override;

	virtual void onSerializeFinished(Scene* scene) override;

	//! Update property by key value
	virtual void setProperty(const std::string& key, const json& val) override;
public:

	void setHandle(std::shared_ptr<SceneObject> obj);

	float getValue() const { return m_value; }
	void setValue(float value);
	//! not invoke onValueChanged
	void setValueWithoutNotify(float value);

	float getSize() const { return m_size; }
	void setSize(float value);

	Direction getDirection() const { return m_direction; }
	void setDirection(Direction value, bool flip = true);
	void setDirection(int value, bool flip = true);


	Event<float>& getOnValueChanged() { return m_onValueChangedEvent; }

	//! NormalColor
	void setNormalColor(float r, float g, float b, float a = 1);
	void setNormalColor(const Vec4& value);
	const Vec4 getNormalColor()  const { return m_normalColor; }

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
	
protected:

	virtual void _onTouchPress(EventContext* context) override;
	virtual void _onTouchRelease(EventContext* context) override;
	virtual void _onTouchDrag(EventContext* context) override;
	virtual void _onClick(EventContext* context) override;

	void _update();
	void _drag(Vec2 value);
	void _updateDrag(Vec2 handleCorner, float remainingSize);

	AxisBar getAxisBar() const;
	bool isReverseValue() const;

	void _flipLayoutAxes(std::shared_ptr<RectTransform> rect, bool keepPositioning, bool recursive);
	void _flipLayoutOnAxis(std::shared_ptr<RectTransform> rect, int axis, bool keepPositioning, bool recursive);

	Vec4 getColorByState(int state) const;
	void changeState(int state, bool forced = false);

	void _clickRepeat(Vec2 clickPoint);

protected:
	bool m_bIsSelectable = false;
	bool m_bIsDragging = false;
	bool m_bIsCaptureTouch = false;

	float m_value;
	float m_size;
	Direction m_direction;


	Vec2 m_clickedPos;
	Vec2 m_deltaPos;
	Vec2 m_offsetPos;
	Vec2 m_minPos;
	Vec2 m_maxPos;
	
	//! Stored current Content Size and recheck when start drag
	Vec2 m_currentContentSize;

	bool m_dirtySetObj;
	std::shared_ptr<RectTransform> m_rect;
	std::shared_ptr<RectTransform> m_rectHandle;
	std::shared_ptr<RectTransform> m_rectArea;
	std::shared_ptr<UIImage> m_imgHandle;

	std::string m_contentUUID;

	Event<float> m_onValueChangedEvent;

	//! Color Tint
	int m_handleState;
	Vec4 m_normalColor;
	Vec4 m_pressedColor;
	Vec4 m_disableColor;
	float m_fadeDuration;
	std::shared_ptr<Tweener> m_tween = nullptr;

protected:
	//! Serialize
	virtual void to_json(json& j) const override;

	//! Deserialize
	virtual void from_json(const json& j) override;
};

NS_IGE_SCENE_END


#endif //__UI_SCROLL_BAR_H__