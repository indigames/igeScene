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
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				trans->setPosition(value.getVec3());
			}
		}
		break;
	case TweenPropType::X:
		{
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				auto position = trans->getPosition();
				trans->setPosition(Vec3(value.x, position.Y(), position.Z()));
			}
		}
		break;
	case TweenPropType::Y:
		{
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				auto position = trans->getPosition();
				trans->setPosition(Vec3(position.X(), value.x, position.Z()));
			}
		}
		break;
	case TweenPropType::Z:
		{
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				auto position = trans->getPosition();
				trans->setPosition(Vec3(position.X(), position.Y(), value.x));
			}
		}
		break;
	case TweenPropType::Width:
		{
			RectTransform* trans = dynamic_cast<RectTransform*>(comp.get());
			if (trans) {
				auto size = trans->getSize();
				trans->setSize(Vec2(value.x, size.Y()));
			}
		}
		break;
	case TweenPropType::Height:
		{
			RectTransform* trans = dynamic_cast<RectTransform*>(comp.get());
			if (trans) {
				auto size = trans->getSize();
				trans->setSize(Vec2(size.X(), value.x));
			}
		}
		break;
	case TweenPropType::Rotation:
		{
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				trans->setRotation(value.getVec3());
			}
		}
		break;
	case TweenPropType::RotationQuat:
		{
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				Quat quat(value.x, value.y, value.z, value.w);
				trans->setRotation(quat);
			}
		}
		break;
	case TweenPropType::Scale:
		{
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				trans->setScale(value.getVec3());
			}
		}
		break;
	case TweenPropType::ScaleX:
		{
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				auto scale = trans->getScale();
				trans->setScale(Vec3(value.x, scale.Y(), scale.Z()));
			}
		}
		break;
	case TweenPropType::ScaleY:
		{
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				auto scale = trans->getScale();
				trans->setScale(Vec3(scale.X(), value.x, scale.Z()));
			}
		}
		break;
	case TweenPropType::ScaleZ:
		{
			TransformComponent* trans = dynamic_cast<TransformComponent*>(comp.get());
			if (trans) {
				auto scale = trans->getScale();
				trans->setScale(Vec3(scale.X(), scale.Y(), value.x));
			}
		}
		break;
	case TweenPropType::Alpha:
		{
			SpriteComponent* sprite = dynamic_cast<SpriteComponent*>(comp.get());
			if (sprite) {
				sprite->setAlpha(value.x);
			}
		}
		break;
	case TweenPropType::Progress:
		{
			UIImage* img = dynamic_cast<UIImage*>(comp.get());
			if (img) {
				img->setFillAmount(value.x);
			}
		}
		break;
	}
}

NS_IGE_SCENE_END