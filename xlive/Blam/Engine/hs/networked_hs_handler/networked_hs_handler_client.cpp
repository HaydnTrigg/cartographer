#include "stdafx.h"
#include "networked_hs_handler_client.h"
#include "../hs.h"

#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "Blam/Engine/cutscene/cinematics.h"
#include "Blam/Engine/devices/devices.h"
#include "Blam/Engine/Simulation/simulation_world.h"

// Stores all the functions recieved from the host
static std::list<s_networked_hs_function> g_hs_client_function_backlog;

// Table of functions to execute depending on hs function type
static networked_hs_functions_table_t networked_hs_function_table[e_hs_function::_e_hs_function_size];

void initialize_networked_hs_function_table()
{
	for (unsigned short i = 0; i < e_hs_function::_e_hs_function_size; i++)
	{
		networked_hs_function_table[i] = get_networked_hs_lamda_function((e_hs_function)i);
	}
}

void store_hs_functions(const s_networked_hs_function* data)
{
	g_hs_client_function_backlog.push_back(*data);
}

void call_networked_hs_function(const s_networked_hs_function* data)
{
	LOG_INFO_GAME("attempting to execute halo script function: {} executed on the client", hs_function_strings[data->function_type]);

	networked_hs_function_table[data->function_type](data);
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

networked_hs_functions_table_t get_networked_hs_lamda_function(const e_hs_function function_type)
{
	switch (function_type)
	{
	case e_hs_function_fade_out:
	{
		return [](const s_networked_hs_function* data)
		{
			const s_hs_fade_args* args = (const s_hs_fade_args*)data->arg_buffer;
			fade_out(args->color.red, args->color.green, args->color.blue, args->ticks);
		};
	}
	case e_hs_function_fade_in:
	{
		return [](const s_networked_hs_function* data)
		{
			const s_hs_fade_args* args = (const s_hs_fade_args*)data->arg_buffer;
			fade_in(args->color.red, args->color.green, args->color.blue, args->ticks);
		};
	}
	case e_hs_function_print:
	{
		return [](const s_networked_hs_function* data)
		{
			const char* text = (const char*)data->arg_buffer;
			print_to_console(text);
		};
	}
	case e_hs_function_player_enable_input:
	{
		return [](const s_networked_hs_function* data)
		{
			const bool* enable = (const bool*)data->arg_buffer;
			player_enable_input(*enable);
		};
	}
	case e_hs_function_camera_control:
	{
		return [](const s_networked_hs_function* data)
		{
			const bool* control = (const bool*)data->arg_buffer;
			camera_control(*control);
		};
	}
	case e_hs_function_ai_allegiance:
	{
		return [](const s_networked_hs_function* data)
		{
			const WORD* teams = (const WORD*)data->arg_buffer;
			ai_allegiance(*teams, teams[2]);
		};
	}
	case e_hs_function_sound_impulse_start:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_sound_impulse_start_args* args = (s_hs_sound_impulse_start_args*)data->arg_buffer;
			args->object = simulation_gamestate_entity_get_object_index(args->object);

			sound_impulse_start(args->sound, args->object, args->scale);
		};
	}
	case e_hs_function_ai_play_line_on_object:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_ai_play_line_on_object_args* args = (s_hs_ai_play_line_on_object_args*)data->arg_buffer;
			args->object = (args->object == NULL ? NULL : simulation_gamestate_entity_get_object_index(args->object));

			ai_play_line_on_object(args->object, args->sound);
		};
	}
	case e_hs_function_camera_set_animation_relative:
	{
		return [](const s_networked_hs_function* data)
		{
			const s_hs_camera_set_animation_relative_args* args = (const s_hs_camera_set_animation_relative_args*)data->arg_buffer;
			camera_set_animation_relative(args->animation_path, args->animation, args->unit, args->cutscene_flag_index);
		};
	}
	case e_hs_function_cinematic_start:
	{
		return [](const s_networked_hs_function* data)
		{
			cinematic_start();
		};
	}
	case e_hs_function_cinematic_stop:
	{
		return [](const s_networked_hs_function* data)
		{
			cinematic_stop();
		};
	}
	case e_hs_function_custom_animation_relative:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_custom_animation_relative_args* args = (s_hs_custom_animation_relative_args*)data->arg_buffer;
			args->object = simulation_gamestate_entity_get_object_index(args->object);
			args->relative_object = simulation_gamestate_entity_get_object_index(args->relative_object);

			if (!custom_animation_relative(args->object, args->animation_path, args->animation, args->interpolates_into_animation, args->relative_object))
			{
				print_to_console("custom_animation_relative returned false on clients, this is very bad!!!!");
			}
		};
	}
	case e_hs_function_object_cinematic_lod:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_object_cinematic_lod_args* args = (s_hs_object_cinematic_lod_args*)data->arg_buffer;
			args->object = simulation_gamestate_entity_get_object_index(args->object);

			object_cinematic_lod((unsigned __int16)args->object, args->enable);
		};
	}
	case e_hs_function_device_animate_position:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_device_animate_position_args* args = (s_hs_device_animate_position_args*)data->arg_buffer;
			args->device = simulation_gamestate_entity_get_object_index(args->device);

			device_animate_position(args->device, args->target_pos, args->total_time, args->acceleration_time, args->deceleration_time, args->interpolate);
		};
	}
	case e_hs_function_device_set_position_track:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_device_set_position_track_args* args = (s_hs_device_set_position_track_args*)data->arg_buffer;
			args->device = simulation_gamestate_entity_get_object_index(args->device);
			uint32_t string_id = args->animation.get_id();

			if (args->device == DATUM_INDEX_NONE) { return; }

			device_set_position_track(args->device, args->animation, args->interpolation_time);
		};
	}
	case e_hs_function_switch_bsp:
	case e_hs_function_switch_bsp_by_name:
	{
		return [](const s_networked_hs_function* data)
		{
			const __int16* bsp_index = (const __int16*)data->arg_buffer;
			switch_bsp(*bsp_index);
		};
	}
	case e_hs_function_custom_animation_loop:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_custom_animation_args* args = (s_hs_custom_animation_args*)data->arg_buffer;
			args->object = simulation_gamestate_entity_get_object_index(args->object);

			if (!custom_animation_loop(args->object, args->animation_path, args->animation, args->interpolates_into_animation))
			{
				print_to_console("custom_animation_loop returned false on clients, this is very bad!!!!");
			}
		};
	}
	case e_hs_function_camera_set_field_of_view:
	{
		return [](const s_networked_hs_function* data)
		{
			const s_hs_camera_set_field_of_view_args* args = (const s_hs_camera_set_field_of_view_args*)data->arg_buffer;
			camera_set_field_of_view(args->fov, args->ticks);
		};
	}
	case e_hs_function_objects_attach:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_objects_attach_args* args = (s_hs_objects_attach_args*)data->arg_buffer;
			args->parent_object = simulation_gamestate_entity_get_object_index(args->parent_object);
			args->child_object = simulation_gamestate_entity_get_object_index(args->child_object);

			objects_attach(args->parent_object, args->parent_marker, args->child_object, args->child_marker);
		};
	}
	case e_hs_function_object_hide:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_object_hide_args* args = (s_hs_object_hide_args*)data->arg_buffer;
			args->object = simulation_gamestate_entity_get_object_index(args->object);

			object_hide(args->object, args->hidden);
		};
	}
	case e_hs_function_game_save_cinematic_skip:
	{
		return [](const s_networked_hs_function* data)
		{
			game_save_cinematic_skip();
		};
	}
	case e_hs_function_cinematic_lighting_set_primary_light:
	{
		return [](const s_networked_hs_function* data)
		{
			const s_hs_cinematic_lighting_set_light_args* args = (const s_hs_cinematic_lighting_set_light_args*)data->arg_buffer;
			cinematic_lighting_set_primary_light(args->pitch, args->yaw, args->r, args->g, args->b);
		};
	}
	case e_hs_function_cinematic_lighting_set_secondary_light:
	{
		return [](const s_networked_hs_function* data)
		{
			const s_hs_cinematic_lighting_set_light_args* args = (const s_hs_cinematic_lighting_set_light_args*)data->arg_buffer;
			cinematic_lighting_set_secondary_light(args->pitch, args->yaw, args->r, args->g, args->b);
		};
	}
	case e_hs_function_cinematic_lighting_set_ambient_light:
	{
		return [](const s_networked_hs_function* data)
		{
			const s_hs_cinematic_lighting_set_ambient_light_args* args = (const s_hs_cinematic_lighting_set_ambient_light_args*)data->arg_buffer;
			cinematic_lighting_set_ambient_light(args->r, args->g, args->b);
		};
	}
	case e_hs_function_object_uses_cinematic_lighting:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_object_uses_cinematic_lighting_args* args = (s_hs_object_uses_cinematic_lighting_args*)data->arg_buffer;
			args->object = simulation_gamestate_entity_get_object_index(args->object);

			object_uses_cinematic_lighting(args->object, args->enabled);
		};
	}
	case e_hs_function_pvs_set_object:
	{
		return [](const s_networked_hs_function* data)
		{
			datum* object_datum = (datum*)data->arg_buffer;
			*object_datum = simulation_gamestate_entity_get_object_index(*object_datum);

			pvs_set_object(*object_datum);
		};
	}
	case e_hs_function_pvs_clear:
	{
		return [](const s_networked_hs_function* data)
		{
			pvs_clear();
		};
	}
	case e_hs_function_device_set_overlay_track:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_device_set_overlay_track_args* args = (s_hs_device_set_overlay_track_args*)data->arg_buffer;
			args->device = simulation_gamestate_entity_get_object_index(args->device);

			device_set_overlay_track(args->device, args->animation);
		};
	}
	case e_hs_function_device_animate_overlay:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_device_animate_overlay_args* args = (s_hs_device_animate_overlay_args*)data->arg_buffer;
			args->device = simulation_gamestate_entity_get_object_index(args->device);

			device_animate_overlay(args->device, args->position, args->time, args->unk1, args->unk2);
		};
	}
	case e_hs_function_object_dynamic_simulation_disable:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_object_dynamic_simulation_disable_args* args = (s_hs_object_dynamic_simulation_disable_args*)data->arg_buffer;
			args->object = simulation_gamestate_entity_get_object_index(args->object);

			object_dynamic_simulation_disable(args->object, args->disable_dynamic_simulation);
		};
	}
	case e_hs_function_game_save:
	{
		return [](const s_networked_hs_function* data)
		{
			game_save();
		};
	}
	case e_hs_function_game_revert:
	{
		return [](const s_networked_hs_function* data)
		{
			game_revert();
		};
	}
	case e_hs_function_cinematic_skip_start_internal:
	{
		return [](const s_networked_hs_function* data)
		{
			s_cinematic_globals::cinematic_skip_start_internal();
		};
	}
	case e_hs_function_cinematic_skip_stop_internal:
	{
		return [](const s_networked_hs_function* data)
		{
			s_cinematic_globals::cinematic_skip_stop_internal();
		};
	}
	case e_hs_function_device_set_power:
	{
		return [](const s_networked_hs_function* data)
		{
			s_hs_device_set_pos_or_power_args* args = (s_hs_device_set_pos_or_power_args*)data->arg_buffer;
			args->device = simulation_gamestate_entity_get_object_index(args->device);

			device_set_power(args->device, args->power_or_pos);
		};
	}
	default:
	{
		return [](const s_networked_hs_function* data)
		{
			LOG_ERROR_GAME("HS FUNCTION WITH TYPE THAT ISNT HANDLED IS TRYING TO RUN ON A CLIENT");
		};
	}
	}
}
