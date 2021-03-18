#include "components/tween/Tweener.h"
#include "scene/SceneObject.h"
#include <math.h>

NS_IGE_SCENE_BEGIN

static const float _PiOver2 = (float)(PI * 0.5f);
static const float _TwoPi = (float)(PI * 2);

float Tweener::evaluate(EaseType easeType, float time, float duration, float overshootOrAmplitude, float period) {
	if (duration > 0) {
		switch (easeType) {
		default:
		case EaseType::Linear:
			return time / duration;
		case EaseType::SineIn:
			return -(float)cos(time / duration * _PiOver2) + 1;
		case EaseType::SineOut:
			return (float)sin(time / duration * _PiOver2);
		case EaseType::SineInOut:
			return -0.5f * ((float)cos(PI * time / duration) - 1);
		case EaseType::QuadIn:
			time /= duration;
			return time * time;
		case EaseType::QuadOut:
			time /= duration;
			return -time * (time - 2);
		case EaseType::QuadInOut:
			time /= duration * 0.5f;
			if (time < 1) return 0.5f * time * time;
			--time;
			return -0.5f * (time * (time - 2) - 1);
		}
	}
	return 0;
}

Tweener::Tweener() : m_target(nullptr),
	m_onStart(nullptr),
	m_onUpdate(nullptr),
	m_onComplete(nullptr),
	m_onComplete0(nullptr),
	m_referenceId(-1),
	m_propType(TweenPropType::None),
	m_repeat(0)
{

}

Tweener::~Tweener()
{
	_reset();
}

void Tweener::release() {
	_reset();
}

Tweener* Tweener::setDelay(float value)
{
	m_delay = value;
	return this;
}

Tweener* Tweener::setDuration(float value)
{
	m_duration = value;
	return this;
}

Tweener* Tweener::setBreakpoint(float value)
{
	m_breakpoint = value;
	return this;
}

Tweener* Tweener::setRepeat(int repeat, LoopType loopType)
{
	m_repeat = repeat < -1 ? -1 : repeat;
	m_loopType = loopType;
	return this;
}

Tweener* Tweener::setLoopType(LoopType value)
{
	m_loopType = value;
	return this;
}

Tweener* Tweener::setTimeScale(float value)
{
	m_timeScale = value;
	return this;
}

Tweener* Tweener::setSnapping(bool value) 
{
	m_snapping = value;
	return this;
}

Tweener* Tweener::setEaseType(EaseType value) {
	m_easeType = value;
	return this;
}

Tweener* Tweener::setTarget(SceneObject* value, uint64_t referenceId)
{
	m_target = value;
	m_referenceId = referenceId;
	return this;
}

Tweener* Tweener::setTarget(SceneObject* value, uint64_t referenceId, TweenPropType propType)
{
	m_target = value;
	m_referenceId = referenceId;
	m_propType = propType;
	return this;
}

Tweener* Tweener::onUpdate(TweenCallback callback)
{
	m_onUpdate = callback;
	return this;
}

Tweener* Tweener::onStart(TweenCallback callback)
{
	m_onStart = callback;
	return this;
}

Tweener* Tweener::onComplete(TweenCallback callback)
{
	m_onComplete = callback;
	return this;
}

Tweener* Tweener::onComplete(TweenCallback0 callback)
{
	m_onComplete0 = callback;
	return this;
}

Tweener* Tweener::setPaused(bool paused)
{
	m_paused = paused;
	return this;
}

void Tweener::seek(float time)
{
	if (m_killed)
		return;

	m_elapsedTime = time;
	if (m_elapsedTime < m_delay)
	{
		if (m_started)
			m_elapsedTime = m_delay;
		else
			return;
	}

	update();
}

void Tweener::kill(bool complete) 
{
	if (m_killed)
		return;
	if (complete) {
		if (m_ended == 0) 
		{
			if (m_breakpoint >= 0)
				m_elapsedTime = m_delay + m_breakpoint;
			else if (m_repeat >= 0)
				m_elapsedTime = m_delay + m_breakpoint * (m_repeat + 1);
			else
				m_elapsedTime = m_delay + m_duration * 2;
			update();
		}

		invokeCompleteCallback();
	}

	m_killed = true;
}

