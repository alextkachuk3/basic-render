#pragma once
#include "v2.h"

union V3
{
	struct
	{
		float x, y, z;
	};

	struct
	{
		V2 xy;
		float Ignored0;
	};

	struct
	{
		float Ignored1;
		V2 yz;
	};

	float e[3];

	V3();
	V3(float X, float Y, float Z);

	V3 operator+(const V3& other) const;
	V2 getXY() const;
	V2 getYZ() const;
};
