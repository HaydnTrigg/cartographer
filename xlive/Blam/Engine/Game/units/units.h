#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"

namespace units
{
	void __cdecl unit_delete_all_weapons(datum unit_idx);
	signed int __cdecl inventory_next_weapon(datum unit_idx);
	bool __cdecl assign_equipment_to_unit(datum unit_idx, datum object_idx, short a3);
}
