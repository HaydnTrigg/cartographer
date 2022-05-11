#pragma once
#include "stdafx.h"
#include <stdint.h>
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Math\real_math.h"

/*********************************************************************
* name: shader_template
* group_tag : stem
* header size : 96
* *********************************************************************/
#pragma pack(1)
struct shader_template_group_definition
{
	data_block documentation;//0x0
	string_id default_material_name;//0x8
	PAD(0x2);//0xC
	enum class e_flags : __int16
	{
		force_active_camo = FLAG(0),
		water = FLAG(1),
		foliage = FLAG(2),
		hide_standard_parameters = FLAG(3),
	};
	e_flags flags;//0xE
	struct s_properties_block
	{
		enum class e_property : __int16
		{
			unused = 0,
			diffuse_map = 1,
			lightmap_emissive_map = 2,
			lightmap_emissive_color = 3,
			lightmap_emissive_power = 4,
			lightmap_resolution_scale = 5,
			lightmap_half_life = 6,
			lightmap_diffuse_scale = 7,
			lightmap_alpha_test_map = 8,
			lightmap_translucent_map = 9,
			lightmap_translucent_color = 10,
			lightmap_translucent_alpha = 11,
			active_camo_map = 12,
			lightmap_foliage_scale = 13,
		};
		e_property property;//0x0
		PAD(0x2);//0x2
		string_id parameter_name;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_properties_block, 0x8);
	tag_block<s_properties_block> properties;//0x10
	struct s_categories_block
	{
		string_id name;//0x0
		struct s_parameters_block
		{
			string_id name;//0x0
			data_block explanation;//0x4
			enum class e_type : __int16
			{
				Bitmap = 0,
				Value = 1,
				Color = 2,
				Switch = 3,
			};
			e_type type;//0xC
			enum class e_flags : __int16
			{
				animated = FLAG(0),
				hide_bitmap_reference = FLAG(1),
			};
			e_flags flags;//0xE
			tag_reference default_bitmap;//0x10
			float default_const_value;//0x18
			real_color_rgb default_const_color;//0x1C
			enum class e_bitmap_type : __int16
			{
				NUM_2d = 0,
				NUM_3d = 1,
				cube_map = 2,
			};
			e_bitmap_type bitmap_type;//0x28
			PAD(0x2);//0x2A
			enum class e_bitmap_animation_flags : __int16
			{
				scale_uniform = FLAG(0),
				scale = FLAG(1),
				translation = FLAG(2),
				rotation = FLAG(3),
				index = FLAG(4),
			};
			e_bitmap_animation_flags bitmap_animation_flags;//0x2C
			PAD(0x2);//0x2E
			float bitmap_scale;//0x30
		};
		TAG_BLOCK_SIZE_ASSERT(s_parameters_block, 0x34);
		tag_block<s_parameters_block> parameters;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_categories_block, 0xC);
	tag_block<s_categories_block> categories;//0x18
	tag_reference light_response;//0x20
	struct s_lods_block
	{
		float projected_diameter;//0x0

		struct s_pass_block
		{
			enum class e_layer : __int16
			{
				texaccum = 0,
				environment_map = 1,
				selfillumination = 2,
				overlay = 3,
				transparent = 4,
				lightmap_indirect = 5,
				diffuse = 6,
				specular = 7,
				shadow_generate = 8,
				shadow_apply = 9,
				boom = 10,
				fog = 11,
				sh_prt = 12,
				active_camo = 13,
				water_edge_blend = 14,
				decal = 15,
				active_camo_stencil_modulate = 16,
				hologram = 17,
				light_albedo = 18,
			};
			e_layer layer;//0x0
			PAD(0x2);//0x2
			tag_reference pass;//0x4
			PAD(0xC);//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_pass_block, 0x18);
		tag_block<s_pass_block> pass;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_lods_block, 0xC);
	tag_block<s_lods_block> lods;//0x28
	struct s_NUM__block
	{
		__int32 NUM_;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_NUM__block, 0x4);
	tag_block<s_NUM__block> eMPTYSTRING1;//0x30
	TAG_BLOCK_SIZE_ASSERT(s_NUM__block, 0x4);
	tag_block<s_NUM__block> eMPTYSTRING2;//0x38

	tag_reference aux_1_shader;//0x40
	enum class e_aux_1_layer : __int16
	{
		texaccum = 0,
		environment_map = 1,
		selfillumination = 2,
		overlay = 3,
		transparent = 4,
		lightmap_indirect = 5,
		diffuse = 6,
		specular = 7,
		shadow_generate = 8,
		shadow_apply = 9,
		boom = 10,
		fog = 11,
		sh_prt = 12,
		active_camo = 13,
		water_edge_blend = 14,
		decal = 15,
		active_camo_stencil_modulate = 16,
		hologram = 17,
		light_albedo = 18,
	};
	e_aux_1_layer aux_1_layer;//0x48
	PAD(0x2);//0x4A
	tag_reference aux_2_shader;//0x4C
	enum class e_aux_2_layer : __int16
	{
		texaccum = 0,
		environment_map = 1,
		selfillumination = 2,
		overlay = 3,
		transparent = 4,
		lightmap_indirect = 5,
		diffuse = 6,
		specular = 7,
		shadow_generate = 8,
		shadow_apply = 9,
		boom = 10,
		fog = 11,
		sh_prt = 12,
		active_camo = 13,
		water_edge_blend = 14,
		decal = 15,
		active_camo_stencil_modulate = 16,
		hologram = 17,
		light_albedo = 18,
	};
	e_aux_2_layer aux_2_layer;//0x54
	PAD(0x2);//0x56

	struct s_postprocess_definition_block
	{
		struct s_levels_of_detail_block
		{
			__int16 block_index_data;//0x0
			__int32 available_layers;//0x2
			float projected_height_percentage;//0x6
		};
		TAG_BLOCK_SIZE_ASSERT(s_levels_of_detail_block, 0xA);
		tag_block<s_levels_of_detail_block> levels_of_detail;//0x0
		struct s_layers_block
		{
			__int16 block_index_data;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_layers_block, 0x2);
		tag_block<s_layers_block> layers;//0x8
		struct s_passes_block
		{
			tag_reference pass;//0x0
			__int16 block_index_data;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_passes_block, 0xA);
		tag_block<s_passes_block> passes;//0x10
		struct s_implementations_block
		{
			__int16 block_index_data1;//0x0
			__int16 block_index_data2;//0x2
			__int16 block_index_data3;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_implementations_block, 0x6);
		tag_block<s_implementations_block> implementations;//0x18
		struct s_remappings_block
		{
			PAD(0x3);//0x0
			__int8 source_index;//0x3
		};
		TAG_BLOCK_SIZE_ASSERT(s_remappings_block, 0x4);
		tag_block<s_remappings_block> remappings;//0x20
	};
	TAG_BLOCK_SIZE_ASSERT(s_postprocess_definition_block, 0x28);
	tag_block<s_postprocess_definition_block> postprocessDefinition;//0x58
};
TAG_GROUP_SIZE_ASSERT(shader_template_group_definition, 0x60);
#pragma pop