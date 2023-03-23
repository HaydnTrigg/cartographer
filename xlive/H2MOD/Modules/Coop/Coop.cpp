#include "stdafx.h"
#include "Coop.h"

#include "H2MOD.h"
#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"
namespace coop
{
	void PreOnMapLoadPatches(s_game_options* game_options)
	{
		//See map_slots.cpp the range of valid singleplayer maps is hard defined.
		if (game_options->engine_type == _multiplayer && game_options->map_id >= INT32_MAX - 1000 && game_options->map_id <= INT32_MAX)
		{
			game_options->engine_type = _single_player;
			game_options->coop = 1;
			game_options->game_variant.variant_game_engine_index = _game_engine_campaign;
		}
	}

	void PostMapLoadPatches()
	{
		if (!Memory::IsDedicatedServer() && h2mod->GetEngineType() == _single_player)
		{
			WriteValue<BYTE>(Memory::GetAddress(0x23EC55 + 1), 0);	// Prevent the game from pausing during the game
		}
	}
}
