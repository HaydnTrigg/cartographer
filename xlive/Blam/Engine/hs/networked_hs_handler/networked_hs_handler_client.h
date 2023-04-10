#pragma once
#include "networked_hs_handler.h"
typedef void(__cdecl* networked_hs_functions_table_t)(const s_networked_hs_function* data);

extern std::list<s_networked_hs_function> g_hs_client_function_backlog;

// Sets up the networked hs function table
void initialize_networked_hs_function_table();

// Stores recieved hs functions in a list to be executed later on hs_update
void store_hs_functions(const s_networked_hs_function* data);

/* Calls the hs function based on the s_networked_hs_function provided
Only executes something if we manually add the function to the hs_sync_table and add logic to handle it here*/
void call_networked_hs_function(const s_networked_hs_function* data);

// Loops through the stored functions and if one matches the next script id to be executred call it
void client_execute_stored_hs_commands();

// Depending on the function type passed return a lambda function that's used to handle that hs function on the client
networked_hs_functions_table_t get_networked_hs_lambda_function(const e_hs_function function_type);
