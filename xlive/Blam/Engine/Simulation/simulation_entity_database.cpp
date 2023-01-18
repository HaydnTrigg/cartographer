#include "stdafx.h"
#include "simulation_entity_database.h"

void simulation_action_object_update(const datum object_index, const int flags)
{
	typedef void(__cdecl* entity_set_unk_flags_t)(const datum object_index, const int flags);
	auto p_simulation_action_object_update = Memory::GetAddress<entity_set_unk_flags_t>(0x1B6685, 0x1B05B5);

	p_simulation_action_object_update(object_index, flags);
}
