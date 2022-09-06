#pragma once
#include "Blam\Engine\Game\GameOptions.h"

#define campaign_modifier_none              0
#define campaign_modifier_ogh2				1
#define campaign_modifier_randomizer        2
#define campaign_modifier_jackal_snipers    4
#define campaign_modifier_big_head			8
#define campaign_modifier_tiny_chief		16

namespace CampaignModifiers
{
	class c_campaign_modifiers
	{
	public:
		static void ApplyEngineModifiers(s_game_options* options);
		static void ApplyMapModifiers();
		static void SetModifier(const __int8 modifier);
		static __int8 GetModifiers();
	};

	void AddOrRemoveModifier(const __int8 modifier);
	void MainMenuPatches();
	void JackalSniperEdits();
	void RandomizerEdits();
	void BigHeadEdits();
	void TinyPlayerEdits(datum playerDatumIdx);
	void ScaleBiped(const datum bipd_datum, const float scale_multiplier);
	void ScaleCollision(const datum coll_datum, const float scale_multiplier);
	void Initialize();
}