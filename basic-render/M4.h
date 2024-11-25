#pragma once

#include "V4.h"
#include <math.h>

union M4
{
	V4 v[4];
	f32 e[16];

	inline static M4 Identity();
	inline static M4 Scale(f32 X, f32 Y, f32 Z);
	inline static M4 Rotation(f32 X, f32 Y, f32 Z);
	inline static M4 Translation(f32 X, f32 Y, f32 Z);

	inline V4 operator*(const V4& B) const;
	inline M4 operator*(const M4& B) const;
};

inline M4 M4::Identity()
{
	M4 Result = {};
	Result.v[0] = V4(1.0f, 0.0f, 0.0f, 0.0f);
	Result.v[1] = V4(0.0f, 1.0f, 0.0f, 0.0f);
	Result.v[2] = V4(0.0f, 0.0f, 1.0f, 0.0f);
	Result.v[3] = V4(0.0f, 0.0f, 0.0f, 1.0f);
	return Result;
}

inline M4 M4::Scale(f32 X, f32 Y, f32 Z)
{
	M4 Result = Identity();
	Result.v[0].x = X;
	Result.v[1].y = Y;
	Result.v[2].z = Z;
	return Result;
}

inline M4 M4::Rotation(f32 X, f32 Y, f32 Z)
{
	M4 RotateX = Identity();
	RotateX.v[1].y = cos(X);
	RotateX.v[2].y = -sin(X);
	RotateX.v[1].z = sin(X);
	RotateX.v[2].z = cos(X);

	M4 RotateY = Identity();
	RotateY.v[0].x = cos(Y);
	RotateY.v[2].x = -sin(Y);
	RotateY.v[0].z = sin(Y);
	RotateY.v[2].z = cos(Y);

	M4 RotateZ = Identity();
	RotateZ.v[0].x = cos(Z);
	RotateZ.v[1].x = -sin(Z);
	RotateZ.v[0].y = sin(Z);
	RotateZ.v[1].y = cos(Z);

	return RotateZ * RotateY * RotateX;
}

inline M4 M4::Translation(f32 X, f32 Y, f32 Z)
{
	M4 Result = Identity();
	Result.v[3].xyz = V3(X, Y, Z);
	return Result;
}

inline V4 M4::operator*(const V4& B) const
{
	return v[0] * B.x + v[1] * B.y + v[2] * B.z + v[3] * B.w;
}

inline M4 M4::operator*(const M4& B) const
{
	M4 Result = {};
	Result.v[0] = (*this) * B.v[0];
	Result.v[1] = (*this) * B.v[1];
	Result.v[2] = (*this) * B.v[2];
	Result.v[3] = (*this) * B.v[3];
	return Result;
}
