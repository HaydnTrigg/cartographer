#include "stdafx.h"

#include "TagFixes.h"
#include "Blam/Cache/TagGroups/shader_definition.hpp"
#include "Blam/Cache/TagGroups/light_definition.h"
#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam/Cache/TagGroups/shader_template_definition.hpp"
#include "H2MOD.h"
#include "H2MOD\Tags\TagInterface.h"
#include "Util\Hooks\Hook.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"

namespace TagFixes
{
	namespace
	{
		void fix_shader_template_nvidia()
		{
			
			auto alpha_test_new_datum = tag_loader::Get_tag_datum("shaders\\shader_templates\\opaque\\tex_bump_alpha_test_single_pass_new", blam_tag::tag_group_type::shadertemplate, "new_shaders");
			auto alpha_test_new_pass_datum = tag_loader::Get_tag_datum("shaders\\shader_passes\\lightmap\\lightmap_tex_detail_alpha_test_new", blam_tag::tag_group_type::shaderpass, "new_shaders");
			auto alpha_test_datum = tags::find_tag(blam_tag::tag_group_type::shadertemplate, "shaders\\shader_templates\\opaque\\tex_bump_alpha_test_single_pass");

			if (alpha_test_new_datum != DATUM_INDEX_NONE && alpha_test_datum != DATUM_INDEX_NONE)
			{
				auto alpha_test = tags::get_tag<blam_tag::tag_group_type::shadertemplate, shader_template_group_definition>(alpha_test_datum);

				tag_loader::Load_tag(alpha_test_new_datum, true, "new_shaders");
				tag_loader::Push_Back();

				alpha_test->postprocessDefinition[0]->passes[0]->pass.TagIndex = tag_loader::ResolveNewDatum(alpha_test_new_pass_datum);
				alpha_test->postprocessDefinition[0]->passes[6]->pass.TagIndex = tag_loader::ResolveNewDatum(alpha_test_new_pass_datum);
			}
		}
		void fix_dynamic_lights()
		{
			// TODO FIXME: this breaks other shadows
			return;

			auto cinematic_shadow_datum = tags::find_tag(blam_tag::tag_group_type::vertexshader, "rasterizer\\vertex_shaders_dx9\\shadow_buffer_generation_cinematic");
			auto shadow_datum = tags::find_tag(blam_tag::tag_group_type::shaderpass, "shaders\\shader_passes\\shadow\\shadow_generate");
			byte* shadow_tag = tags::get_tag<blam_tag::tag_group_type::shaderpass, BYTE>(shadow_datum);
			if(shadow_tag != nullptr)
			{
				auto *shadow_pp = reinterpret_cast<tags::tag_data_block*>(shadow_tag + 0x1C);
				if(shadow_pp->block_count > 0 && shadow_pp->block_data_offset != -1)
				{
					auto shadow_pp_data = tags::get_tag_data() + shadow_pp->block_data_offset;
					auto *shadow_impl_block = reinterpret_cast<tags::tag_data_block*>(shadow_pp_data);
					if(shadow_impl_block->block_count > 0 && shadow_impl_block->block_data_offset != -1)
					{
						auto *shadow_impl = tags::get_tag_data() + shadow_impl_block->block_data_offset;
						tag_reference* impl_1 = reinterpret_cast<tag_reference*>(shadow_impl + (0x14A) + 0xFC);
						tag_reference* impl_2 = reinterpret_cast<tag_reference*>(shadow_impl + (0x14A*2) + 0xFC);

						impl_1->TagIndex = cinematic_shadow_datum;
						//TODO: Re-enable this once the vertex shaders for shadows are fixed.
						//impl_2->TagIndex = cinematic_shadow_datum.data;
					}
				}
			}
		}
		void ShaderSpecularFix()
		{
			//Fix the Master Chief FP Arms Shader
			auto fp_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\masterchief\\fp\\shaders\\fp_arms");
			if (fp_shader_datum != DATUM_INDEX_NONE)
			{
				auto fp_shader = tags::get_tag<blam_tag::tag_group_type::shader, shader_definition>(fp_shader_datum);
				fp_shader->lightmapSpecularBrightness = 1.0f;
			}

			//Fix the Visor(s)
			auto tex_bump_env_datum = tags::find_tag(blam_tag::tag_group_type::shadertemplate, "shaders\\shader_templates\\opaque\\tex_bump_env");

			//Fix the Visor
			auto visor_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\masterchief\\shaders\\masterchief_visor");
			if (visor_shader_datum != DATUM_INDEX_NONE)
			{
				auto visor_shader = tags::get_tag_fast<shader_definition>(visor_shader_datum);
				visor_shader->postprocessDefinition[0]->shaderTemplateIndex = tex_bump_env_datum;
			}

			//Fix the Grunt Shaders
			auto grunt_arm_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\grunt\\shaders\\grunt_arms");
			if (grunt_arm_shader_datum != DATUM_INDEX_NONE)
			{
				auto grunt_arm_shader = tags::get_tag_fast<shader_definition>(grunt_arm_shader_datum);
				grunt_arm_shader->lightmapSpecularBrightness = 1.0f;
			}

			auto grunt_backpack_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\grunt\\shaders\\grunt_backpack");
			if (grunt_backpack_shader_datum != DATUM_INDEX_NONE)
			{
				auto grunt_backpack_shader = tags::get_tag_fast<shader_definition>(grunt_backpack_shader_datum);
				grunt_backpack_shader->lightmapSpecularBrightness = 1.0f;
			}

			auto grunt_torso_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\grunt\\shaders\\grunt_torso");
			if (grunt_torso_shader_datum != DATUM_INDEX_NONE)
			{
				auto grunt_torso_shader = tags::get_tag_fast<shader_definition>(grunt_torso_shader_datum);
				grunt_torso_shader->lightmapSpecularBrightness = 1.0f;
			}

		}

