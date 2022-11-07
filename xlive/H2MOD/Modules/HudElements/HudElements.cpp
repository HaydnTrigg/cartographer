#include "stdafx.h"

#include "HudElements.h"
#include "H2MOD\Engine\Engine.h"
#include "Blam\Engine\Camera\Camera.h"
#include "Blam\Engine\Game\GameEngineGlobals.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\CustomVariantSettings\CustomVariantSettings.h"
#include "H2MOD\Modules\Input\KeyboardInput.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\Shell\Startup\Startup.h"
#include "H2MOD\Utils\Utils.h"
#include "H2MOD\Tags\TagInterface.h"
#include "Blam\Cache\TagGroups\bitmap_definition.hpp"
#include "Util\Hooks\Hook.h"

#include "Blam\Engine\IceCreamFlavor\IceCreamFlavor.h"

#define _USE_MATH_DEFINES
#include <math.h>

static bool b_showHUD = true;
static bool b_showFirstPerson = true;
static bool RenderIngameChat() {
	int GameGlobals = *Memory::GetAddress<int*>(0x482D3C);
	DWORD* GameEngine = (DWORD*)(GameGlobals + 0x8);

	if (H2Config_hide_ingame_chat) {
		datum local_player_datum_index = h2mod->get_player_datum_index_from_controller_index(0);
		if (s_player::GetPlayer(DATUM_INDEX_TO_ABSOLUTE_INDEX(local_player_datum_index))->is_chatting == 2) {
			extern void hotkeyFuncToggleHideIngameChat();
			hotkeyFuncToggleHideIngameChat();
		}
		return true;
	}

	else if (*GameEngine != 3 && Engine::get_game_life_cycle() == _life_cycle_in_game) {
		//Enable chat in engine mode and game state mp.
		return false;
	}
	else {
		//original test - if is campaign
		return *GameEngine == 1;
	}
}

static bool __cdecl RenderFirstPersonCheck(unsigned int a1)
{
	return !b_showFirstPerson;
}

static bool __cdecl RenderHudCheck(unsigned int a1)
{
	// TODO: cleanup
	static bool hud_opacity_reset = true;
	DWORD new_hud_globals = *(DWORD*)(H2BaseAddr + 0x9770F4);
	float& hud_opacity = *(float*)(new_hud_globals + 0x228); // set the opacity

	if (!b_showHUD || ice_cream_flavor_available(_blind))
	{
		hud_opacity = 0.f;
		hud_opacity_reset = false;
	}
	else if (!hud_opacity_reset)
	{
		hud_opacity = 1.f;
		hud_opacity_reset = true;
	}

	return false;
}

typedef void(__cdecl render_camera_build_projection_t)(s_camera*, int, int);
render_camera_build_projection_t* p_render_camera_build_projection;

void __cdecl render_camera_build_projection_hook(s_camera* camera, int frustum_bounds, int out_projection)
{
	float old_camera_field_of_view = camera->field_of_view;
	
	if (H2Config_static_first_person) 
	{
		camera->field_of_view = ((64.f * M_PI) / 180.0f) * 0.78500003f;
	}
	
	p_render_camera_build_projection(camera, frustum_bounds, out_projection);
	
	camera->field_of_view = old_camera_field_of_view;
}

float __fastcall sub_46EDAF(int a1)
{
	typedef float(__cdecl game_ticks_to_seconds_t)(float ticks);
	auto p_game_ticks_to_seconds = Memory::GetAddress<game_ticks_to_seconds_t*>(0x7C0A9);
	__int8* byte_8BF900 = Memory::GetAddress<__int8*>(0x4BF900);

	double v1; // st7
	float result; // xmm0_4
	float v3; // [esp+4h] [ebp-4h]

	v1 = p_game_ticks_to_seconds(byte_8BF900[a1]) * 4.0;
	if (v1 < 0.0)
		return 0.0;
	result = 1.0;
	v3 = v1;
	if (v3 <= 1.0)
		return v1;
	return result;
}

