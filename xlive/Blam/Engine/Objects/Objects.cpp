#include "stdafx.h"
#include "Objects.h"

#include "Blam/Cache/TagGroups/object_definition.hpp"
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/Memory/bitstream.h"
#include "Blam/Engine/Objects/Objects.h"
#include "Blam/Engine/physics/havok.h"
#include "Blam/Engine/Players/Players.h"
#include "Blam/Engine/Simulation/GameInterface/SimulationGameUnits.h"
#include "Blam/Engine/Simulation/simulation.h"
#include "Blam/Engine/Simulation/simulation_entity_database.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/PlayerRepresentation/PlayerRepresentation.h"
#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"

namespace Engine::Objects
{
	void create_new_placement_data(s_object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, int a4)
	{
		LOG_TRACE_GAME("{}: {:X}, object_owner: {:X}, unk: {:X})", __FUNCTION__, object_definition_idx, object_owner_idx, a4);

		typedef void(__cdecl* object_placement_data_new_t)(void*, datum, datum, int);
		auto p_object_placement_data_new = Memory::GetAddress<object_placement_data_new_t>(0x132163, 0x121033);

		p_object_placement_data_new(object_placement_data, object_definition_idx, object_owner_idx, a4);
	}

	//Pass new placement data into Create_object_new
	datum object_new(s_object_placement_data* object_placement_data)
	{
		LOG_TRACE_GAME("{}", __FUNCTION__);

		typedef datum(__cdecl* object_new_t)(s_object_placement_data*);
		auto p_object_new = Memory::GetAddress<object_new_t>(0x136CA7, 0x125B77);

		return p_object_new(object_placement_data);
	}

	//Pass datum from new object into object to sync
	void simulation_action_object_create(const datum object_idx)
	{
		typedef void(__cdecl* simulation_action_object_create_t)(datum);
		auto p_simulation_action_object_create = Memory::GetAddress<simulation_action_object_create_t>(0x1B8D14, 0x1B2C44);

		return p_simulation_action_object_create(object_idx);
	}

	void object_delete(const datum object_idx)
	{
		typedef void(__cdecl* object_delete_t)(datum);
		auto p_object_delete = Memory::GetAddress<object_delete_t>(0x136005, 0x124ED5);

		p_object_delete(object_idx);
	}

	void object_wake(const datum object_datum)
	{
		typedef void(__cdecl* object_wake_t)(const datum object_datum);
		auto object_wake = Memory::GetAddress<object_wake_t>(0x12FA1E, 0x11E8E1);

		object_wake(object_datum);
	}

	void __cdecl object_disconnect_from_map(const datum object_index)
	{
		typedef void(__cdecl* object_disconnect_from_map_t)(const datum object_index);
		auto object_disconnect_from_map = Memory::GetAddress<object_disconnect_from_map_t>(0x136226, 0x125136);

		object_disconnect_from_map(object_index);
	}

	void __cdecl object_reconnect_to_map(const void* location_struct, const datum object_index)
	{
		typedef void(__cdecl* object_reconnect_to_map_t)(const void* location_struct, const datum object_index);
		auto object_reconnect_to_map = Memory::GetAddress<object_reconnect_to_map_t>(0x1360CE, 0x124F9E);

		object_reconnect_to_map(location_struct, object_index);
	}

	void object_compute_node_matrices_with_children(const datum object_datum)
	{
		typedef void(__cdecl* object_compute_node_matrices_non_recursive_t)(const datum object_datum);
		auto object_compute_node_matrices_non_recursive = Memory::GetAddress<object_compute_node_matrices_non_recursive_t>(0x1353E6);
		typedef bool(__cdecl* object_compute_node_matrices_locations_t)(const datum object_datum);
		auto object_compute_node_matrices_locations = Memory::GetAddress<object_compute_node_matrices_locations_t>(0x1363D5);

		const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
		s_object_data_definition* next_object = nullptr;
		object_compute_node_matrices_non_recursive(object_datum);
		object_compute_node_matrices_locations(object_datum);
		for (unsigned long i = object->current_weapon_datum; i != -1; i = next_object->next_index)
		{
			next_object = object_get_fast_unsafe(i);
			if (((1 << next_object->object_type) & 0x80u) == 0)
				object_compute_node_matrices_with_children(i);
		}
	}

