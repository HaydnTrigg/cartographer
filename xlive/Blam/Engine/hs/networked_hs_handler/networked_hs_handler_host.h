#pragma once
#include "networked_hs_handler.h"

// Gets ready to send a script function to all the clients in a match
// Modifies the arguments sent depending on the hs function called
void send_hs_function_to_clients(s_networked_hs_function* function);

// Modifies the arguments sent to the client depending on the script operation being executed
void modify_sent_arguments(s_networked_hs_function* function);

// Sends a packet to a client with the hs function data
void send_hs_function_packet(int peerIdx, s_networked_hs_function* data);
