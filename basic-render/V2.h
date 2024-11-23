#pragma once

union V2
{
	struct
	{
		float x, y;
	};

	float e[2];

	V2();
	V2(float value);
	V2(float X, float Y);

	V2 operator+(const V2& other) const;
	V2 operator*(float scalar) const;
	V2 operator*(const V2& other) const;
	V2 operator/(float scalar) const;

	friend V2 operator*(float scalar, const V2& v2);
};
