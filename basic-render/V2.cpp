#include "v2.h"

V2::V2() : x(0), y(0) {}

V2::V2(f32 value) : x(value), y(value) {}

V2::V2(f32 X, f32 Y) : x(X), y(Y) {}

V2 V2::operator+(const V2& other) const
{
	return V2(x + other.x, y + other.y);
}

V2 V2::operator-(const V2& other) const
{
	return V2(x - other.x, y - other.y);
}

V2 V2::operator*(f32 scalar) const
{
	return V2(x * scalar, y * scalar);
}

V2 V2::operator*(const V2& other) const
{
	return V2(x * other.x, y * other.y);
}

V2 V2::operator/(f32 scalar) const
{
	return V2(x / scalar, y / scalar);
}

f32 V2::CrossProduct2d(V2 A, V2 B)
{
	return A.x * B.y - A.y * B.x;
}

V2 operator*(f32 scalar, const V2& v2)
{
	return V2(scalar * v2.x, scalar * v2.y);
}
