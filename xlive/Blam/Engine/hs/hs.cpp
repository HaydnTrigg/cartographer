#include "stdafx.h"
#include "hs.h"

#include "Blam/Engine/Objects/Objects.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "Blam/Engine/Players/Players.h"
#include "Blam/Engine/Simulation/simulation_world.h"
#include "H2MOD.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "Util/Hooks/Hook.h"

namespace hs
{
	typedef void* (__cdecl* hs_arguments_evaluate_t)(__int16 op_code, unsigned __int16 thread_id, bool unk_bool);
	hs_arguments_evaluate_t p_hs_arguments_evaluate;
	typedef char* (__cdecl hs_return_t)(int a1, int a2);
	hs_return_t* p_hs_return;

	typedef int(__cdecl unit_kill_t)(datum unitDatum);
	unit_kill_t* p_unit_kill;
	void UnitKill(datum unitDatum)
	{
		p_unit_kill(unitDatum);
	}

	typedef bool(__cdecl unit_in_vehicle_t)(datum unitDatum);
	unit_in_vehicle_t* p_unit_in_vehicle;
	bool UnitInVehicle(datum unitDatum)
	{
		return p_unit_in_vehicle(unitDatum);
	}

	typedef float(__cdecl unit_get_health_t)(datum unitDatum);
	unit_get_health_t* p_unit_get_health;
	float UnitGetHealth(datum unitDatum)
	{
		return p_unit_get_health(unitDatum);
	}

	typedef float(__cdecl unit_get_shield_t)(datum unitDatum);
	unit_get_shield_t* p_unit_get_shield;
	float UnitGetShield(datum unitDatum)
	{
		return p_unit_get_shield(unitDatum);
	}

	typedef void(__cdecl physics_set_gravity_t)(float multiplier);
	physics_set_gravity_t* p_physics_set_gravity;
	void PhysicsSetGravity(float Multiplier)
	{
		p_physics_set_gravity(Multiplier);
	}

	typedef void(__cdecl physics_set_velocity_frame_t)(float unk1, float unk2, float unk3);
	physics_set_velocity_frame_t* p_physics_set_velocity_frame;
	void PhysicsSetVelocityFrame(float unk1, float unk2, float unk3)
	{
		p_physics_set_velocity_frame(unk1, unk2, unk3);
	}

	typedef void(__cdecl render_lights_enable_cinematic_shadow_t)(bool unk1, datum objectDatum, string_id StringId, float unk2);
	render_lights_enable_cinematic_shadow_t* p_render_lights_enable_cinematic_shadow;
	void RenderLightsEnableCinenaticShadow(bool unk1, datum objectDatum, string_id StringId, float unk2)
	{
		p_render_lights_enable_cinematic_shadow(unk1, objectDatum, StringId, unk2);
	}

	typedef void(__cdecl object_destroy_t)(datum object_datum_index);
	object_destroy_t* p_object_destroy;
	void ObjectDestroy(datum object_datum_index)
	{
		p_object_destroy(object_datum_index);
	}

	typedef void(__cdecl* fade_out_t)(float r, float g, float b, __int16 ticks);
	fade_out_t p_fade_out;
	void fade_out(float r, float g, float b, __int16 ticks)
	{
		p_fade_out(r, g, b, ticks);
	}

	typedef void(__cdecl* fade_in_t)(float r, float g, float b, __int16 ticks);
	fade_in_t p_fade_in;
	void fade_in(float r, float g, float b, __int16 ticks)
	{
		p_fade_in(r, g, b, ticks);
	}

	typedef void(__cdecl* player_enable_input_t)(bool will_enable);
	player_enable_input_t p_player_enable_input;
	void player_enable_input(bool will_enable)
	{
		p_player_enable_input(will_enable);
	}

	typedef void(__cdecl* camera_control_t)(bool control);
	camera_control_t p_camera_control;
	void camera_control(bool control)
	{
		p_camera_control(control);
	}

	typedef void(__cdecl* ai_allegiance_t)(__int16 team1, __int16 team2);
	ai_allegiance_t p_ai_allegiance;
	void __cdecl ai_allegiance(__int16 team1, __int16 team2)
	{
		p_ai_allegiance(team1, team2);
	}

