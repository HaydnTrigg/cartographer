#include "stdafx.h"
#include "devices.h"
#include "Blam/Engine/Objects/Objects.h"

bool device_set_position_animation(const datum device_datum, const string_id animation)
{
	typedef bool (__cdecl *device_set_position_animation_t)(const datum device_datum, const string_id animation);
	auto p_device_set_position_animation = Memory::GetAddress<device_set_position_animation_t>(0x163A2E);

	return p_device_set_position_animation(device_datum, animation);
}

void device_set_power(const datum device_datum, const float power)
{
    typedef void(__cdecl* sub_5632E3_t)(const datum object_datum);
    auto p_sub_5632E3 = Memory::GetAddress<sub_5632E3_t>(0x1632E3);
    typedef void(__cdecl* object_wake_t)(const datum object_datum);
    auto p_object_wake = Memory::GetAddress<object_wake_t>(0x12FA1E);
    typedef bool(__cdecl* device_group_set_t)(const datum device_datum, const datum device_group_datum, const float power);
    auto o_device_group_set = Memory::GetAddress<device_group_set_t>(0x164FE6);

    if (device_datum != -1)
    {
        s_device_data_group* device = (s_device_data_group*)object_get_fast_unsafe(device_datum);
        p_sub_5632E3(device_datum);
        device->power = power;
        p_object_wake(device_datum);
        o_device_group_set(device_datum, device->device_groups_datum, power);
    }
}
