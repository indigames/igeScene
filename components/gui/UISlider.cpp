#include "components/gui/UISlider.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "components/tween/Tween.h"
#include "components/tween/Tweener.h"



NS_IGE_SCENE_BEGIN

UISlider::UISlider(SceneObject& owner) :
	Component(owner),
	m_min(0), m_max(1), m_value(0), m_wholeNumbers(false),
	m_rectFill(nullptr), m_rectHandle(nullptr), m_imgHandle(nullptr),
	m_normalColor(1.0f, 1.0f, 1.0f, 1.0f), m_pressedColor(0.78f, 0.78f, 0.78f, 1.0f), m_disableColor(0.78f, 0.78f, 0.78f, 1.0f), m_fadeDuration(0.1f),
	m_dirtySetObj(false), m_bIsHorizontal(false), m_bIsRevert(false), m_direction(Direction::LeftToRight), m_bIsInit(false)
{
	m_bIsInteractable = true;
	init();
}

UISlider::~UISlider() 
{
	clear();

	if (getOwner()) {
		getOwner()->removeEventListener((int)EventType::TouchBegin, m_instanceId);
		getOwner()->removeEventListener((int)EventType::TouchEnd, m_instanceId);
		getOwner()->removeEventListener((int)EventType::TouchMove, m_instanceId);
	}
	
}

void UISlider::init() 
{
	
	getOwner()->setIsInteractable(m_bIsInteractable);
	getOwner()->setIsRaycastTarget(m_bIsInteractable);

	if (getOwner()) {
		getOwner()->removeEventListener((int)EventType::TouchBegin, m_instanceId);
		getOwner()->removeEventListener((int)EventType::TouchEnd, m_instanceId);
		getOwner()->removeEventListener((int)EventType::TouchMove, m_instanceId);
	}

	getOwner()->addEventListener((int)EventType::TouchBegin, std::bind(&UISlider::_onTouchPress, this, std::placeholders::_1), m_instanceId);
	getOwner()->addEventListener((int)EventType::TouchEnd, std::bind(&UISlider::_onTouchRelease, this, std::placeholders::_1), m_instanceId);
	getOwner()->addEventListener((int)EventType::TouchMove, std::bind(&UISlider::_onTouchDrag, this, std::placeholders::_1), m_instanceId);

	m_bIsInit = true;

	onChangeDirection();
}

void UISlider::clear() {
	if (m_rectHandle) {
		if(m_rectHandle->getOwner() != nullptr)
			m_rectHandle->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
	}
	m_imgHandle = nullptr;
	m_rectHandle = nullptr;
	if (m_rectFill) {
		if (m_rectFill->getOwner() != nullptr)
			m_rectFill->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
	}
	m_rectFill = nullptr;
}

void UISlider::_update() {
	float percent = MATH_MIN(m_value / m_max, 1);
	updateWithPercent(percent, false);
}

bool UISlider::updateWithPercent(float percent, bool manual)
{
	percent = MATH_CLAMP(percent, 0, 1);
	bool kq = false;
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
			this->getOwner()->dispatchEvent((int)EventType::Changed, Value(m_value));
			kq = true;
		}
	}

	//!Update Position handle
	if (m_rectHandle) {
		auto currentPos = m_rectHandle->getAnchoredPosition();
		if (m_bIsHorizontal)
		{
			if (m_bIsRevert)
				m_rectHandle->setAnchor(Vec4(0, 1 - percent, 1, 1 - percent));
			else
				m_rectHandle->setAnchor(Vec4(0, percent, 1, percent));
		}
		else {
			if (m_bIsRevert)
				m_rectHandle->setAnchor(Vec4(1 - percent, 0, 1 - percent, 1));
			else
				m_rectHandle->setAnchor(Vec4(percent, 0, percent, 1));
		}

		if(m_bIsHorizontal)
			currentPos[1] = 0;
		else
			currentPos[0] = 0;
		m_rectHandle->setAnchoredPosition(currentPos);
	}
	//!Update Fill bar
	if (m_rectFill) {
		if (m_bIsHorizontal)
		{
			if (m_bIsRevert)
				m_rectFill->setAnchor(Vec4(0, 1 - percent, 1, 1));
			else
				m_rectFill->setAnchor(Vec4(0, 0, 1, percent));
		}
		else
		{
			if (m_bIsRevert)
				m_rectFill->setAnchor(Vec4(1 - percent, 0, 1, 1));
			else
				m_rectFill->setAnchor(Vec4(0, 0, percent, 1));
		}
		m_rectFill->setOffset(Vec4(0, 0, 0, 0));
	}

	return kq;
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

