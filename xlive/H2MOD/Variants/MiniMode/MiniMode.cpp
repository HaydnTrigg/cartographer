#include "stdafx.h"

#include "MiniMode.h"
#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "H2MOD\Modules\CampaignModifiers\CampaignModifiers.h"
#include "H2MOD\Tags\TagInterface.h"

MiniMode::MiniMode()
{
}

void MiniMode::initClient()
{
}

void MiniMode::Initialize()
{
}

void MiniMode::Dispose()
{
}

CustomVariantId MiniMode::GetVariantId()
{
	return CustomVariantId::_id_mini_mode;
}

void MiniMode::OnMapLoad(ExecTime execTime, s_game_options* gameOptions)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		MultiplayerTinyPlayerTagEdits();
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void MiniMode::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
{
	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		break;

		// postspawn handler
	case ExecTime::_postEventExec:
			CampaignModifiers::TinyPlayerEdits(playerIdx);
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void MiniMode::OnObjectDamage(ExecTime execTime, datum unitDatumIdx, int a2, bool a3, bool a4)
{
}

void MiniMode::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{

}

bool MiniMode::OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx)
{
	return false;
}

bool MiniMode::OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6)
{
	return false;
}

void MultiplayerTinyPlayerTagEdits()
{
	datum chief_mp_bipd_datum = tags::find_tag(blam_tag::tag_group_type::biped, "objects\\characters\\masterchief\\masterchief_mp");
	if (chief_mp_bipd_datum != DATUM_INDEX_NONE)
	{
		CampaignModifiers::ScaleBiped(chief_mp_bipd_datum);
	}

	datum chief_bipd_datum = tags::find_tag(blam_tag::tag_group_type::biped, "objects\\characters\\masterchief\\masterchief");
	if (chief_bipd_datum != DATUM_INDEX_NONE)
	{
		CampaignModifiers::ScaleBiped(chief_bipd_datum);
	}

	datum elite_mp_bipd_datum = tags::find_tag(blam_tag::tag_group_type::biped, "objects\\characters\\elite\\elite_mp");
	if (elite_mp_bipd_datum != DATUM_INDEX_NONE)
	{
		CampaignModifiers::ScaleBiped(elite_mp_bipd_datum);
	}

	datum elite_bipd_datum = tags::find_tag(blam_tag::tag_group_type::biped, "objects\\characters\\elite\\elite");
	if (elite_bipd_datum != DATUM_INDEX_NONE)
	{
		CampaignModifiers::ScaleBiped(elite_bipd_datum);
	}
}