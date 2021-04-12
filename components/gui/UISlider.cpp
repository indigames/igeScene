#include "components/gui/UISlider.h"
#include "scene//SceneObject.h"

#include "components/tween/Tween.h"
#include "components/tween/Tweener.h"

NS_IGE_SCENE_BEGIN

UISlider::UISlider(SceneObject& owner) :
	Component(owner), 
	m_min(0), m_max(1), m_value(0), m_wholeNumbers(false), 
	m_fillObj(nullptr), m_handleObj(nullptr), m_bgObj(nullptr), m_rectBG(nullptr), m_rectFill(nullptr), m_rectHandle(nullptr), m_imgHandle(nullptr), 
	m_normalColor(1.0f, 1.0f, 1.0f, 1.0f), m_pressedColor(0.78f, 0.78f, 0.78f, 1.0f), m_disableColor(0.78f, 0.78f, 0.78f, 0.5f), m_fadeDuration(0.1f)
{

	_init();

	getOwner()->addEventListener((int)EventType::TouchBegin, std::bind(&UISlider::_onTouchPress, this, std::placeholders::_1), m_instanceId);
	getOwner()->addEventListener((int)EventType::TouchEnd, std::bind(&UISlider::_onTouchRelease, this, std::placeholders::_1), m_instanceId);
	getOwner()->addEventListener((int)EventType::TouchMove, std::bind(&UISlider::_onTouchDrag, this, std::placeholders::_1), m_instanceId);
}

UISlider::~UISlider() 
{
	if (m_bgObj)
	{
		if (m_rectBG) m_rectBG = nullptr;
		m_bgObj = nullptr;
	}

	if (m_fillObj) 
	{
		if (m_rectFill) m_rectFill = nullptr;
		m_fillObj = nullptr;
	}

	if (m_handleObj)
	{
		if (m_imgHandle) m_imgHandle = nullptr;
		if (m_rectHandle) m_rectHandle = nullptr;
		m_handleObj = nullptr;
	}

	getOwner()->removeEventListener((int)EventType::TouchBegin, m_instanceId);
	getOwner()->removeEventListener((int)EventType::TouchEnd, m_instanceId);
	getOwner()->removeEventListener((int)EventType::TouchMove, m_instanceId);
}

void UISlider::_init() {
	if (m_fillObj == nullptr || m_handleObj == nullptr) return;
	auto uiImage = m_fillObj->getComponent<UIImage>();
	auto uiBtn = m_handleObj->getComponent<UIImage>();
	
}

void UISlider::_update() {
	float percent = MATH_MIN(m_value / m_max, 1);
	updateWithPercent(percent, false);
}

void UISlider::updateWithPercent(float percent, bool manual) 
{
	percent = MATH_CLAMP(percent, 0, 1);
	if (manual)
	{
		float newValue = m_min + (m_max - m_min) * percent;
		if (newValue < m_min)
			newValue = m_min;
		if (newValue > m_max)
			newValue = m_max;
		if (m_wholeNumbers)
		{
			newValue = round(newValue);
			percent = MATH_CLAMP((newValue - m_min) / (m_max - m_min), 0, 1);
		}

		if (newValue != m_value)
		{
			m_value = newValue;
			if (this->getOwner()->dispatchEvent((int)EventType::Changed, Value(m_value)))
				return;
		}
	}

	//!Update Position handle



	//!Update Fill bar
	

}

void UISlider::setMin(float value) {
	if (m_min != value) {
		m_min = value;
		_update();
	}
}

void UISlider::setMax(float value)
{
	if (m_max != value)
	{
		m_max = value;
		_update();
	}
}

void UISlider::setValue(float value)
{
	if (m_value != value)
	{
		m_value = value;
		_update();
	}
}

void UISlider::setWholeNumbers(bool value)
{
	if (m_wholeNumbers != value)
	{
		m_wholeNumbers = value;
		_update();
	}
}

void UISlider::setBackgroundObject(std::shared_ptr<SceneObject> obj)
{
	if (m_bgObj != obj) {
		m_bgObj = obj;
		m_rectBG = m_bgObj->getComponent<RectTransform>();
		if (m_rectBG)
		{
			m_rectBG->setAnchor(Vec4(0, 0.25f, 1, 0.75f));
			auto size = m_rectBG->getSize();
			size[1] *= 0.5f;
			m_rectBG->setSize(size);
		}
		_update();
	}
}

