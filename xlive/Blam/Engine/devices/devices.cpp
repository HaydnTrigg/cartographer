#include "stdafx.h"
#include "devices.h"
#include "Blam/Enums/HaloStrings.h"
#include "Blam/Cache/TagGroups/device_definition.hpp"
#include "Blam/Cache/TagGroups/model_definition.hpp"
#include "Blam/Cache/TagGroups/render_model_definition.hpp"
#include "Blam/Engine/Game/GameTimeGlobals.h"
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "Blam/Engine/Objects/Objects.h"
#include "H2MOD/Tags/TagInterface.h"

bool device_set_position_animation(const datum device_datum, const string_id animation)
{
    typedef bool (__cdecl *device_set_position_animation_t)(const datum device_datum, const string_id animation);
	auto p_device_set_position_animation = Memory::GetAddress<device_set_position_animation_t>(0x163A2E, 0x1594F1);

	return p_device_set_position_animation(device_datum, animation);
}

void device_set_power(const datum device_datum, const float power)
{
    typedef void(__cdecl* device_set_animation_changed_t)(const datum object_datum);
    auto p_device_set_animation_changed = Memory::GetAddress<device_set_animation_changed_t>(0x1632E3, 0x158DA6);
    typedef bool(__cdecl* device_group_set_t)(const datum device_datum, const datum device_group_datum, const float power);
    auto p_device_group_set = Memory::GetAddress<device_group_set_t>(0x164FE6, 0x15AAA9);

    if (device_datum != -1)
    {
        s_device_data_group* device = (s_device_data_group*)object_get_fast_unsafe(device_datum);
        p_device_set_animation_changed(device_datum);
        device->power = power;
        Engine::Objects::object_wake(device_datum);
        p_device_group_set(device_datum, device->device_groups_datum, power);
    }
}

