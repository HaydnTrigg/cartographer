#include "stdafx.h"
#include "cinematics.h"

s_cinematic_globals* s_cinematic_globals::get()
{
	return *Memory::GetAddress<s_cinematic_globals**>(0x48227C, 0x4F3DB0);
}
void s_cinematic_globals::cinematic_skip_start_internal()
{
	get()->flags.b_cinematic_skip_start = true;
}

void s_cinematic_globals::cinematic_skip_stop_internal()
{
	get()->flags.b_cinematic_skip_start = false;
}
