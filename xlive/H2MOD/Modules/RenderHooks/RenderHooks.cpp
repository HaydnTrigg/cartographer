#include "stdafx.h"

#include "RenderHooks.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "Util\Hooks\Hook.h"

bool ras_layer_overrides[RenderHooks::end];
bool geo_render_overrides[24];

#define MAX_DECALS 1024u
int new_decal_array[MAX_DECALS];

namespace RenderHooks
{
	DWORD layer_calls[]
	{
		0x28053A,
		0x280557,
		0x280577,
		0x280597,
		0x2805B7,
		0x2805EF,
		0x280627,
		0x280659,
		0x280678,
		0x280699,
		0x2806BA,
		0x2806F2,
		0x28072A,
		0x28074C,
		0x28077A,
		0x280799,
		0x2807B7
	};
	DWORD render_calls[]
	{
		0x190B0F,
		0x1914D4,
		0x191668,
		0x191680,
		0x191698,
		0x1916CC,
		0x1916E4,
		0x191702,
		0x19171A,
		0x19175A,
		0x19179A,
		0x191888,
		0x191944,
		0x19195C,
		0x1919DA,
		0x191A79,
		0x191B1A,
		0x2265E2
	};

	bool* reset_screen;

	typedef bool(__cdecl initialize_rasterizer_layer_t)(e_layer_type type, unsigned int width, unsigned int height, bool fmt, int a5);
	initialize_rasterizer_layer_t* p_initialize_rasterizer_layer;

	typedef void(__cdecl render_geometry_t)(e_render_geometry_type type);
	render_geometry_t* p_render_geometry;

	int getWidth(e_layer_type e, unsigned int width)
	{
		switch (e)
		{
		
		case RenderHooks::dynamic_shadows:
			switch (H2Config_Override_Shadows)
			{
			case tex_low:
				return 512;
			case tex_high:
				return 2048;
			case tex_ultra:
				return 4096;
			}
			break;

		case RenderHooks::lightmap_shadows_1:
		case RenderHooks::lightmap_shadows_2:
			switch (H2Config_Override_Shadows)
			{
			case tex_low:
				return 512;
			case tex_high:
				return 2048;
			case tex_ultra:
				return 4096;
			}
			break;

		case RenderHooks::water:
				switch (H2Config_Override_Water)
				{
				case tex_low:
					return 512;
				case tex_high:
					return 2048;
				case tex_ultra:
					return 4096;
				}
			break;

		default:
			break;
		}

		return width;
	}
	int getHeight(e_layer_type e, unsigned int height)
	{
		if (e == e_layer_type::lightmap_shadows_1 || e == e_layer_type::lightmap_shadows_2)
			switch (H2Config_Override_Shadows)
			{
				case tex_low:
					return 512;
				case tex_high:
					return 2048;
				case tex_ultra:
					return 4096;
			}
		if (e == e_layer_type::dynamic_shadows) {
			switch (H2Config_Override_Shadows)
			{
				case tex_low:
					return 512;
				case tex_high:
					return 2048;
				case tex_ultra:
					return 4096;
			}
		}
		if (e == e_layer_type::water)
		{
			switch (H2Config_Override_Water)
			{
				case tex_low:
					return 256;
				case tex_high:
					return 1024;
				case tex_ultra:
					return 2048;
			}
		}
		return height;
	}
	bool __cdecl initialize_rasterizer_layer_hook(e_layer_type type, unsigned int width, unsigned int height, bool fmt, int a5)
	{
		int texture_width = getWidth(type, width);
		int texture_height = getHeight(type, height);

		if (ras_layer_overrides[(int)type - 1])
		{
			texture_width = 128;
			texture_height = 128;
		}
		LIMITED_LOG(15, LOG_TRACE_GAME, L"{} - : {} {} {} {} {}", __FUNCTIONW__, type, texture_width, texture_height, fmt, a5);
		return p_initialize_rasterizer_layer(type, texture_width, texture_height, fmt, a5);
	}

	void __cdecl render_geometry_hook(e_render_geometry_type type)
	{
		if (!geo_render_overrides[(int)type - 1]) {
			return p_render_geometry(type);
		}
	}

