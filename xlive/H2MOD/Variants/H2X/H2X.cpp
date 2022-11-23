#include "stdafx.h"

#include "H2X.h"

#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Engine\Game\GameTimeGlobals.h"
#include "H2MOD.h"
#include "H2MOD\Tags\TagInterface.h"

#include "Util\Hooks\Hook.h"

std::vector<H2X::h2x_mod_info> weapons =
{
	{ "objects\\weapons\\rifle\\sniper_rifle\\sniper_rifle", 0.535f, 0.5f, 0, false },
	{ "objects\\weapons\\rifle\\battle_rifle\\battle_rifle", 0.295f, 0.26f, 0, false },
	{ "objects\\weapons\\pistol\\magnum\\magnum", 0.13f, 0.1f, 0, false },
	{ "objects\\weapons\\rifle\\shotgun\\shotgun", 1.035f, 1.0f, 0, false },
	{ "objects\\weapons\\rifle\\beam_rifle\\beam_rifle", 0.25875f, 0.25f, 0, false },
	{ "objects\\weapons\\support_low\\brute_shot\\brute_shot", 0.39f, 0.3f, 0, false },
	{ "objects\\weapons\\pistol\\plasma_pistol\\plasma_pistol", 0.11f, 0.05f, 0, false },
	{ "objects\\weapons\\rifle\\covenant_carbine\\covenant_carbine", 0.20f, 0.14f, 1, false },
	{ "objects\\weapons\\support_high\\rocket_launcher\\rocket_launcher", 0.85f, 0.8f, 0, false },
	{ "objects\\weapons\\rifle\\plasma_rifle\\plasma_rifle", 8.5f, 9.0f, 0, true },
	{ "objects\\weapons\\rifle\\brute_plasma_rifle\\brute_plasma_rifle", 10.0f, 11.0f, 0, true }
};

float __cdecl game_seconds_to_ticks_real_weapon_adjust(float s)
{
	// if the recovery time set in the tags isn't at least game's seconds per tick (i.e. 1/tickrate seconds) or a multiple of it,
	// it'll result in fractional ticks that cannot be handled that easily in a fixed time step, where a game tick advances a certain amount of seconds/milliseconds per game tick
	// e.g 0.14 seconds of carbine fire recovery time will result in 30 ticks * 0.14 recovery = 4.2 ticks of recovery time, with .2 fractional tick at 30hz 
	// and 8.4 with .4 fractional ticks at 60hz by using the same logic
	// causing differences between tickrates (in this case between 30 and 60)

	float tick_difference = 0.0f;
	if (trunc(s) != s)
		tick_difference = blam_max(time_globals::get_ticks_difference_real() - 1.0f, 0.0f);
	float seconds_to_ticks_adjusted = (float)time_globals::get()->ticks_per_second * s + tick_difference;
	return seconds_to_ticks_adjusted;
}