	bool object_has_animation_manager(const datum object_index)
	{
		return (object_get_fast_unsafe(object_index)->animation_manager_index != 0xFFFF);
	}

	real_matrix4x3* object_get_node_matrix(const datum object_datum, const __int16 node_index)
	{
		const s_object_data_definition* object = object_get_fast_unsafe(object_datum);
		return(real_matrix4x3*)((char*)object + 52 * node_index + object->nodes_offset);
	}

	void __cdecl object_type_fix_transform(const datum object, const real_point3d* position, const real_vector3d* up, const real_vector3d* forward)
	{
		typedef void(__cdecl* object_type_fix_transform_t)(const datum object, const real_point3d* position, const real_vector3d* up, const real_vector3d* forward);
		auto object_type_fix_transform = Memory::GetAddress<object_type_fix_transform_t>(0x186355, 0x1703F1);
		
		object_type_fix_transform(object, position, up, forward);
	}

	void object_set_position(const real_vector3d* up,
		const real_vector3d* forward,
		const datum object_datum,
		const real_point3d* position,
		const void* location_struct,
		const bool some_bool,
		const bool b_dont_update_simulation) // New field b_dont_update_simulation
	{
		real_vector3d forward_1;
		real_point3d position_copy;
		real_vector3d up_copy;
		real_vector3d forward_copy;

		if (position)
		{
			position_copy.x = position->x;
			position_copy.y = position->y;
			position_copy.z = position->z;
		}
		if (forward)
		{
			forward_copy.i = forward->i;
			forward_copy.j = forward->j;
			forward_copy.k = forward->k;
		}
		if (up)
		{
			up_copy.i = up->i;
			up_copy.j = up->j;
			up_copy.k = up->k;
		}
		object_type_fix_transform(object_datum, &position_copy, &forward_copy, &up_copy);
		if (!object_set_position_internal(object_datum, &position_copy, &forward_copy, &up_copy, location_struct, true, true, some_bool, false, b_dont_update_simulation)
			&& !simulation_query_object_is_predicted(object_datum))
		{
			object_delete(object_datum);
		}
		object_wake(object_datum);
	}

	void object_set_position_direct(const datum object_datum, const real_point3d* position, const real_vector3d* forward, const real_vector3d* up, 
		const void* location, 
		const bool b_dont_update_simulation)
		// New field b_dont_update_simulation
	{
		object_set_position(up, forward, object_datum, position, location, false, b_dont_update_simulation);
	}

	bool __cdecl object_set_position_internal(datum object_datum,
		const real_point3d* object_placement,
		const real_vector3d* forward,
		const real_vector3d* up,
		const void* location_struct,
		const bool b_compute_node_matricies,
		const bool b_set_havok_position,
		const bool bool2,
		const bool b_shouldnt_reconnect_to_map,
		const bool b_dont_update_simulation)	// New field b_dont_update_simulation
	{

		s_object_data_definition* object = object_get_fast_unsafe(object_datum);
		bool some_flag_is_true = object->object_flags & 1;
		bool isnt_out_of_bounds = true;
		if (some_flag_is_true && !b_shouldnt_reconnect_to_map) { object_disconnect_from_map(object_datum); }

		if (object_placement)
		{
			if (object->parent_datum == DATUM_INDEX_NONE
				&& (object_placement->x < SHRT_MIN || object_placement->x > SHRT_MAX
					|| object_placement->y < SHRT_MIN || object_placement->y > SHRT_MAX
					|| object_placement->z < SHRT_MIN || object_placement->z > SHRT_MAX))
			{
				isnt_out_of_bounds = false;
			}
			else
			{
				object->position = *object_placement;
				if (!b_dont_update_simulation)
				{
					simulation_action_object_update(object_datum, 2);
				}
			}
		}
		if (up)
		{
			object->orientation = *forward;
			object->up = *up;
			if (!b_dont_update_simulation)
			{
				simulation_action_object_update(object_datum, 4);
			}
		}

		if (b_set_havok_position) { havok_object_set_position(object_datum, object_placement == 0, bool2); }

		if (b_compute_node_matricies) { object_compute_node_matrices_with_children(object_datum); }

		if (some_flag_is_true && !b_shouldnt_reconnect_to_map) { object_reconnect_to_map(location_struct, object_datum); }

		return isnt_out_of_bounds;
	}

#pragma region Biped variant patches
	void update_biped_object_variant_data(datum object_idx, int variant_index)
	{
		s_biped_data_definition* biped_object = (s_biped_data_definition*)object_try_and_get_and_verify_type(object_idx, FLAG(e_object_type::biped));
		// set this data only if we are dealing with a biped
		if (biped_object != NULL)
		{
			biped_object->variant_name = variant_index;
			// addDebugText("set the variant data to: %u for biped", variant_index);
		}
	}