bool __cdecl device_set_position_track(datum device_datum, const string_id animation_string_id, const float interpolation_time)
{
    typedef double(__thiscall* get_authored_time_t)(void* _this);
    auto c_animation_channel__get_authored_time = Memory::GetAddress<get_authored_time_t>(0x11227A, 0x10379A);
    typedef void(__thiscall* animation_get_root_matrix_t)(void* _this, int a1, float animation_played_length, s_render_model_group_definition* render_model, real_matrix4x3* matrix);
    auto c_animation_manager__animation_get_root_matrix = Memory::GetAddress<animation_get_root_matrix_t>(0xF4364, 0xF5BDE);
    typedef datum(__thiscall* find_node_t)(void* _this, string_id string);
    auto c_animation_manager__find_node = Memory::GetAddress<find_node_t>(0xF427A, 0xF5AF4);
    typedef void(__cdecl* object_start_interpolation_t)(const datum object_datum, const float time);
    auto object_start_interpolation = Memory::GetAddress<object_start_interpolation_t>(0x132D5C, 0x121C2C);
    typedef double(__thiscall* get_authored_duration_t)(void* _this);
    auto c_animation_channel__get_authored_duration = Memory::GetAddress<get_authored_duration_t>(0x112AB4, 0x103FD4);
    typedef bool(__cdecl* object_can_interpolate_t)(const datum a1);
    auto object_can_interpolate = Memory::GetAddress<object_can_interpolate_t>(0x1318B8, 0x120788);
    
    real_matrix4x3 temp_matrix;
    real_vector3d vector;
    float magnitude;
    float inverse_square_rate_of_anim = 0.0f;
    
    real_matrix4x3 animation_matrix_initial;
    real_matrix4x3 matrix_current_animation_time;
    real_matrix4x3 object_matrix_current_animation;
    real_matrix4x3 animation_matrix_from_initial_and_current;
    real_matrix4x3 matrix_initial_inversed;

    real_orientation* node_orientations;
    real_orientation* original_orientations;

    if (device_datum == DATUM_INDEX_NONE) { return false; }
    s_device_data_group* device = object_try_and_get_and_verify_type<s_device_data_group>(device_datum, 
        FLAG(e_object_type::light_fixture) | FLAG(e_object_type::machine) | FLAG(e_object_type::control));
    if (!device) { return false; }

    auto device_definition = tags::get_tag_fast<s_device_group_definition>(device->tag_definition_index);
    if (device_definition->objectTag.model.TagIndex == DATUM_INDEX_NONE) { return false; }

    auto model_definition = tags::get_tag_fast<s_model_group_definition>(device_definition->objectTag.model.TagIndex);
    if (model_definition->render_model.TagIndex == DATUM_INDEX_NONE) { return false; }
    auto render_model_definition = tags::get_tag_fast<s_render_model_group_definition>(model_definition->render_model.TagIndex);

    if (!Engine::Objects::object_has_animation_manager(device_datum)) { return false; }

    void* animation_manager = (char*)device + device->animation_manager_index;
    matrix4x3_from_point_and_vectors(&object_matrix_current_animation, &device->position, &device->orientation, &device->up);
    real_matrix4x3 initial_device_matrix = object_matrix_current_animation;

    bool some_flag_is_true = (device->flags & 8) != 0;
    if (some_flag_is_true)
    {
        const datum animation_tagblock_index = device->position_channel.base_channel_settings.animation_tagblock_index;
        const float authored_time = (float)c_animation_channel__get_authored_time(&device->position_channel);
        c_animation_manager__animation_get_root_matrix(animation_manager, animation_tagblock_index, 0.0f, render_model_definition, &animation_matrix_initial);
        c_animation_manager__animation_get_root_matrix(animation_manager, animation_tagblock_index, authored_time, render_model_definition, &matrix_current_animation_time);
        
        if (datum node = c_animation_manager__find_node(animation_manager, HaloString::HS_ORIGIN_MARKER);
            node == DATUM_INDEX_NONE)
        {
            matrix4x3_inverse(&animation_matrix_initial, &matrix_initial_inversed);
            matrix4x3_multiply(&matrix_current_animation_time, &matrix_initial_inversed, &animation_matrix_from_initial_and_current);
            matrix4x3_multiply(&initial_device_matrix, &animation_matrix_from_initial_and_current, &initial_device_matrix);
        }
        else
        {
            initial_device_matrix = *Engine::Objects::object_get_node_matrix(device_datum, node);
        }
        float tick_length = time_globals::get_seconds_per_tick();
        if (tick_length > 0.0)
        {
            float authored_time_1 = authored_time - tick_length;
            if (authored_time_1 > 0.0f)
            {
                c_animation_manager__animation_get_root_matrix(animation_manager, animation_tagblock_index, authored_time_1, render_model_definition, &temp_matrix);
                vector_from_points3d(&matrix_current_animation_time.position, &temp_matrix.position, &vector);
                magnitude = (float)vector.magnitude_squared3d();
                inverse_square_rate_of_anim = (magnitude > 0.0f ? sqrt(magnitude) / tick_length : 0.0f);        
            }
        }
    }

    device->position_args.current_position = 0.0f;
    device->position_args.start_velocity = 0.0f;
    device->position_args.target_position1 = 0.0f;
    device->position_args.target_position2 = 0.0f;
    device->position_args.initial_velocity = 0.0f;
    device->position_args.current_displacement = 0.0f;
    device->position_args.displacement_to_travel = 0.0f;
    device->device_position = 0.0f;
    device->device_flags = 0;

    if (some_flag_is_true && device->parent_datum == -1)
    {
        real_vector3d forward = initial_device_matrix.vectors.forward;
        real_vector3d up = initial_device_matrix.vectors.up;
        normalize3d(&forward);
        normalize3d(&up);
        if (fabs(object_matrix_current_animation.position.x - initial_device_matrix.position.x) >= 0.000099999997
            || fabs(object_matrix_current_animation.position.y - initial_device_matrix.position.y) >= 0.000099999997
            || fabs(object_matrix_current_animation.position.z - initial_device_matrix.position.z) >= 0.000099999997
            || fabs(object_matrix_current_animation.vectors.forward.i - forward.i) >= 0.000099999997
            || fabs(object_matrix_current_animation.vectors.forward.j - forward.j) >= 0.000099999997
            || fabs(object_matrix_current_animation.vectors.forward.k - forward.k) >= 0.000099999997
            || fabs(object_matrix_current_animation.vectors.up.i - up.i) >= 0.000099999997
            || fabs(object_matrix_current_animation.vectors.up.j - up.j) >= 0.000099999997
            || fabs(object_matrix_current_animation.vectors.up.k - up.k) >= 0.000099999997)
        {   
            
            Engine::Objects::object_set_position_direct(device_datum, &initial_device_matrix.position, &forward, &up, nullptr, true);
            if (object_can_interpolate(device_datum))
            {
                real_orientation orientation[2];
                s_object_data_definition* object = object_get_fast_unsafe(device_datum);
                node_orientations = (real_orientation*)((char*)object + object->node_orientation_offset);
                original_orientations = (real_orientation*)((char*)object + object->original_orientation_offset);
                matrix4x3_to_orientation(&object_matrix_current_animation, orientation);
                orientations_multiply(orientation, node_orientations, node_orientations);
                orientations_multiply(orientation, original_orientations, original_orientations);
                matrix4x3_from_point_and_vectors(&animation_matrix_from_initial_and_current, &device->position, &device->orientation, &device->up);
                matrix4x3_inverse(&animation_matrix_from_initial_and_current, &matrix_initial_inversed);
                matrix4x3_to_orientation(&matrix_initial_inversed, orientation);
                orientations_multiply(orientation, node_orientations, node_orientations);
                orientations_multiply(orientation, original_orientations, original_orientations);
            }
        }
    }

    if (interpolation_time > 0.0 && object_can_interpolate(device_datum))
    {
        object_start_interpolation(device_datum, interpolation_time);
    }

    bool animation_is_set = device_set_position_animation(device_datum, animation_string_id);
    if (animation_is_set)
    {
        if (inverse_square_rate_of_anim > 0.0)
        {
            double authored_duration = c_animation_channel__get_authored_duration(&device->position_channel);
            if (authored_duration > 0.0)
            {
                c_animation_manager__animation_get_root_matrix(
                    animation_manager,
                    device->position_channel.base_channel_settings.animation_tagblock_index,
                    0.0,
                    render_model_definition,
                    &temp_matrix);
                c_animation_manager__animation_get_root_matrix(
                    animation_manager,
                    device->position_channel.base_channel_settings.animation_tagblock_index,
                    (float)authored_duration,
                    render_model_definition,
                    &matrix_current_animation_time);
                vector_from_points3d(&matrix_current_animation_time.position, &temp_matrix.position, &vector);
                magnitude = (float)vector.magnitude_squared3d();

                // Set forward.i to (sqrt(magnitude) * inverse_square_rate_of_anim) if magnitude is greater than 0
                device->position_args.start_velocity = (magnitude > 0.0f ? sqrt(magnitude) * inverse_square_rate_of_anim : device->position_args.start_velocity);
            }
        }
    }
    else
    {
        device_set_position_animation(device_datum, HS_POSITION);
    }
    Engine::Objects::object_wake(device_datum);
    Engine::Objects::object_compute_node_matrices_with_children(device_datum);
    device->flags |= 4u;
    if (animation_is_set)
        device->flags |= 8;
    else
        device->flags &= 0xFFFFFFF7;
    return animation_is_set;
}

