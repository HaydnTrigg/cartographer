#include "stdafx.h"
#include "hs.h"

#include "Blam/Engine/Players/Players.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "H2MOD.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "Util/Hooks/Hook.h"

namespace hs
{
	typedef void* (__cdecl* hs_arguments_evaluate_t)(__int16 op_code, unsigned __int16 thread_id, bool unk_bool);
	hs_arguments_evaluate_t p_hs_arguments_evaluate;

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

	typedef int(__cdecl* ai_play_line_on_object_t)(int object_datum, char* sound);
	ai_play_line_on_object_t p_ai_play_line_on_object;
	int __cdecl ai_play_line_on_object(int object_datum, char* sound)
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

	typedef void(__cdecl* cinematic_stop_t)();
	cinematic_stop_t p_cinematic_stop;
	void cinematic_stop()
	{
		p_cinematic_stop();
	}

	typedef void(__cdecl* custom_animation_relative_t)(const datum object_datum, const string_id animation_path, const string_id animation, const bool is_relative, const WORD object_name_index);
	custom_animation_relative_t p_custom_animation_relative;
	void custom_animation_relative(const datum object_datum, const string_id animation_path, const string_id animation, const bool interpolates_into_animation, const WORD object_name_index)
	{
		return p_custom_animation_relative(object_datum, animation_path, animation, interpolates_into_animation, object_name_index);
	}

	typedef void(__cdecl* object_create_anew_t)(const WORD object_name_index);
	object_create_anew_t p_object_create_anew;
	void __cdecl object_create_anew(const WORD object_name_index)
	{
		typedef void(__cdecl* object_delete_t)(const datum object_index);
		object_delete_t p_object_delete = Memory::GetAddress<object_delete_t>(0x136005, 0x124ED5);
		typedef datum(__cdecl* object_create_t)(const datum object_name_index, bool some_bool);
		object_create_t p_object_create = Memory::GetAddress<object_create_t>(0x138369, 0x127239);
		typedef bool(__cdecl* is_object_player_or_horse_t)(const datum object_index);
		is_object_player_or_horse_t p_is_object_player_or_horse = Memory::GetAddress<is_object_player_or_horse_t>(0x1347FB, 0x1236CB);
		typedef datum(__cdecl* object_index_from_name_index_t)(const WORD object_name_index);
		object_index_from_name_index_t p_object_index_from_name_index = Memory::GetAddress<object_index_from_name_index_t>(0x1335FB, 0x1224CB);

		datum object_index;

		if (object_name_index != 0xFFFF)
		{
			object_index = p_object_index_from_name_index(object_name_index);
			if (object_index != 0xFFFFFFFF && !p_is_object_player_or_horse(object_index))
				p_object_delete(object_index);
			object_index = p_object_create(object_name_index, false);
		}
	}

	typedef void(__cdecl* object_cinematic_lod_t)(const WORD object_name_index, bool enable);
	object_cinematic_lod_t p_object_cinematic_lod;
	void object_cinematic_lod(const WORD object_name_index, bool enable)
	{
		p_object_cinematic_lod(object_name_index, enable);
	}

	void __cdecl print_to_console(const char* output)
	{
		std::string finalOutput("[HSC Print] "); finalOutput += output;
		addDebugText(finalOutput.c_str());
	}

	void* __cdecl hs_arguments_evaluate(__int16 op_code, unsigned __int16 thread_id, bool unk_bool)
	{
		typedef void* (__cdecl* hs_evaluate_t)(unsigned __int16 thread_id, __int16 arg_count, int arg_array, bool unk_bool);
		hs_evaluate_t p_hs_evaluate = Memory::GetAddress<hs_evaluate_t>(0x956FA, 0xAA8FA);
		HaloScriptCommand** hs_function_table = Memory::GetAddress<HaloScriptCommand**>(0x41C5B0, 0x3C0028);

		bool found = false;
		void* args = p_hs_evaluate(thread_id, hs_function_table[op_code]->arg_count, (int)hs_function_table[op_code]->arg_array, unk_bool);

		if (NetworkSession::GetLocalSessionState() != _network_session_state_none)
		{
			LOG_ERROR_GAME("[{}] {}", __FUNCTION__, op_code);

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

				if (!found || !args) { return args; }

				for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
				{
					if (NetworkSession::PlayerIsActive(i))
					{
						NetworkMessage::SendHSFunction(NetworkSession::GetPeerIndex(i), (e_hs_function)op_code, sizeof(s_networked_hs_function::arg_buffer), args);
					}
				}
				return args;
			}
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
			const s_hs_sound_impulse_args* args = (const s_hs_sound_impulse_args*)data->arg_buffer;
			sound_impulse_start(args->sound, args->object_datum, args->scale);
			break;
		}
		case e_hs_function_ai_play_line_on_object:
		{
			const s_hs_ai_play_line_on_object_args* args = (const s_hs_ai_play_line_on_object_args*)data->arg_buffer;
			ai_play_line_on_object(args->object_datum, args->sound);
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
			const s_hs_custom_animation_relative_args* args = (const s_hs_custom_animation_relative_args*)data->arg_buffer;
			custom_animation_relative(args->object_datum, args->animation_path, args->animation, args->interpolates_into_animation, args->object_name_index);
			break;
		}
		case e_hs_function_object_create_anew:
		{
			const WORD* object_name_index = (const WORD*)data->arg_buffer;
			object_create_anew(*object_name_index);
			break;
		}
		case e_hs_function_object_cinematic_lod:
		{
			const s_hs_object_cinematic_lod_args* args = (const s_hs_object_cinematic_lod_args*)data->arg_buffer;
			object_cinematic_lod(args->object_name, args->enable);
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
		DETOUR_ATTACH(p_object_create_anew, Memory::GetAddress<object_create_anew_t>(0xFE444, 0xE5280), object_create_anew);
		DETOUR_COMMIT();


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
			p_custom_animation_relative = Memory::GetAddress<custom_animation_relative_t>(0x978BC);
			p_object_cinematic_lod = Memory::GetAddress<object_cinematic_lod_t>(0x133BE1);
		}
	}
}
