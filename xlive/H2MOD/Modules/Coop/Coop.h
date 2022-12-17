#pragma once
#include "Blam/Engine/Game/GameOptions.h"

namespace coop
{
	void PreOnMapLoadPatches(s_game_options* game_options);
	void PostMapLoadPatches();
}
