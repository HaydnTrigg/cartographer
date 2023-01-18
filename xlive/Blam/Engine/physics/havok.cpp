#include "stdafx.h"
#include "havok.h"

void havok_object_set_position(const datum object_index, const bool bool0, const bool bool1)
{
	typedef void(__cdecl* havok_object_set_position_t)(const datum object_index, const bool bool0, const bool bool1);
	auto havok_object_set_position = Memory::GetAddress<havok_object_set_position_t>(0x9FF6A, 0x921CA);

	havok_object_set_position(object_index, bool0, bool1);
}
