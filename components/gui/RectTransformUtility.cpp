#include "components/gui/RectTransformUtility.h"

NS_IGE_SCENE_BEGIN

bool RectTransformUtility::RectangleContainsPoint(RectTransform* rect, Vec2 point)
{
	if (rect == nullptr) return false;
	auto cpoint = rect->globalToLocal(Vec3(point[0], point[1], rect->getWorldPosition()[3]));
	auto size = rect->getSize();
	if(std::abs(cpoint[0]) <= (size[0] / 2) && std::abs(cpoint[1]) <= (size[1] / 2))
		return true;
	return false;
}

Vec2 RectTransformUtility::WorldToLocalPointInRectable(RectTransform* rect, Vec2 point)
{
	Vec2 outPoint(0,0);
	if (rect == nullptr) return outPoint;
	auto cpoint = rect->globalToLocal(Vec3(point[0], point[1], rect->getWorldPosition()[3]));
	outPoint[0] = cpoint[0];
	outPoint[1] = cpoint[1];
	return outPoint;
}

NS_IGE_SCENE_END