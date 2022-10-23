#include "stdafx.h"

#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "Blam\Cache\TagGroups\new_hud_definition.hpp"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "Util\Hooks\Hook.h"

#include "ui_redraw.h"

#define RES_INCREASE 4

namespace UI_Redraw
{
	const short resolution_increase = 4;

	void __cdecl update_hud_elements_display_settings(int new_hud_size, int new_safe_area)
	{
		const rect2d* rasterizer_globals_screen_bounds = Memory::GetAddress<rect2d*>(0xA3E410);
		float *some_hud_elements_scale = Memory::GetAddress<float*>(0x464028);
		float *g_hud_scale = Memory::GetAddress<float*>(0x46402C);
		float *g_safe_area = Memory::GetAddress<float*>(0x9770F0);

		typedef DWORD(__cdecl* get_safe_area_t)();
		auto p_get_safe_area = Memory::GetAddress<get_safe_area_t>(0x263BDB);
		typedef DWORD(__cdecl* get_hud_size_t)();
		auto p_get_hud_size = Memory::GetAddress<get_hud_size_t>(0x263BD5);

		int screen_bounds; // esi
		DWORD hud_size; // eax
		float safe_area; // xmm0_4
		float hud_scale; // xmm0_4
		DWORD v7; // eax

		screen_bounds = rasterizer_globals_screen_bounds->bottom - rasterizer_globals_screen_bounds->top;
		if (new_safe_area == -1)
			new_safe_area = p_get_safe_area();
		hud_size = new_hud_size;
		if (new_hud_size == -1)
			hud_size = p_get_hud_size();
		if (new_safe_area)
			safe_area = screen_bounds * 0.050000001;
		else
			safe_area = 0.0;
		*g_safe_area = safe_area;
		hud_scale = screen_bounds * 0.0010416667;
		*some_hud_elements_scale = hud_scale;
		if (!hud_size)
			goto LABEL_11;
		v7 = hud_size - 1;
		if (v7)
		{
			if (v7 == 1)
			{
			LABEL_12:
				*g_hud_scale = hud_scale / 4;
				return;
			}
		LABEL_11:
			hud_scale *= 0.80000001;
			goto LABEL_12;
		}
		*g_hud_scale = (hud_scale * 0.89999998) / 4;
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

	void FixScaling() 
	{
		const float og_res = *Memory::GetAddress<float*>(0x3E6A9C);

		PatchCall(Memory::GetAddress(0x25E1FC), update_hud_elements_display_settings);
		PatchCall(Memory::GetAddress(0x264058), update_hud_elements_display_settings);
		PatchCall(Memory::GetAddress(0x26406F), update_hud_elements_display_settings);

		// Update hud so at first launch so everything is proper on startup
		update_hud_elements_display_settings(-1, -1);

		WriteValue<float_t>(Memory::GetAddress(0x3E6A9C), og_res * resolution_increase);
	}
}
