#include "v3.h"

V3::V3() : x(0), y(0), z(0) {}

V3::V3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

V3 V3::operator+(const V3& other) const
{
	return V3(x + other.x, y + other.y, z + other.z);
}

V2 V3::getXY() const
{
	return V2(x, y);
}

V2 V3::getYZ() const
{
	return V2(y, z);
}
