#pragma once
#include "Blam\Engine\Game\math\real_math.h"

struct physics_constants
{
	float gravity;
	float some_scale;
	float dword8;
	real_point3d zero_vector;
	DWORD character_ground_adhesion_forces_enable_time;
	static physics_constants* get();
	static float get_default_gravity();
};