void H2X::ApplyMapLoadPatches(bool enable)
{
	for (auto& weapon : weapons)
	{
		auto required_datum = tags::find_tag(blam_tag::tag_group_type::weapon, weapon.tag_string);
		BYTE* weapon_tag = tags::get_tag<blam_tag::tag_group_type::weapon, BYTE>(required_datum);
		if (weapon_tag != nullptr)
			{
				int barrel_data_block_size = 236;
				tags::tag_data_block* barrel_data_block = reinterpret_cast<tags::tag_data_block*>(weapon_tag + 720);

				if (barrel_data_block->block_data_offset != -1)
				{
					*(float*)(tags::get_tag_data()
						+ barrel_data_block->block_data_offset
						+ barrel_data_block_size * weapon.barrel_data_block_index
						+ (weapon.rounds_per_second_based ? 8 : 32)) = (enable ? weapon.h2x_rate_of_fire : weapon.original_rate_of_fire);
			}
		}
	}

	if (enable && !Memory::IsDedicatedServer() && h2mod->GetEngineType() == e_engine_type::_multiplayer)
	{
		// H2X Sound_Classes
		*(float*)(&tags::get_tag_data()[0x4821C]) = 0.0f; /*H2X projectile_impact Index 0 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48220]) = -4.0f; /*H2X projectile_impact Index 0 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48278]) = 0.0f; /*H2X projectile_detonation Index 1 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x4827C]) = -4.0f; /*H2X projectile_detonation Index 1 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x482D4]) = 0.0f; /*H2X projectile_flyby Index 2 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x482D8]) = -4.0f; /*H2X projectile_flyby Index 2 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x4838C]) = 0.0f; /*H2X weapon_fire Index 4 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48390]) = -4.0f; /*H2X weapon_fire Index 4 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x483E8]) = 0.0f; /*H2X weapon_ready Index 5 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x483EC]) = -4.0f; /*H2X weapon_ready Index 5 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48444]) = 0.0f; /*H2X weapon_reload Index 6 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48448]) = -4.0f; /*H2X weapon_reload Index 6 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x484A0]) = 0.0f; /*H2X weapon_empty Index 7 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x484A4]) = -4.0f; /*H2X weapon_empty Index 7 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x484FC]) = 0.0f; /*H2X weapon_charge Index 8 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48500]) = -4.0f; /*H2X weapon_charge Index 8 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48558]) = 0.0f; /*H2X weapon_overheat Index 9 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x4855C]) = -4.0f; /*H2X weapon_overheat Index 9 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x485B4]) = 0.0f; /*H2X weapon_idle Index 10 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x485B8]) = -4.0f; /*H2X weapon_idle Index 10 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48610]) = 0.0f; /*H2X weapon_melee Index 11 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48614]) = -4.0f; /*H2X weapon_melee Index 11 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x4866C]) = 0.0f; /*H2X weapon_animation Index 12 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48670]) = -4.0f; /*H2X weapon_animation Index 12 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x486C8]) = -64.0f; /*H2X object_impacts Index 13 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x486CC]) = -4.0f; /*H2X object_impacts Index 13 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48724]) = -12.0f; /*H2X particle_impacts Index 14 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48728]) = -4.0f; /*H2X particle_impacts Index 14 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48894]) = -32.0f; /*H2X unit_footsteps Index 18 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48898]) = -9.0f; /*H2X unit_footsteps Index 18 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x4894C]) = 0.0f; /*H2X unit_animation Index 20 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48950]) = -2.0f; /*H2X unit_animation Index 20 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48A04]) = 0.0f; /*H2X vehicle_collision Index 22 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48A08]) = -4.0f; /*H2X vehicle_collision Index 22 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48A60]) = 0.0f; /*H2X vehicle_engine Index 23 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48A64]) = -4.0f; /*H2X vehicle_engine Index 23 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48ABC]) = 0.0f; /*H2X vehicle_animation Index 24 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48AC0]) = -4.0f; /*H2X vehicle_animation Index 24 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48C2C]) = 0.0f; /*H2X device_machinery Index 28 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48C30]) = -4.0f; /*H2X device_machinery Index 28 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48C2C]) = 0.0f; /*H2X device_stationary Index 29 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48C30]) = -4.0f; /*H2X device_stationary Index 29 Gains Bounds upper*/
				  
		*(float*)(&tags::get_tag_data()[0x48E54]) = 0.0f; /*H2X ambient_machinery Index 34 Gains Bounds lower*/
		*(float*)(&tags::get_tag_data()[0x48E58]) = -4.0f; /*H2X ambient_machinery Index 34 Gains Bounds upper*/
	}
}


void H2X::ApplyPatches()
{
	// PatchCall(Memory::GetAddress(0x15C4C3), game_seconds_to_ticks_real_weapon_adjust);
	// PatchCall(Memory::GetAddress(0x15C5B3), game_seconds_to_ticks_real_weapon_adjust);
}
