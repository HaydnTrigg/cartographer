#include "stdafx.h"

#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Cache\TagGroups\user_interface_screen_widget_definition.hpp"
//#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\TagInterface.h"
#include "H2MOD\Variants\XboxTick\XboxTick.h"

#include "CampaignModifiers.h"

namespace CampaignModifiers 
{
	static e_campaign_modifiers campaign_modifiers = campaign_modifier_none;

	void MainMenuPatches()
	{
		auto load_level_wigt_datum = tags::find_tag(blam_tag::tag_group_type::userinterfacescreenwidgetdefinition, "ui\\screens\\game_shell\\load_level\\load_level");
		if (load_level_wigt_datum != DATUM_INDEX_NONE)
		{
			auto load_level_wigt = tags::get_tag_fast<s_user_interface_screen_widget_definition>(load_level_wigt_datum);
			load_level_wigt->button_key_type = s_user_interface_screen_widget_definition::e_button_key_type::x_settings_a_select_b_back;
		}
	}

	void c_campaign_modifiers::ApplyEngineModifiers(s_game_options* options)
	{
		if (campaign_modifiers & campaign_modifier_ogh2)
		{
			options->tickrate = XboxTick::setTickRate(true);
		}
	}

	void c_campaign_modifiers::ApplyMapModifiers()
	{
		if (campaign_modifiers & campaign_modifier_randomizer)
		{

		}

		if (campaign_modifiers & campaign_modifier_jackal_snipers)
		{
			JackalSniperEdits();
		}
	}

	void c_campaign_modifiers::SetModifier(e_campaign_modifiers modifier)
	{
		switch (modifier)
		{
		case CampaignModifiers::campaign_modifier_ogh2:
			campaign_modifiers = campaign_modifier_ogh2;
			break;
		case CampaignModifiers::campaign_modifier_randomizer:
			campaign_modifiers = campaign_modifier_randomizer;
			break;
		case CampaignModifiers::campaign_modifier_jackal_snipers:
			campaign_modifiers = campaign_modifier_jackal_snipers;
			break;
		default:
			break;
		}

	}

	void CampaignModifiers::JackalSniperEdits()
	{
		__int8 i = 0;
		__int8 jackal_character_index = 0;
		__int8 beamrifle_weapon_index = 0;

		auto scnr_datum = tags::find_tags(blam_tag::tag_group_type::scenario);
		{
			for (auto scnr_item : scnr_datum)
			{
				auto scnr = tags::get_tag<blam_tag::tag_group_type::scenario, s_scenario_group_definition>(scnr_item.first);

				for (i = 0; i < scnr->character_palette.size; i++)
				{
					if (tags::get_tag_name(scnr->character_palette[i]->reference.TagIndex) == "objects\\characters\\jackal\\ai\\jackal")
					{
						jackal_character_index = i;
						break;
					}
				}

				for (i = 0; i < scnr->weapon_palette.size; i++)
				{
					if (tags::get_tag_name(scnr->weapon_palette[i]->name.TagIndex) == "objects\\weapons\\rifle\\beam_rifle\\beam_rifle")
					{
						beamrifle_weapon_index = i;
						break;
					}
				}

				for (auto squad : scnr->squads)
				{
					squad.character_type = jackal_character_index;
					squad.initial_weapon = beamrifle_weapon_index;
					squad.initial_secondary_weapon = beamrifle_weapon_index;
				}
			}
		}
	}

	void CampaignModifiers::Initialize()
	{
		//EventHandler::register_callback(c_campaign_modifiers::ApplyMapModifiers, EventType::gamelifecycle_change, EventExecutionType::execute_after);
	}
}
