#ifndef __TWEEN_PROP_TYPE_H__
#define __TWEEN_PROP_TYPE_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

USING_NS_PYXIE
namespace ige::scene {

	class TweenValue;
	class SceneObject;

	enum class TweenPropType
	{
		None,
		X,
		Y,
		Z,
		Position,
		Width,
		Height,
		Size,
		ScaleX,
		ScaleY,
		ScaleZ,
		Scale,
		Rotation,
		RotationQuat,
		Alpha,
		Progress,
	};

	class TweenPropTypeUtils {
	public:
		static void setProps(SceneObject* target, uint64_t componentID, TweenPropType type, const TweenValue& value);
	};

}

#endif __TWEEN_PROP_TYPE_H__