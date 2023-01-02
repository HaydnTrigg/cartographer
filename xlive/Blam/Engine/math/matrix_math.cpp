#include "stdafx.h"
#include "matrix_math.h"
#include "Blam/Math/real_vector3d.h"

void __fastcall cross_product3d(const real_vector3d* up, const real_vector3d* forward, real_vector3d* out)
{
    out->i = (up->j * forward->k) - (up->k * forward->j);
    out->j = (up->k * forward->i) - (up->i * forward->k);
    out->k = (up->i * forward->j) - (up->j * forward->i);
}

// TODO Reimplement properly later
void __fastcall matrix3x3_rotation_to_quaternion(const real_matrix3x3* matrix, real_orientation* orientation)
{
    typedef void(__cdecl* matrix3x3_rotation_to_quaternion_t)(const real_matrix3x3* matrix, real_orientation* orientation);
    auto p_matrix3x3_rotation_to_quaternion = Memory::GetAddress<matrix3x3_rotation_to_quaternion_t>(0x78FFA);

    p_matrix3x3_rotation_to_quaternion(matrix, orientation);
}

void __fastcall matrix4x3_rotation_from_vectors(real_matrix4x3* matrix, const real_vector3d* forward, const real_vector3d* up)
{
    matrix->scale = 1.0;
    matrix->vectors.forward = *forward;
    cross_product3d(up, forward, &matrix->vectors.left);
    matrix->vectors.up = *up;
    matrix->position = {0.0f, 0.0f, 0.0f};
}

void __fastcall matrix4x3_from_point_and_vectors(real_matrix4x3* matrix, const real_point3d* position, const real_vector3d* forward, const real_vector3d* up)
{
    matrix4x3_rotation_from_vectors(matrix, forward, up);
    matrix->position = *position;
}

void __fastcall matrix4x3_inverse(const real_matrix4x3* input, real_matrix4x3* out)
{
    float temp_float;
    real_point3d inverse_pos;

    if (input->scale == 1.0)
    {
        out->scale = 1.0;
    }
    else
    {
        if (input->scale < 0.0)
        {
            temp_float = (input->scale <= -0.000099999997f ? input->scale : -0.000099999997f);
        }
        else
        {
            temp_float = (input->scale <= 0.000099999997f ? 0.000099999997f : input->scale);
        }
        out->scale = 1.0f / temp_float;
        inverse_pos.x = -input->position.x * out->scale;
        inverse_pos.y = -input->position.y * out->scale;
        inverse_pos.z = -input->position.z * out->scale;
    }
    out->vectors = input->vectors;
    out->inverse_rotation();

    out->position.x = ((inverse_pos.x * out->vectors.forward.i) + (inverse_pos.y * out->vectors.left.i)) + (inverse_pos.z * out->vectors.up.i);
    out->position.y = ((inverse_pos.x * out->vectors.forward.j) + (inverse_pos.y * out->vectors.left.j)) + (inverse_pos.z * out->vectors.up.j);
    out->position.z = ((inverse_pos.x * out->vectors.forward.k) + (inverse_pos.y * out->vectors.left.k)) + (inverse_pos.z * out->vectors.up.k);
}

