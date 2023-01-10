#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"

// TODO
// More Research on these classes for h2v specifically
// Some things may be incorrectly labeled

#pragma pack(push, 1)
struct c_animation_id
{
	WORD subgraph;
	WORD index;
};
CHECK_STRUCT_SIZE(c_animation_id, 4);

struct c_base_channel_settings
{
	datum animation_absolute_datum;
	datum animation_tagblock_index;
	c_animation_id animation_id;
	unsigned short animation_playback_flags;
	char animation_graph_location;
};
CHECK_STRUCT_SIZE(c_base_channel_settings, 15);

struct c_channel_base
{
	c_base_channel_settings base_channel_settings;
	byte parent_slider_index;
	//string_id animation_string;
	char mixing_board_parent_slider_type;
	byte unk;
	short pad;
	float field14;
	float speed;
	float animation_ticks_remaining;
};
CHECK_STRUCT_SIZE(c_channel_base, 32);

struct c_animation_channel
{
	c_channel_base base;
	c_animation_id animation_ids[2];
	short animation_id_related[2];
	short animation_frame_event_types1;
	short animation_frame_event_types2;
	short animation_frame_event_types3;
	byte animation_state_flag_bits;
	char pad;
};
CHECK_STRUCT_SIZE(c_animation_channel, 52);
#pragma pack(pop)
