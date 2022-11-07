#include "stdafx.h"

#include "Camera.h"

__int8 world_to_screen(s_camera* camera, real_matrix4x3* projection, rect2d* screen, real_point3d* text_world_position, float* text_screen_location)
{
    rect2d* p_screen; // ecx
    byte result; // al
    float vertical_size; // xmm4_4
    float horizontal_size; // xmm5_4

    p_screen = screen;
    result = 0;
    if (!screen)
        p_screen = &camera->screen;
    if (text_world_position->z >= 0.0)
    {
        text_screen_location[0] = 0.0;
        text_screen_location[1] = 0.0;
    }
    else
    {
        vertical_size = (p_screen->left - p_screen->right);
        horizontal_size = (p_screen->bottom - p_screen->top);
        text_screen_location[0] = ((projection[2].left.i * text_world_position->x) + (projection[2].translation.z * text_world_position->z)) * (-1.0 / text_world_position->z);
        text_screen_location[1] = -0.0 - (((projection[2].up.k * text_world_position->y) + (projection[3].scale * text_world_position->z)) * (-1.0 / text_world_position->z));
        if (text_screen_location[0] >= -1.0 && text_screen_location[0] <= 1.0 && text_screen_location[1] >= -1.0 && text_screen_location[1] <= 1.0)
            result = 1;
        text_screen_location[0] = (((text_screen_location[0] + 1.0) * 0.5) * vertical_size) + p_screen->left;
        text_screen_location[1] = (((text_screen_location[1] + 1.0) * 0.5) * horizontal_size) + p_screen->top;
    }
    return result;
}