/*
void __fastcall hud_draw_player_text_hook(float a1) 
{
	typedef bool(sub_620FBD_t)();
	auto p_sub_620FBD = Memory::GetAddress<sub_620FBD_t*>(0x220FBD);

	typedef int(sub_664F3C_t)();
	auto p_sub_664F3C = Memory::GetAddress<sub_664F3C_t*>(0x264F3C);

	typedef int (game_seconds_to_ticks_round_t)(float a1);
	auto p_game_seconds_to_ticks_round = Memory::GetAddress<game_seconds_to_ticks_round_t*>(0x7C0DB);

	typedef void(__fastcall initialize_game_state_player_iterator_t)(int *_this);
	auto p_initialize_game_state_player_iterator = Memory::GetAddress<initialize_game_state_player_iterator_t*>(0x7C0DB);

	typedef bool(__fastcall players_iterator_next_t)(int* _this);
	auto p_players_iterator_next = Memory::GetAddress<players_iterator_next_t*>(0x5B120);

	typedef int(__fastcall get_last_iterated_datum_index_t)(int* _this);
	auto p_get_last_iterated_datum_index = Memory::GetAddress<get_last_iterated_datum_index_t*>(0x5B15A);

	typedef __int8(__fastcall sub_46A656_t)(unsigned __int16 a1, int a2);
	auto p_sub_46A656 = Memory::GetAddress<sub_46A656_t*>(0x6A656);

	typedef int(__cdecl sub_477080_t)(rect2d* a1);
	auto p_sub_477080 = Memory::GetAddress<sub_477080_t*>(0x77080);

	typedef int(__cdecl sub_477071_t)(rect2d* a1);
	auto p_sub_477071 = Memory::GetAddress<sub_477071_t*>(0x77071);
	
	typedef __int16*(__cdecl sub_477049_t)(rect2d* a1, __int16 a2, __int16 a3, __int16 a4, __int16 a5);
	auto p_sub_477049 = Memory::GetAddress<sub_477049_t*>(0x77049);

	typedef void(__cdecl sub_673167_t)();
	auto p_sub_673167 = Memory::GetAddress<sub_673167_t*>(0x77049);

	typedef void(__cdecl render_scoreboard_primitives_t)(rect2d* a1, int* a2);
	auto p_render_scoreboard_primitives = Memory::GetAddress<render_scoreboard_primitives_t*>(0x77049);

	typedef float(__fastcall sub_46EDAF_t)(int a1, double a2);
	auto p_sub_46EDAF = Memory::GetAddress<sub_46EDAF_t*>(0x6EDAF);

	typedef int(__cdecl sub_4749F7_t)(int a2, float arg4);
	auto p_sub_4749F7 = Memory::GetAddress<sub_4749F7_t*>(0x749F7);

	real_matrix4x3* global_projection = Memory::GetAddress<real_matrix4x3*>(0x4E673C);
	s_camera* global_camera = Memory::GetAddress<s_camera*>(0x4E66C8);
	s_game_engine_globals* game_engine_globals = s_game_engine_globals::get();
	int local_player_render_index = *Memory::GetAddress<int*>(0x4E6800);
	float* off_8127A0 = Memory::GetAddress<float*>(0x4127A0);

	int local_player_render_index_copy; // esi
	int player_index; // edi
	int v3; // eax
	int last_iterated_datum_index; // eax
	int v5; // eax
	float v6; // xmm0_4
	__int16 v7; // ax
	float v8; // xmm0_4
	float* render_text_flag; // [esp+0h] [ebp-28h]
	__int16 arg4a; // [esp+0h] [ebp-28h]
	double arg4b; // [esp+0h] [ebp-28h]
	int a2a[4]; // [esp+Ch] [ebp-1Ch] BYREF
	double a1a; // [esp+1Ch] [ebp-Ch] BYREF

	if (!h2mod->GetEngineType() || p_sub_620FBD())
		return;
	local_player_render_index_copy = local_player_render_index;
	player_index = h2mod->get_player_datum_index_from_controller_index(local_player_render_index);
	if (local_player_render_index_copy != -1)
	{
		if (((unsigned __int8)(1 << local_player_render_index_copy) & game_engine_globals->gap2[0]) != 0)
		{
			if (p_sub_664F3C() <= 0)
			{
				game_engine_globals->gap2[0] &= ~(1 << local_player_render_index_copy);
			}
			else
			{
				v3 = p_game_seconds_to_ticks_round(3.0);
				if (++game_engine_globals->gap2[local_player_render_index_copy + 1] > v3)
					game_engine_globals->gap2[0] &= ~(1 << local_player_render_index_copy);
			}
		}
		else
		{
			game_engine_globals->gap2[local_player_render_index_copy + 1] = 0;
		}
	}
	if (player_index != -1)
	{
		p_initialize_game_state_player_iterator(a2a);
		while (p_players_iterator_next(a2a))
		{
			if (p_get_last_iterated_datum_index(a2a) != player_index)
			{
				last_iterated_datum_index = p_get_last_iterated_datum_index(a2a);
				p_sub_46A656(player_index, last_iterated_datum_index);
				*(float*)&a1a = a1;
				if (a1 > 0.0)
				{
					//render_text_flag = (float*)a1a;
					v5 = p_get_last_iterated_datum_index((int*)a2a);
					draw_player_names_above_head_hook(v5, *(float*)&render_text_flag);
				}
			}
		}
	}
	if (local_player_render_index_copy == -1)
	{
		v6 = 1.0;
	LABEL_19:
		a2a[1] = *off_8127A0;
		a2a[2] = off_8127A0[1];
		a2a[3] = off_8127A0[2];
		a2a[0] = v6;
		arg4a = p_sub_477080(&global_camera->screen);
		v7 = p_sub_477071(&global_camera->screen);
		p_sub_477049((rect2d*)&a1a, 0, 0, v7, arg4a);
		p_sub_673167();
		p_render_scoreboard_primitives((rect2d*)&a1a, a2a);
		//nullsub_66();
		goto LABEL_20;
	}
	v6 = game_engine_globals->unk_local_player_hud_field[local_player_render_index_copy];
	if (v6 > 0.0)
		goto LABEL_19;
LABEL_20:
	if (player_index != -1)
	{
		v8 = sub_46EDAF(local_player_render_index_copy);
		*(float*)&a1a = v8;
		if (v8 >= 0.0)
		{
			if (v8 <= 1.0)
			{
				if (v8 <= 0.0)
					return;
			}
			else
			{
				a1a = 0x3F800000;
			}
			*(float*)&arg4b = pow(*(float*)&a1a, 1.899999976158142);
			p_sub_4749F7(player_index, *(float*)&arg4b);
		}
	}
}*/


