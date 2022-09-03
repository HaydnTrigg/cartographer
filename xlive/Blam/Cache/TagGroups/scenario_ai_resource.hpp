#pragma once
#include "..\Blam\Cache\TagGroups.hpp"
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"

/*********************************************************************
* name: scenario_ai_resource
* group_tag : ai**
* header size : 228
* *********************************************************************/

#pragma pack(push,1)
struct scenario_ai_resource_block
{
    tag_block<s_scenario_group_definition::s_style_palette_block> stylePalette;

    tag_block<s_scenario_group_definition::s_squad_groups_block> squadGroups;

    tag_block<s_scenario_group_definition::s_squads_block> squads;

    tag_block<s_scenario_group_definition::s_zones_block> zones;

    tag_block<s_scenario_group_definition::s_character_palette_block> characterPalette;

    tag_block<s_scenario_group_definition::s_ai_animation_references_block> aIAnimationReferences;

    tag_block<s_scenario_group_definition::s_ai_script_references_block> aIScriptReferences;

    tag_block<s_scenario_group_definition::s_ai_recording_references_block> aIRecordingReferences;

    tag_block<s_scenario_group_definition::s_ai_conversations_block> aIConversations;

    tag_block<s_scenario_group_definition::s_scripting_data_block> scriptingData;

    tag_block<s_scenario_group_definition::s_orders_block> orders;

    tag_block<s_scenario_group_definition::s_triggers_block> triggers;

    tag_block<s_scenario_group_definition::s_weapon_palette_block> bSPPreferences;

    tag_block<s_scenario_group_definition::s_weapons_block> weaponReferences;

    tag_block<s_scenario_group_definition::s_vehicle_palette_block> vehicleReferences;

    tag_block<s_scenario_group_definition::s_vehicles_block> vehicleDatumReferences;

    tag_block<s_scenario_group_definition::s_mission_dialogue_block> missionDialogueScenes;

    tag_block<s_scenario_group_definition::s_flocks_block> flocks;

    tag_block<s_scenario_group_definition::s_kill_trigger_volumes_block> triggerVolumeReferences;
};
#pragma pack(pop)
