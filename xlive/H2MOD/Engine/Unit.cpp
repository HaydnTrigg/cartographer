#include "stdafx.h"

#include "Engine.h"

#include "Blam\Enums\HaloStrings.h"
#include "Util\Hooks\Hook.h"
namespace Engine
{
	namespace Unit
	{
		void __cdecl remove_equipment(datum unit_idx)
		{
			//LOG_TRACE_GAME("unit_reset_equipment(unit_datum_index: %08X)", unit_datum_index);
			typedef void(__cdecl* unit_reset_equipment_t)(datum unit_idx);
			auto p_unit_reset_equipment = Memory::GetAddress<unit_reset_equipment_t>(0x1441E0, 0x133030);
			if (!DATUM_IS_NONE(unit_idx))
			{
				p_unit_reset_equipment(unit_idx);
			}
		}

		signed int __cdecl inventory_next_weapon(datum unit_idx)
		{
			//LOG_TRACE_GAME("unit_inventory_next_weapon(unit: %08X)", unit);
			typedef signed int(__cdecl* unit_inventory_next_weapon_t)(unsigned short unit);
			auto p_unit_inventory_next_weapon = Memory::GetAddress<unit_inventory_next_weapon_t>(0x139E04, 0x0);

			return p_unit_inventory_next_weapon(unit_idx);
		}

		bool __cdecl assign_equipment_to_unit(datum unit_idx, datum object_idx, short a3)
		{
			//LOG_TRACE_GAME("assign_equipment_to_unit(unit: %08X,object_index: %08X,unk: %08X)", unit, object_index, unk);
			typedef bool(__cdecl* assign_equipment_to_unit_t)(datum unit_idx, int object_idx, short unk);
			auto passign_equipment_to_unit = Memory::GetAddress<assign_equipment_to_unit_t>(0x1442AA, 0x1330FA);

			return passign_equipment_to_unit(unit_idx, object_idx, a3);
		}

		void __fastcall unit_get_head_position(int a1, real_point3d* a2)
		{
			typedef signed __int16(__cdecl object_get_marker_by_string_id_t)(int a1, string_id a2, matrix2x4x3* a3, __int16 a4);
			auto p_object_get_marker_by_string_id = Memory::GetAddress<object_get_marker_by_string_id_t*>(0x13277C);

			struct matrix2x4x3 a3;

			p_object_get_marker_by_string_id(a1, HS_HEAD, &a3, 1);
			memcpy(a2, &a3.matrix1.translation, sizeof(real_point3d));
		}
	}
}