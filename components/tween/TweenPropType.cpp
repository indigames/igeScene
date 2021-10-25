#include "components/tween/TweenPropType.h"
#include "components/tween/TweenValue.h"
#include "scene/SceneObject.h"

#include "components/IgeComponent.h"

NS_IGE_SCENE_BEGIN

void TweenPropTypeUtils::setProps(SceneObject* target, uint64_t componentID, TweenPropType type, const TweenValue& value)
{
	SceneObject* g = dynamic_cast<SceneObject*>(target);
	if (g == nullptr) return;
	auto comp = g->getComponent(componentID);
	if (comp == nullptr) return;
	switch (type) {
	case TweenPropType::Position:
		{
			auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				trans->setLocalPosition(value.getVec3());
			}
		}
		break;
	case TweenPropType::X:
		{
			auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				auto position = trans->getLocalPosition();
				trans->setLocalPosition(Vec3(value.x, position.Y(), position.Z()));
			}
		}
		break;
	case TweenPropType::Y:
		{
		auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				auto position = trans->getLocalPosition();
				trans->setLocalPosition(Vec3(position.X(), value.x, position.Z()));
			}
		}
		break;
	case TweenPropType::Z:
		{
			auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				auto position = trans->getLocalPosition();
				trans->setLocalPosition(Vec3(position.X(), position.Y(), value.x));
			}
		}
		break;
	case TweenPropType::Width:
		{
			auto trans = std::dynamic_pointer_cast<RectTransform>(comp);
			if (trans) {
				auto size = trans->getSize();
				trans->setSize(Vec2(value.x, size.Y()));
			}
		}
		break;
	case TweenPropType::Height:
		{
			auto trans = std::dynamic_pointer_cast<RectTransform>(comp);
			if (trans) {
				auto size = trans->getSize();
				trans->setSize(Vec2(size.X(), value.x));
			}
		}
		break;
	case TweenPropType::Rotation:
		{
			auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				trans->setLocalRotation(value.getVec3());
			}
		}
		break;
	case TweenPropType::RotationQuat:
		{
			auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				Quat quat(value.x, value.y, value.z, value.w);
				trans->setLocalRotation(quat);
			}
		}
		break;
	case TweenPropType::Scale:
		{
			auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				trans->setLocalScale(value.getVec3());
			}
		}
		break;
	case TweenPropType::ScaleX:
		{
			auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				auto scale = trans->getScale();
				trans->setLocalScale(Vec3(value.x, scale.Y(), scale.Z()));
			}
		}
		break;
	case TweenPropType::ScaleY:
		{
			auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				auto scale = trans->getScale();
				trans->setLocalScale(Vec3(scale.X(), value.x, scale.Z()));
			}
		}
		break;
	case TweenPropType::ScaleZ:
		{
			auto trans = std::dynamic_pointer_cast<TransformComponent>(comp);
			if (trans) {
				auto scale = trans->getScale();
				trans->setLocalScale(Vec3(scale.X(), scale.Y(), value.x));
			}
		}
		break;
	case TweenPropType::Alpha:
		{
			auto sprite = std::dynamic_pointer_cast<SpriteComponent>(comp);
			if (sprite) {
				sprite->setAlpha(value.x);
			}
		}
		break;
	case TweenPropType::Progress:
		{
			auto img = std::dynamic_pointer_cast<UIImage>(comp);
			if (img) {
				img->setFillAmount(value.x);
			}
		}
		break;
	}
}

NS_IGE_SCENE_END