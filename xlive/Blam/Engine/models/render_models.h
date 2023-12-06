#pragma once
#include "Blam/Engine/math/matrix_math.h"
#include "Blam/Engine/tag_files/string_id.h"

#define MAXIMUM_REGIONS_PER_MODEL 16
#define MAXIMUM_PERMUTATIONS_PER_MODEL_REGION 32
#define MAXIMUM_NODES_PER_MODEL 255
#define MAXIMUM_SECTIONS_PER_RENDER_MODEL 255
#define NUMBER_OF_GEOMETRY_DETAIL_LEVELS 6
#define MAXIMUM_NODES_PER_RENDER_MODEL_SECTION 40
#define MAXIMUM_MARKER_GROUPS_PER_RENDER_MODEL 4096
#define MAXIMUM_MARKERS_PER_RENDER_MODEL_MARKER_GROUP 256

int32 __cdecl render_model_find_marker_group_by_name(datum render_model_index, string_id name);

void __cdecl render_model_apply_two_bone_ik(datum render_model_index, 
	int32 node_index,
	real_matrix4x3* matrix_1,
	real_matrix4x3* matrix_2,
	real32 ratio,
	int32 node_matrices_count,
	real_matrix4x3* out);
