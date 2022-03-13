#include "ui_redraw.h"

#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam\Cache\TagGroups\new_hud_definition.hpp"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "Util\Hooks\Hook.h"

#define _USE_MATH_DEFINES

namespace UI_Redraw
{
	const short resolution_increase = 4;

	void CreateRedrawnHUD() {
		InjectRedrawnUI();
		FixScaling();
	}

	void InjectRedrawnUI() {
		auto new_hud_redraw_datum = tag_loader::Get_tag_datum("ui\\hud\\masterchief_redraw", blam_tag::tag_group_type::newhuddefinition, "ui_redraw");

		auto masterchief_mp_datum = tags::find_tag(blam_tag::tag_group_type::biped, "objects\\characters\\masterchief\\masterchief_mp");
		auto masterchief = tags::get_tag<blam_tag::tag_group_type::biped, s_biped_group_definition>(masterchief_mp_datum);

		if (!DATUM_IS_NONE(new_hud_redraw_datum))
		{
			tag_loader::Load_tag(new_hud_redraw_datum, true, "ui_redraw");
			tag_loader::Push_Back();

			masterchief->unitTag.new_hud_interfaces[1]->new_unit_hud_interface.TagIndex = tag_loader::ResolveNewDatum(new_hud_redraw_datum);
		}
	}

	void FixScaling() {
		float og_scale = *Memory::GetAddress<float*>(0x46402C);
		float og_res = *Memory::GetAddress<float*>(0x3E6A9C);
		WriteValue<float_t>(Memory::GetAddress(0x46402C), og_scale / resolution_increase);
		WriteValue<float_t>(Memory::GetAddress(0x3E6A9C), og_res * resolution_increase);
	}
}
