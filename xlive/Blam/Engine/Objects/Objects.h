#pragma once

#include "Blam/Math/BlamMath.h"
#include "Blam/Engine/DataArray/DataArray.h"
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/Players/PlayerActions.h"
#include "Blam/Engine/Objects/ObjectPlacementData.h"

enum e_object_team : BYTE
{
	// MP
	_object_team_red = 0,
	_object_team_blue = 1,
	_object_team_yellow = 2,
	_object_team_green = 3,
	_object_team_purple = 4,
	_object_team_orange = 5,
	_object_team_brown = 6,
	_object_team_pink = 7,
	_object_team_end = 8,

	// SP
	Default = 0,
	player = 1,
	Human = 2,
	Covenant = 3,
	Flood = 4,
	Sentinel = 5,
	Heretic = 6,
	Prophet = 7,

	// unasigned team ids
	Unused8 = 8,
	Unused9 = 9,
	Unused10 = 10,
	Unused11 = 11,
	Unused12 = 12,
	Unused13 = 13,
	Unused14 = 14,
	Unused15 = 15,

	// Shared
	None = 255
};

enum e_object_type : signed char
{
	biped = 0,
	vehicle,
	weapon,
	equipment,
	garbage,
	projectile,
	scenery,
	machine,
	control,
	light_fixture,
	sound_scenery,
	crate,
	creature,
};

enum e_unit_weapons
{
	PrimaryWeapon,
	SecondaryWeapon,
	DualWeildWeapon
};

enum e_grenades : BYTE
{
	Fragmentation,
	Plasma
};

enum e_weapon_index : WORD
{
	Primary = 0xFF00,
	Secondary = 0xFF01,
	DualWeild = 0x0201
};

enum e_biped_physics_mode : BYTE
{
	mode_ground = 1,
	mode_flying,
	mode_dead,
	mode_posture,
	mode_climbing,
	mode_melee
};

#pragma pack(push, 1)
struct s_object_data_definition
{
	datum tag_definition_index;
	DWORD object_flags;
	void* object_header_block;
	datum next_index;
	datum current_weapon_datum;		//maybe attachment or child
	datum parent_datum;
	WORD unit_in_vehicle_flag;
	INT16 placement_index;
	char gap_1C[12];
	DWORD location[2];
	real_point3d center;
	float radius;
	real_point3d shadow_origin_point;
	float shadow_sphere_radius;
	real_point3d dynamic_light_sphere_offset;
	float dynamic_light_sphere_radius;
	float unk5C;
	real_point3d position;
	real_vector3d orientation;
	real_vector3d up;
	real_point3d translational_velocity;
	real_vector3d angular_velocity;
	float scale;
	datum unique_id;
	__int16 origin_bsp_index;
	e_object_type object_type;//TODO: confirm if its object_type or object_type_flags
	char gap_AB[1];
	__int16 name_list_index;
	char gap_5[1];
	char netgame_equipment_index;
	char placement_policy;
	char gap_6[3];
	datum havok_datum;
	char gap_B8[8];
	WORD flags_C0;
	WORD field_C2;
	DWORD field_C4;
	DWORD field_C8;
	unsigned int field_CC;
	__int16 field_D0;
	BYTE model_variant_id;//hlmt variant tag_block index
	char gap_D3;
	datum simulation_entity_index;
	char field_D8;
	char gap_D9[7];
	WORD destroyed_constraints_flag;
	WORD loosened_constraints_flag;
	float body_max_vitality;
	float shield_max_vitality;
	float body_current_vitality;
	float shield_current_vitality;
	DWORD gap_F4[4];
	WORD shield_stun_ticks;
	WORD body_stun_ticks;
	char gap_108[2];
	WORD field_10A;		//(field_10A & 4) != 0 -- > object_is_dead
	byte pad[2];
	__int16 original_orientation_offset;
	WORD word110;
	__int16 node_orientation_offset;
	__int16 node_buffer_size;
	__int16 nodes_offset;
	PAD(18);
	__int16 animation_manager_index;
};
#pragma pack(pop)
CHECK_STRUCT_OFFSET(s_object_data_definition, node_buffer_size, 0x114);
CHECK_STRUCT_OFFSET(s_object_data_definition, nodes_offset, 0x116);
CHECK_STRUCT_SIZE(s_object_data_definition, 0x12C);

struct s_unit_data_definition : s_object_data_definition
{
	char gap_12C[4];
	DWORD field_130;
	datum simulation_actor_index;
	DWORD unit_flags;		  //(unit_data->unit_flags & 8) != 0   -->active_camo_active
							  //unit_data->unit_flags |= 2         -->unit_is_alive
	e_object_team unit_team;
	char pad[3];
	datum controlling_player_index;
	char gap_142[12];
	DWORD control_flags;
	DWORD control_flags_2;
	DWORD animation_state;
	real_vector3d desired_facing;
	real_vector3d desired_aiming;
	real_vector3d aiming_vector;
	real_vector3d aiming_vector_velocity;
	real_vector3d desired_looking;
	real_vector3d looking_vector;
	real_vector3d looking_vector_velocity;
	DWORD field_1B0;
	DWORD field_1B4;
	DWORD field_1B8;
	real_vector3d throttle;
	char aiming_speed;			//might not be char
	char gap_1C9[3];
	float trigger;
	float secondary_trigger;
	s_aim_assist_targetting_data target_info;
	char gap_1F8[24];
	DWORD parent_seat_index;
	char gap_214[20];
	WORD weapon_set_identifier;
	char gap_22A[39];
	char current_grenade_index;
	WORD grenade_counts_mask;
	char gap_254;
	char zoom_level;
	char gap_256[110];
	float active_camo_power;
	char gap_2C8[4];
	float active_camo_regrowth;
	PAD(144);
};
CHECK_STRUCT_SIZE(s_unit_data_definition, 0x360);

