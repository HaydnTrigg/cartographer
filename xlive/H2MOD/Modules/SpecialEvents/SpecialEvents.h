#pragma once
namespace SpecialEvents
{
	enum e_event_type
	{
		_no_event = 0,
		_christmas = 1,
		_st_paddys = 2,
		_mook_maddness = 3,
		_halloween = 4 
	};

	e_event_type getCurrentEvent();
	void ChristmasOnMapLoad();
	void PaddysOnMapLoad();
	void AddNewMarkers();
	void ApplyHooks();
	void Initialize();
}