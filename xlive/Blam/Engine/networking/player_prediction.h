#pragma once



struct s_player_prediction
{
	int predicted_entity_index;
	real_vector3d point_quantized;
	real_vector3d prediction_axis;
	real_vector3d prediction_axis2;
	real_point3d prediction_translation_velocity;
	real_point3d prediction_angular_velocity;
};
ASSERT_STRUCT_SIZE(s_player_prediction, 0x40);
