#pragma once
#include "networked_hs_handler.h"

extern std::list<s_networked_hs_function> g_hs_client_function_backlog;

// Stores recieved hs functions in a list to be executed later on hs_update
void store_hs_functions(const s_networked_hs_function* data);

/* Calls the hs function based on the s_networked_hs_function provided
Only executes something if we manually add the function to the hs_sync_table and add logic to handle it here*/
void call_networked_hs_function(const s_networked_hs_function* data);

// Loops through the stored functions and if one matches the next script id to be executred call it
void client_execute_stored_hs_commands();