void device_animate(s_device_animation_args* control,
    float target_pos,
    float total_time,
    float acceleration_time,
    float deceleration_time,
    float start_velocity,
    float end_velocity)
{
    total_time = (total_time <= 0.0f ? 0.0f : total_time);                                  // Limit range of total_time to 0.0 minimum
    acceleration_time = (acceleration_time <= 0.0f ? 0.0f : acceleration_time);             // Limit range of acceleration_time to 0.0 minimum
    deceleration_time = (deceleration_time <= 0.0f ? 0.0f : deceleration_time);             // Limit range of deceleration_time to 0.0 minimum
    acceleration_time = (acceleration_time > total_time ? total_time : acceleration_time);  // Set acceleration_time to total_time if its greater

    // If total_time - acceleration_time is less than time decelerating set the deceler ation time to total_time - acceleration_time
    deceleration_time = ((total_time - acceleration_time) <= deceleration_time ? total_time - acceleration_time : deceleration_time);

    // Limit range of current_position to 0.0 min and 1.0 max
    const float current_position = (control->current_position < 0.0f ? 0.0f : (control->current_position > 1.0f ? 1.0f : control->current_position));

    // Limit range of target_pos to 0.0 min and 1.0 max
    target_pos = (target_pos < 0.0f ? 0.0f : (target_pos > 1.0f ? 1.0f : target_pos));

    // set default values for device matrix
    control->current_position = target_pos;
    control->target_position1 = target_pos;
    control->target_position2 = target_pos;
    control->current_displacement = 0.0;
    control->displacement_to_travel = 0.0;
    control->start_velocity = 0.0;
    control->initial_velocity = 0.0;
    control->total_time = total_time;

    // Exit if total_time is less than one minus epsilon  
    if (fabs(total_time) < 0.000099999997) { return; }

    long double pos_difference = target_pos - current_position;
    float abs_pos_difference = fabs((float)pos_difference);

    // Exit if position difference is less than epsilon
    if (abs_pos_difference < 0.000001f) { return; }
    
    const float half_of_acceleration_time = acceleration_time * 0.5f;
    const float half_of_deceleration_time = deceleration_time * 0.5f;

    // Gets the time where the device is at full speed and adds it to half of the time accelerating and decelerating
    const float animation_time_within_speed_threshold = (((total_time - acceleration_time) - deceleration_time) + half_of_deceleration_time) + half_of_acceleration_time;

    control->target_position2 = target_pos;
    control->current_position = current_position;
    control->target_position1 = current_position;
    control->current_displacement = 0.0;
    control->displacement_to_travel = abs_pos_difference;
    control->start_velocity = start_velocity;
    if (animation_time_within_speed_threshold == 0.0f)
    {
        control->initial_velocity = (float)pos_difference / total_time;
    }
    else
    {
        control->initial_velocity = (((float)pos_difference - (half_of_acceleration_time * start_velocity)) - (half_of_deceleration_time * end_velocity)) 
            / animation_time_within_speed_threshold;
    }

    if (acceleration_time > 0.0f)
    {
        const float acceleration = (control->initial_velocity - start_velocity) / acceleration_time;
        if (acceleration != 0.0f)
        {
            control->current_displacement = ((control->initial_velocity * control->initial_velocity) - (start_velocity * start_velocity))
                / (acceleration * 2.0f);
        }
    }
    if (deceleration_time > 0.0f)
    {
        const float acceleration = (end_velocity - control->initial_velocity) / deceleration_time;
        if (acceleration != 0.0f)
        {
            control->displacement_to_travel = abs_pos_difference - 
                (((end_velocity * end_velocity) - (control->initial_velocity * control->initial_velocity)) / (acceleration * 2.0f));
        }
    }
}

