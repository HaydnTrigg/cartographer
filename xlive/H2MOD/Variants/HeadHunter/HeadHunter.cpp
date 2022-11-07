#include "stdafx.h"

#include "HeadHunter.h"

#include "Blam\Engine\Camera\Camera.h"
#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"
#include "Blam\Engine\Networking\Session\NetworkSession.h"

#include "H2MOD\Engine\Engine.h"
#include "H2MOD\GUI\imgui_integration\imgui_handler.h"
#include "H2MOD\GUI\imgui_integration\ImGui_HeadHunter_Skull_Count.h"
#include "H2MOD\Modules\CustomMenu\CustomLanguage.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Modules\HaloScript\HaloScript.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "H2MOD\Utils\Utils.h"

std::unordered_map<int, std::unordered_map<e_headhunter_sounds, const wchar_t*>> headhunterSoundTable;
bool b_firstSpawn = false;
__int8 skull_count[16] = { 0 };
datum skull_datum;

HeadHunter::HeadHunter()
{
}

HeadHunter::~HeadHunter()
{
}

void HeadHunter::TriggerSound(e_headhunter_sounds sound, int sleep)
{
	if (headhunterSoundTable.count(H2Config_language.code_main)) 
	{
		LOG_TRACE_GAME(L"[h2mod-headhunter] Triggering sound {}", headhunterSoundTable[H2Config_language.code_main][sound]);
		h2mod->custom_sound_play(headhunterSoundTable[H2Config_language.code_main][sound], sleep);
	}
	else 
	{
		LOG_TRACE_GAME(L"[h2mod-headhunter] Triggering sound {}", headhunterSoundTable[0][sound]);
		h2mod->custom_sound_play(headhunterSoundTable[0][sound], sleep);
	}
}

void HeadHunter::SpawnPlayerClientSetup()
{
	if (b_firstSpawn)
	{
		HeadHunter::TriggerSound(_snd_head_hunter, 1000);
		b_firstSpawn = false;
	}
}

void HeadHunter::SpawnSkull(datum unit_datum)
{
	s_biped_data_definition* biped_unit = (s_biped_data_definition*)object_try_and_get_and_verify_type(unit_datum, FLAG(e_object_type::biped));

	if (biped_unit != NULL)
	{
		s_object_placement_data nObject;

		Engine::Objects::create_new_placement_data(&nObject, e_weapons_datum_index::ball, -1, 0);

		nObject.position = biped_unit->position;
		nObject.translational_velocity = biped_unit->translational_velocity;

		datum new_object_datum = Engine::Objects::object_new(&nObject);
		if (!DATUM_IS_NONE(new_object_datum))
			Engine::Objects::simulation_action_object_create(new_object_datum);
	}
}

typedef void(__thiscall *update_player_score_t)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
extern update_player_score_t p_update_player_score;

void HeadHunter::PickupSkull(datum playerIdx, datum skullDatum)
{
	__int8 absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);

	if (!DATUM_IS_NONE(skullDatum))
	{
		typedef char* (__cdecl* get_score_data_ptr)();
		auto p_get_score_data_ptr = Memory::GetAddress<get_score_data_ptr>(0x6B8A7, 0x6AD32);

		char* player_score_data = p_get_score_data_ptr();

		// Dont automatically pick up skulls if player has 10
		if (player_score_data && skull_count[absPlayerIdx] < 10)
		{
			skull_count[absPlayerIdx]++;
			HaloScript::ObjectDestroy(skullDatum);
			SendSkullCounts();
			HeadHunter::TriggerSound(_snd_skull_taken, 500);
		}
	}
}
void HeadHunter::initClient()
{
	skull_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\headhunter_ball\\headhunter_ball", blam_tag::tag_group_type::crate, "carto_shared");
	if (skull_datum != DATUM_INDEX_NONE)
	{
		tag_loader::Load_tag(skull_datum, true, "carto_shared");
		tag_loader::Push_Back();
	}
	b_firstSpawn = true;
	h2mod->disable_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);
	headhunterSoundTable[_lang_id_english][e_headhunter_sounds::_snd_head_hunter]	= L"sounds/en/headhunter.wav";
	headhunterSoundTable[_lang_id_english][e_headhunter_sounds::_snd_skull_scored]	= L"sounds/en/skull_scored.wav";
	headhunterSoundTable[_lang_id_english][e_headhunter_sounds::_snd_skull_taken]	= L"sounds/en/skull_taken.wav";
	headhunterSoundTable[_lang_id_spanish][e_headhunter_sounds::_snd_head_hunter]	= L"sounds/es/headhunter.wav";
	headhunterSoundTable[_lang_id_spanish][e_headhunter_sounds::_snd_skull_scored]	= L"sounds/es/skull_scored.wav";
	headhunterSoundTable[_lang_id_spanish][e_headhunter_sounds::_snd_skull_taken]	= L"sounds/es/skull_taken.wav";
}

void HeadHunter::Initialize()
{
	if (!Memory::IsDedicatedServer())
	{
		HeadHunter::initClient();
	}
}

void HeadHunter::Dispose()
{
	g_headhunter_skull_count = false;
}

CustomVariantId HeadHunter::GetVariantId()
{
	return CustomVariantId::_id_headhunter;
}

