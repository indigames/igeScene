#ifndef __RECT_TRANSFORM_UTILITY_H__
#define __RECT_TRANSFORM_UTILITY_H__

#include "core/igeSceneMacros.h"
#include "utils/PyxieHeaders.h"

#include "components/gui/RectTransform.h"

USING_NS_PYXIE

NS_IGE_SCENE_BEGIN

class RectTransformUtility
{
public:
	static bool RectangleContainsPoint(RectTransform* rect, Vec2 point);
	static Vec2 WorldToLocalPointInRectable(RectTransform* rect, Vec2 point);	
};

NS_IGE_SCENE_END

#endif //__RECT_TRANSFORM_UTILITY_H__