	//rasterizer_near_clip_distance <real>
	//Changed from game default of 0.06 to 0.0601
	inline void SunflareFix()
	{
		WriteValue<float>(Memory::GetAddress(0x468150), 0.0601f);
	}

#pragma region Decal Function Rewrites
	__int8* __cdecl sub_6755BB(int a1)
	{
		s_data_array* decals = *Memory::GetAddress<s_data_array**>(0x4CE89C);

		char* data; // eax
		bool v2; // zf
		__int8* result; // eax

		data = decals->data;
		v2 = *(DWORD*)&data[68 * (unsigned __int16)a1 + 60] == a1;
		result = &data[68 * (unsigned __int16)a1];
		if (v2)
			new_decal_array[(unsigned __int16)*((DWORD*)result + 1)] = a1;
		return result;
	}

	void __cdecl sub_675835(int a1)
	{
		if (a1 != -1 && a1 <= MAX_DECALS - 1)
			new_decal_array[a1] = -1;
	}

	bool __cdecl rasterizer_decal_vertices_locked_proc(unsigned __int16 a1)
	{
		s_data_array* decals = *Memory::GetAddress<s_data_array**>(0x4CE89C);

		return (decals->name[68 * (unsigned __int16)new_decal_array[a1] + 2] & 3) != 0;
	}

	int __cdecl rasterizer_decal_vertices_purge_proc(unsigned __int16 a1)
	{
		typedef int (__cdecl sub_4AD2E7_t)(int a1);
		auto p_sub_4AD2E7 = Memory::GetAddress<sub_4AD2E7_t*>(0xAD2E7);

		int result;

		result = new_decal_array[a1];
		if (result != -1)
		{
			result = p_sub_4AD2E7(new_decal_array[a1]);
			new_decal_array[a1] = -1;
		}
		return result;
	}

	void __cdecl decals_initialize()
	{
		int c_gamestate_allocation_ptr = *Memory::GetAddress<int*>(0x47CD48);

		typedef s_data_array* (__cdecl game_state_data_new_t)(char* a1, int elements_total, int element_size, char a4, int a5);
		auto p_game_state_data_new = Memory::GetAddress<game_state_data_new_t*>(0x66AD5);

		typedef __int8* (__cdecl game_state_malloc_t)(char* a1, char* a2, unsigned int a3);
		auto p_game_state_malloc = Memory::GetAddress<game_state_malloc_t*>(0x2FF1D);

		typedef int (game_state_gpu_malloc_t)();
		auto p_game_state_gpu_malloc = Memory::GetAddress<game_state_gpu_malloc_t*>(0x2755EF);

		typedef char* (game_state_lru_cache_new_t)(char* a1,
			int a2,
			int a3,
			int total_elements,
			void* a5,
			void* a6,
			int a7,
			int a8);
		auto p_game_state_lru_cache_new = Memory::GetAddress<game_state_lru_cache_new_t*>(0x9A633);

		//int* decal_array = Memory::GetAddress<int*>(0xA55780);	// defined on stack with 1024 elements
		s_data_array** decals = Memory::GetAddress<s_data_array**>(0x4CE89C);
		__int8** decal_globals = Memory::GetAddress<__int8**>(0x4CE898);
		void** decal_vertex_cache = Memory::GetAddress<void**>(0xA56798);
		IDirect3DDevice9* g_d3d_device_9 = *Memory::GetAddress<IDirect3DDevice9**>(0xA3C6B4);
		DWORD** decal_ps_function_pointers = Memory::GetAddress<DWORD**>(0x468754);
		IDirect3DPixelShader9** decal_ps_ptrs = Memory::GetAddress<IDirect3DPixelShader9**>(0xA56784);

		*decals = p_game_state_data_new("decals", MAX_DECALS, 68, 0, c_gamestate_allocation_ptr);
		*decal_globals = p_game_state_malloc("decal globals", 0, 15404u);
		p_game_state_gpu_malloc();

		for (int i = 0; i < 5; ++i)
			g_d3d_device_9->CreatePixelShader(decal_ps_function_pointers[i], &decal_ps_ptrs[i]);

		*decal_vertex_cache = p_game_state_lru_cache_new(
			"decal vertex cache",
			5118,
			5,
			MAX_DECALS,
			rasterizer_decal_vertices_purge_proc,
			rasterizer_decal_vertices_locked_proc,
			0,
			c_gamestate_allocation_ptr);
		memset(new_decal_array, -1, MAX_DECALS);
	}
#pragma endregion Decal Function Rewrites

