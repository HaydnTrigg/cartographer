#pragma once
#include "Blam/Engine/animations/animation_channel.h"
#include "Blam/Engine/Objects/Objects.h"
#include "Blam/Math/real_math.h"

struct s_device_data_group : s_object_data_definition
{
	DWORD flags;
	DWORD device_groups_datum;
	float power;
	DWORD gap138;
	DWORD device_groups_datum_2;
	float gap140;
	DWORD gap144;
	float gap148;
	real_matrix4x3 matrix;
	DWORD gap180;
	DWORD gap184;
	DWORD gap188;
	c_animation_channel some_position_struct;
	c_animation_channel some_overlay_struct2;
};
CHECK_STRUCT_SIZE(s_device_data_group, 0x1CC);

bool __cdecl device_set_position_animation(datum device_datum, string_id animation);
void device_set_power(const datum device_datum, const float power);
