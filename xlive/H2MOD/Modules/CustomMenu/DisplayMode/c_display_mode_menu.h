#pragma once

#include "..\CustomLanguage.h"
#include "..\CustomMenuGlobals.h"

class c_display_mode_list : public c_list_widget
{
public:
	// 3 items displayed at once in the list
	char item_list[132 * 3];
	int field_2C0;
	c_slot2<c_display_mode_list> slot_2_unk;
	int default_selected_button;
	// button handler callback

	c_display_mode_list(int _flags);

	virtual char* get_item_list() override
	{
		return item_list; // returns pointer to edit list
	}

	virtual int get_list_item_count() override
	{
		// returns edit list count
		return 3;
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
			set_widget_label_from_string_id_reimpl(v3, list_item_index + 1, CMLabelMenuId_DisplayMode);
		}
	}

	// button handler
	void button_handler(int* a2, int* a3);
};
static_assert(offsetof(c_display_mode_list, slot_2_unk.field_8) == 580);
static_assert(offsetof(c_display_mode_list, list_data_array) == 112);
static_assert(offsetof(c_display_mode_list, gap_70[56]) == 172);

class c_display_mode_list_menu : protected c_screen_with_menu
{
public:
	static void* __cdecl open(s_new_ui_screen_parameters* a1);

	c_display_mode_list_menu(int _ui_channel, int a4, __int16 _flags);

	// interface
	virtual int custom_deconstructor(BYTE flags) override
	{
		return c_screen_with_menu::custom_deconstructor(flags);
	};

	// c_screen_with_menu specific interface
	virtual int IUnkFunc23(int a2) override
	{
		// int __stdcall sub_2111ab_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description)
		return sub_2111ab_CMLTD((int)this, a2, CMLabelMenuId_DisplayMode, 0xFFFFFFF0, 0xFFFFFFF1);
		// return c_screen_with_menu::IUnkFunc23(a2);
	}

	virtual int IUnkFunc24() override
	{
		display_mode_list.set_selected_list_button_idx(display_mode_list.default_selected_button != -1 ? display_mode_list.default_selected_button : 0);
		return c_screen_with_menu::IUnkFunc24();
	}

	virtual void* get_open_menu_cb() override
	{
		return c_display_mode_list_menu::open;
	}

	c_display_mode_list display_mode_list;
private:
};

void display_mode_patches();
void SetBorderlessWindowed();