	void __cdecl effects_initialize()
	{
		const __int8 multiplier = 4;

		int c_gamestate_allocation_ptr = *Memory::GetAddress<int*>(0x47CD48);

		typedef s_data_array* (__cdecl game_state_data_new_t)(char* a1, int elements_total, int element_size, char a4, int a5);
		auto p_game_state_data_new = Memory::GetAddress<game_state_data_new_t*>(0x66AD5);

		s_data_array** effect = Memory::GetAddress<s_data_array**>(0x4CE884);
		s_data_array** effect_location = Memory::GetAddress<s_data_array**>(0x4CE880);
		s_data_array** particle_system = Memory::GetAddress<s_data_array**>(0x4D83D4);
		s_data_array** particles = Memory::GetAddress<s_data_array**>(0x4DD08C);
		s_data_array** particle_emitter = Memory::GetAddress<s_data_array**>(0x4DD090);
		s_data_array** particle_location = Memory::GetAddress<s_data_array**>(0x4DD094);

		*effect = p_game_state_data_new("effect", 659, 400, 0, c_gamestate_allocation_ptr);	// Only 659 effects before crash
		*effect_location = p_game_state_data_new("effect location", 512 * multiplier, 60, 0, c_gamestate_allocation_ptr);
		*particle_system = p_game_state_data_new("particle_system", 128 * multiplier, 84, 0, c_gamestate_allocation_ptr);
		*particles = p_game_state_data_new("particles", 1024 * multiplier, 64, 0, c_gamestate_allocation_ptr);
		*particle_emitter = p_game_state_data_new("particle_emitter", 256 * multiplier, 76, 0, c_gamestate_allocation_ptr);
		*particle_location = p_game_state_data_new("particle_location", 256 * multiplier, 52, 0, c_gamestate_allocation_ptr);
	}

	void ResetDevice()
	{
		*reset_screen = true;
	}

	void ApplyHooks()
	{
		SunflareFix();

		/*
		WriteValue(Memory::GetAddress(0x3A09C0), decals_initialize);
		PatchCall(Memory::GetAddress(0x2756B9), rasterizer_decal_vertices_locked_proc);
		PatchCall(Memory::GetAddress(0x2756BE), rasterizer_decal_vertices_purge_proc);
		PatchCall(Memory::GetAddress(0xACE64), sub_675835);
		PatchCall(Memory::GetAddress(0xACF49), sub_6755BB);
		PatchCall(Memory::GetAddress(0xACDC0), sub_6755BB);*/

		WriteValue(Memory::GetAddress(0x3A0A2C), effects_initialize);

		//p_initialize_rasterizer_layer = (p_initialize_rasterizer_layer)DetourFunc(Memory::GetAddress<BYTE*>(0x28024C), (BYTE*)initialize_rasterizer_layer_hook, 7);
		//p_render_geometry = (render_geometry_t)DetourFunc(Memory::GetAddress<BYTE*>(0x1A155C), (BYTE*)render_geometry_hook, 13);
		p_initialize_rasterizer_layer = Memory::GetAddress<initialize_rasterizer_layer_t*>(0x28024C);
		p_render_geometry = Memory::GetAddress<render_geometry_t*>(0x1A155C);
		for(auto &call : layer_calls)
		{
			PatchCall(Memory::GetAddress(call), initialize_rasterizer_layer_hook);
		}
		for(auto &call : render_calls)
		{
			PatchCall(Memory::GetAddress(call), render_geometry_hook);
		}
	}

	void Initialize()
	{
		reset_screen = Memory::GetAddress<bool*>(0xA3E4D4);
		ApplyHooks();
	}
}
