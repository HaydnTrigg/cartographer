#pragma once
#include "Blam/Math/real_math.h"

void quaternion_transform_point(const real_quaternion* in, const real_point3d* point, real_point3d* out);
void quaternions_multiply(const real_quaternion* in, const real_quaternion* in2, real_quaternion* out);
void orientations_multiply(const real_orientation* in, const real_orientation* in2, real_orientation* out);