	typedef void(__cdecl* sound_impulse_start_t)(char* sound, int object_datum, float scale);
	sound_impulse_start_t p_sound_impulse_start;
	void __cdecl sound_impulse_start(char* sound, int object_datum, float scale)
	{
		p_sound_impulse_start(sound, object_datum, scale);
	}

	typedef float (__cdecl* ai_play_line_on_object_t)(int object_datum, string_id sound);
	ai_play_line_on_object_t p_ai_play_line_on_object;
	float __cdecl ai_play_line_on_object(int object_datum, string_id sound)
	{
		return p_ai_play_line_on_object(object_datum, sound);
	}

	typedef void(__cdecl* camera_set_animation_relative_t)(const char* animation_path, const char* animation, int unit, const WORD cutscene_flag_index);
	camera_set_animation_relative_t p_camera_set_animation_relative;
	void __cdecl camera_set_animation_relative(const char* animation_path, const char* animation, int unit, const WORD cutscene_flag_index)
	{
		p_camera_set_animation_relative(animation_path, animation, unit, cutscene_flag_index);
	}

	typedef void(__cdecl* cinematic_start_t)();
	cinematic_start_t p_cinematic_start;
	void cinematic_start()
	{
		p_cinematic_start();
	}

	void* __cdecl cinematic_start_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool)
	{
		for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
		{
			if (NetworkSession::PlayerIsActive(i))
			{
				NetworkMessage::SendHSFunction(NetworkSession::GetPeerIndex(i), e_hs_function_cinematic_start, sizeof(s_networked_hs_function::arg_buffer), nullptr);
			}
		}
		cinematic_start();

		return p_hs_return(thread_id, 0);
	}

	typedef void(__cdecl* cinematic_stop_t)();
	cinematic_stop_t p_cinematic_stop;
	void cinematic_stop()
	{
		p_cinematic_stop();
	}

	void* __cdecl cinematic_stop_evaluate(__int16 op_code, int thread_id, char unk_bool)
	{
		for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
		{
			if (NetworkSession::PlayerIsActive(i))
			{
				NetworkMessage::SendHSFunction(NetworkSession::GetPeerIndex(i), e_hs_function_cinematic_stop, sizeof(s_networked_hs_function::arg_buffer), nullptr);
			}
		}
		cinematic_stop();

		return p_hs_return(thread_id, 0);
	}

	typedef bool(__cdecl* custom_animation_relative_t)(const datum object_datum, const datum animation_path, const string_id animation, const bool interpolates_into_animation, const datum relative_object);
	custom_animation_relative_t p_custom_animation_relative;
	bool custom_animation_relative(const datum object_datum, const datum animation_path, const string_id animation, const bool interpolates_into_animation, const datum relative_object)
	{
		return p_custom_animation_relative(object_datum, animation_path, animation, interpolates_into_animation, relative_object);
	}

	typedef void(__cdecl* object_cinematic_lod_t)(const WORD object_name_index, bool enable);
	object_cinematic_lod_t p_object_cinematic_lod;
	void object_cinematic_lod(const WORD object_name_index, bool enable)
	{
		p_object_cinematic_lod(object_name_index, enable);
	}

	typedef void(__cdecl* device_animate_position_t)(const datum device_datum, const float position, const float time, const float unk1, const float unk2, const bool interpolate);
	device_animate_position_t p_device_animate_position;
	void device_animate_position(const datum device_datum, const float position, const float time, const float unk1, const float unk2, const bool interpolate)
	{
		p_device_animate_position(device_datum, position, time, unk1, unk2, interpolate);
	}

	typedef bool(__cdecl* device_set_position_track_t)(const datum device, const string_id animation, const float interpolation_time);
	device_set_position_track_t p_device_set_position_track;
	bool device_set_position_track(const datum device, const string_id animation, const float interpolation_time)
	{
		return p_device_set_position_track(device, animation, interpolation_time);
	}

	typedef void(__cdecl* switch_bsp_t)(const __int16 bsp_index);
	switch_bsp_t p_switch_bsp;
	void switch_bsp(const __int16 bsp_index)
	{
		p_switch_bsp(bsp_index);
	}

	typedef bool(__cdecl* custom_animation_loop_t)(const datum object_datum, const datum animation_path, const string_id animation, const bool interpolates_into_animation);
	custom_animation_loop_t p_custom_animation_loop;
	bool custom_animation_loop(const datum object_datum, const datum animation_path, const string_id animation, const bool interpolates_into_animation)
	{
		return p_custom_animation_loop(object_datum, animation_path, animation, interpolates_into_animation);
	}

	typedef void(__cdecl* camera_set_field_of_view_t)(const float fov, const short ticks);
	camera_set_field_of_view_t p_camera_set_field_of_view;
	void camera_set_field_of_view(const float fov, const short ticks)
	{
		p_camera_set_field_of_view(fov, ticks);
	}

	typedef void(__cdecl* objects_attach_t)(const datum parent_object, const string_id parent_marker, const datum child_object, const string_id child_marker);
	objects_attach_t p_objects_attach;
	void objects_attach(const datum parent_object, const string_id parent_marker, const datum child_object, const string_id child_marker)
	{
		p_objects_attach(parent_object, parent_marker, child_object, child_marker);
	}

	typedef void(__cdecl* object_hide_t)(const datum object, const bool hidden);
	object_hide_t p_object_hide;
	void object_hide(const datum object, const bool hidden)
	{
		p_object_hide(object, hidden);
	}

	typedef void(__cdecl* game_save_cinematic_skip_t)();
	game_save_cinematic_skip_t p_game_save_cinematic_skip;
	void game_save_cinematic_skip()
	{
		p_game_save_cinematic_skip();
	}

	void* __cdecl game_save_cinematic_skip_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool)
	{
		for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
		{
			if (NetworkSession::PlayerIsActive(i))
			{
				NetworkMessage::SendHSFunction(NetworkSession::GetPeerIndex(i), e_hs_function_game_save_cinematic_skip, sizeof(s_networked_hs_function::arg_buffer), nullptr);
			}
		}
		game_save_cinematic_skip();

		return p_hs_return(thread_id, 0);
	}

	typedef void(__cdecl* cinematic_lighting_set_primary_light_t)(const float pitch, const float yaw, const float r, const float g, const float b);
	cinematic_lighting_set_primary_light_t p_cinematic_lighting_set_primary_light;
	void cinematic_lighting_set_primary_light(const float pitch, const float yaw, const float r, const float g, const float b)
	{
		p_cinematic_lighting_set_primary_light(pitch, yaw, r, g, b);
	}

	typedef void(__cdecl* cinematic_lighting_set_secondary_light_t)(const float pitch, const float yaw, const float r, const float g, const float b);
	cinematic_lighting_set_secondary_light_t p_cinematic_lighting_set_secondary_light;
	void cinematic_lighting_set_secondary_light(const float pitch, const float yaw, const float r, const float g, const float b)
	{
		p_cinematic_lighting_set_secondary_light(pitch, yaw, r, g, b);
	}

	typedef void(__cdecl* cinematic_lighting_set_ambient_light_t)(const float r, const float g, const float b);
	cinematic_lighting_set_ambient_light_t p_cinematic_lighting_set_ambient_light;
	void cinematic_lighting_set_ambient_light(const float r, const float g, const float b)
	{
		p_cinematic_lighting_set_ambient_light(r, g, b);
	}

	typedef void(__cdecl* object_uses_cinematic_lighting_t)(const datum object, const bool enabled);
	object_uses_cinematic_lighting_t p_object_uses_cinematic_lighting;
	void object_uses_cinematic_lighting(const datum object, const bool enabled)
	{
		p_object_uses_cinematic_lighting(object, enabled);
	}

	void pvs_set_object(const datum object)
	{
		if (object == DATUM_INDEX_NONE)
		{
			s_game_globals::get()->pvs_object_is_set = 0;
		}
		else
		{
			s_game_globals::get()->pvs_object_is_set = 1;
			s_game_globals::get()->pvs_object_datum = object;
		}
	}

	void pvs_clear()
	{
		s_game_globals::get()->pvs_object_is_set = 0;
	}

	void* __cdecl pvs_clear_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool)
	{
		for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
		{
			if (NetworkSession::PlayerIsActive(i))
			{
				NetworkMessage::SendHSFunction(NetworkSession::GetPeerIndex(i), e_hs_function_pvs_clear, sizeof(s_networked_hs_function::arg_buffer), nullptr);
			}
		}
		pvs_clear();

		return p_hs_return(thread_id, 0);
	}

	typedef bool(__cdecl* device_set_overlay_track_t)(const datum device, const string_id animation);
	device_set_overlay_track_t p_device_set_overlay_track;
	bool device_set_overlay_track(const datum device, const string_id animation)
	{
		return p_device_set_overlay_track(device, animation);
	}

	typedef void(__cdecl* device_animate_overlay_t)(const datum device_datum, const float position, const float time, const float unk1, const float unk2);
	device_animate_overlay_t p_device_animate_overlay;
	void device_animate_overlay(const datum device_datum, const float position, const float time, const float unk1, const float unk2)
	{
		p_device_animate_overlay(device_datum, position, time, unk1, unk2);
	}

	void __cdecl print_to_console(const char* output)
	{
		std::string finalOutput("[HSC Print] "); finalOutput += output;
		addDebugText(finalOutput.c_str());
	}

	// Modifies the arguments sent to the client depending on the script operation being executed
	void modify_sent_arguments(void* old_args, byte* new_args, unsigned __int16 op_code)
	{
		memcpy(new_args, old_args, HS_SENT_BUFFER_SIZE);

		switch (op_code)
		{
		// Send the entity index instead of the object index since object index isnt the same on the client
		case e_hs_function_device_set_position_track:
		{
			s_hs_device_set_position_track_args* args = (s_hs_device_set_position_track_args*)new_args;
			const s_object_data_definition* device_object = object_get_fast_unsafe(args->device);
			
			if (args->device == DATUM_INDEX_NONE) { break; }

			args->device = device_object->simulation_entity_index;
			break;
		}
		// Send the entity index instead of the object index since object index isnt the same on the client
		case e_hs_function_device_animate_position:
		{
			s_hs_device_animate_position_args* args = (s_hs_device_animate_position_args*)new_args;
			const s_object_data_definition* device_object = object_get_fast_unsafe(args->device);

			if (args->device == DATUM_INDEX_NONE) { break; }

			args->device = device_object->simulation_entity_index;
			break;
		}
		// Send the entity index instead of the object index since object index isnt the same on the client
		case e_hs_function_object_cinematic_lod:
		{
			s_hs_object_cinematic_lod_args* args = (s_hs_object_cinematic_lod_args*)new_args;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { break; }

			args->object = object->simulation_entity_index;
			break;
		}
		// Send the entity index instead of the object index since object index isnt the same on the client
		case e_hs_function_custom_animation_relative:
		{
			s_hs_custom_animation_relative_args* args = (s_hs_custom_animation_relative_args*)new_args;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);
			const s_object_data_definition* relative_object = object_get_fast_unsafe(args->relative_object);

			if (args->object == DATUM_INDEX_NONE) { break; }
			args->object = object->simulation_entity_index;

			if (args->relative_object == DATUM_INDEX_NONE) { break; }
			args->relative_object = relative_object->simulation_entity_index;
			break;
		}
		// Send the entity index instead of the object index since object index isnt the same on the client
		case e_hs_function_ai_play_line_on_object:
		{
			s_hs_ai_play_line_on_object_args* args = (s_hs_ai_play_line_on_object_args*)new_args;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { break; }

			args->object = object->simulation_entity_index;
			break;
		}
		case e_hs_function_custom_animation_loop:
		{
			s_hs_custom_animation_args* args = (s_hs_custom_animation_args*)new_args;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { break; }

			args->object = object->simulation_entity_index;
			break;
		}
		case e_hs_function_objects_attach:
		{
			s_hs_objects_attach_args* args = (s_hs_objects_attach_args*)new_args;
			const s_object_data_definition* parent_object = object_get_fast_unsafe(args->parent_object);
			const s_object_data_definition* child_object = object_get_fast_unsafe(args->child_object);

			if (args->parent_object == DATUM_INDEX_NONE || args->child_object == DATUM_INDEX_NONE) { break; }

			args->parent_object = parent_object->simulation_entity_index;
			args->child_object = child_object->simulation_entity_index;
			break;
		}
		case e_hs_function_object_hide:
		{			
			s_hs_object_hide_args* args = (s_hs_object_hide_args*)new_args;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { break; }

			args->object = object->simulation_entity_index;
			break;
		}
		case e_hs_function_object_uses_cinematic_lighting:
		{
			s_hs_object_uses_cinematic_lighting_args* args = (s_hs_object_uses_cinematic_lighting_args*)new_args;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { break; }

			args->object =  object->simulation_entity_index;
			break;
		}
		case e_hs_function_pvs_set_object:
		{
			datum* object_datum = (datum*)new_args;
			const s_object_data_definition* object = object_get_fast_unsafe(*object_datum);

			if (*object_datum == DATUM_INDEX_NONE) { break; }

			*object_datum = object->simulation_entity_index;
			break;
		}
		case e_hs_function_device_set_overlay_track:
		{
			s_hs_device_set_overlay_track_args* args = (s_hs_device_set_overlay_track_args*)new_args;
			const s_object_data_definition* device_object = object_get_fast_unsafe(args->device);

			if (args->device == DATUM_INDEX_NONE) { break; }

			args->device = device_object->simulation_entity_index;
			break;
		}
		// Send the entity index instead of the object index since object index isnt the same on the client
		case e_hs_function_device_animate_overlay:
		{
			s_hs_device_animate_overlay_args* args = (s_hs_device_animate_overlay_args*)new_args;
			const s_object_data_definition* device_object = object_get_fast_unsafe(args->device);

			if (args->device == DATUM_INDEX_NONE) { break; }

			args->device = device_object->simulation_entity_index;
			break;
		}
		}
	}

	void send_script_arguments_to_clients(void* args, unsigned __int16 op_code)
	{
		bool found = false;
		byte new_args_buffer[HS_SENT_BUFFER_SIZE];
		void* new_args = &new_args_buffer;

		//If host send out the packets
		if (h2mod->GetEngineType() == _single_player && NetworkSession::LocalPeerIsSessionHost())
		{
			for (unsigned short i = 0; i < HS_SYNC_TABLE_SIZE; i++)
			{
				if (hs_sync_table[i] == op_code)
				{
					found = true;
				}
			}

			if (!found || !args) { return; }

			modify_sent_arguments(args, new_args_buffer, op_code);

			for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
			{
				if (NetworkSession::PlayerIsActive(i))
				{
					NetworkMessage::SendHSFunction(NetworkSession::GetPeerIndex(i), (e_hs_function)op_code, sizeof(s_networked_hs_function::arg_buffer), new_args);
				}
			}
			return;
		}
	}

	void* __cdecl hs_arguments_evaluate(unsigned __int16 op_code, unsigned __int16 thread_id, bool unk_bool)
	{
		typedef void* (__cdecl* hs_evaluate_t)(unsigned __int16 thread_id, __int16 arg_count, int arg_array, bool unk_bool);
		hs_evaluate_t p_hs_evaluate = Memory::GetAddress<hs_evaluate_t>(0x956FA, 0xAA8FA);
		HaloScriptCommand** hs_function_table = Memory::GetAddress<HaloScriptCommand**>(0x41C5B0, 0x3C0028);

		void* args = p_hs_evaluate(thread_id, hs_function_table[op_code]->arg_count, (int)hs_function_table[op_code]->arg_array, unk_bool);

		// Added for script syncing in coop
		if (NetworkSession::GetLocalSessionState() != _network_session_state_none)
		{
			send_script_arguments_to_clients(args, op_code);
		}

		return args;
	}

	void CallNetworkedHSFunction(const s_networked_hs_function* data)
	{
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
		default:
		{
			print_to_console("HS FUNCTION WITH TYPE THAT DOSENT EXIST SENT");
			break;
		}
		}
	}
	void ApplyHooks()
	{
		// Hook arguments evaluate function so we can add our functionality for syncing the scripts
		DETOUR_BEGIN();
		DETOUR_ATTACH(p_hs_arguments_evaluate, Memory::GetAddress<hs_arguments_evaluate_t>(0x9581D, 0xAAA1D), hs_arguments_evaluate);
		DETOUR_COMMIT();
		
		// Write pointers for script functions that have no arguments
		WritePointer(Memory::GetAddress(0x3C0CD4, 0x37D5EC), cinematic_start_evaluate);
		WritePointer(Memory::GetAddress(0x3C0CE4, 0x37D5FC), cinematic_stop_evaluate);
		WritePointer(Memory::GetAddress(0x3C25AC, 0x37EEC4), game_save_cinematic_skip_evaluate);
		WritePointer(Memory::GetAddress(0x3C071C, 0x37D034), pvs_clear_evaluate);
		
		// hook the print command to redirect the output to our console
		if (!Memory::IsDedicatedServer())
		{
			PatchCall(Memory::GetAddress(0xE9E50), print_to_console);
		}
	}

	void Initialize()
	{
		ApplyHooks();

		p_hs_arguments_evaluate = Memory::GetAddress<hs_arguments_evaluate_t>(0x9581D, 0xAAA1D);
		p_hs_return = Memory::GetAddress<hs_return_t*>(0x9505D, 0xAA25D);

		p_unit_kill = Memory::GetAddress<unit_kill_t*>(0x13B514, 0x12A363);
		p_unit_in_vehicle = Memory::GetAddress<unit_in_vehicle_t*>(0x1846D9, 0x16E775);
		p_unit_get_health = Memory::GetAddress<unit_get_health_t*>(0x184477, 0x165E13);
		p_unit_get_shield = Memory::GetAddress<unit_get_shield_t*>(0x18447C, 0x16E518);
		p_physics_set_gravity = Memory::GetAddress<physics_set_gravity_t*>(0xB3C00, 0xA3E13);
		p_physics_set_velocity_frame = Memory::GetAddress<physics_set_velocity_frame_t*>(0xB3D5B, 0xA3F6E);
		p_render_lights_enable_cinematic_shadow = Memory::GetAddress<render_lights_enable_cinematic_shadow_t*>(0x19245A);
		p_object_destroy = Memory::GetAddress<object_destroy_t*>(0xFDCFD, 0x124ED5);
		p_fade_out = Memory::GetAddress<fade_out_t>(0xA3CCA, 0x95F2A);
		p_fade_in = Memory::GetAddress<fade_in_t>(0xA402C, 0x9628C);

		if (!Memory::IsDedicatedServer())
		{
			p_player_enable_input = Memory::GetAddress<player_enable_input_t>(0x51464);
			p_camera_control = Memory::GetAddress<camera_control_t>(0x5A181);
			p_ai_allegiance = Memory::GetAddress<ai_allegiance_t>(0x30F39A);
			p_sound_impulse_start = Memory::GetAddress<sound_impulse_start_t>(0x8A825);
			p_ai_play_line_on_object = Memory::GetAddress<ai_play_line_on_object_t>(0x30F886);
			p_camera_set_animation_relative = Memory::GetAddress<camera_set_animation_relative_t>(0x978BC);
			p_cinematic_start = Memory::GetAddress<cinematic_start_t>(0x3A6D0);
			p_cinematic_stop = Memory::GetAddress<cinematic_stop_t>(0x3A8C9);
			p_custom_animation_relative = Memory::GetAddress<custom_animation_relative_t>(0x18565F);
			p_object_cinematic_lod = Memory::GetAddress<object_cinematic_lod_t>(0x133BE1);
			p_device_animate_position = Memory::GetAddress<device_animate_position_t>(0x163911);
			p_device_set_position_track = Memory::GetAddress<device_set_position_track_t>(0x164257);
			p_switch_bsp = Memory::GetAddress<switch_bsp_t>(0x39563);
			p_custom_animation_loop = Memory::GetAddress<custom_animation_loop_t>(0x18563E);
			p_camera_set_field_of_view = Memory::GetAddress<camera_set_field_of_view_t>(0x97F38);
			p_objects_attach = Memory::GetAddress<objects_attach_t>(0x14B7DC);
			p_object_hide = Memory::GetAddress<object_hide_t>(0xFDD52);
			p_game_save_cinematic_skip = Memory::GetAddress<game_save_cinematic_skip_t>(0x9E46B);
			p_cinematic_lighting_set_primary_light = Memory::GetAddress<cinematic_lighting_set_primary_light_t>(0x130900);
			p_cinematic_lighting_set_secondary_light = Memory::GetAddress<cinematic_lighting_set_secondary_light_t>(0x1309BE);
			p_cinematic_lighting_set_ambient_light = Memory::GetAddress<cinematic_lighting_set_ambient_light_t>(0x130A52);
			p_object_uses_cinematic_lighting = Memory::GetAddress<object_uses_cinematic_lighting_t>(0x133C17);
			p_device_set_overlay_track = Memory::GetAddress<device_set_overlay_track_t>(0x1648E7);
			p_device_animate_overlay = Memory::GetAddress<device_animate_overlay_t>(0x1639A5);
		}
	}
}
