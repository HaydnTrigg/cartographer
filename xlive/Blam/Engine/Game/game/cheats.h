#pragma once
#include "stdafx.h"
#include "Blam/Engine/Game/game/game.h"
#include "Util/Memory.h"

enum e_skulls
{
	_envy,
	_grunt_birthday_party,
	_assassians,
	_thunderstorm,
	_famine,
	_IWHBYD,
	_blind,
	_ghost,
	_black_eye,
	_catch,
	_sputnik,
	_iron,
	_mythic,
	_anger,
	_whuppopotamus,

	_skulls_end
};

//static bool skull_enabled[16];

namespace cheats
{
	static bool ice_cream_flavor_available(e_skulls skull)
	{
		return skull < _skulls_end && s_game_globals::game_is_campaign() && Memory::GetAddress<bool*>(0x4D8320)[skull];
	}

	/*
	static void __cdecl ice_cream_activation(unsigned int skull)
	{
		int first_valid; // eax
		int player_information_tagblock_ptr; // eax
		int player_information_tagblock; // ecx
		int sound; // ecx
		string_id skull_string_id; // [esp+Ch] [ebp-8h]

		if (skull <= 14 && !skull_enabled[skull])
		{
			skull_enabled[skull] = 1;
			hud_clear_messages();
			skull_string_id = dword_7BCAF8[skull];
			first_valid = local_user_index_get_first_valid();
			display_generic_hud_string(first_valid, skull_string_id);
			fade_in_flash(COERCE_FLOAT_(1.0), COERCE_FLOAT_(1.0), COERCE_INT(1.0), 20);
			player_information_tagblock_ptr = *(global_globals_tag + 0x134);
			player_information_tagblock = 0;
			if (player_information_tagblock_ptr != -1)
				player_information_tagblock = player_information_tagblock_ptr + tag_data_1;
			sound = *(player_information_tagblock + 0x118);
			if (sound != -1)
				unspatialized_impulse_sound_new(sound, 1.0);
		}
	}*/


	// New Carto Functions
	static bool* ice_cream_flavor_state()
	{
		return Memory::GetAddress<bool*>(0x4D8320);
	}
}
