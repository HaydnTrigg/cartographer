#include "stdafx.h"
#include "networked_hs_handler.h"
#include "../hs.h"

long g_next_hs_function_id = 0;		// The next id for a hs function we want to execute

s_networked_hs_function populate_networked_hs_function(const void* args_src, const e_hs_function function_type)
{
	s_networked_hs_function args_dest;

	args_dest.function_type = function_type;
	memset(args_dest.arg_buffer, 0, HS_SENT_BUFFER_SIZE);

	// Sending text over a packet requires us to copy the memory differently compared to how we normally copy the arg buffer
	if (args_src != nullptr)
	{
		if (function_type == e_hs_function::e_hs_function_print)
		{
			const char* text = *(const char**)args_src;
			memcpy(args_dest.arg_buffer, text, HS_SENT_BUFFER_SIZE);
		}
		else
		{
			memcpy(args_dest.arg_buffer, args_src, HS_SENT_BUFFER_SIZE);
		}
	}

	return args_dest;
}
