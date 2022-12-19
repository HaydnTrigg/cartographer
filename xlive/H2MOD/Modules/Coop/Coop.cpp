#include "stdafx.h"
#include "Coop.h"

#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"
namespace coop
{
	void PreOnMapLoadPatches(s_game_options* game_options)
	{
		if (game_options->m_engine_type == _multiplayer && tags::get_cache_header()->type == s_cache_header::e_scnr_type::SinglePlayerScenario)
		{
			game_options->m_engine_type = _single_player;
			game_options->coop = 1;
		}
	}

	void PostMapLoadPatches()
	{
		if (!Memory::IsDedicatedServer())
		{
			WriteValue<BYTE>(Memory::GetAddress(0x23EC55 + 1), 0);	// Prevent the game from pausing during the game
		}
	}
}
