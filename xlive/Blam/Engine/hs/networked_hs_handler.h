#pragma once
#include "hs_function.h"

#define HS_SENT_BUFFER_SIZE 128
#define HS_SYNC_TABLE_SIZE 30

struct s_networked_hs_function
{
	e_hs_function function_type;
	char arg_buffer[HS_SENT_BUFFER_SIZE];
};

const e_hs_function hs_sync_table[HS_SYNC_TABLE_SIZE]
{
	e_hs_function_fade_out,
	e_hs_function_fade_in,
	e_hs_function_print,
	e_hs_function_player_enable_input,
	e_hs_function_camera_control,
	e_hs_function_ai_allegiance,
	e_hs_function_sound_impulse_start,
	e_hs_function_ai_play_line_on_object,
	e_hs_function_camera_set_animation_relative,
	e_hs_function_cinematic_start,
	e_hs_function_cinematic_stop,
	e_hs_function_custom_animation_relative,
	//e_hs_function_object_cinematic_lod,
	e_hs_function_device_animate_position,
	e_hs_function_device_set_position_track,
	e_hs_function_switch_bsp,
	e_hs_function_switch_bsp_by_name,
	e_hs_function_custom_animation_loop,
	e_hs_function_camera_set_field_of_view,
	e_hs_function_objects_attach,
	e_hs_function_object_hide,
	e_hs_function_game_save_cinematic_skip,
	e_hs_function_cinematic_lighting_set_primary_light,
	e_hs_function_cinematic_lighting_set_secondary_light,
	e_hs_function_cinematic_lighting_set_ambient_light,
	e_hs_function_object_uses_cinematic_lighting,
	e_hs_function_pvs_set_object,
	e_hs_function_pvs_clear,
	e_hs_function_device_set_overlay_track,
	e_hs_function_device_animate_overlay
};

void modify_sent_arguments(void* old_args, byte* new_args, unsigned __int16 op_code);
void send_script_arguments_to_clients(void* args, unsigned __int16 op_code);
void call_networked_hs_function(const s_networked_hs_function* data);
