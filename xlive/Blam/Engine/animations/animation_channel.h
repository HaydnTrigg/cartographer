#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"

struct c_animation_channel
{
	__int16 animation_absolute_datum;
	__int16 unk;
	__int16 animation_tagblock_index;
	__int16 field6;
	string_id animation_string;
	DWORD fieldC;
	DWORD some_flags;
	DWORD field14;
	float speed;
	float animation_ticks_remaining;
};
CHECK_STRUCT_SIZE(c_animation_channel, 32);