void device_animate_position(const datum device_datum,
    const float target_pos,
    const float total_time,
    const float acceleration_time,
    const float deceleration_time,
    const bool interpolate)
{
    s_device_data_group* device;

    if (device_datum == DATUM_INDEX_NONE) { return; }   // If device_datum is invalid exit
    
    device = object_try_and_get_and_verify_type<s_device_data_group>(device_datum,
        FLAG(e_object_type::light_fixture) | FLAG(e_object_type::machine) | FLAG(e_object_type::control));

    if (!device) { return; }                            // If device is invalid exit

    if ((device->flags & 8) != 0)
    {
        float start_velocity = (interpolate ? device->position_args.start_velocity : 0.0f);

        device->flags = device->flags & 0xFFFFFFDF;
        device_animate(&device->position_args, target_pos, total_time, acceleration_time, deceleration_time, start_velocity, 0.0);
        Engine::Objects::object_wake(device_datum);
    }
}

void device_animate_overlay(const datum device_datum,
    const float target_pos,
    const float total_time,
    const float acceleration_time,
    const float deceleration_time)
{
    s_device_data_group* device;

    if (device_datum == DATUM_INDEX_NONE) { return; }   // If device_datum is invalid exit

    device = object_try_and_get_and_verify_type<s_device_data_group>(device_datum,
        FLAG(e_object_type::light_fixture) | FLAG(e_object_type::machine) | FLAG(e_object_type::control));

    if (!device) { return; }                            // If device is invalid exit

    if ((device->flags & 16) != 0)
    {
        device->flags &= 0xFFFFFFBF;
        device_animate(&device->overlay_args, target_pos, total_time, acceleration_time, deceleration_time, 0.0, 0.0);
        Engine::Objects::object_wake(device_datum);
    }
}