void UISlider::setFillObject(std::shared_ptr<SceneObject> obj)
{
	if (obj) {
		if (m_rectFill) {
			if (m_rectFill->getOwner() != nullptr)
				m_rectFill->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
			m_rectFill = nullptr;
		}

		m_rectFill = obj->getComponent<RectTransform>();
		obj->addEventListener((int)EventType::Delete, [this](auto val) {
			m_rectFill = nullptr;
			}, m_instanceId);
		_update();
	}
	else
	{
		if (m_rectFill) {
			if (m_rectFill->getOwner() != nullptr)
				m_rectFill->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
			m_rectFill = nullptr;
		}
	}
}

void UISlider::setHandleObject(std::shared_ptr<SceneObject> obj)
{
	if (obj) {
		if (m_rectHandle) {
			if (m_rectHandle->getOwner() != nullptr)
				m_rectHandle->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
			m_rectHandle = nullptr;
			m_imgHandle = nullptr;
		}

		m_rectHandle = obj->getComponent<RectTransform>();
		m_imgHandle = obj->getComponent<UIImage>();
		obj->addEventListener((int)EventType::Delete, [this](auto val) {
			m_rectHandle = nullptr;
			m_imgHandle = nullptr;
		}, m_instanceId);
		_update();
	}
	else
	{
		if (m_rectHandle) {
			if (m_rectHandle->getOwner() != nullptr)
				m_rectHandle->getOwner()->removeEventListener((int)EventType::Delete, m_instanceId);
			m_rectHandle = nullptr;
			m_imgHandle = nullptr;
		}
	}
}

void UISlider::_onTouchPress(EventContext* context)
{
	if (!isInteractable()) return;
	context->stopPropagation();
	auto inputContext = dynamic_cast<InputEventContext*>(context);
	if (inputContext == nullptr) return;
	inputContext->captureTouch();

	changeState(1);
	
	if (m_max == m_min) return;
	if (m_rectHandle == nullptr) return;
	auto handleParent = m_rectHandle->getOwner()->getParent();
	if (handleParent == nullptr) return;
	auto parentRect = handleParent->getRectTransform();
	if (parentRect == nullptr) return;
	auto parentSize = parentRect->getSize();

	auto clickPoint = inputContext->getInput()->getPosition();
	auto point = parentRect->globalToLocal(Vec3(clickPoint[0], clickPoint[1], parentRect->getPosition()[3]));
	float percent = MATH_CLAMP((m_value - m_min) / (m_max - m_min), 0, 1);
	
	m_clickPos = clickPoint;
	float deltaX = point[0] / parentSize[0] + 0.5f;
	float deltaY = point[1] / parentSize[1] + 0.5f;
	
	if (m_bIsHorizontal)
		percent = deltaY;
	else
		percent = deltaX;

	if (m_bIsRevert)
		percent = 1 - percent;

	percent = MATH_CLAMP(percent, 0, 1);
	updateWithPercent(percent, true);
}

void UISlider::_onTouchDrag(EventContext* context)
{
	if (!isInteractable()) return;
	context->stopPropagation();
	auto inputContext = dynamic_cast<InputEventContext*>(context);
	if (inputContext == nullptr) return;
	if (m_max == m_min) return;
	if (m_rectHandle == nullptr) return;

	auto handleParent = m_rectHandle->getOwner()->getParent();
	if (handleParent == nullptr) return;
	auto parentRect = handleParent->getRectTransform();
	if (parentRect == nullptr) return;
	auto parentSize = parentRect->getSize();
	if (inputContext->getInput() == nullptr) return;
	auto clickPoint = inputContext->getInput()->getPosition();
	auto point = parentRect->globalToLocal(Vec3(clickPoint[0], clickPoint[1], parentRect->getPosition()[3]));
	float percent = MATH_CLAMP((m_value - m_min) / (m_max - m_min), 0, 1);

	float deltaX = (clickPoint[0] - m_clickPos[0]) / parentSize[0];
	float deltaY = (clickPoint[1] - m_clickPos[1]) / parentSize[1];
	
	if (m_bIsRevert) 
	{
		deltaX = -deltaX;
		deltaY = -deltaY;
	}

	if (m_bIsHorizontal) 
		percent += deltaY;
	else
		percent += deltaX;
		
	percent = MATH_CLAMP(percent, 0, 1);
	
	if(updateWithPercent(percent, true))
		m_clickPos = clickPoint;
}

