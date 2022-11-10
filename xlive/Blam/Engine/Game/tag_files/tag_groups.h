#pragma once
#include "Blam/Cache/DataTypes/BlamTag.h"

#pragma pack(push, 1)
struct s_tag_data_iterator
{
	DWORD dword0;
	DWORD dword4;
	datum current_tag_datum;
	datum next_tag_datum;
	blam_tag tag_type;
	datum get_next_datum();
};
static_assert(sizeof(s_tag_data_iterator) == 20, "Size incorrect");
#pragma pack(pop)
