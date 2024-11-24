#pragma once
#include "v2.h"

union V3
{
	struct
	{
		f32 x, y, z;
	};

	struct
	{
		V2 xy;
		f32 Ignored0;
	};

	struct
	{
		float Ignored1;
		V2 yz;
	};

	f32 e[3];

	V3();
	V3(f32 X, f32 Y, f32 Z);

	V3 operator+(const V3& other) const;
	V2 getXY() const;
	V2 getYZ() const;
};
