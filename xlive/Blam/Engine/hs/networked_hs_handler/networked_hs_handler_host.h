#pragma once
#include "networked_hs_handler.h"

typedef void(__cdecl* modify_hs_arguments_table_t)(const s_networked_hs_function* data);

// Sets up the modify hs args function table
void initialize_modify_hs_arguments_table();

// Gets ready to send a script function to all the clients in a match
// Modifies the arguments sent depending on the hs function called
void send_hs_function_to_clients(s_networked_hs_function* function);

// Sends a packet to a client with the hs function data
void send_hs_function_packet(int peerIdx, s_networked_hs_function* data);

// Modifies the arguments sent to the client depending on the script operation being executed
modify_hs_arguments_table_t get_modify_lambda_function(const e_hs_function function_type);