void HudElements::setCrosshairSize(bool mapLoadContext)
{
	static bool crosshairInit = false;
	static point2d* defaultCrosshairSizes = nullptr;

	if (Memory::IsDedicatedServer())
		return;

	// if we are in a "mapLoadContext", save default crosshair size and delete if we previously saved something
	if (mapLoadContext)
	{
		if (defaultCrosshairSizes != nullptr)
		{
			delete[] defaultCrosshairSizes;
			defaultCrosshairSizes = nullptr;
		}
		crosshairInit = false;
	}

	if (h2mod->GetEngineType() != e_engine_type::_main_menu) {
		auto hud_reticles_datum = tags::find_tag(blam_tag::tag_group_type::bitmap, "ui\\hud\\bitmaps\\new_hud\\crosshairs\\hud_reticles");
		if (hud_reticles_datum != DATUM_INDEX_NONE)
		{
			auto hud_reticles_data = tags::get_tag_fast<bitmap_definition>(hud_reticles_datum);

			for (auto i = 0; i < hud_reticles_data->bitmaps.size; i++)
			{
				if (!crosshairInit)
				{
					if (defaultCrosshairSizes == nullptr) defaultCrosshairSizes = new point2d[hud_reticles_data->bitmaps.size];

					defaultCrosshairSizes[i] = hud_reticles_data->bitmaps[i]->size;
				}

				hud_reticles_data->bitmaps[i]->size = point2d
				{
					(short)round(defaultCrosshairSizes[i].x * H2Config_crosshair_scale),
					(short)round(defaultCrosshairSizes[i].y * H2Config_crosshair_scale)
				};
			}
			crosshairInit = true;
		}
	}
}

