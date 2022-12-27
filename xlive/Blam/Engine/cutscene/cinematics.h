#pragma once
#include "Blam/Cache/DataTypes/StringID.h"

struct s_cinematic_globals_flags
{
	bool b_show_letterbox;
	bool b_cinematic_in_progress;
	bool b_cinematic_skip_start;
	bool b_suppress_bsp_object_creation;
};

struct s_cutscene_title
{
	unsigned short cutscene_title;
	__int16 cutscene_title_time;
};


struct s_cinematic_globals_cutscene_flags
{
	bool unk1;
	bool unk2;
	bool b_is_outro_cutscene;
	bool unk4;
};

struct s_cinematic_globals
{
	float letterbox_scale;
	s_cinematic_globals_flags flags;
	s_cutscene_title cutscene_title[4];
	string_id cinematic_title_string_id;
	float cinematic_title_time;
	s_cinematic_globals_cutscene_flags cutscene_flags;

	static s_cinematic_globals* get();
	static void cinematic_skip_start_internal();
	static void cinematic_skip_stop_internal();
};

