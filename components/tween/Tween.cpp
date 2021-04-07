#include "components/tween/Tween.h"
#include "components/tween/TweenManager.h"
#include "components/tween/Tweener.h"
#include "components/TransformComponent.h"


#include "scene/SceneManager.h"
#include "scene/Scene.h"

NS_IGE_SCENE_BEGIN

std::shared_ptr<Tweener> Tween::tween(float start, float end, float duration)
{
	if (SceneManager::getInstance() == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene != nullptr && scene->getTweenManager() != nullptr) {
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->to(start, end, duration);
			return tween;
		}
	}
	return nullptr;
}

std::shared_ptr<Tweener> Tween::tween(const Vec2& start, const Vec2& end, float duration)
{
	if (SceneManager::getInstance() == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene != nullptr && scene->getTweenManager() != nullptr) {
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->to(start, end, duration);
			return tween;
		}
	}
	return nullptr;
}

std::shared_ptr<Tweener> Tween::tween(const Vec3& start, const Vec3& end, float duration)
{
	if (SceneManager::getInstance() == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene != nullptr && scene->getTweenManager() != nullptr) {
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->to(start, end, duration);
			return tween;
		}
	}
	return nullptr;
}

std::shared_ptr<Tweener> Tween::tween(const Vec4& start, const Vec4& end, float duration)
{
	if (SceneManager::getInstance() == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene != nullptr && scene->getTweenManager() != nullptr) {
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->to(start, end, duration);
			return tween;
		}
	}
	return nullptr;
}


std::shared_ptr<Tweener> Tween::doMove(TransformComponent* component, Vec3 start, Vec3 end, float duration)
{
	if (SceneManager::getInstance() == nullptr || component == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene)
	{
		auto owner = component->getOwner();
		auto id = component->getInstanceId();
		if (scene->getTweenManager()->isTweening(owner, id, TweenPropType::Position)) 
		{
			scene->getTweenManager()->killTweens(owner, id, TweenPropType::Position, false);
		}
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->setTarget(owner, id, TweenPropType::Position)
				->to(start, end, duration);
		}
		return tween;
	}
	return nullptr;
}

std::shared_ptr<Tweener> Tween::doMoveX(TransformComponent* component, float start, float end, float duration)
{
	if (SceneManager::getInstance() == nullptr || component == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene)
	{
		auto owner = component->getOwner();
		auto id = component->getInstanceId();
		if (scene->getTweenManager()->isTweening(owner, id, TweenPropType::X))
		{
			scene->getTweenManager()->killTweens(owner, id, TweenPropType::X, false);
		}
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->setTarget(owner, id, TweenPropType::X)
				->to(start, end, duration);
		}
		return tween;
	}
	return nullptr;
}

std::shared_ptr<Tweener> Tween::doMoveY(TransformComponent* component, float start, float end, float duration)
{
	if (SceneManager::getInstance() == nullptr || component == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene)
	{
		auto owner = component->getOwner();
		auto id = component->getInstanceId();
		if (scene->getTweenManager()->isTweening(owner, id, TweenPropType::Y))
		{
			scene->getTweenManager()->killTweens(owner, id, TweenPropType::Y, false);
		}
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->setTarget(owner, id, TweenPropType::Y)
				->to(start, end, duration);
		}
		return tween;
	}
	return nullptr;
}

std::shared_ptr<Tweener> Tween::doMoveZ(TransformComponent* component, float start, float end, float duration)
{
	if (SceneManager::getInstance() == nullptr || component == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene)
	{
		auto owner = component->getOwner();
		auto id = component->getInstanceId();
		if (scene->getTweenManager()->isTweening(owner, id, TweenPropType::Z))
		{
			scene->getTweenManager()->killTweens(owner, id, TweenPropType::Z, false);
		}
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->setTarget(owner, id, TweenPropType::Z)
				->to(start, end, duration);
		}
		return tween;
	}
	return nullptr;
}


std::shared_ptr<Tweener> Tween::doRotate(TransformComponent* component, Vec3 start, Vec3 end, float duration) {
	if (SceneManager::getInstance() == nullptr || component == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene)
	{
		auto owner = component->getOwner();
		auto id = component->getInstanceId();
		if (scene->getTweenManager()->isTweening(owner, id, TweenPropType::Rotation))
		{
			scene->getTweenManager()->killTweens(owner, id, TweenPropType::Rotation, false);
		}
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->setTarget(owner, id, TweenPropType::Rotation)
				->to(start, end, duration);
		}
		return tween;
	}
	return nullptr;
}

std::shared_ptr<Tweener> Tween::doRotate(TransformComponent* component, Quat start, Quat end, float duration) {
	if (SceneManager::getInstance() == nullptr || component == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene)
	{
		auto owner = component->getOwner();
		auto id = component->getInstanceId();
		if (scene->getTweenManager()->isTweening(owner, id, TweenPropType::RotationQuat))
		{
			scene->getTweenManager()->killTweens(owner, id, TweenPropType::RotationQuat, false);
		}
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			Vec4 _start(start.X(), start.Y(), start.Z(), start.W());
			Vec4 _end(end.X(), end.Y(), end.Z(), end.W());

			tween->setTarget(owner, id, TweenPropType::RotationQuat)
				->to(_start, _end, duration);
		}
		return tween;
	}
	return nullptr;
}

std::shared_ptr<Tweener> Tween::doScale(TransformComponent* component, Vec3 start, Vec3 end, float duration) {
	if (SceneManager::getInstance() == nullptr || component == nullptr) return nullptr;
	auto scene = SceneManager::getInstance()->getCurrentScene();
	if (scene)
	{
		auto owner = component->getOwner();
		auto id = component->getInstanceId();
		if (scene->getTweenManager()->isTweening(owner, id, TweenPropType::Scale))
		{
			scene->getTweenManager()->killTweens(owner, id, TweenPropType::Scale, false);
		}
		auto tween = scene->getTweenManager()->createTween();
		if (tween) {
			tween->setTarget(owner, id, TweenPropType::Scale)
				->to(start, end, duration);
		}
		return tween;
	}
	return nullptr;
}

NS_IGE_SCENE_END