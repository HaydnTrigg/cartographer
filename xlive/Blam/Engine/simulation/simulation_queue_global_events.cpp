#include "stdafx.h"
#include "simulation_queue_global_events.h"

#include "simulation.h"
#include "simulation_encoding.h"
#include "game_interface/simulation_game_action.h"

#include "game/game.h"
#include "game/game_engine.h"
#include "memory/bitstream.h"

static void simulation_queue_global_event_allocate_and_insert(e_event_queue_type type, void* data, int32 data_size)
{
    s_simulation_queue_element* element = NULL;
    c_simulation_world* world = simulation_get_world();
    world->simulation_queue_allocate(type, data_size, &element);
    if (element)
    {
        csmemcpy((void*)element->data, data, data_size);
        world->simulation_queue_enqueue(element);
    }
}

void simulation_queue_game_global_event_insert(e_simulation_queue_global_event_type global_event_type)
{
    if (!game_is_playback())
    {
        uint8 data[128];
        c_bitstream stream(data, sizeof(data));
        stream.begin_writing(1);
        stream.write_integer("global-event-type", global_event_type, 3);
        if (!stream.error_occured())
        {
            simulation_queue_global_event_allocate_and_insert(_simulation_queue_element_type_game_global_event, data, stream.get_space_used_in_bytes());
        }
        stream.finish_writing(NULL);
    }
    return;
}

void simulation_queue_game_global_event_apply(const s_simulation_queue_element* element)
{
    c_bitstream stream(element->data, element->data_size);
    stream.begin_reading();
    e_simulation_queue_global_event_type type = (e_simulation_queue_global_event_type)stream.read_integer("global-event-type", 3);
    if (!stream.error_occured())
    {
        switch (type)
        {
        case _simulation_queue_game_global_event_type_claim_authority:
            break;
        case _simulation_queue_game_global_event_type_set_simulation_to_distributed_server:
            break;
        case _simulation_queue_game_global_event_type_set_simulation_to_distributed_client:
            break;
        case _simulation_queue_game_global_event_type_game_won:
            break;
        case _simulation_queue_game_global_event_main_revert_map:
            break;
        case _simulation_queue_game_global_event_main_reset_map:
            break;
        case _simulation_queue_game_global_event_main_save_and_exit_campaign:
            break;
        case _simulation_queue_game_global_event_notify_reset_complete:
            break;
        default:
            break;
        }
    }
    stream.finish_reading();
}

void simulation_queue_player_event_insert(e_simulation_queue_player_event_type event_type, datum player_index, const s_simulation_queue_player_event_data* event_data)
{
    if (!game_is_playback())
    {
        uint8 data[128];
        c_bitstream stream(data, sizeof(data));

        uint16 abs_player_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index);

        stream.begin_writing(1);
        stream.write_integer("player-index", abs_player_index, k_player_index_bit_count);
        stream.write_bool("active", event_data->active);
        if (!stream.error_occured())
        {
            simulation_queue_global_event_allocate_and_insert(_simulation_queue_element_type_player_event, data, stream.get_space_used_in_bytes());
        }
        stream.finish_writing(NULL);
    }
    return;
}

void simulation_queue_player_event_apply(const s_simulation_queue_element* element)
{
	c_bitstream stream(element->data, element->data_size);
	stream.begin_reading();

	uint16 abs_player_index = stream.read_integer("player-index", k_player_index_bit_count);
	bool active = stream.read_bool("active");

	if (!stream.error_occured())
	{
		datum player_index = player_index_from_absolute_player_index(abs_player_index);
		s_player* player = (s_player*)datum_get(s_player::get_data(), player_index);
		if (TEST_BIT(player->flags, 0) != active)
		{
			SET_FLAG(player->flags, 0, active);
			if (active)
			{
				game_engine_player_activated(player_index);
			}
		}
		simulation_action_game_engine_player_update(player_index, 0x200);
	}

    stream.finish_reading();
	return;
}

void simulation_queue_player_update_insert(const simulation_player_update* player_update)
{
    if (!game_is_playback())
    {
        uint8 data[k_simulation_payload_size_max];
        c_bitstream stream(data, sizeof(data));
        stream.begin_writing(1);
        simulation_player_update_encode(&stream, player_update);
        if (!stream.error_occured())
        {
            simulation_queue_global_event_allocate_and_insert(_simulation_queue_element_type_player_update_event, data, stream.get_space_used_in_bytes());
        }
        stream.finish_writing(NULL);
    }
    return;
}

void simulation_queue_player_update_apply(const s_simulation_queue_element* element)
{
    simulation_player_update update;
    csmemset(&update, 0, sizeof(simulation_player_update));

    c_bitstream stream(element->data, element->data_size);
    stream.begin_reading();
    simulation_player_update_decode(&stream, &update);

    if (stream.error_occured())
    {
        // ASSERT HERE
    }
    else if (!simulation_players_apply_update(&update))
    {
        simulation_get_globals()->fatal_error = true;
    }

    stream.finish_reading();
    return;
}