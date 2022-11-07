#pragma once

#include "Blam\Math\real_math.h"
#include "H2MOD\Variants\VariantSystem.h"

enum e_headhunter_sounds
{
	_snd_head_hunter,
	_snd_skull_scored,
	_snd_skull_taken
};

extern __int8 skull_count[16];
extern std::unordered_map<int, std::unordered_map<e_headhunter_sounds, const wchar_t*>> headhunterSoundTable;
extern bool b_firstSpawn;

class HeadHunter : public ICustomGameVariant
{
public:
	virtual void Initialize() override;
	virtual void Dispose() override;
	virtual CustomVariantId GetVariantId();

	// on map load can be used as Initialize
	virtual void OnMapLoad(ExecTime execTime, s_game_options* gameOptions) override;
	virtual void OnPlayerSpawn(ExecTime execTime, datum playerIdx) override;
	virtual void OnObjectDamage(ExecTime execTime, datum unitDatumIdx, int a2, bool a3, bool a4) override
	{
	}

	// unused bellow
	virtual void OnPlayerDeath(ExecTime execTime, datum playerIdx) override;

	virtual bool OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx) override;

	virtual bool OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6) override;

	HeadHunter();
	~HeadHunter();
	static void initClient();
	static void SpawnSkull(datum playerIdx);
	static void PickupSkull(datum playerIdx, datum skullDatum);
	static void TriggerSound(e_headhunter_sounds sound, int sleep);
	static void SpawnPlayerClientSetup();
};

void SendSkullCounts();
void update_text_headhunter_hook(int datum_index, wchar_t* player_name);
void draw_text_at_offset(wchar_t* string, real_point3d location);
void draw_skull_indicators();