void HeadHunter::OnMapLoad(ExecTime execTime, s_game_options* gameOptions)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		switch (h2mod->GetEngineType())
			{
				// cleanup when loading main menu
			case _multiplayer:
				this->Initialize();
				break;
			/*case _main_menu:
				this->Dispose();
				break;*/
			default:
				break;
			}
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void HeadHunter::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		break;

		// postspawn handler
	case ExecTime::_postEventExec:
		if (!Memory::IsDedicatedServer())
			g_headhunter_skull_count = true;
			HeadHunter::SpawnPlayerClientSetup();
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void HeadHunter::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (!s_game_globals::game_is_predicted())
			// Spawn the player's head
			HeadHunter::SpawnSkull(playerUnitDatum);

			// If the player has other skulls stored force him to barf them all out
			if (skull_count[absPlayerIdx] != 0)
			{
				for (__int8 i = 0; skull_count[absPlayerIdx] > i; i++)
				{
					HeadHunter::SpawnSkull(playerUnitDatum);
				}
				skull_count[absPlayerIdx] = 0;
				SendSkullCounts();
			}
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

bool HeadHunter::OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6)
{
	int absPlayerIdx = a2;
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// dont give player points unless in the hill
		if (a5 == 7 || a5 == 9) {}
		else
		{
			// Dont update score unless they have skulls
			if (skull_count[absPlayerIdx] != 0)
			{
				p_update_player_score(thisptr, absPlayerIdx, a3, skull_count[absPlayerIdx], a5, a6);
				skull_count[absPlayerIdx] = 0;
				SendSkullCounts();

				HeadHunter::TriggerSound(_snd_skull_scored, 500);
			}
		}
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}

	return true;
}

bool HeadHunter::OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		if (!s_game_globals::game_is_predicted())
		{
			s_weapon_data_definition* weaponObject = object_get_fast_unsafe<s_weapon_data_definition>(objectIdx);

			if (DATUM_INDEX_TO_ABSOLUTE_INDEX(weaponObject->tag_definition_index) == DATUM_INDEX_TO_ABSOLUTE_INDEX(e_weapons_datum_index::ball))
			{
				HeadHunter::PickupSkull(playerIdx, objectIdx);
				handled = true;
			}
		}
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}

	return handled;
}

void SendSkullCounts()
{
	for (__int8 i = 0; i < 16; i++)
	{
		NetworkMessage::SendSkullCounts(NetworkSession::GetPeerIndex(i));
	}
}

void update_text_headhunter_hook(int datum_index, wchar_t *player_name)
{
	__int8 absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index);
	wchar_t symbol[2] = { (wchar_t)0x2605, '\0' };

	switch (skull_count[absPlayerIdx])
	{
	case 0:
		wcsncpy(player_name, L"0\0", 2);
		break;
	case 1:
		wcsncpy(player_name, L"1\0", 2);
		break;
	case 2:
		wcsncpy(player_name, L"2\0", 2);
		break;
	case 3:
		wcsncpy(player_name, L"3\0", 2);
		break;
	case 4:
		wcsncpy(player_name, L"4\0", 2);
		break;
	case 5:
		wcsncpy(player_name, L"5\0", 2);
		break;
	case 6:
		wcsncpy(player_name, L"6\0", 2);
		break;
	case 7:
		wcsncpy(player_name, L"7\0", 2);
		break;
	case 8:
		wcsncpy(player_name, L"8\0", 2);
		break;
	case 9:
		wcsncpy(player_name, L"9\0", 2);
		break;
	case 10:
		wcsncpy(player_name, symbol, 2);
		break;

	// This should never ever execute
	default:
		wcsncpy(player_name, L"OOPSIE POOPSIE\0", 16);
		break;
	}
}

// Move this
void draw_text_at_offset(wchar_t* p_string, real_point3d location)
{
	typedef real_point3d* (__cdecl matrix4x3_transform_point_t)(real_matrix4x3* a1, real_point3d* v1, real_point3d* v2);
	auto p_matrix4x3_transform_point = Memory::GetAddress<matrix4x3_transform_point_t*>(0x7795A);

	typedef __int8 (__cdecl p_world_to_screen_t)(s_camera* camera, 
		real_matrix4x3* projection, 
		SMALL_RECT* screen, 
		real_point3d* text_world_position, 
		float* text_screen_location);
	auto p_world_to_screen = Memory::GetAddress<p_world_to_screen_t*>(0x194BE5);

	typedef void(__cdecl sub_6309D6_t)(wchar_t* p_string);
	auto p_sub_6309D6 = Memory::GetAddress<sub_6309D6_t*>(0x2309D6);

	typedef void(__cdecl render_player_name_text_t)(const wchar_t* string, float a2, float* a3);
	auto p_render_player_name_text = Memory::GetAddress<render_player_name_text_t*>(0x6ADCD);

	real_matrix4x3* global_projection = Memory::GetAddress<real_matrix4x3*>(0x4E673C);
	s_camera* global_camera = Memory::GetAddress<s_camera*>(0x4E66C8);

	float text_screen_location[2];
	real_point3d text_world_position;

	location.z + 0.50000001;
	p_matrix4x3_transform_point(global_projection, &location, &text_world_position);
	if (p_world_to_screen(global_camera, global_projection, 0, &location, text_screen_location))
	{
		p_render_player_name_text(p_string, 1, text_screen_location);
	}
}

void draw_skull_indicators()
{
	auto it = new s_data_iterator<s_object_header>(get_objects_header());
	while (it->get_next_datum())
	{
		auto i = it->get_current_datum();
		datum i_datum = i->datum_salt << 16;
		i_datum += *(short*)&i->flags;
		auto object = reinterpret_cast<s_object_data_definition*>(i->object);

		//Skull Symbol
		wchar_t symbol[2] = { (wchar_t)0xE1C1, '\0' };

		if (DATUM_INDEX_TO_ABSOLUTE_INDEX(object->tag_definition_index) == DATUM_INDEX_TO_ABSOLUTE_INDEX(e_weapons_datum_index::ball))
		{
			draw_text_at_offset(symbol, object->position);
		}
	}
}