struct s_biped_data_definition : s_unit_data_definition
{
	PAD(0x3F4 - sizeof(s_unit_data_definition));
	e_biped_physics_mode biped_mode;//0x3F4
	PAD(0x480 - 0x3F5);

	// NEW DATA
	string_id variant_name;
};
CHECK_STRUCT_SIZE(s_biped_data_definition, 0x480 + 4);

struct s_weapon_data_definition : s_object_data_definition
{
	PAD(304);
};
CHECK_STRUCT_SIZE(s_weapon_data_definition, 0x25C);

enum e_object_header_flag :BYTE
{
	_object_header_active_bit = 0x1,
	_object_header_requires_motion_bit = 0x2,
	object_header_flags_4 = 0x4,
	_object_header_being_deleted_bit = 0x8,
	object_header_flags_10 = 0x10,
	_object_header_connected_to_map_bit = 0x20,
	_object_header_child_bit = 0x40,
};

struct s_object_header {
	__int16 datum_salt; //0x00
	e_object_header_flag flags; // 0x02
	e_object_type type; // 0x03
	__int16 unk__;  // 0x04
	__int16 object_data_size;  //0x06
	char* object; //0x08 -
};
CHECK_STRUCT_SIZE(s_object_header, 0xC);

struct s_object_globals
{
	bool initialized;
	byte b_collecting_garbage;
	byte b_unsafe_garbage_collection;
	byte b_objects_updating;
	__int16 ticks;
	WORD unk6;
	DWORD unk8;
	DWORD unkC;
	DWORD unk10;
	int total_game_time_ticks;
	DWORD unk18;
	real_color_rgb cinematic_ambient_light_color;
	real_color_rgb cinematic_light_vector;
	float unkfloat1;
	float unkfloat2;
	real_color_rgb cinematic_primary_light_color;
	real_vector3d cinematic_primary_light_vector;
	real_color_rgb cinematic_secondary_light_color;
	real_vector3d cinematic_secondary_light_vector;
	WORD unk6C;
	float global_object_functions[4];
	bool b_custom_animations_hold_on_last_frame;
	bool b_custom_animations_prevent_lipsync_head_movement;
	bool b_warthog_chaingun_light_on;
	bool pad83;
};

static s_data_array* get_objects_header()
{
	return *Memory::GetAddress<s_data_array**>(0x4E461C, 0x50C8EC);
};

// Gets the header of the object, containing some details
static s_object_header* get_objects_header(datum object_idx)
{
	auto objects_header = get_objects_header();
	return (s_object_header*)(&objects_header->data[objects_header->datum_element_size * DATUM_INDEX_TO_ABSOLUTE_INDEX(object_idx)]);
}

// Get the object fast, with no validation from datum index
template<typename T = s_object_data_definition>
static T* object_get_fast_unsafe(datum object_idx)
{
	return (T*)get_objects_header(object_idx)->object;
}

static real_matrix4x3* get_object_nodes(datum object_idx, int* out_node_count)
{
	auto object = object_get_fast_unsafe(object_idx);
	*out_node_count = object->node_buffer_size / sizeof(real_matrix4x3);
	return (real_matrix4x3*)((char*)object + object->nodes_offset);
}

// Gets the object and verifies the type, returns NULL if object doesn't match object type flags
template<typename T = s_object_data_definition>
static T* object_try_and_get_and_verify_type(datum object_idx, int object_type_flags)
{
	auto p_object_try_and_get_and_verify_type = Memory::GetAddress<char* (__cdecl*)(datum, int)>(0x1304E3, 0x11F3A6);
	return (T*)p_object_try_and_get_and_verify_type(object_idx, object_type_flags);
}

// TODO remove
namespace Engine::Objects
{
	void create_new_placement_data(s_object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, int a4);
	datum object_new(s_object_placement_data* object_placement_data);
	void apply_biped_object_definition_patches();
	void simulation_action_object_create(datum object_idx);
	void object_delete(const datum object_idx);
	void object_wake(const datum object_datum);
	bool object_has_animation_manager(const datum object_index);
	void object_compute_node_matrices_with_children(const datum object_datum);
	real_matrix4x3* object_get_node_matrix(const datum object_datum, const __int16 node_index);
	void __cdecl object_type_fix_transform(const datum object, const real_point3d* position, const real_vector3d* up, const real_vector3d* forward);
	void object_set_position(const real_vector3d* forward_ptr, const real_vector3d* up, const datum object_datum, const real_point3d* position, const void* location_struct, const bool some_bool);
	void object_set_position_direct(const datum object_datum, const real_point3d* position, const real_vector3d* forward, const real_vector3d* up, const void* location);
	bool __cdecl object_set_position_internal(datum object_datum,
		const real_point3d* object_placement,
		const real_vector3d* forward,
		const real_vector3d* up,
		const void* location_struct,
		const bool b_compute_node_matricies,
		const bool b_set_havok_position,
		const bool bool2,
		const bool b_shouldnt_reconnect_to_map);

	int object_get_count();
	int object_count_from_iter();
}
