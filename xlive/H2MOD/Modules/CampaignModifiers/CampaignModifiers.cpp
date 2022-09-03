#include "stdafx.h"

#include "H2MOD.h"
#include "Blam\Cache\DataTypes\BlamTag.h"
#include "Blam\Cache\DataTypes\StringID.h"
#include "Blam\Cache\TagGroups\character_definition.hpp"
#include "Blam\Cache\TagGroups\model_definition.hpp"
#include "Blam\Cache\TagGroups\projectile_definition.hpp"
#include "Blam\Cache\TagGroups\render_model_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_ai_resource.hpp"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Cache\TagGroups\user_interface_screen_widget_definition.hpp"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Enums\HaloStrings.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\TagInterface.h"
#include "H2MOD\Variants\XboxTick\XboxTick.h"
#include "Util\Hooks\Hook.h"

#include "CampaignModifiers.h"

namespace CampaignModifiers 
{
	std::random_device rng;
	std::mt19937 urng(rng());
	static e_campaign_modifiers campaign_modifiers = campaign_modifier_big_head;

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
		if (h2mod->GetEngineType() == e_engine_type::_single_player) 
		{
			if (campaign_modifiers & campaign_modifier_randomizer)
			{
				RandomizerEdits();
			}

			if (campaign_modifiers & campaign_modifier_jackal_snipers)
			{
				JackalSniperEdits();
			}

			if (campaign_modifiers & campaign_modifier_big_head)
			{
				BigHeadEdits();
			}
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
		__int8 jackal_character_index = 0;
		__int8 beamrifle_weapon_index = 0;
		short i = 0;
		short j = 0;
		const auto jackal = tags::get_tag_fast<s_model_group_definition>
			(tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\jackal\\jackal"));

		auto scnr_datum = tags::find_tags(blam_tag::tag_group_type::scenario);
		for (auto& scnr_item : scnr_datum)
		{
			auto scnr = tags::get_tag_fast<s_scenario_group_definition>(scnr_item.first);

			for (i = 0; i < scnr->character_palette.size; i++)
			{
				if (tags::get_tag_name(scnr->character_palette[i]->reference.TagIndex) == "objects\\characters\\jackal\\ai\\jackal_sniper")
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

			for (i = 0; i < scnr->squads.size; i++)
			{
				scnr->squads[i]->major_upgrade = s_scenario_group_definition::s_squads_block::e_major_upgrade::none;
				scnr->squads[i]->character_type = jackal_character_index;
				scnr->squads[i]->initial_weapon = beamrifle_weapon_index;
				scnr->squads[i]->initial_secondary_weapon = beamrifle_weapon_index;

				for (j = 0; j < scnr->squads[i]->starting_locations.size; j++)
				{
					scnr->squads[i]->starting_locations[j]->character_type = -1;
					scnr->squads[i]->starting_locations[j]->initial_weapon = -1;
					scnr->squads[i]->starting_locations[j]->initial_secondary_weapon = -1;
					scnr->squads[i]->starting_locations[j]->actor_variant_name = NULL;
				}
			}
		}
	}

	template <typename T>
	void randomize_placement(T& type, int size)
	{
		for (auto& placement : type)
		{
			placement.type = (rand() % (size + 1));
		}
	}

	void CampaignModifiers::RandomizerEdits()
	{
		auto scnr_datum = tags::find_tags(blam_tag::tag_group_type::scenario);
		auto weap_datum = tags::find_tags(blam_tag::tag_group_type::weapon);
		auto proj_datum = tags::find_tags(blam_tag::tag_group_type::projectile);
		std::vector<datum> projectile_datums(0);

		for (auto& scnr_item : scnr_datum)
		{
			auto scnr = tags::get_tag_fast<s_scenario_group_definition>(scnr_item.first);

			randomize_placement(scnr->bipeds, scnr->biped_palette.size);
			randomize_placement(scnr->weapons, scnr->weapon_palette.size);
			randomize_placement(scnr->equipment, scnr->equipment_palette.size);
			randomize_placement(scnr->vehicles, scnr->vehicle_palette.size);
			randomize_placement(scnr->crates, scnr->crates_palette.size);
			//randomize_placement(scnr->scenery, scnr->scenery_palette.size);

			for (auto& squad : scnr->squads)
			{
				squad.character_type = (rand() % (scnr->character_palette.size + 1));
				squad.initial_weapon = (rand() % (scnr->weapon_palette.size + 1));
				squad.initial_secondary_weapon = (rand() % (scnr->weapon_palette.size + 1));
				for (auto& placement : squad.starting_locations)
				{
					placement.character_type = -1;
					placement.initial_weapon = -1;
					placement.initial_secondary_weapon = -1;
				}
			}
		}

		for (auto& proj_item : proj_datum)
		{
			projectile_datums.push_back(proj_item.first);
		}

		for (auto& weap_item : weap_datum)
		{
			auto weap = tags::get_tag_fast<s_weapon_group_definition>(weap_item.first);

			for (auto& barrels : weap->barrels)
			{
				barrels.projectile.TagIndex = projectile_datums[(rand() % (projectile_datums.size()))];
			}
		}
	}

	void CampaignModifiers::BigHeadEdits()
	{
		const __int8 scale_multiplier = 2;

		auto mode_datum = tags::find_tags(blam_tag::tag_group_type::rendermodel);
		for (auto& mode_item : mode_datum)
		{
			auto mode = tags::get_tag_fast<s_render_model_group_definition>(mode_item.first);
			for (__int8 i = 0; i < mode->nodes.size; i++)
			{
				if (mode->nodes[i]->name_old_string_id == HaloString::HS_HEAD)
				{
					mode->nodes[i]->default_scale *= scale_multiplier;
					mode->nodes[i]->inverse_position_x *= scale_multiplier;
					mode->nodes[i]->inverse_position_y *= scale_multiplier;
					mode->nodes[i]->inverse_position_z *= scale_multiplier;
				}
			}
		}
	}

	void CampaignModifiers::Initialize()
	{
		tags::on_map_load(c_campaign_modifiers::ApplyMapModifiers);
	}
}
