#include "stdafx.h"
#include "main.h"

bool* get_main_exit_game()
{
	return Memory::GetAddress<bool*>(0x48220B, 0x4A7082);
}

void main_exit_game()
{
	*get_main_exit_game() = true;
}
