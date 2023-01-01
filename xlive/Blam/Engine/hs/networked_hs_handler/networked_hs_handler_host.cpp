#include "stdafx.h"
#include "networked_hs_handler_host.h"

#include "Blam/Engine/Game/GameGlobals.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"

static modify_hs_arguments_table_t modify_hs_arguments_table[e_hs_function::_e_hs_function_size];

void initialize_modify_hs_arguments_table()
{
	for (unsigned short i = 0; i < e_hs_function::_e_hs_function_size; i++)
	{
		modify_hs_arguments_table[i] = get_modify_lamda_function((e_hs_function)i);
	}
}

void send_hs_function_to_clients(s_networked_hs_function* function)
{
	bool found = false;

	//If host send out the packets
	if (!s_game_globals::game_is_campaign() || !NetworkSession::LocalPeerIsSessionHost()) { return; }

	for (unsigned short i = 0; i < HS_SYNC_TABLE_SIZE; i++)
	{
		if (hs_sync_table[i] == function->function_type)
		{
			found = true;
		}
	}

	if (!found) { return; }

	modify_hs_arguments_table[function->function_type](function);

	for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
	{
		if (NetworkSession::PlayerIsActive(i))
		{
			send_hs_function_packet(NetworkSession::GetPeerIndex(i), function);
		}
	}
	g_next_hs_function_id++;

	memset(function, 0, sizeof(s_networked_hs_function));
}

void send_hs_function_packet(int peerIdx, s_networked_hs_function* data)
{
	s_network_session* session = NetworkSession::GetCurrentNetworkSession();

	s_network_observer* observer = session->p_network_observer;
	s_peer_observer_channel* observer_channel = NetworkSession::GetPeerObserverChannel(peerIdx);

	if (!NetworkSession::LocalPeerIsSessionHost()) { return; }

	if (peerIdx != -1 && !NetworkSession::PeerIndexLocal(peerIdx))
	{
		if (observer_channel->field_1)
		{
			observer->sendNetworkMessage(session->session_index, observer_channel->observer_index, s_network_observer::e_network_message_send_protocol::in_band,
				_hs_function, sizeof(s_networked_hs_function), data);
		}
	}
}

modify_hs_arguments_table_t get_modify_lamda_function(const e_hs_function function_type)
{
	switch (function_type)
	{
	case e_hs_function_device_set_position_track:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_device_set_position_track_args* args = (s_hs_device_set_position_track_args*)function->arg_buffer;
			const s_object_data_definition* device_object = object_get_fast_unsafe(args->device);

			if (args->device == DATUM_INDEX_NONE) { return; }

			args->device = device_object->simulation_entity_index;
		};
	}
	case e_hs_function_device_animate_position:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_device_animate_position_args* args = (s_hs_device_animate_position_args*)function->arg_buffer;
			const s_object_data_definition* device_object = object_get_fast_unsafe(args->device);

			if (args->device == DATUM_INDEX_NONE) { return; }

			args->device = device_object->simulation_entity_index;
		};
	}
	case e_hs_function_object_cinematic_lod:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_object_cinematic_lod_args* args = (s_hs_object_cinematic_lod_args*)function->arg_buffer;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { return; }

			args->object = object->simulation_entity_index;
		};
	}
	case e_hs_function_custom_animation_relative:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_custom_animation_relative_args* args = (s_hs_custom_animation_relative_args*)function->arg_buffer;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);
			const s_object_data_definition* relative_object = object_get_fast_unsafe(args->relative_object);

			if (args->object == DATUM_INDEX_NONE) { return; }
			args->object = object->simulation_entity_index;

			if (args->relative_object == DATUM_INDEX_NONE) { return; }
			args->relative_object = relative_object->simulation_entity_index;
		};
	}
	case e_hs_function_ai_play_line_on_object:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_ai_play_line_on_object_args* args = (s_hs_ai_play_line_on_object_args*)function->arg_buffer;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { return; }

			args->object = object->simulation_entity_index;
		};
	}
	case e_hs_function_custom_animation_loop:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_custom_animation_args* args = (s_hs_custom_animation_args*)function->arg_buffer;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { return; }

			args->object = object->simulation_entity_index;
		};
	}
	case e_hs_function_objects_attach:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_objects_attach_args* args = (s_hs_objects_attach_args*)function->arg_buffer;
			const s_object_data_definition* parent_object = object_get_fast_unsafe(args->parent_object);
			const s_object_data_definition* child_object = object_get_fast_unsafe(args->child_object);

			if (args->parent_object == DATUM_INDEX_NONE || args->child_object == DATUM_INDEX_NONE) { return; }

			args->parent_object = parent_object->simulation_entity_index;
			args->child_object = child_object->simulation_entity_index;
		};
	}
	case e_hs_function_object_hide:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_object_hide_args* args = (s_hs_object_hide_args*)function->arg_buffer;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { return; }

			args->object = object->simulation_entity_index;
		};
	}
	case e_hs_function_object_uses_cinematic_lighting:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_object_uses_cinematic_lighting_args* args = (s_hs_object_uses_cinematic_lighting_args*)function->arg_buffer;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { return; }

			args->object = object->simulation_entity_index;
		};
	}
	case e_hs_function_pvs_set_object:
	{
		return [](const s_networked_hs_function* function)
		{
			datum* object_datum = (datum*)function->arg_buffer;
			const s_object_data_definition* object = object_get_fast_unsafe(*object_datum);

			if (*object_datum == DATUM_INDEX_NONE) { return; }

			*object_datum = object->simulation_entity_index;
		};
	}
	case e_hs_function_device_set_overlay_track:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_device_set_overlay_track_args* args = (s_hs_device_set_overlay_track_args*)function->arg_buffer;
			const s_object_data_definition* device_object = object_get_fast_unsafe(args->device);

			if (args->device == DATUM_INDEX_NONE) { return; }

			args->device = device_object->simulation_entity_index;
		};
	}
	// Send the entity index instead of the object index since object index isnt the same on the client
	case e_hs_function_device_animate_overlay:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_device_animate_overlay_args* args = (s_hs_device_animate_overlay_args*)function->arg_buffer;
			const s_object_data_definition* device_object = object_get_fast_unsafe(args->device);

			if (args->device == DATUM_INDEX_NONE) { return; }

			args->device = device_object->simulation_entity_index;
		};
	}
	case e_hs_function_object_dynamic_simulation_disable:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_object_dynamic_simulation_disable_args* args = (s_hs_object_dynamic_simulation_disable_args*)function->arg_buffer;
			const s_object_data_definition* object = object_get_fast_unsafe(args->object);

			if (args->object == DATUM_INDEX_NONE) { return; }
			args->object = object->simulation_entity_index;
		};
	}
	case e_hs_function_device_set_power:
	case e_hs_function_device_set_position:
	{
		return [](const s_networked_hs_function* function)
		{
			s_hs_device_set_pos_or_power_args* args = (s_hs_device_set_pos_or_power_args*)function->arg_buffer;
			const s_object_data_definition* device_object = object_get_fast_unsafe(args->device);

			if (args->device == DATUM_INDEX_NONE) { return; }
			args->device = device_object->simulation_entity_index;
		};
	}
	default:
	{
		return [](const s_networked_hs_function* data)
		{
		};
	}
	}
}
