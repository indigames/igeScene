#ifndef __TWEEN_VALUE_H__
#define __TWEEN_VALUE_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

USING_NS_PYXIE
namespace ige::scene {

	class TweenValue
	{
	public:
		float x;
		float y;
		float z;
		float w;
		double d;

		TweenValue();
		~TweenValue() {}

		//! Tween Vec2
		Vec2 getVec2() const;
		void setVec2(const Vec2& value);
		//! Tween Vec3
		Vec3 getVec3() const;
		void setVec3(const Vec3& value);
		//! Tween Vec4
		Vec4 getVec4() const;
		void setVec4(const Vec4& value);

		void setZero();
		float operator[] (int index) const;
		float& operator[] (int index);
	};

}

#endif  //__TWEEN_VALUE_H__