void __fastcall matrix4x3_multiply(const real_matrix4x3* matrix1, const real_matrix4x3* matrix2, real_matrix4x3* out)
{
    real_matrix4x3 matrix;
    const real_matrix4x3* p_matrix1;
    const real_matrix4x3* p_matrix2;

    p_matrix1 = matrix1;
    p_matrix2 = matrix2;
    if (matrix1 == out)
    {
        memcpy(&matrix, matrix1, sizeof(matrix));
        p_matrix1 = &matrix;
    }
    if (p_matrix2 == out)
    {
        memcpy(&matrix, p_matrix2, sizeof(matrix));
        p_matrix2 = &matrix;
    }

    out->vectors.forward.i = ((p_matrix1->vectors.forward.i * p_matrix2->vectors.forward.i)
        + (p_matrix1->vectors.left.i * p_matrix2->vectors.forward.j))
        + (p_matrix1->vectors.up.i * p_matrix2->vectors.forward.k);
    out->vectors.forward.j = ((p_matrix1->vectors.forward.j * p_matrix2->vectors.forward.i)
        + (p_matrix1->vectors.left.j * p_matrix2->vectors.forward.j))
        + (p_matrix1->vectors.up.j * p_matrix2->vectors.forward.k);
    out->vectors.forward.k = ((p_matrix1->vectors.forward.k * p_matrix2->vectors.forward.i)
        + (p_matrix1->vectors.left.k * p_matrix2->vectors.forward.j))
        + (p_matrix1->vectors.up.k * p_matrix2->vectors.forward.k);
    out->vectors.left.i = ((p_matrix1->vectors.forward.i * p_matrix2->vectors.left.i)
        + (p_matrix1->vectors.left.i * p_matrix2->vectors.left.j))
        + (p_matrix1->vectors.up.i * p_matrix2->vectors.left.k);
    out->vectors.left.j = ((p_matrix1->vectors.forward.j * p_matrix2->vectors.left.i)
        + (p_matrix1->vectors.left.j * p_matrix2->vectors.left.j))
        + (p_matrix1->vectors.up.j * p_matrix2->vectors.left.k);
    out->vectors.left.k = ((p_matrix1->vectors.forward.k * p_matrix2->vectors.left.i)
        + (p_matrix1->vectors.left.k * p_matrix2->vectors.left.j))
        + (p_matrix1->vectors.up.k * p_matrix2->vectors.left.k);
    out->vectors.up.i = ((p_matrix1->vectors.forward.i * p_matrix2->vectors.up.i) + (p_matrix1->vectors.left.i * p_matrix2->vectors.up.j))
        + (p_matrix1->vectors.up.i * p_matrix2->vectors.up.k);
    out->vectors.up.j = ((p_matrix1->vectors.forward.j * p_matrix2->vectors.up.i) + (p_matrix1->vectors.left.j * p_matrix2->vectors.up.j))
        + (p_matrix1->vectors.up.j * p_matrix2->vectors.up.k);
    out->vectors.up.k = ((p_matrix1->vectors.forward.k * p_matrix2->vectors.up.i) + (p_matrix1->vectors.left.k * p_matrix2->vectors.up.j))
        + (p_matrix1->vectors.up.k * p_matrix2->vectors.up.k);

    out->position.x = p_matrix1->position.x
        + (p_matrix1->scale
            * (((p_matrix1->vectors.forward.i * p_matrix2->position.x)
                + (p_matrix1->vectors.left.i * p_matrix2->position.y))
                + (p_matrix1->vectors.up.i * p_matrix2->position.z)));
    out->position.y = p_matrix1->position.y
        + (p_matrix1->scale
            * (((p_matrix1->vectors.forward.j * p_matrix2->position.x)
                + (p_matrix1->vectors.left.j * p_matrix2->position.y))
                + (p_matrix1->vectors.up.j * p_matrix2->position.z)));
    out->position.z = p_matrix1->position.z
        + (p_matrix1->scale
            * (((p_matrix1->vectors.forward.k * p_matrix2->position.x)
                + (p_matrix1->vectors.left.k * p_matrix2->position.y))
                + (p_matrix1->vectors.up.k * p_matrix2->position.z)));

    out->scale = p_matrix1->scale * p_matrix2->scale;
}

void __fastcall matrix4x3_to_orientation(const real_matrix4x3* matrix, real_orientation* orientation)
{
    matrix3x3_rotation_to_quaternion(&matrix->vectors, orientation);
    orientation[1].i = matrix->position.x;
    orientation[1].j = matrix->position.y;
    orientation[1].k = matrix->position.z;
    orientation[1].w = matrix->scale;
}

void matrix4x3_rotation_to_quaternion(const real_matrix4x3* matrix, real_quaternion* out_quat)
{
    typedef void(__cdecl* matrix4x3_rotation_to_quaternion_t)(const real_matrix4x3*, real_quaternion*);
    auto p_matrix4x3_rotation_to_quaternion = Memory::GetAddressRelative<matrix4x3_rotation_to_quaternion_t>(0x4791A2);
    return p_matrix4x3_rotation_to_quaternion(matrix, out_quat);
}

void quaternion_rotation_to_matrix4x3(const real_quaternion* quat_rot, real_matrix4x3* out_m)
{
    typedef void(__cdecl* quaternion_rotation_to_matrix4x3)(real_matrix4x3*, const real_quaternion*);
    auto p_quaternion_to_matrix4x3 = Memory::GetAddressRelative<quaternion_rotation_to_matrix4x3>(0x4778CB);
    return p_quaternion_to_matrix4x3(out_m, quat_rot);
}

void quaternion_interpolate_and_normalize(const real_quaternion* previous, const real_quaternion* current, float fractional_ticks, real_quaternion* out_quaternion)
{
    typedef void(__cdecl* quaternion_interpolate_and_normalize_t)(const real_quaternion*, const real_quaternion*, float, real_quaternion*);
    auto p_quaternion_interpolate_and_normalize = Memory::GetAddressRelative<quaternion_interpolate_and_normalize_t>(0x434F6F);
    return p_quaternion_interpolate_and_normalize(previous, current, fractional_ticks, out_quaternion);
}

void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, float fractional_ticks, real_matrix4x3* out_mat)
{
    real_quaternion q1_previous, q2_target, q3_interpolated;
    // first we convert matrix to quaternion rotation
    matrix4x3_rotation_to_quaternion(previous, &q1_previous);
    matrix4x3_rotation_to_quaternion(target, &q2_target);
    // interpolate the rotation quaternion
    quaternion_interpolate_and_normalize(&q1_previous, &q2_target, fractional_ticks, &q3_interpolated);
    // and convert back to matrix format, since that's what the game uses
    quaternion_rotation_to_matrix4x3(&q3_interpolated, out_mat);
    // interpolate scale and the position of the node
    scale_interpolate(previous->scale, target->scale, fractional_ticks, &out_mat->scale);
    points_interpolate(&previous->position, &target->position, fractional_ticks, &out_mat->position);
}