void HudElements::setCrosshairPos() {

	if (Memory::IsDedicatedServer())
		return;

	if (!FloatIsNaN(H2Config_crosshair_offset)) {
		tags::tag_data_block* player_controls_block = reinterpret_cast<tags::tag_data_block*>(tags::get_matg_globals_ptr() + 240);
		if (player_controls_block->block_count > 0)
		{
			for (int i = 0; i < player_controls_block->block_count; i++) {
				*(float*)(tags::get_tag_data() + player_controls_block->block_data_offset + 128 * i + 28) = H2Config_crosshair_offset;
			}
		}
	}
}

void HudElements::RadarPatch()
{
	WriteValue<BYTE>(H2BaseAddr + 0x2849C4, (BYTE)4);
	addDebugText("Motion sensor patched successfully.");
}

void HudElements::setFOV() {

	if (Memory::IsDedicatedServer())
		return;

	static float fov = 70.0f * M_PI / 180.0f;
	static bool fov_redirected = false;
	if (H2Config_field_of_view > 0 && H2Config_field_of_view <= 110)
	{
		if (!fov_redirected)
		{
			BYTE opcode[6] = { 0xD9, 0x05, 0x00, 0x00, 0x00, 0x00 };
			WritePointer((DWORD)&opcode[2], &fov);
			WriteBytes(Memory::GetAddress(0x907F3), opcode, sizeof(opcode)); // fld dword ptr[fov]

			fov_redirected = true;
		}

		//const double default_radians_field_of_view = 70.0f * M_PI / 180.0f;
		if (currentVariantSettings.forcedFOV == 0)
			fov = (float)H2Config_field_of_view * M_PI / 180.0f;
		else
			fov = (float)currentVariantSettings.forcedFOV * M_PI / 180.0f;
	}
}

void HudElements::setVehicleFOV() {

	if (Memory::IsDedicatedServer())
		return;

	if (H2Config_vehicle_field_of_view > 0 && H2Config_vehicle_field_of_view <= 110)
	{
		float calculated_radians_FOV;
		if (currentVariantSettings.forcedFOV == 0) {
			calculated_radians_FOV = (float)H2Config_vehicle_field_of_view * M_PI / 180.0f;
			//WriteValue(Memory::GetAddress(0x413780), (float)H2Config_vehicle_field_of_view * M_PI / 180.0f);
		}
			//
		else {
			calculated_radians_FOV = (float)currentVariantSettings.forcedFOV * M_PI / 180.0f;
			//WriteValue(Memory::GetAddress(0x413780), (float)CurrentVariantSettings.ForcedFOV * M_PI / 180.0f);
		}
			//
		WriteValue(Memory::GetAddress(0x413780), calculated_radians_FOV); // Third Person
	}
}


void HudElements::ToggleFirstPerson(bool state)
{
	if (state == NULL)
		b_showFirstPerson = !b_showFirstPerson;
	else
		b_showFirstPerson = state;
}

void HudElements::ToggleHUD(bool state)
{
	if (state == NULL)
		b_showHUD = !b_showHUD;
	else
		b_showHUD = state;
}

void HudElements::OnMapLoad()
{
	setCrosshairSize(true);
	setCrosshairPos();
}

void HudElements::ApplyHooks()
{
	//Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
	PatchCall(Memory::GetAddress(0x22667B), RenderIngameChat);
	PatchCall(Memory::GetAddress(0x226628), RenderIngameChat);
	PatchCall(Memory::GetAddress(0x228579), RenderFirstPersonCheck);
	PatchCall(Memory::GetAddress(0x223955), RenderHudCheck);
	PatchCall(Memory::GetAddress(0x191440), render_camera_build_projection_hook);
	//PatchCall(Memory::GetAddress(0x226716), hud_draw_player_text_hook);
	p_render_camera_build_projection = Memory::GetAddress<render_camera_build_projection_t*>(0x1953f5);
}

void HudElements::Init()
{
	if (H2IsDediServer)
		return;
	KeyboardInput::RegisterHotkey(&H2Config_hotkeyIdToggleHideIngameChat, 
		[]()	{
			H2Config_hide_ingame_chat = !H2Config_hide_ingame_chat;
		}
	);
	ApplyHooks();
	setFOV();
	setVehicleFOV();
	RadarPatch();
}
