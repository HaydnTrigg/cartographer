#pragma once
#include "Blam/Engine/animations/animation_channel.h"
#include "Blam/Engine/Objects/Objects.h"
#include "Blam/Math/real_math.h"

struct s_device_animation_args
{
	float current_position;	// 0.0 -> 1.0 range
	float start_velocity;
	float target_position1;	// 0.0 -> 1.0 range
	float target_position2; // 0.0 -> 1.0 range
	float current_displacement;
	float displacement_to_travel;
	float initial_velocity;
	float total_time;
};
CHECK_STRUCT_SIZE(s_device_animation_args, 32);

struct s_device_data_group : s_object_data_definition
{
	DWORD flags;
	DWORD device_groups_datum;
	float power;
	DWORD gap138;
	DWORD device_position_group_datum;
	float device_position;
	DWORD device_flags;
	float gap148;
	s_device_animation_args position_args;
	s_device_animation_args overlay_args;
	c_channel_base position_channel;
	c_channel_base overlay_channel;
};
CHECK_STRUCT_SIZE(s_device_data_group, 0x1CC);

bool __cdecl device_set_position_animation(const datum device_datum, const string_id animation);
void device_set_power(const datum device_datum, const float power);
bool __cdecl device_set_position_track(const datum device_datum, const string_id animation_string_id, float interpolation_time);
void device_animate(s_device_animation_args* control,
	float target_pos,
	float total_time,
	float acceleration_time,
	float deceleration_time,
	float start_velocity,
	float end_velocity);
void device_animate_position(const datum device_datum,
	const float target_pos,
	const float total_time,
	const float acceleration_time,
	const float deceleration_time,
	const bool interpolate);
void device_animate_overlay(const datum device_datum,
	const float target_pos,
	const float total_time,
	const float acceleration_time,
	const float deceleration_time);
