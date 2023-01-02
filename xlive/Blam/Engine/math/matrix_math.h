#pragma once
#include "Blam/Math/real_math.h"

struct real_matrix3x3
{
	real_vector3d forward = {};
	real_vector3d left = {};
	real_vector3d up = {};
};

struct real_matrix4x3
{
	float scale = 1.0f;
	real_matrix3x3 vectors;
	real_point3d position = {};

	real_matrix4x3() = default;

	real_matrix4x3(const real_quaternion& rotation)
	{
		set_rotation(rotation);
	}

	real_matrix4x3(const real_quaternion& _rotation, const real_point3d& _position) :
		position(_position)
	{
		set_rotation(_rotation);
	}

	BLAM_MATH_INL void inverse_rotation()
	{
		std::swap(vectors.forward.j, vectors.left.i);
		std::swap(vectors.forward.k, vectors.up.i);
		std::swap(vectors.left.k, vectors.up.j);
	}

	void set_rotation(const real_quaternion& rotation)
	{
		float square_len = rotation.get_square_length();
		assert(square_len != 0.0f);
		float s = 2.0f / square_len;

		auto is = rotation.i * s;
		auto js = rotation.j * s;
		auto ks = rotation.k * s;

		auto iw = rotation.w * is;
		auto jw = rotation.w * js;
		auto kw = rotation.w * ks;

		auto ii = rotation.i * is, jj = rotation.j * js, kk = rotation.k * ks;
		auto ij = rotation.i * js, ik = rotation.i * ks, jk = rotation.j * ks;

		vectors.forward =	{ 1.0f - (jj + kk),  ij - kw,            ik + jw };
		vectors.left =		{ ij + kw,           1.0f - (ii + kk),   jk - iw };
		vectors.up =		{ ik - jw,           jk + iw,            1.0f - (ii + jj) };
	}
};
CHECK_STRUCT_SIZE(real_matrix4x3, 52);

void __fastcall cross_product3d(const real_vector3d* up, const real_vector3d* forward, real_vector3d* out);
void __fastcall matrix3x3_rotation_to_quaternion(const real_matrix3x3* matrix, real_orientation* orientation);
void __fastcall matrix4x3_to_orientation(real_matrix4x3* matrix, real_orientation* orientation);
void __fastcall matrix4x3_rotation_from_vectors(real_matrix4x3* matrix, const real_vector3d* forward, const real_vector3d* up);
void __fastcall matrix4x3_from_point_and_vectors(real_matrix4x3* matrix, const real_point3d* position, const real_vector3d* forward, const real_vector3d* up);
void __fastcall matrix4x3_inverse(const real_matrix4x3* input, real_matrix4x3* out);
void __fastcall matrix4x3_multiply(const real_matrix4x3* matrix1, const real_matrix4x3* matrix2, real_matrix4x3* out);

void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, float fractional_ticks, real_matrix4x3* out_mat);