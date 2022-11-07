#pragma once

#include "Blam\Math\BlamMath.h"

#pragma pack(push, 1)
struct s_camera
{
	real_point3d point;
	real_vector3d forward;
	real_vector3d up;
	__int8 gap_24[4];
	float field_of_view;
	float field_2C;
	rect2d screen;
	rect2d frame_bounds;
	float field_40;
	float field_44;
	float field_48;
	__int8 gap_4C[32];
	float field_6C;
	float field_70;
};

struct window_bound
{
	DWORD single_view___;
	DWORD window_bound_index;
	DWORD local_player_index;
	s_camera camera0;
	s_camera camera1;
	BYTE some_unused_data[36];
};
#pragma pack(pop)

__int8 world_to_screen(s_camera* camera, real_matrix4x3* projection, rect2d* screen, real_point3d* text_world_position, float* text_screen_location);
