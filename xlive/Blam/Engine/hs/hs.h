#pragma once
#include "Blam/Cache/DataTypes/StringID.h"
#include "Blam/Math/real_math.h"

namespace hs
{
	enum e_hs_networked_fuction_type : byte
	{
		_hs_fade_out = 0
	};

	struct s_networked_hs_function
	{
		hs::e_hs_networked_fuction_type function_type;
		byte arg_size_in_bits;
		void* args;
	};

	struct hs_fade_args
	{
		real_color_rgb color;
		__int16 ticks;
	};

	void UnitKill(datum unitDatum);
	bool UnitInVehicle(datum unitDatum);
	float UnitGetHealth(datum unitDatum);
	float UnitGetShield(datum unitDatum);

	void PhysicsSetGravity(float Multiplier);
	void PhysicsSetVelocityFrame(float unk1, float unk2, float unk3);
	void RenderLightsEnableCinenaticShadow(bool unk1, datum objectDatum, string_id StringId, float unk2);
	void ObjectDestroy(datum object_datum_index);

	void CallNetworkedHSFunction(const s_networked_hs_function* data);
	void Initialize();
}