	void __cdecl update_object_variant_index_hook(datum object_idx, int variant_index)
	{
		auto p_resolve_variant_index_to_new_variant = Memory::GetAddressRelative<int(__cdecl*)(datum, int)>(0x52FE84, 0x51ED47);
		auto object = object_get_fast_unsafe<s_biped_data_definition>(object_idx);

		object->model_variant_id = p_resolve_variant_index_to_new_variant(object_idx, variant_index);
		// update the biped variant index
		update_biped_object_variant_data(object_idx, variant_index);

		//addDebugText("object_index: %d, model_variant_id: %u", DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index), object->variant_index);
	}

	// original update_object_variant_index is usercall, with data in CX register as first param
	__declspec(naked) void update_object_variant_index_to_cdecl()
	{
		__asm
		{
			// we don't preserve eax, because it's a volatile register thats expected to have 
			// it's value changed

			mov eax, [esp + 4] // copy the variant index
			push eax		   // then push to stack for update_object_variant_index_hook as second param
			push ecx		   // ecx stores the object index (in CX register size), push as first param
			call update_object_variant_index_hook
			add esp, 8			// reset stack after update_object_variant_index_hook
			retn				// return to procedure
		}
	}

	typedef void(__stdcall* c_simulation_unit_entity_definition_creation_encode_t)(void* thisptr, int creation_data_size, void* creation_data, int a3, bitstream* stream);
	c_simulation_unit_entity_definition_creation_encode_t p_c_simulation_unit_entity_definition_encode;

	void __stdcall c_simulation_unit_entity_definition_creation_encode_hook(void* thisptr, int creation_data_size, s_simulation_unit_creation_data* creation_data, int a3, bitstream* stream)
	{
		int object_permutation_idx = creation_data->variant_name.get_packed();
		stream->data_encode_integer("object-permutation-index", object_permutation_idx, 32);
		//addDebugText("encoded entity creation: variant index: %d", object_permutation_index);
		p_c_simulation_unit_entity_definition_encode(thisptr, creation_data_size, creation_data, a3, stream);
	}

	typedef bool(__stdcall* c_simulation_unit_entity_definition_creation_decode_t)(void* thisptr, int creation_data_size, void* creation_data, bitstream* stream);
	c_simulation_unit_entity_definition_creation_decode_t p_c_simulation_unit_entity_definition_decode;

	bool __stdcall c_simulation_unit_entity_definition_creation_decode_hook(void* thisptr, int creation_data_size, s_simulation_unit_creation_data* creation_data, bitstream* stream)
	{
		int object_permutation_idx = stream->data_decode_integer("object-permutation-index", 32);
		creation_data->variant_name = object_permutation_idx;
		//addDebugText("decoded entity creation: variant index: %d", object_permutation_index);
		return p_c_simulation_unit_entity_definition_decode(thisptr, creation_data_size, creation_data, stream);
	}

	int __stdcall c_simulation_object_entity_definition_object_create_object_hook(int thisx, s_simulation_unit_creation_data* creation_data, int a2, int a3, s_object_placement_data* object_placement_data)
	{
		// set the object placement data
		object_placement_data->variant_name = creation_data->variant_name;
		if(*(byte*)((char*)creation_data + 0x10) != -1)
		{
			auto profile = reinterpret_cast<s_player::s_player_properties::s_player_profile*>((char*)creation_data + 0x10);
			datum player_representation_datum = PlayerRepresentation::get_object_datum_from_representation(profile->player_character_type);
			if (player_representation_datum != DATUM_INDEX_NONE)
				object_placement_data->tag_index = player_representation_datum;
		}
		//addDebugText("creating object with variant index: %d", object_placement_data->variant_name);
		return Memory::GetAddress<int(__thiscall*)(int, void*, int, int, s_object_placement_data*)>(0x1F32DB, 0x1DE374)(thisx, creation_data, a2, a3, object_placement_data);
	}

