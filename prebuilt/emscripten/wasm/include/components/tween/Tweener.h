#ifndef __TWEENER_H__
#define __TWEENER_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "components/tween/TweenValue.h"
#include "components/tween/TweenPropType.h"
#include "components/tween/EaseType.h"

USING_NS_PYXIE

namespace ige::scene 
{
	class SceneObject;

	class Tweener
	{
	public:
		typedef std::function<void(Tweener* tweener)> TweenCallback;
		typedef std::function<void()> TweenCallback0;

		enum class LoopType {
			Restart,
			Yoyo,
			Incremental
		};


		Tweener();
		~Tweener();

		void release();

		Tweener* setDelay(float value);
		float getDelay() const { return m_delay; }

		Tweener* setDuration(float value);
		float getDuration() const { return m_duration; }

		Tweener* setBreakpoint(float value);

		Tweener* setRepeat(int repeat, LoopType loopType = LoopType::Restart);
		int getRepeat() const { return m_repeat; }

		Tweener* setLoopType(LoopType loopType);
		LoopType getLoopType() const { return m_loopType; }

		Tweener* setTimeScale(float value);
		float getTimeScale() const { return m_timeScale; }

		Tweener* setSnapping(bool value);
		bool getSnapping() const { return m_snapping; }

		//! EaseType : modify how Tween Value will be updated every frame
		//! Default => same amount from start to end
		//! QuadOut => Fast from start and Slow down when nearly end
		Tweener* setEaseType(EaseType value);
		EaseType getEaseType() const { return m_easeType; }

		Tweener* setTarget(SceneObject* value, uint64_t referenceId);
		Tweener* setTarget(SceneObject* value, uint64_t referenceId, TweenPropType propType);
		SceneObject* getTarget() const { return m_target; }


		Tweener* onUpdate(TweenCallback callback);
		Tweener* onStart(TweenCallback callback);
		Tweener* onComplete(TweenCallback0 callback);
		Tweener* onComplete(TweenCallback callback);

		
		bool isCompleted() const { return m_ended != 0; }
		bool allCompleted() const { return m_ended == 1; }
		Tweener* setPaused(bool paused);

		void seek(float time);
		//! if complete = true : set current value to end;
		void kill(bool complete = false);

		TweenValue startValue;
		TweenValue endValue;
		TweenValue value;
		TweenValue deltaValue;

		Tweener* to(float start, float end, float duration);
		Tweener* to(const Vec2& start, const Vec2& end, float duration);
		Tweener* to(const Vec3& start, const Vec3& end, float duration);
		Tweener* to(const Vec4& start, const Vec4& end, float duration);

	protected:

		static float evaluate(EaseType easeType, float time, float duration, float overshootOrAmplitude = 0, float period = 0);

		void update();

		void invokeStartCallback();
		void invokeUpdateCallback();
		void invokeCompleteCallback();

		void _init();
		void _reset();
		void _update(float dt);

	protected:

		SceneObject* m_target;
		uint64_t m_referenceId;
		TweenPropType m_propType;

		bool m_killed;
		bool m_paused;

		float m_delay;
		float m_duration;
		float m_breakpoint;

		//! -1 : loop
		//! 0 : not repeat
		//! 1 -> n: repeat x times
		int m_repeat;
		LoopType m_loopType;
		float m_timeScale;
		bool m_snapping;
		EaseType m_easeType;

		TweenCallback m_onUpdate;
		TweenCallback m_onStart;
		TweenCallback m_onComplete;
		TweenCallback0 m_onComplete0;

		bool m_started; // flag start
		int m_ended; // flag end ( 0 : running, 1 : all completed; 2 : break)
		float m_elapsedTime;
		float m_normalizedTime;

		friend class TweenManager;
	};

}

#endif //__TWEENER_H__