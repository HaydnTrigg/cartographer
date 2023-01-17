#include "stdafx.h"
#include "real_math.h"

#include "Blam/Engine/structures/structures.h"

float plane3d_distance_to_point(const real_plane3d* plane, const real_point3d* point)
{
    return ((((point->x * plane->normal.i) + (point->y * plane->normal.j)) + (point->z * plane->normal.k)) - plane->distance);
}

__int16 projection_from_vector3d(const real_vector3d* vector)
{
    real_vector3d abs_vector;

    abs_vector.i = fabs(vector->i);
    abs_vector.j = fabs(vector->j);
    abs_vector.k = fabs(vector->k);
    if (abs_vector.k >= abs_vector.j)
    {
        if (abs_vector.k >= abs_vector.i)
        {
            return 2;
        }
    }
    return abs_vector.j >= abs_vector.i;
}

bool projection_sign_from_vector3d(const real_vector3d* vector, short projection)
{
    return *(&vector->i + projection) > 0.0;
}

void project_point3d(const real_point3d* point, const short projection, const bool a3, real_point2d* out)
{
    int index = 3 * (a3 + 2 * projection);
    out->x = *(&point->x + global_projection3d_mappings[index]);
    out->y = *(&point->x + global_projection3d_mappings[index + 1]);
}

float magnitude_squared2d(const real_point2d* point)
{
    return ((point->x * point->x) + (point->y * point->y));
}

float cross_product2d(const real_vector2d *p1, const real_vector2d* p2)
{
    return ((p1->i * p2->j) - (p1->j * p2->i));
}

void vector_from_points2d(const real_point2d* initial_point, const real_point2d* new_point, real_vector2d* vector)
{
    vector->i = new_point->x - initial_point->x;
    vector->j = new_point->y - initial_point->y;
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
    // Move inputs to local variables in case one of our inputs is also an output
    real_quaternion temp[2];
    const real_quaternion* p_in = in;
    if (in == out)
    {
        memcpy(temp, in, sizeof(temp));
        p_in = temp;
    }
    const real_quaternion* p_in2 = in2;
    if (in2 == out)
    {
        memcpy(temp, in2, sizeof(temp));
        p_in2 = temp;
    }

    out->vector.i = (((p_in2->w * p_in->vector.i) + (p_in2->vector.k * p_in->vector.j)) + (p_in->w * p_in2->vector.i)) - (p_in2->vector.j * p_in->vector.k);
    out->vector.j = (((p_in->vector.k * p_in2->vector.i) + (p_in2->w * p_in->vector.j)) + (p_in->w * p_in2->vector.j)) - (p_in->vector.i * p_in2->vector.k);
    out->vector.k = (((p_in2->w * p_in->vector.k) + (p_in2->vector.j * p_in->vector.i)) + (p_in->w * p_in2->vector.k)) - (p_in2->vector.i * p_in->vector.j);
    out->w = (((p_in2->w * p_in->w) - (p_in->vector.i * p_in2->vector.i)) - (p_in2->vector.j * p_in->vector.j)) - (p_in->vector.k * p_in2->vector.k);
}

void orientations_multiply(const real_orientation* in, const real_orientation* in2, real_orientation* out)
{    
    // Move inputs to local variables in case one of our inputs is also an output
    real_orientation temp[2];
    const real_orientation* p_in = in;
    if (in == out)
    {
        memcpy(temp, in, sizeof(temp));
        p_in = temp;
    }
    const real_orientation* p_in2 = in2;
    if (in2 == out)
    {
        memcpy(temp, in2, sizeof(temp));
        p_in2 = temp;
    }

    quaternions_multiply(p_in, p_in2, out);
    quaternion_transform_point(p_in, &p_in2[1].vector, &out[1].vector);
    if (p_in[1].w == 1.0f)
    {
        out[1].w = p_in2[1].w;
    }
    else
    {
        scale_vector3d(&out[1].vector, p_in[1].w, &out[1].vector);
        out[1].w = p_in2[1].w * p_in[1].w;
    }
    add_vectors3d(&p_in[1].vector, &out[1].vector, &out[1].vector);
}
