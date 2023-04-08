#pragma once
#include "hs_function.h"
#include "Blam/Cache/DataTypes/StringID.h"

struct HaloScriptGlobal
{
	e_hs_type type;
	BYTE padding[2];
	void* data;
};

#pragma region Halo Script Functions
typedef void* (__cdecl* hs_arguments_evaluate_t)(__int16 op_code, unsigned __int16 thread_id, bool unk_bool);
typedef char* (__cdecl* hs_return_t)(int a1, int a2);
typedef void(__cdecl* no_arg_hs_function_t)();

void unit_kill(datum unitDatum);
bool unit_in_vehicle(datum unitDatum);
float unit_get_health(datum unitDatum);
float unit_get_shield(datum unitDatum);
void physics_set_gravity(float Multiplier);
void physics_set_velocity_frame(float unk1, float unk2, float unk3);
void render_lights_enable_cinenatic_shadow(bool unk1, datum objectDatum, string_id StringId, float unk2);
void object_destroy(datum object_datum_index);
void fade_out(float r, float g, float b, __int16 ticks);
void fade_in(float r, float g, float b, __int16 ticks);
void player_enable_input(bool will_enable);
void camera_control(bool control);
void __cdecl ai_allegiance(__int16 team1, __int16 team2);
void __cdecl sound_impulse_start(char* sound, int object_datum, float scale);
float __cdecl ai_play_line_on_object(int object_datum, string_id sound);
void __cdecl camera_set_animation_relative(const char* animation_path, const char* animation, int unit, const WORD cutscene_flag_index);
void cinematic_start();
void* __cdecl cinematic_start_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool);
void cinematic_stop();
void* __cdecl cinematic_stop_evaluate(__int16 op_code, int thread_id, char unk_bool);
bool custom_animation_relative(const datum object_datum, const datum animation_path, const datum animation, const bool interpolates_into_animation, const datum relative_object);
void object_cinematic_lod(const WORD object_name_index, bool enable);
void switch_bsp(const __int16 bsp_index);
bool custom_animation_loop(const datum object_datum, const datum animation_path, const string_id animation, const bool interpolates_into_animation);
void camera_set_field_of_view(const float fov, const short ticks);
void objects_attach(const datum parent_object, const string_id parent_marker, const datum child_object, const string_id child_marker);
void object_hide(const datum object, const bool hidden);
void game_save_cinematic_skip();
void* __cdecl game_save_cinematic_skip_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool);
void cinematic_lighting_set_primary_light(const float pitch, const float yaw, const float r, const float g, const float b);
void cinematic_lighting_set_secondary_light(const float pitch, const float yaw, const float r, const float g, const float b);
void cinematic_lighting_set_ambient_light(const float r, const float g, const float b);
void object_uses_cinematic_lighting(const datum object, const bool enabled);
void pvs_set_object(const datum object);
void pvs_clear();
void* __cdecl pvs_clear_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool);
bool device_set_overlay_track(const datum device, const string_id animation);
void __cdecl object_dynamic_simulation_disable(const datum object, bool disable_dynamic_simulation);
void __cdecl print_to_console(const char* output);
void game_save();
void* __cdecl game_save_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool);
void game_revert();
void* __cdecl game_revert_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool);
void* __cdecl cinematic_skip_start_internal_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool);
void* __cdecl cinematic_skip_stop_internal_evaluate(const __int16 op_code, const int thread_id, const bool unk_bool);

char* hs_return(int a1, int a2);
#pragma endregion

void hs_initialize();
