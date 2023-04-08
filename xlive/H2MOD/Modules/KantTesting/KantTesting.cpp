#include "stdafx.h"

#include "KantTesting.h"
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"
#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "Blam\Cache\TagGroups\globals_definition.hpp"
#include "Blam\Cache\TagGroups\model_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_lightmap_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_structure_bsp_definition.hpp"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Engine\Game\GameEngineGlobals.h"
#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Engine\Players\Players.h"
#include "Blam\LazyBlam\LazyBlam.hpp"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "Blam\Engine\Memory\bitstream.h"
#include "Blam\Engine\Memory\bitstream.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "Blam/Engine/Simulation/GameInterface/SimulationGameEntities.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD\Modules\PlayerRepresentation\PlayerRepresentation.h"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "Util\Hooks\Hook.h"


namespace KantTesting
{
	typedef void(__cdecl* game_state_call_before_load_proces_t)();
	game_state_call_before_load_proces_t p_game_state_call_before_load_procs;

	void __cdecl game_state_call_before_load_procs()
	{
		p_game_state_call_before_load_procs();
	}


	typedef int(__cdecl* sub_5AE6F1_t)(char a1);
	sub_5AE6F1_t p_sub_5AE6F1;

	int __cdecl sub_5AE6F1(char a1)
	{
		return p_sub_5AE6F1(a1);
	}
	void dump_entity_table()
	{
		auto simulation_world = Memory::GetAddress<char**>(0x5178DC);
		auto distributed_world = (char**)(*simulation_world + 4);
		auto simulation_entity_database = (*distributed_world + 0x20A0 + 20);
		for (auto i = 0; i < 1024; i++)
		{
			auto test = (s_simulation_game_entity*)simulation_entity_database + 36 * i;
			if (test->entity_index != -1)
			{
				auto test_o = object_get_fast_unsafe<s_object_data_definition>(test->object_index);
				auto name = tags::get_tag_name(test_o->tag_definition_index);
				LOG_INFO_GAME("[Entity Dump] {:x} {:x} {}", test->entity_index, test->object_index, name);
			}
			else
				break;
		}
		LOG_INFO_GAME("[Entity Dump] Dumped");
	}
	typedef void(__cdecl* simulation_action_object_create_t)(int object_index);
	simulation_action_object_create_t p_simulation_action_object_create;
	
	void regenerate_entity_table()
	{
		auto scen = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
		std::vector<datum> simulation_tag_datums;
		for (auto simulation_definition_entry : scen->simulation_definition_table)
			simulation_tag_datums.emplace_back(simulation_definition_entry.tag);

		auto objects_table = s_data_iterator<s_object_header>(get_objects_header());
		while(objects_table.get_next_datum())
		{
			auto object = (s_object_data_definition*)objects_table.get_current_datum()->object;
			if(std::find(simulation_tag_datums.begin(), simulation_tag_datums.end(), object->tag_definition_index) != simulation_tag_datums.end())
			{
				objects_table.get_current_datum()->flags = (e_object_header_flag)(((byte)objects_table.get_current_datum()->flags) | object_header_flags_10);
				p_simulation_action_object_create(objects_table.get_current_absolute_index());
			}
		}
	}

	typedef void(__cdecl* simulation_player_collection_clear_t)(char* a1);
	simulation_player_collection_clear_t p_simulation_player_collection_clear;

	typedef void(__thiscall* c_simulation_world_reset_world_t)(char* simulation_world);
	c_simulation_world_reset_world_t p_c_simulation_world_reset_world;

	typedef void(__thiscall* c_entity_database_reset_t)(char* entity_database);
	c_entity_database_reset_t p_c_entity_database_reset;

	typedef void(__cdecl* game_state_revert_t)(char arg0);
	game_state_revert_t p_game_state_revert;

