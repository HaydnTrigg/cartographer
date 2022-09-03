#pragma once

#include "..\CustomLanguage.h"
#include "..\CustomMenuGlobals.h"

enum e_user_interface_event_types :unsigned __int32
{
	_user_interface_event_type0 = 0x0,
	_user_interface_event_type1 = 0x1,
	_user_interface_event_type2 = 0x2,
	_user_interface_event_type3 = 0x3,
	_user_interface_event_type4 = 0x4,
	_user_interface_event_type5 = 0x5,
	_user_interface_event_type6 = 0x6,
	_user_interface_event_type7 = 0x7,
	_user_interface_event_type8 = 0x8,
	_user_interface_event_type9 = 0x9,
	_user_interface_event_type10 = 0xA,
	_user_interface_event_type11 = 0xB,
	_user_interface_event_type_automation = 0xC,
	_user_interface_event_type13 = 0xD,
	_user_interface_event_type14 = 0xE,
	_user_interface_event_type15 = 0xF,
};

enum e_user_interface_automation_modes : unsigned __int16
{
	_user_interface_automation_mode_none = 0x0,
	_user_interface_automation_mode_campaign_single_player = 0x1,
	_user_interface_automation_mode_splitscreen_multiplayer = 0x2,
	_user_interface_automation_mode_system_link_auto = 0x3,
	_user_interface_automation_mode_system_link_auto_host = 0x4,
	_user_interface_automation_mode_system_link_auto_join = 0x5,
	_user_interface_automation_mode_system_link_manual = 0x6,
	_user_interface_automation_mode_xbox_live_auto = 0x7,
	_user_interface_automation_mode_xbox_live_auto_host = 0x8,
	_user_interface_automation_mode_xbox_live_auto_join = 0x9,
	_user_interface_automation_mode_xbox_live_manual = 0xA,
	_user_interface_automation_mode_matchmaking = 0xB,
	k_user_interface_automation_modes_count = 0xC,
};

struct s_event_record
{
	e_user_interface_event_types type;
	DWORD controller;
	DWORD field_8;
	e_user_interface_automation_modes automation_mode;
};

class c_campaign_modifiers_list : public c_list_widget
{
public:
	// 8 items displayed at once in the list
	char item_list[132 * 8];
	int field_2C0;
	c_slot2<c_campaign_modifiers_list> slot_2_unk;
	int default_selected_button;
	// button handler callback

	c_campaign_modifiers_list(int _flags);

	virtual char* get_item_list() override
	{
		return item_list; // returns pointer to edit list
	}

	virtual int get_list_item_count() override
	{
		// returns edit list count
		return 8;
	}

	void get_label(int a1, int a2) override
	{
		auto p_sub_211909 = Memory::GetAddress<int(__thiscall*)(int, int, int, int)>(0x211909);
		auto p_sub_21bf85 = Memory::GetAddress<void(__thiscall*)(int, int)>(0x21bf85);

		// a1 = ptr to account_list_items[idx]
		__int16 list_item_index = *(WORD*)(a1 + 112);
		int v3 = p_sub_211909(a1, 6, 0, 0);
		if (v3)
		{
			set_widget_label_from_string_id_reimpl(v3, list_item_index + 1, CMLabelMenuId_CampaignModifiers);
		}
	}

	// button handler
	void button_handler(int* a2, int* a3);
};
static_assert(offsetof(c_campaign_modifiers_list, slot_2_unk.field_8) == 1240);
static_assert(offsetof(c_campaign_modifiers_list, list_data_array) == 112);
static_assert(offsetof(c_campaign_modifiers_list, gap_70[56]) == 172);

class c_campaign_modifiers_list_menu : protected c_screen_with_menu
{
public:
	static void* __cdecl open(s_new_ui_screen_parameters* a1);

	c_campaign_modifiers_list_menu(int _ui_channel, int a4, __int16 _flags);

	// interface
	virtual int custom_deconstructor(BYTE flags) override
	{
		return c_screen_with_menu::custom_deconstructor(flags);
	};

	// c_screen_with_menu specific interface
	virtual int IUnkFunc23(int a2) override
	{
		// int __stdcall sub_2111ab_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description)
		return sub_2111ab_CMLTD((int)this, a2, CMLabelMenuId_CampaignModifiers, 0xFFFFFFF0, 0xFFFFFFF1);
		// return c_screen_with_menu::IUnkFunc23(a2);
	}

	virtual int IUnkFunc24() override
	{
		campaign_modifiers_list.set_selected_list_button_idx(campaign_modifiers_list.default_selected_button != -1 ? campaign_modifiers_list.default_selected_button : 0);
		return c_screen_with_menu::IUnkFunc24();
	}

	virtual void* get_open_menu_cb() override
	{
		return c_campaign_modifiers_list_menu::open;
	}

	c_campaign_modifiers_list campaign_modifiers_list;
private:
};

__int8 _fastcall c_load_level_event_handler(int This, int EDX, s_event_record* a2);
