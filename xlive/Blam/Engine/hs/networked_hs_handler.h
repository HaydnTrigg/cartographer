#pragma once
#include "hs_function.h"

#define HS_SENT_BUFFER_SIZE 128
#define HS_SYNC_TABLE_SIZE 34

extern long g_next_hs_function_id;

struct s_networked_hs_function
{
	e_hs_function function_type;
	unsigned long script_id;
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
	//e_hs_function_object_cinematic_lod, (crashes, something something objects still need to be fixed)
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
	e_hs_function_device_animate_overlay,
	e_hs_function_object_dynamic_simulation_disable,
	e_hs_function_game_save,
	e_hs_function_game_revert,
	//e_hs_function_cinematic_skip_start_internal, (clients have control over the cutscene skip, dosent apply for host)
	//e_hs_function_cinematic_skip_stop_internal, (clients have control over the cutscene skip, dosent apply for host)
	e_hs_function_device_set_power,
	//e_hs_function_device_set_position (this is already synced by the host)
};

// Gets ready to send a script function to all the clients in a match
// Modifies the arguments sent depending on the hs function called
void send_hs_function_to_clients(s_networked_hs_function* function);

// Modifies the arguments sent to the client depending on the script operation being executed
void modify_sent_arguments(s_networked_hs_function* function);

// Sends a packet to a client with the hs function data
void send_hs_function_packet(int peerIdx, s_networked_hs_function* data);

// Returns a s_networked_hs_function struct with arguments (or lack of) and the function type
s_networked_hs_function populate_networked_hs_function(const void* args_src, const e_hs_function function_type);

// Stores recieved hs functions in a list to be executed later on hs_update
void store_hs_functions(const s_networked_hs_function* data);

/* Calls the hs function based on the s_networked_hs_function provided
Only executes something if we manually add the function to the hs_sync_table and add logic to handle it here*/
void call_networked_hs_function(const s_networked_hs_function* data);

// Loops through the stored functions and if one matches the next script id to be executred call it
void client_execute_stored_hs_commands();
