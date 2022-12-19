#include "stdafx.h"
#include "simulation_world.h"

datum simulation_gamestate_entity_get_object_index(datum entity_seed)
{
	typedef datum(__cdecl* simulation_gamestate_entity_get_object_index_t)(datum entity_seed);
	simulation_gamestate_entity_get_object_index_t p_simulation_gamestate_entity_get_object_index;
	p_simulation_gamestate_entity_get_object_index = Memory::GetAddress<simulation_gamestate_entity_get_object_index_t>(0x1F2211);

	return p_simulation_gamestate_entity_get_object_index(entity_seed);
}