	void __cdecl game_state_revert(char a1)
	{
		auto b_b = Memory::GetAddress<bool*>(0x5178D2, 0);
		auto p1 = s_player::GetPlayer(0);
		auto o1 = object_get_fast_unsafe<s_biped_data_definition>(p1->unit_index);

		auto unk = Memory::GetAddress<char**>(0x5178E0);
		dump_entity_table();
		auto simulation_world = Memory::GetAddress<char**>(0x5178DC);
		auto distributed_world = (char**)(*simulation_world + 4);
		p_c_simulation_world_reset_world(*simulation_world);
 		p_c_entity_database_reset(*distributed_world + 0x20A0);
		//p_simulation_player_collection_clear(*unk + 140);
		//*b_b = true;
		p_game_state_revert(a1);
		//*b_b = false;
	}
	void MapLoad()
	{
		if (h2mod->GetEngineType() == _multiplayer)
		{
		}
	}

	void force_client_bluescreen()
	{
		auto p = NetworkSession::GetPeerIndex(1);
		for(int i = 0; i < 16; i++)
		{
			NetworkSession::GetCurrentNetworkSession()->peer_observer_channels[i].field_1 = false;
			for (int i = 0; i < 16; i++)
			{
				s_network_channel* network_channels = *Memory::GetAddress<s_network_channel**>(0x4FADBC, 0x525274);
				network_channels[i].field_1D = 0;
			}
		}
	}
	void force_client_bluescreen2()
	{
		auto p = NetworkSession::GetPeerIndex(1);
		for (int i = 0; i < 16; i++)
		{
			s_network_channel* network_channels = *Memory::GetAddress<s_network_channel**>(0x4FADBC, 0x525274);
			network_channels[i].field_1D = 0;
		}
	}
	auto key = VK_OEM_MINUS;
	auto key2 = VK_OEM_PLUS;

	typedef bool(__cdecl* machine_update_t)(datum machine_datum_index);
	machine_update_t p_machine_update;
	bool machine_update_hook(datum machine_datum_index)
	{
		bool result = p_machine_update(machine_datum_index);
		s_object_data_definition* object = object_get_fast_unsafe(machine_datum_index);

		if (s_game_globals::game_is_campaign() && NetworkSession::LocalPeerIsSessionHost())
		{
			typedef void(__cdecl* simulation_action_object_update_t)(datum object_datum_index, int flags);
			auto simulation_action_object_update = Memory::GetAddress<simulation_action_object_update_t>(0x1B6685);
			simulation_action_object_update(machine_datum_index, 4);
		}

		return result;
	}

	void Initialize()
	{
		DETOUR_BEGIN();
		DETOUR_ATTACH(p_machine_update, Memory::GetAddress<machine_update_t>(0x18D8C7, 0x0), machine_update_hook);
		DETOUR_COMMIT()

		KeyboardInput::RegisterHotkey(&key, regenerate_entity_table);
		KeyboardInput::RegisterHotkey(&key2, dump_entity_table);
		if (ENABLEKANTTEST) {
			DETOUR_BEGIN();
			DETOUR_ATTACH(p_game_state_call_before_load_procs, Memory::GetAddress<game_state_call_before_load_proces_t>(0x8C245, 0), game_state_call_before_load_procs);
			DETOUR_ATTACH(p_game_state_revert, Memory::GetAddress < game_state_revert_t>(0x305DA, 0), game_state_revert);
			DETOUR_COMMIT()

			p_sub_5AE6F1 = Memory::GetAddress<sub_5AE6F1_t>(0x1AE6F1, 0);
			p_simulation_player_collection_clear = Memory::GetAddress<simulation_player_collection_clear_t>(0x1E13FF);
			p_c_simulation_world_reset_world = Memory::GetAddress<c_simulation_world_reset_world_t>(0x1DD0EA);
			p_c_entity_database_reset = Memory::GetAddress<c_entity_database_reset_t>(0x1D389E);
			p_simulation_action_object_create = Memory::GetAddress<simulation_action_object_create_t>(0x1B8D14);
		//	if (!Memory::isDedicatedServer())
			//{
			//tags::on_map_load(MapLoad);
		//	}
		}
	}
}
