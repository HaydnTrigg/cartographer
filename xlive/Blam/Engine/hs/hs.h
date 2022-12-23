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
bool custom_animation_relative(const datum object_datum, const datum animation_path, const string_id animation, const bool interpolates_into_animation, const datum relative_object);
void object_cinematic_lod(const WORD object_name_index, bool enable);
void device_animate_position(const datum device_datum, const float position, const float time, const float unk1, const float unk2, const bool interpolate);
bool device_set_position_track(const datum device, const string_id animation, const float interpolation_time);
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
void device_animate_overlay(const datum device_datum, const float position, const float time, const float unk1, const float unk2);
void __cdecl print_to_console(const char* output);
#pragma endregion

void hs_initialize();