	__declspec(naked) void c_simulation_object_entity_definition_object_create_object_to_stdcall()
	{
		__asm
		{
			pop eax // pop return address
			push ecx // push ecx as first param
			push eax // push the return address back on stack
			jmp c_simulation_object_entity_definition_object_create_object_hook
		}
	}

	typedef bool(__cdecl* set_unit_color_data_t)(int, unsigned __int16, int a3);
	set_unit_color_data_t p_set_unit_color_data;

	bool __cdecl set_unit_color_data_hook(int a1, unsigned __int16 a2, int a3)
	{
		auto object_creation_data = (s_simulation_unit_creation_data*)(a1 - 0x10);
		int object_permutation_idx = object_creation_data->variant_name.get_packed();

		//if (object_permutation_index == 0)
		if(*(byte*)((char*)a1) != -1)
			return p_set_unit_color_data(a1, a2, a3);

		return 0;
	}

	void __stdcall object_build_creation_data_hook(datum object_idx, s_simulation_unit_creation_data* object_creation_data)
	{
		auto p_object_build_creation_data = Memory::GetAddress<void(__stdcall*)(datum, void*)>(0x1F24ED, 0x1DD586);

		p_object_build_creation_data(object_idx, object_creation_data);

		auto object = object_get_fast_unsafe<s_biped_data_definition>(object_idx);

		object_creation_data->variant_name = object->variant_name;

		//addDebugText("object build creation data: variant index: %d", object->variant_index);
	}

	void apply_biped_object_definition_patches()
	{
		LOG_INFO_GAME("{} - applying hooks", __FUNCTION__);

		// increase the data size for biped representation
		WriteValue<unsigned short>(Memory::GetAddressRelative(0x81E9A8, 0x7C1EB8) + 8, sizeof(s_biped_data_definition));

		// hook the function that updates the variant
		WriteJmpTo(Memory::GetAddressRelative(0x52FED3, 0x51ED96), update_object_variant_index_to_cdecl);

		// increase the size of the unit entity creation definition packet
		WriteValue<DWORD>(Memory::GetAddress(0x1F8028, 0x1E1D8E) + 1, sizeof(s_simulation_unit_creation_data));

		// increase the minimum required bitstream size
		WriteValue<BYTE>(Memory::GetAddressRelative(0x5F84B9, 0x5E221F) + 3, 5 + 32); // + 32 is quite much, but idk how bitstream will behave with less than 32 when dealin with integers

		PatchCall(Memory::GetAddressRelative(0x5F807A, 0x5E1DE0), object_build_creation_data_hook);

		//This encodes the unit creation packet, only gets executed on host.
		p_c_simulation_unit_entity_definition_encode = (c_simulation_unit_entity_definition_creation_encode_t)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8503, 0x1E2269), (BYTE*)c_simulation_unit_entity_definition_creation_encode_hook, 10);

		//This decodes the unit creation packet, only gets executed on client.
		p_c_simulation_unit_entity_definition_decode = (c_simulation_unit_entity_definition_creation_decode_t)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8557, 0x1E22BD), (BYTE*)c_simulation_unit_entity_definition_creation_decode_hook, 11);

		// Hooks a call within the creat_unit property on the client side in order to set their permutation index before spawn.
		PatchCall(Memory::GetAddress(0x1F9E6C, 0x1E3BD4), c_simulation_object_entity_definition_object_create_object_to_stdcall);

		// Hooks the part of the unit spawn from simulation that handles setting their color data in order to ensure AI do not have their color overridden
		PatchCall(Memory::GetAddress(0x1F9E34, 0x1E3B9C), set_unit_color_data_hook);
		p_set_unit_color_data = Memory::GetAddress<set_unit_color_data_t>(0x6E5C3, 0x6D1BF);
	}

	int object_get_count()
	{
		s_data_iterator object_it(get_objects_header());
		return object_it.get_data_count();
	}

	int object_count_from_iter()
	{
		s_data_iterator object_it(get_objects_header());
		int count = 0;
		while (object_it.get_next_datum())
		{
			count++;
		}
		return count;
	}
#pragma endregion
}