void Tweener::_init()
{
	m_delay = 0;
	m_duration = 0;
	m_breakpoint = -1;
	m_timeScale = 1;
	m_snapping = false;
	m_repeat = 0;
	m_loopType = LoopType::Restart;
	m_started = false;
	m_paused = false;
	m_killed = false;
	m_elapsedTime = 0;
	m_normalizedTime = 0;
	m_ended = 0;
	startValue.setZero();
	endValue.setZero();
	value.setZero();
	deltaValue.setZero();
}

void Tweener::_reset()
{
	m_target = nullptr;
	m_onStart = m_onUpdate = nullptr;
	m_onComplete = nullptr;
	m_onComplete0 = nullptr;
}

void Tweener::_update(float dt)
{
	if (m_ended != 0) //Maybe completed by seek
	{
		invokeCompleteCallback();
		m_killed = true;
		return;
	}

	if (m_timeScale != 1)
		dt *= m_timeScale;
	if (dt == 0)
		return;

	m_elapsedTime += dt;
	update();

	if (m_ended != 0)
	{
		if (!m_killed)
		{
			invokeCompleteCallback();
			m_killed = true;
		}
	}
}

void Tweener::update()
{
	m_ended = 0;

	if (!m_started)
	{
		if (m_elapsedTime < m_delay)
			return;
		m_started = true;
		invokeStartCallback();
		if (m_killed)
			return;
	}

	bool reversed = false;
	float tt = m_elapsedTime - m_delay;
	if (m_breakpoint >= 0 && tt >= m_breakpoint)
	{
		tt = m_breakpoint;
		m_ended = 2;
	}

	if (m_repeat > 0) {
		int round = (int)floor(tt / m_duration);
		tt -= m_duration * round;
		if (m_loopType == LoopType::Yoyo)
			reversed = round % 2 == 1;

		if (m_repeat > 0 && m_repeat - round < 0)
		{
			if (m_loopType == LoopType::Yoyo)
				reversed = m_repeat % 2 == 1;
			tt = m_duration;
			m_ended = 1;
		}
	}
	else if (m_repeat == -1) //!Loop forever
	{
		int round = (int)floor(tt / m_duration);
		tt -= m_duration * round;
		if (m_loopType == LoopType::Yoyo)
			reversed = round % 2 == 1;
	}
	else if (tt >= m_duration) 
	{
		tt = m_duration;
		m_ended = 1;
	}

	/*float time = reversed ? (m_duration - tt) : tt;
	time /= m_duration;*/
	
	m_normalizedTime = evaluate(m_easeType, reversed ? (m_duration - tt) : tt, m_duration,
		1.70158f, 0);

	value.setZero();
	deltaValue.setZero();

	for (int i = 0; i < 4; i++) {
		float n1 = startValue[i];
		float n2 = endValue[i];
		float f = n1 + (n2 - n1) * m_normalizedTime;
		if (m_snapping)
			f = roundf(f);
		deltaValue[i] = f - value[i];
		value[i] = f;
	}
	value.d = value.x;

	if (m_target != nullptr && m_propType != TweenPropType::None)
	{
		SceneObject* obj = dynamic_cast<SceneObject*>(m_target);
		if (obj != nullptr)
		{
			TweenPropTypeUtils::setProps(obj, m_referenceId, m_propType, value);
		}
	}

	invokeUpdateCallback();
}

void Tweener::invokeStartCallback()
{
	if (m_onStart != nullptr)
		m_onStart(this);
}

void Tweener::invokeUpdateCallback()
{
	if (m_onUpdate != nullptr)
		m_onUpdate(this);
}

void Tweener::invokeCompleteCallback()
{
	if (m_onComplete != nullptr)
		m_onComplete(this);
	if (m_onComplete0 != nullptr)
		m_onComplete0();
}

Tweener* Tweener::to(float start, float end, float duration)
{
	startValue.x = start;
	endValue.x = end;
	value.x = start;
	m_duration = duration;
	return this;
}

Tweener* Tweener::to(const Vec2& start, const Vec2& end, float duration)
{
	startValue.setVec2(start);
	endValue.setVec2(end);
	value.setVec2(start);
	m_duration = duration;
	return this;
}

Tweener* Tweener::to(const Vec3& start, const Vec3& end, float duration)
{
	startValue.setVec3(start);
	endValue.setVec3(end);
	value.setVec3(start);
	m_duration = duration;
	return this;
}

Tweener* Tweener::to(const Vec4& start, const Vec4& end, float duration)
{
	startValue.setVec4(start);
	endValue.setVec4(end);
	value.setVec4(start);
	m_duration = duration;
	return this;
}

NS_IGE_SCENE_END