		void font_table_fix()
		{
			WriteValue<int>(Memory::GetAddress(0x464940), 0);
			WriteValue<int>(Memory::GetAddress(0x46494C), 0);
			WriteValue<int>(Memory::GetAddress(0x464958), 0);
			WriteValue<int>(Memory::GetAddress(0x464964), 0);
		}
		void shader_lod_max()
		{
			auto shaders = tags::find_tags(blam_tag::tag_group_type::shader);
			for (auto& shader_item : shaders)
			{
				auto shader = tags::get_tag<blam_tag::tag_group_type::shader, shader_definition>(shader_item.first);
				shader->shaderLODBias = shader_definition::_Never;
			}
		}

		//Hacky thingy for flags in the light tagblock
		//Should Prob Remove when a better solution is found
		//Apparently this is the best solution?????
		template<class T> inline T operator~ (T a) { return (T)~(int)a; }
		template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
		template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
		template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
		template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
		template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
		template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

		void light_framerate_killer()
		{
			auto lights = tags::find_tags(blam_tag::tag_group_type::light);
			for (auto& light_item : lights)
			{
				auto light = tags::get_tag<blam_tag::tag_group_type::light, s_light_group_definition>(light_item.first);
				light->flags |= s_light_group_definition::e_flags::light_framerate_killer;
			}
		}

		void fall_damage_fix()
		{
			auto bipeds = tags::find_tags(blam_tag::tag_group_type::biped);
			for (auto& biped_item : bipeds)
			{
				auto biped = tags::get_tag<blam_tag::tag_group_type::biped, s_biped_group_definition>(biped_item.first);
				biped->unitTag.objectTag.min_abs_acc_default *= 2.0f;
				biped->unitTag.objectTag.max_abs_acc_default *= 2.0f;
			}
		}
	}

	void OnMapLoad()
	{
		if (!Memory::IsDedicatedServer()) {
			if (h2mod->GetEngineType() != e_engine_type::_main_menu) {
				fix_shader_template_nvidia();
			}
			ShaderSpecularFix();
			fix_dynamic_lights();
			font_table_fix();
			if (H2Config_shader_lod_max)
			{
				shader_lod_max();
			}
			if (H2Config_light_suppressor)
			{
				light_framerate_killer();
			}
		}
		// both client/server
		fall_damage_fix();
	}

	void Initalize()
	{
		tags::on_map_load(OnMapLoad);
	}
}