void UISlider::setFillObject(std::shared_ptr<SceneObject> obj)
{
	if (m_fillObj != obj) {
		m_fillObj = obj;
		m_rectFill = m_fillObj->getComponent<RectTransform>();
		_update();
	}
}

void UISlider::setHandleObject(std::shared_ptr<SceneObject> obj)
{
	if (m_handleObj != obj) {
		m_handleObj = obj;
		m_rectHandle = m_handleObj->getComponent<RectTransform>();
		_update();
	}
}

void UISlider::_onTouchPress(EventContext* context)
{
	auto& color = getColorByState(1);
}

void UISlider::_onTouchDrag(EventContext* context)
{

}

void UISlider::_onTouchRelease(EventContext* context)
{

}

//! Set Normal Color
void UISlider::setColor(const Vec4& value)
{
	if (m_normalColor != value) {
		m_normalColor = value;
		if( m_imgHandle != nullptr)
			m_imgHandle->setColor(m_normalColor);
	}
}

void UISlider::setColor(float r, float g, float b, float a) {
	Vec4 value(r, g, b, a);
	value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
	value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
	value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
	value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
	setColor(value);
}

//! Set Pressed Color
void UISlider::setPressedColor(const Vec4& value)
{
	if (m_pressedColor != value) {
		m_pressedColor = value;
	}
}

void UISlider::setPressedColor(float r, float g, float b, float a) {
	Vec4 value(r, g, b, a);
	value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
	value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
	value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
	value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
	setPressedColor(value);
}

//! Set Disabled Color
void UISlider::setDisabledColor(const Vec4& value)
{
	if (m_disableColor != value) {
		m_disableColor = value;
	}
}

void UISlider::setDisabledColor(float r, float g, float b, float a) {
	Vec4 value(r, g, b, a);
	value[0] = MATH_CLAMP(value[0], 0.0f, 1.0f);
	value[1] = MATH_CLAMP(value[1], 0.0f, 1.0f);
	value[2] = MATH_CLAMP(value[2], 0.0f, 1.0f);
	value[3] = MATH_CLAMP(value[3], 0.0f, 1.0f);
	setDisabledColor(value);
}

//! Set Interactable
void UISlider::setInteractable(bool value) {
	m_bIsInteractable = value;
	getOwner()->setIsInteractable(m_bIsInteractable);
}

//! Set Direction
void UISlider::setDirection(int value) {
	setDirection((Direction)value);
}

void UISlider::setDirection(Direction value) {
	if (value != m_direction) {
		m_direction = value;
		onChangeDirection();
	}
}


Vec4 UISlider::getColorByState(int state) const
{
	switch (state) {
	case 0:
	default:
		return m_normalColor;
	case 1:
		return m_pressedColor;
	case 2:
		return m_disableColor;
	}
}

void UISlider::changeState(int state, bool forced)
{
	if (m_imgHandle == nullptr) return;

	if (m_handleState != state || forced)
	{
		m_handleState = state;
		auto col = m_imgHandle->getColor();
		auto t_col = getColorByState(m_handleState);
		if (col != t_col) {
			if (m_tween != nullptr) {
				m_tween->kill();
				m_tween = nullptr;
			}

			m_tween = Tween::tween(col, t_col, m_fadeDuration);
			m_tween->onUpdate([this](Tweener* tweener) {
				m_imgHandle->setColor(tweener->value.getVec4());
				})->onComplete([this]() {});

		}
		else if (col != m_normalColor) {
			m_imgHandle->setColor(m_normalColor);
		}
	}
}

void UISlider::onChangeDirection() 
{
	
}
//! Json 
void UISlider::to_json(json& j) const
{
	Component::to_json(j);
	j["color"] = getColor();
	j["pressedcolor"] = getPressedColor();
	j["disabledcolor"] = getDisabledColor();
	j["fadeduration"] = getFadeDuration();
	
}

//! Deserialize
void UISlider::from_json(const json& j)
{
	setColor(j.at("color"));
	setPressedColor(j.at("pressedcolor"));
	setDisabledColor(j.at("disabledcolor"));
	setFadeDuration(j.at("fadeduration"));
	Component::from_json(j);
}

NS_IGE_SCENE_END