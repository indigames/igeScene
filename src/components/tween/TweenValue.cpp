#include "components/tween/TweenValue.h"

NS_IGE_SCENE_BEGIN

TweenValue::TweenValue() : x(0), y(0), z(0), w(0), d(0)
{

}

Vec2 TweenValue::getVec2() const
{
	return Vec2(x, y);
}

void TweenValue::setVec2(const Vec2& value) 
{
	x = value.X();
	y = value.Y();
}

Vec3 TweenValue::getVec3() const
{
	return Vec3(x, y, z);
}

void TweenValue::setVec3(const Vec3& value)
{
	x = value.X();
	y = value.Y();
	z = value.Z();
}

Vec4 TweenValue::getVec4() const
{
	return Vec4(x, y, z, w);
}

void TweenValue::setVec4(const Vec4& value)
{
	x = value.X();
	y = value.Y();
	z = value.Z();
	w = value.W();
}

float TweenValue::operator[](int index) const
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
#ifdef __cpp_exceptions
    default:
        throw "Index out of bounds: " + std::to_string(index);
#endif
    }
}

float& TweenValue::operator[](int index)
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
#ifdef __cpp_exceptions
    default:
        throw "Index out of bounds: " + std::to_string(index);
#endif
    }
}

void TweenValue::setZero()
{
    x = y = z = w = d = 0;
}

NS_IGE_SCENE_END