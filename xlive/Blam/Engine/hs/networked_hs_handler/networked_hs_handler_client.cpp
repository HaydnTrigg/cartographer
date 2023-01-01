#include "stdafx.h"
#include "networked_hs_handler_client.h"
#include "../hs.h"

#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "Blam/Engine/cutscene/cinematics.h"
#include "Blam/Engine/devices/devices.h"
#include "Blam/Engine/Simulation/simulation_world.h"

std::list<s_networked_hs_function> g_hs_client_function_backlog;	// Stores all the functions recieved from the host

void store_hs_functions(const s_networked_hs_function* data)
{
	g_hs_client_function_backlog.push_back(*data);
}

void call_networked_hs_function(const s_networked_hs_function* data)
{
	LOG_INFO_GAME("halo script function: {} executed on the client", hs_function_strings[data->function_type]);

	switch (data->function_type)
	{
	case e_hs_function_fade_out:
	{
		const s_hs_fade_args* args = (const s_hs_fade_args*)data->arg_buffer;
		fade_out(args->color.red, args->color.green, args->color.blue, args->ticks);
		break;
	}
	case e_hs_function_fade_in:
	{
		const s_hs_fade_args* args = (const s_hs_fade_args*)data->arg_buffer;
		fade_in(args->color.red, args->color.green, args->color.blue, args->ticks);
		break;
	}
	case e_hs_function_print:
	{
		const char* text = (const char*)data->arg_buffer;
		print_to_console(text);
		break;
	}
	case e_hs_function_player_enable_input:
	{
		const bool* enable = (const bool*)data->arg_buffer;
		player_enable_input(*enable);
		break;
	}
	case e_hs_function_camera_control:
	{
		const bool* control = (const bool*)data->arg_buffer;
		camera_control(*control);
		break;
	}
	case e_hs_function_ai_allegiance:
	{
		const WORD* teams = (const WORD*)data->arg_buffer;
		ai_allegiance(*teams, teams[2]);
		break;
	}
	case e_hs_function_sound_impulse_start:
	{
		s_hs_sound_impulse_start_args* args = (s_hs_sound_impulse_start_args*)data->arg_buffer;
		args->object = simulation_gamestate_entity_get_object_index(args->object);

		sound_impulse_start(args->sound, args->object, args->scale);
		break;
	}
	case e_hs_function_ai_play_line_on_object:
	{
		s_hs_ai_play_line_on_object_args* args = (s_hs_ai_play_line_on_object_args*)data->arg_buffer;
		args->object = (args->object == NULL ? NULL : simulation_gamestate_entity_get_object_index(args->object));

		ai_play_line_on_object(args->object, args->sound);
		break;
	}
	case e_hs_function_camera_set_animation_relative:
	{
		const s_hs_camera_set_animation_relative_args* args = (const s_hs_camera_set_animation_relative_args*)data->arg_buffer;
		camera_set_animation_relative(args->animation_path, args->animation, args->unit, args->cutscene_flag_index);
		break;
	}
	case e_hs_function_cinematic_start:
	{
		cinematic_start();
		break;
	}
	case e_hs_function_cinematic_stop:
	{
		cinematic_stop();
		break;
	}
	case e_hs_function_custom_animation_relative:
	{
		s_hs_custom_animation_relative_args* args = (s_hs_custom_animation_relative_args*)data->arg_buffer;
		args->object = simulation_gamestate_entity_get_object_index(args->object);
		args->relative_object = simulation_gamestate_entity_get_object_index(args->relative_object);

		if (!custom_animation_relative(args->object, args->animation_path, args->animation, args->interpolates_into_animation, args->relative_object))
		{
			print_to_console("custom_animation_relative returned false on clients, this is very bad!!!!");
		}
		break;
	}
	case e_hs_function_object_cinematic_lod:
	{
		s_hs_object_cinematic_lod_args* args = (s_hs_object_cinematic_lod_args*)data->arg_buffer;
		args->object = simulation_gamestate_entity_get_object_index(args->object);

		object_cinematic_lod((unsigned __int16)args->object, args->enable);
		break;
	}
	case e_hs_function_device_animate_position:
	{
		s_hs_device_animate_position_args* args = (s_hs_device_animate_position_args*)data->arg_buffer;
		args->device = simulation_gamestate_entity_get_object_index(args->device);

		device_animate_position(args->device, args->position, args->time, args->unk1, args->unk2, args->interpolate);
		break;
	}
	case e_hs_function_device_set_position_track:
	{
		s_hs_device_set_position_track_args* args = (s_hs_device_set_position_track_args*)data->arg_buffer;
		args->device = simulation_gamestate_entity_get_object_index(args->device);
		uint32_t string_id = args->animation.get_id();

		device_set_position_track(args->device, args->animation, args->interpolation_time);
		break;
	}
	case e_hs_function_switch_bsp:
	case e_hs_function_switch_bsp_by_name:
	{
		const __int16* bsp_index = (const __int16*)data->arg_buffer;
		switch_bsp(*bsp_index);
		break;
	}
	case e_hs_function_custom_animation_loop:
	{
		s_hs_custom_animation_args* args = (s_hs_custom_animation_args*)data->arg_buffer;
		args->object = simulation_gamestate_entity_get_object_index(args->object);

		if (!custom_animation_loop(args->object, args->animation_path, args->animation, args->interpolates_into_animation))
		{
			print_to_console("custom_animation_loop returned false on clients, this is very bad!!!!");
		}
		break;
	}
	case e_hs_function_camera_set_field_of_view:
	{
		const s_hs_camera_set_field_of_view_args* args = (const s_hs_camera_set_field_of_view_args*)data->arg_buffer;
		camera_set_field_of_view(args->fov, args->ticks);
		break;
	}
	case e_hs_function_objects_attach:
	{
		s_hs_objects_attach_args* args = (s_hs_objects_attach_args*)data->arg_buffer;
		args->parent_object = simulation_gamestate_entity_get_object_index(args->parent_object);
		args->child_object = simulation_gamestate_entity_get_object_index(args->child_object);

		objects_attach(args->parent_object, args->parent_marker, args->child_object, args->child_marker);
		break;
	}
	case e_hs_function_object_hide:
	{
		s_hs_object_hide_args* args = (s_hs_object_hide_args*)data->arg_buffer;
		args->object = simulation_gamestate_entity_get_object_index(args->object);

		object_hide(args->object, args->hidden);
		break;
	}
	case e_hs_function_game_save_cinematic_skip:
	{
		game_save_cinematic_skip();
		break;
	}
	case e_hs_function_cinematic_lighting_set_primary_light:
	{
		const s_hs_cinematic_lighting_set_light_args* args = (const s_hs_cinematic_lighting_set_light_args*)data->arg_buffer;
		cinematic_lighting_set_primary_light(args->pitch, args->yaw, args->r, args->g, args->b);
		break;
	}
	case e_hs_function_cinematic_lighting_set_secondary_light:
	{
		const s_hs_cinematic_lighting_set_light_args* args = (const s_hs_cinematic_lighting_set_light_args*)data->arg_buffer;
		cinematic_lighting_set_secondary_light(args->pitch, args->yaw, args->r, args->g, args->b);
		break;
	}
	case e_hs_function_cinematic_lighting_set_ambient_light:
	{
		const s_hs_cinematic_lighting_set_ambient_light_args* args = (const s_hs_cinematic_lighting_set_ambient_light_args*)data->arg_buffer;
		cinematic_lighting_set_ambient_light(args->r, args->g, args->b);
		break;
	}
	case e_hs_function_object_uses_cinematic_lighting:
	{
		s_hs_object_uses_cinematic_lighting_args* args = (s_hs_object_uses_cinematic_lighting_args*)data->arg_buffer;
		args->object = simulation_gamestate_entity_get_object_index(args->object);

		object_uses_cinematic_lighting(args->object, args->enabled);
		break;
	}
	case e_hs_function_pvs_set_object:
	{
		datum* object_datum = (datum*)data->arg_buffer;
		*object_datum = simulation_gamestate_entity_get_object_index(*object_datum);

		pvs_set_object(*object_datum);
		break;
	}
	case e_hs_function_pvs_clear:
	{
		pvs_clear();
		break;
	}
	case e_hs_function_device_set_overlay_track:
	{
		s_hs_device_set_overlay_track_args* args = (s_hs_device_set_overlay_track_args*)data->arg_buffer;
		args->device = simulation_gamestate_entity_get_object_index(args->device);

		device_set_overlay_track(args->device, args->animation);
		break;
	}
	case e_hs_function_device_animate_overlay:
	{
		s_hs_device_animate_overlay_args* args = (s_hs_device_animate_overlay_args*)data->arg_buffer;
		args->device = simulation_gamestate_entity_get_object_index(args->device);

		device_animate_overlay(args->device, args->position, args->time, args->unk1, args->unk2);
		break;
	}
	case e_hs_function_object_dynamic_simulation_disable:
	{
		s_hs_object_dynamic_simulation_disable_args* args = (s_hs_object_dynamic_simulation_disable_args*)data->arg_buffer;
		args->object = simulation_gamestate_entity_get_object_index(args->object);

		object_dynamic_simulation_disable(args->object, args->disable_dynamic_simulation);
		break;
	}
	case e_hs_function_game_save:
	{
		game_save();
		break;
	}
	case e_hs_function_game_revert:
	{
		game_revert();
		break;
	}
	case e_hs_function_cinematic_skip_start_internal:
	{
		s_cinematic_globals::cinematic_skip_start_internal();
		break;
	}
	case e_hs_function_cinematic_skip_stop_internal:
	{
		s_cinematic_globals::cinematic_skip_stop_internal();
		break;
	}
	case e_hs_function_device_set_power:
	{
		s_hs_device_set_pos_or_power_args* args = (s_hs_device_set_pos_or_power_args*)data->arg_buffer;
		args->device = simulation_gamestate_entity_get_object_index(args->device);

		device_set_power(args->device, args->power_or_pos);
		break;
	}
	default:
	{
		LOG_INFO_GAME("HS FUNCTION WITH TYPE THAT DOSENT EXIST SENT");
		break;
	}
	}
}

void client_execute_stored_hs_commands()
{
	for (auto it = g_hs_client_function_backlog.begin(); it != g_hs_client_function_backlog.end(); it++)
	{
		if (it->script_id == g_next_hs_function_id)
		{
			g_next_hs_function_id++;

			call_networked_hs_function(&*it);
			g_hs_client_function_backlog.erase(it);
			return;
		}
	}
}