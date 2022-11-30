#include "stdafx.h"

#include "CustomVariantSettings.h"

#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/memory/bitstream.h"
#include "Blam/Engine/networking/logic/network_life_cycle.h"
#include "Blam/Engine/physics/physics_constants.h"
#include "Blam/Engine/units/units.h"
#include "Blam/Engine/game/player_control.h"
#include "Blam/Engine/networking/messages/network_message_type_collection.h"
#include "Blam/Engine/networking/session/network_session.h"

#include "H2MOD/Modules/HudElements/HudElements.h"
#include "H2MOD/Modules/Stats/StatsHandler.h"
#include "H2MOD/Utils/Utils.h"
#include "Util/Hooks/Hook.h"

CustomVariantSettings::s_variantSettings currentVariantSettings;
CustomVariantSettings::s_variantSettings defaultCustomVariantSettings;
std::map<std::wstring, CustomVariantSettings::s_variantSettings> customVariantSettingsMap;

namespace CustomVariantSettings
{
	void __cdecl EncodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data)
	{
		stream->data_encode_bits("gravity", &data->gravity, sizeof(data->gravity) * CHAR_BIT); //16.
		stream->data_encode_bits("game speed", &data->gameSpeed, sizeof(data->gameSpeed) * CHAR_BIT);
		stream->data_encode_bool("Infinite Ammo", data->infiniteAmmo);
		stream->data_encode_bool("Explosion Physics", data->explosionPhysics);
		stream->data_encode_integer("Hill Rotation", (byte)data->hillRotation, 8);
		stream->data_encode_bool("Infinite Grenades", data->infiniteGrenades);
		stream->data_encode_bits("Forced FOV", &data->forcedFOV, sizeof(data->forcedFOV) * CHAR_BIT);
	}
	bool __cdecl DecodeVariantSettings(bitstream* stream, int a2, s_variantSettings* data)
	{
		double gravity;
		stream->data_decode_bits("gravity", &gravity, sizeof(gravity) * CHAR_BIT);
		data->gravity = gravity;
		double gamespeed;
		stream->data_decode_bits("game speed", &gamespeed, sizeof(gamespeed) * CHAR_BIT);
		data->gameSpeed = gamespeed;
		
		data->infiniteAmmo = stream->data_decode_bool("Infinite Ammo");
		data->explosionPhysics = stream->data_decode_bool("Explosion Physics");
		data->hillRotation = (e_hill_rotation)stream->data_decode_integer("Hill Rotation", 8);
		data->infiniteGrenades = stream->data_decode_bool("Infinite Grenades");
		double ForcedFOV;
		stream->data_decode_bits("Forced FOV", &ForcedFOV, sizeof(ForcedFOV) * CHAR_BIT);
		data->forcedFOV = ForcedFOV;
		return stream->overflow() == false;
	}

	void UpdateCustomVariantSettings(s_variantSettings* data)
	{
		currentVariantSettings = *data;
	}

	void SendCustomVariantSettings(int peerIndex)
	{
		s_network_session* session = NetworkSession::GetCurrentNetworkSession();
		if (NetworkSession::LocalPeerIsSessionHost())
		{
			//TODO: Find and map out struct with current variant information.
			auto VariantName = std::wstring(Memory::GetAddress<wchar_t*>(0, 0x534A18));

			auto customVariantSetting = customVariantSettingsMap.find(VariantName);
			if (customVariantSetting != customVariantSettingsMap.end())
			{
				currentVariantSettings = customVariantSetting->second;
				if (currentVariantSettings != defaultCustomVariantSettings) {
					s_network_observer* observer = session->p_network_observer;
					s_peer_observer_channel* observer_channel = NetworkSession::GetPeerObserverChannel(peerIndex);
					if (peerIndex != -1 && !NetworkSession::PeerIndexLocal(peerIndex))
					{
						if (observer_channel->field_1)
							observer->sendNetworkMessage(session->session_index, observer_channel->observer_index,
								s_network_observer::e_network_message_send_protocol::in_band, _custom_variant_settings,
								CustomVariantSettingsPacketSize, &currentVariantSettings);
					}
				}
			}
		}
	}
	void ResetSettings()
	{
		currentVariantSettings = defaultCustomVariantSettings;
		ApplyCustomSettings(&currentVariantSettings);
	}

	void OnPlayerSpawn(datum PlayerDatum)
	{
		if (NetworkSession::LocalPeerIsSessionHost()) {
			if (currentVariantSettings.infiniteGrenades)
			{
				units::carto_set_player_unit_grenades_count(DATUM_INDEX_TO_ABSOLUTE_INDEX(PlayerDatum), Fragmentation, 4, false);
				units::carto_set_player_unit_grenades_count(DATUM_INDEX_TO_ABSOLUTE_INDEX(PlayerDatum), Plasma, 4, false);
			}
		}
	}

	void ApplyCustomSettings(s_variantSettings* newVariantSettings)
	{
		//
		//Anything to be done on host and client goes here.
		//
		physics_constants::get()->gravity = newVariantSettings->gravity * physics_constants::get_default_gravity();
		time_globals::get()->game_speed = newVariantSettings->gameSpeed;
		//mov [ecx+6], ax
		static BYTE InfiniteAmmoMagazineASM[] = { 0x66, 0x89, 0x41, 0x06 };
		//movss [edi+00000184],xmm0
		static BYTE InfiniteAmmoBatteryASM[] = { 0xF3, 0x0F, 0x11, 0x87, 0x84, 0x01, 0x00, 0x00 };
		if (newVariantSettings->infiniteAmmo)
		{
			//Nop remove ammo from clips
			NopFill(Memory::GetAddress(0x15F3EA, 0x1436AA), sizeof(InfiniteAmmoMagazineASM));
			//Nop remove energy from battery.
			NopFill(Memory::GetAddress(0x15f7c6, 0x143A86), sizeof(InfiniteAmmoBatteryASM));
		}
		else
		{
			WriteBytes(Memory::GetAddress(0x15F3EA, 0x1436AA), InfiniteAmmoMagazineASM, sizeof(InfiniteAmmoMagazineASM));
			WriteBytes(Memory::GetAddress(0x15f7c6, 0x143A86), InfiniteAmmoBatteryASM, sizeof(InfiniteAmmoBatteryASM));
		}

		static BYTE InfiniteGrenadesASM[] = { 0x80, 0x84, 0x30, 0x52, 0x02, 0x00, 0x00, 0xFF };
		if (newVariantSettings->infiniteGrenades)
		{
			NopFill(Memory::GetAddress(0x1666A8, 0x15C168), sizeof(InfiniteGrenadesASM));
		}
		else
		{
			WriteBytes(Memory::GetAddress(0x1666A8, 0x15C168), InfiniteGrenadesASM, sizeof(InfiniteGrenadesASM));
		}

		//Client Only
		if (!Memory::IsDedicatedServer()) {
			if (newVariantSettings->explosionPhysics)
				WriteValue(Memory::GetAddress(0x17a44b), (BYTE)0x1E);
			else
				WriteValue(Memory::GetAddress(0x17a44b), (BYTE)0);

			if (newVariantSettings->forcedFOV != 0)
			{
				HudElements::setFOV();
				HudElements::setVehicleFOV();
			}
		}

		//Server Only
		if (Memory::IsDedicatedServer())
		{

		}

		//Host Only
		if (NetworkSession::LocalPeerIsSessionHost())
		{
			// *((_DWORD *)v1 + 94) = seconds_to_ticks_imprecise(1);
			//Changing the argument passed to seconds_to_ticks_impercise.
			WriteValue(Memory::GetAddress(0x55d01, 0x5e1f9), (BYTE)newVariantSettings->spawnProtection);
		}
	}
	void ApplyCurrentSettings()
	{
		// no idea why this is needed to be executed on blue screen but whatever
		ApplyCustomSettings(&currentVariantSettings);
	}
	void OnGameLifeCycleUpdate(e_game_life_cycle state)
	{
		switch (state)
		{
		case _life_cycle_in_game:
			ApplyCurrentSettings();
			break;
		case _life_cycle_none:
		case _life_cycle_pre_game:
		case _life_cycle_post_game:
			ResetSettings();
			break;
		default:
			break;
		}
	}
	void OnNetworkPlayerEvent(int peerIndex, EventHandler::NetworkPlayerEventType type)
	{
		if(type == EventHandler::NetworkPlayerEventType::add)
		{
			SendCustomVariantSettings(peerIndex);
		}
	}
	void OnMatchCountdown()
	{
		//
		//Anything host related before the game starts goes here.
		//
		for (auto i = 0; i < NetworkSession::GetPeerCount(); i++)
		{
			if (!NetworkSession::PeerIndexLocal(i))
				SendCustomVariantSettings(i);
		}
	}

	void Initialize()
	{		
		EventHandler::register_callback(OnGameLifeCycleUpdate, EventType::gamelifecycle_change);
		EventHandler::register_callback(OnMatchCountdown, EventType::countdown_start, EventExecutionType::execute_after);
		EventHandler::register_callback(OnNetworkPlayerEvent, EventType::network_player, EventExecutionType::execute_after);
		EventHandler::register_callback(ApplyCurrentSettings, EventType::blue_screen, EventExecutionType::execute_after);
		EventHandler::register_callback(OnPlayerSpawn, EventType::player_spawn, EventExecutionType::execute_after);
	}
}