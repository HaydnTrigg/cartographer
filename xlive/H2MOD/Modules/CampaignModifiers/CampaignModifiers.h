#pragma once
#include "Blam\Engine\Game\GameOptions.h"

namespace CampaignModifiers
{
	enum e_campaign_modifiers : __int8 {
		campaign_modifier_none = 0 << 0	,
		campaign_modifier_ogh2 = 1 << 1,
		campaign_modifier_randomizer = 1 << 2,
		campaign_modifier_jackal_snipers = 1 << 3
	};

	static class c_campaign_modifiers
	{
	public:
		static void ApplyEngineModifiers(s_game_options* options);
		static void ApplyMapModifiers();
		static void c_campaign_modifiers::SetModifier(e_campaign_modifiers modifier);
	};

	void MainMenuPatches();
	void JackalSniperEdits();
	void Initialize();
}