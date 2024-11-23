#include "v2.h"

V2::V2() : x(0), y(0) {}

V2::V2(float value) : x(value), y(value) {}

V2::V2(float X, float Y) : x(X), y(Y) {}

V2 V2::operator+(const V2& other) const
{
	return V2(x + other.x, y + other.y);
}

V2 V2::operator*(float scalar) const
{
	return V2(x * scalar, y * scalar);
}

V2 V2::operator*(const V2& other) const
{
	return V2(x * other.x, y * other.y);
}

V2 V2::operator/(float scalar) const
{
	return V2(x / scalar, y / scalar);
}

V2 operator*(float scalar, const V2& v2)
{
	return V2(scalar * v2.x, scalar * v2.y);
}
