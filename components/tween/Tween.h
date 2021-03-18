#ifndef __TWEEN_H__
#define __TWEEN_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"


USING_NS_PYXIE
namespace ige::scene {

	class Tweener;
	class TransformComponent;

	class Tween
	{
	public:
		static std::shared_ptr<Tweener> tween(float start, float end, float duration);
		static std::shared_ptr<Tweener> tween(Vec2 start, Vec2 end, float duration);
		static std::shared_ptr<Tweener> tween(Vec3 start, Vec3 end, float duration);
		static std::shared_ptr<Tweener> tween(Vec4 start, Vec4 end, float duration);

		static std::shared_ptr<Tweener> doMove(TransformComponent* component, Vec3 start, Vec3 end, float duration);
		static std::shared_ptr<Tweener> doMoveX(TransformComponent* component, float start, float end, float duration);
		static std::shared_ptr<Tweener> doMoveY(TransformComponent* component, float start, float end, float duration);
		static std::shared_ptr<Tweener> doMoveZ(TransformComponent* component, float start, float end, float duration);

		static std::shared_ptr<Tweener> doRotate(TransformComponent* component, Vec3 start, Vec3 end, float duration);
		static std::shared_ptr<Tweener> doRotate(TransformComponent* component, Quat start, Quat end, float duration);

		static std::shared_ptr<Tweener> doScale(TransformComponent* component, Vec3 start, Vec3 end, float duration);
	};

}

#endif