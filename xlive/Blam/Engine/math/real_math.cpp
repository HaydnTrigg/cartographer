#include "stdafx.h"
#include "real_math.h"
#include "Blam/Math/real_vector3d.h"

void quaternion_transform_point(const real_quaternion* in, const real_point3d* point, real_point3d* out)
{
    // We need to copy these to their own variables for some reason or else the math ends up wrong?????????
    const real_point3d local_point = *point;
    const real_quaternion local_quaternion = *in;

    float double_w_squared_minus_one = ((local_quaternion.w * local_quaternion.w) * 2.0f) - 1.0f;
    float point_and_quad_multiplied_times_two = (((point->x * local_quaternion.vector.i) + (local_point.y * local_quaternion.vector.j)) + (point->z * local_quaternion.vector.k)) * 2.0f;
    float double_w = local_quaternion.w * 2.0f;
    float xz_difference = (local_point.x * local_quaternion.vector.k) - (local_quaternion.vector.i * point->z);
    out->x = (
                (
                    (
                        (local_quaternion.vector.j * point->z) - (local_point.y * in->vector.k)
                    ) * double_w
                ) + (local_quaternion.vector.i * point_and_quad_multiplied_times_two)
             ) +  
             (local_point.x * double_w_squared_minus_one);

    out->y = ((in->vector.j * point_and_quad_multiplied_times_two) + (point->y * double_w_squared_minus_one)) + (xz_difference * double_w);
   
    out->z = ((in->vector.k * point_and_quad_multiplied_times_two) + (point->z * double_w_squared_minus_one)) + 
             (
                (
                    (local_quaternion.vector.i * local_point.y) - (local_point.x * local_quaternion.vector.j)
                ) * double_w
             );
}

void quaternions_multiply(const real_quaternion* in, const real_quaternion* in2, real_quaternion* out)
{
    out->vector.i = (((in2->w * in->vector.i) + (in2->vector.k * in->vector.j)) + (in->w * in2->vector.i)) - (in2->vector.j * in->vector.k);
    out->vector.j = (((in->vector.k * in2->vector.i) + (in2->w * in->vector.j)) + (in->w * in2->vector.j)) - (in->vector.i * in2->vector.k);
    out->vector.k = (((in2->w * in->vector.k) + (in2->vector.j * in->vector.i)) + (in->w * in2->vector.k)) - (in2->vector.i * in->vector.j);
    out->w = (((in2->w * in->w) - (in->vector.i * in2->vector.i)) - (in2->vector.j * in->vector.j)) - (in->vector.k * in2->vector.k);
}

void orientations_multiply(const real_orientation* in, const real_orientation* in2, real_orientation* out)
{    
    quaternions_multiply(in, in2, out);
    quaternion_transform_point(in, &in2[1].vector, &out[1].vector);
    if (in[1].w == 1.0f)
    {
        out[1].w = in2[1].w;
    }
    else
    {
        scale_vector3d(&out[1].vector, in[1].w, &out[1].vector);
        out[1].w = in2[1].w * in[1].w;
    }
    add_vectors3d(&in[1].vector, &out[1].vector, &out[1].vector);
}