void UISlider::_onTouchRelease(EventContext* context)
{
	if (!isInteractable()) return;
	context->stopPropagation();
	changeState(0);
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
	bool oldRot = m_bIsHorizontal;
	switch (m_direction) {
	case Direction::LeftToRight:
		m_bIsHorizontal = false;
		m_bIsRevert = false;
		break;
	case Direction::RightToLeft:
		m_bIsHorizontal = false;
		m_bIsRevert = true;
		break;
	case Direction::BottomToTop:
		m_bIsHorizontal = true;
		m_bIsRevert = false;
		break;
	case Direction::TopToBottom:
		m_bIsHorizontal = true;
		m_bIsRevert = true;
		break;
	}
	
	if (!m_bIsInit) return;

	if (oldRot != m_bIsHorizontal) {
		//! Change Parent Size 
		auto rect = getOwner()->getRectTransform();
		if (rect)
		{
			auto size = rect->getSize();
			Vec2 newSize(size[1], size[0]);
			rect->setSize(newSize);
		}

		//! Change Anchor, offset
		for (auto& child : getOwner()->getChildren())
		{
			if (!child.expired())
			{
				onRotateDirection(child.lock());
			}
		}

	}

	_update();

	if (oldRot != m_bIsHorizontal) {
		//! Raise flag to update rect value
		for (auto& child : getOwner()->getChildren())
		{
			if (!child.expired())
			{
				auto childTransform = child.lock()->getComponent<RectTransform>();
				if (childTransform)
				{
					childTransform->setLocalToRectDirty();
				}
			}
		}
	}
}

void UISlider::onRotateDirection(std::shared_ptr<SceneObject> obj)
{
	if (obj)
	{
		auto rect = obj->getComponent<RectTransform>();
		if (rect)
		{
			auto anchor = rect->getAnchor();
			auto offset = rect->getOffset();
			auto size = rect->getSize();
			Vec4 newAnchor(anchor[1], anchor[0], anchor[3], anchor[2]);
			rect->setAnchor(newAnchor);
			Vec4 newOffset(offset[1], offset[0], offset[3], offset[2]);
			rect->setOffset(newOffset);
		}

		for (auto& child : obj->getChildren())
		{
			if (!child.expired())
			{
				onRotateDirection(child.lock());
			}
		}
	}
}

void UISlider::onSerializeFinished(Scene* scene) 
{
	if (m_dirtySetObj) {
		m_dirtySetObj = false;
		if (!m_fillUUID.empty()) {
			auto obj = scene->findObjectByUUID(m_fillUUID);
			if (obj) {
				setFillObject(obj);
			}
		}
		if (!m_handleUUID.empty()) {
			auto obj = scene->findObjectByUUID(m_handleUUID);
			if (obj) {
				setHandleObject(obj);
			}
		}
	}
}

//! Json 
void UISlider::to_json(json& j) const
{
	Component::to_json(j);
	j["color"] = getColor();
	j["pressedcolor"] = getPressedColor();
	j["disabledcolor"] = getDisabledColor();
	j["fadeduration"] = getFadeDuration();
	j["fill"] = m_rectFill ? m_rectFill->getOwner()->getUUID() : "";
	j["handle"] = m_rectHandle ? m_rectHandle->getOwner()->getUUID() : "";
	j["direction"] = (int)getDirection();
	j["wholenumbers"] = getWholeNumbers();
	j["min"] = getMin();
	j["max"] = getMax();
	j["value"] = getValue();
}

//! Deserialize
void UISlider::from_json(const json& j)
{
	m_bIsInit = false;
	m_fillUUID = j.value("fill", "");
	m_handleUUID = j.value("handle", "");
	setColor(j.at("color"));
	setPressedColor(j.at("pressedcolor"));
	setDisabledColor(j.at("disabledcolor"));
	setFadeDuration(j.at("fadeduration"));
	setWholeNumbers(j.at("wholenumbers"));
	setMin(j.value("min", 0));
	setMax(j.value("max", 1));
	setValue(j.value("value", 0));
	setDirection((int)j.at("direction"));
	m_dirtySetObj = true;
	Component::from_json(j);
	init();
}

//! Update property by key value
void UISlider::setProperty(const std::string& key, const json& val)
{
	if (key.compare("fill") == 0)
	{
		m_fillUUID = val;
	}
	else if (key.compare("handle") == 0)
	{
		m_handleUUID = val;
	}
	else if (key.compare("color") == 0)
	{
		setColor(val);
	}
	else if (key.compare("pressedcolor") == 0)
	{
		setPressedColor(val);
	}
	else if (key.compare("disabledcolor") == 0)
	{
		setDisabledColor(val);
	}
	else if (key.compare("fadeduration") == 0)
	{
		setFadeDuration(val);
	}
	else if (key.compare("wholenumbers") == 0)
	{
		setWholeNumbers(val);
	}
	else if (key.compare("min") == 0)
	{
		setMin(val);
	}
	else if (key.compare("max") == 0)
	{
		setMax(val);
	}
	else if (key.compare("value") == 0)
	{
		setValue(val);
	}
	else if (key.compare("direction") == 0)
	{
		setDirection((int)val);
	}
	else
	{
		Component::setProperty(key, val);
	}

	m_dirtySetObj = true;
}

NS_IGE_SCENE_END