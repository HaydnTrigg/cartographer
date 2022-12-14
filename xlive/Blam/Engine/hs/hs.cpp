#include "stdafx.h"
#include "hs.h"

#include "Blam/Engine/Players/Players.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "Util/Hooks/Hook.h"

namespace hs
{
	typedef void*(__cdecl hs_arguments_evaluate_t)(__int16 op_code, unsigned __int16 thread_id, bool unk_bool);
	hs_arguments_evaluate_t* p_hs_arguments_evaluate;
	typedef char*(__cdecl hs_return_t)(int a1, int a2);
	hs_return_t* p_hs_return;

	typedef int(__cdecl unit_kill_t)(datum unitDatum);
	unit_kill_t* p_unit_kill;
	void UnitKill(datum unitDatum)
	{
		p_unit_kill(unitDatum);
	}
	
	typedef bool(__cdecl unit_in_vehicle_t)(datum unitDatum);
	unit_in_vehicle_t* p_unit_in_vehicle;
	bool UnitInVehicle(datum unitDatum)
	{
		return p_unit_in_vehicle(unitDatum);
	}

	typedef float(__cdecl unit_get_health_t)(datum unitDatum);
	unit_get_health_t* p_unit_get_health;
	float UnitGetHealth(datum unitDatum)
	{
		return p_unit_get_health(unitDatum);
	}

	typedef float(__cdecl unit_get_shield_t)(datum unitDatum);
	unit_get_shield_t* p_unit_get_shield;
	float UnitGetShield(datum unitDatum)
	{
		return p_unit_get_shield(unitDatum);
	}

	typedef void(__cdecl physics_set_gravity_t)(float multiplier);
	physics_set_gravity_t* p_physics_set_gravity;
	void PhysicsSetGravity(float Multiplier)
	{
		p_physics_set_gravity(Multiplier);
	}

	typedef void(__cdecl physics_set_velocity_frame_t)(float unk1, float unk2, float unk3);
	physics_set_velocity_frame_t* p_physics_set_velocity_frame;
	void PhysicsSetVelocityFrame(float unk1, float unk2, float unk3)
	{
		p_physics_set_velocity_frame(unk1, unk2, unk3);
	}

	typedef void(__cdecl render_lights_enable_cinematic_shadow_t)(bool unk1, datum objectDatum, string_id StringId, float unk2);
	render_lights_enable_cinematic_shadow_t* p_render_lights_enable_cinematic_shadow;
	void RenderLightsEnableCinenaticShadow(bool unk1, datum objectDatum, string_id StringId, float unk2)
	{
		p_render_lights_enable_cinematic_shadow(unk1, objectDatum, StringId, unk2);
	}

	typedef void(__cdecl object_destroy_t)(datum object_datum_index);
	object_destroy_t* p_object_destroy;
	void ObjectDestroy(datum object_datum_index)
	{
		p_object_destroy(object_datum_index);
	}

	typedef void(__cdecl fade_out_t)(float r, float g, float b, __int16 ticks);
	fade_out_t* p_fade_out;
	s_hs_fade_args* __cdecl fade_out_evaluate(__int16 op_code, int thread_id, bool unk_bool)
	{
		s_hs_fade_args* args;

		args = (s_hs_fade_args*)p_hs_arguments_evaluate(op_code, thread_id, unk_bool);
		if (!args) { return args; }

		for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
		{
			if (NetworkSession::PlayerIsActive(i))
			{
				NetworkMessage::SendHSFunction(NetworkSession::GetPeerIndex(i), _hs_fade_out, sizeof(s_hs_fade_args), args);
			}
		}

		p_fade_out(args->color.red, args->color.green, args->color.blue, args->ticks);
		return (s_hs_fade_args*)p_hs_return(thread_id, 0);
	}

	typedef void(__cdecl fade_in_t)(float r, float g, float b, __int16 ticks);
	fade_in_t* p_fade_in;
	s_hs_fade_args* __cdecl fade_in_evaluate(__int16 op_code, int thread_id, char unk_bool)
	{
		s_hs_fade_args* args;

		args = (s_hs_fade_args*)p_hs_arguments_evaluate(op_code, thread_id, unk_bool);
		if (!args) { return args; }

		for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
		{
			if (NetworkSession::PlayerIsActive(i))
			{
				NetworkMessage::SendHSFunction(NetworkSession::GetPeerIndex(i), _hs_fade_in, sizeof(s_hs_fade_args), args);
			}
		}

		p_fade_in(args->color.red, args->color.green, args->color.blue, args->ticks);
		return (s_hs_fade_args*)p_hs_return(thread_id, 0);
	}

	void CallNetworkedHSFunction(const s_networked_hs_function* data)
	{
		switch ((byte)data->function_type)
		{
		case _hs_fade_out:
		{
			const s_hs_fade_args* args = (const s_hs_fade_args*)data->arg_buffer;
			p_fade_out(args->color.red, args->color.green, args->color.blue, args->ticks);
			break;
		}
		case _hs_fade_in:
		{
			const s_hs_fade_args* args = (const s_hs_fade_args*)data->arg_buffer;
			p_fade_in(args->color.red, args->color.green, args->color.blue, args->ticks);
			break;
		}
		default:
			break;
		}
	}

	void ApplyHooks()
	{
		WritePointer(Memory::GetAddress(0x3C0CBC, 0x37D5D4), fade_out_evaluate);
		WritePointer(Memory::GetAddress(0x3C0CA4, 0x37D5BC), fade_in_evaluate);
	}

	void Initialize()
	{
		ApplyHooks();

		p_hs_arguments_evaluate = Memory::GetAddress<hs_arguments_evaluate_t*>(0x9581D, 0xAAA1D);
		p_hs_return = Memory::GetAddress<hs_return_t*>(0x9505D, 0xAA25D);

		p_unit_kill	= Memory::GetAddress<unit_kill_t*>(0x13B514, 0x12A363);
		p_unit_in_vehicle = Memory::GetAddress<unit_in_vehicle_t*>(0x1846D9, 0x16E775);
		p_unit_get_health = Memory::GetAddress<unit_get_health_t*>(0x184477, 0x165E13);
		p_unit_get_shield = Memory::GetAddress<unit_get_shield_t*>(0x18447C, 0x16E518);
		p_physics_set_gravity	= Memory::GetAddress<physics_set_gravity_t*>(0xB3C00, 0xA3E13);
		p_physics_set_velocity_frame = Memory::GetAddress<physics_set_velocity_frame_t*>(0xB3D5B, 0xA3F6E);
		p_render_lights_enable_cinematic_shadow = Memory::GetAddress<render_lights_enable_cinematic_shadow_t*>(0x19245A);
		p_object_destroy = Memory::GetAddress<object_destroy_t*>(0xFDCFD, 0x124ED5);
		p_fade_out = Memory::GetAddress<fade_out_t*>(0xA3CCA, 0x95F2A);
		p_fade_in = Memory::GetAddress<fade_in_t*>(0xA402C, 0x9628C);
	}
}
