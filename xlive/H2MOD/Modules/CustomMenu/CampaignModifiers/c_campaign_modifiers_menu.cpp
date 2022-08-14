#include "stdafx.h"

#include "c_campaign_modifiers_menu.h"

#include "H2MOD\GUI\GUI.h"
#include "H2MOD\Utils\Utils.h"
#include "Util\Hooks\Hook.h"
#include "H2MOD\Modules\CampaignModifiers\CampaignModifiers.h"

#define CUSTOM_GAME_MENU_ID 60

static void CampaignModifiersSetupButtons() {
	add_cartographer_label(CMLabelMenuId_CampaignModifiers, 1, H2CustomLanguageGetLabel(CMLabelMenuId_CampaignModifiers, 0xFFFF0000), true);
	add_cartographer_label(CMLabelMenuId_CampaignModifiers, 2, H2CustomLanguageGetLabel(CMLabelMenuId_CampaignModifiers, 0xFFFF0001), true);
	add_cartographer_label(CMLabelMenuId_CampaignModifiers, 3, H2CustomLanguageGetLabel(CMLabelMenuId_CampaignModifiers, 0xFFFF0002), true);
	add_cartographer_label(CMLabelMenuId_CampaignModifiers, 4, H2CustomLanguageGetLabel(CMLabelMenuId_CampaignModifiers, 0xFFFF0003), true);
	add_cartographer_label(CMLabelMenuId_CampaignModifiers, 5, H2CustomLanguageGetLabel(CMLabelMenuId_CampaignModifiers, 0xFFFF0004), true);
	add_cartographer_label(CMLabelMenuId_CampaignModifiers, 6, H2CustomLanguageGetLabel(CMLabelMenuId_CampaignModifiers, 0xFFFF0005), true);
	add_cartographer_label(CMLabelMenuId_CampaignModifiers, 7, H2CustomLanguageGetLabel(CMLabelMenuId_CampaignModifiers, 0xFFFF0006), true);
	add_cartographer_label(CMLabelMenuId_CampaignModifiers, 8, H2CustomLanguageGetLabel(CMLabelMenuId_CampaignModifiers, 0xFFFF0007), true);
}

void* __cdecl c_campaign_modifiers_list_menu::open(s_new_ui_screen_parameters* parameters)
{
	c_campaign_modifiers_list_menu* campaign_modifiers_list_menu = nullptr;
	BYTE* ui_buffer = (BYTE*)ui_memory_pool_allocate(sizeof(c_campaign_modifiers_list_menu), 0);

	CampaignModifiersSetupButtons();

	if (ui_buffer) {
		campaign_modifiers_list_menu = new (ui_buffer) c_campaign_modifiers_list_menu(
			parameters->ui_channel,
			parameters->field_8, HIWORD(parameters->flags)
		); // manually call the constructor
	}

	campaign_modifiers_list_menu->field_6C = true;
	user_interface_register_screen_to_channel(campaign_modifiers_list_menu, parameters);
	return campaign_modifiers_list_menu;
}

c_campaign_modifiers_list_menu::c_campaign_modifiers_list_menu(int _ui_channel, int a4, __int16 _flags) :
	c_screen_with_menu(CUSTOM_GAME_MENU_ID, _ui_channel, a4, _flags, &this->campaign_modifiers_list, true),
	campaign_modifiers_list(_flags)
{
}

c_campaign_modifiers_list::c_campaign_modifiers_list(int _flags) :
	c_list_widget(_flags, true)
{
	auto sub_2113C6 = Memory::GetAddress<int(__thiscall*)(void*)>(0x2113C6);
	auto sub_2113D3 = Memory::GetAddress<void* (__thiscall*)(void*, void*)>(0x2113D3);
	auto c_list_item_widget_ctor = Memory::GetAddress<void* (__thiscall*)(void*)>(0x21FFC9);
	auto c_list_item_widget_dctor = Memory::GetAddress<void(__thiscall*)(void*)>(0xA551);
	auto p_vector_constructor = Memory::GetAddress<void(__stdcall*)(void*, unsigned int, int, void* (__thiscall*)(void*), void(__thiscall*)(void*))>(0x28870B);

	// this constructs the buttons list widgets drawn inside the window
	// brightness menu can display 4 widget list buttons at once
	p_vector_constructor(this->item_list, 132, 8, c_list_item_widget_ctor, c_list_item_widget_dctor);

	this->field_2C0 = 0;
	sub_2113C6(&this->slot_2_unk.field_8);

	//this->slot_2_unk.c_slot_vtbl = &off_1229700;
	// this->slot_2_unk.c_slot_vtbl = NULL;
	// not sure why this exists, it just calls the function to the button handler
	// TODO implement virtual interface for slot_1, slot_2
	this->slot_2_unk.c_slot_vtbl = Memory::GetAddressRelative<void*>(0x7D9700);
	this->slot_2_unk.ui_screen_edit_list = this;
	this->slot_2_unk.button_handler_cb = &c_campaign_modifiers_list::button_handler;
	this->default_selected_button = 0;

	int button_count = 8;
	s_data_array* display_mode_list_data = allocate_list_data("campaign modifiers list", button_count, 4);
	this->list_data_array = display_mode_list_data;

	s_data_array::data_make_valid(display_mode_list_data);

	for (int i = 0; i < this->list_data_array->datum_max_elements; i++) {
		s_data_array::datum_new_in_range(this->list_data_array);
	}

	// no fucking clue what's this, maybe related to this->slot_2_unk.c_slot_vtbl data offset
	// because this->slot_2_unk.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-708) {
		sub_2113D3(&this->gap_70[56], nullptr);
	}
	else {
		sub_2113D3(&this->gap_70[56], &this->slot_2_unk.field_8);
	}
}

void c_campaign_modifiers_list::button_handler(int* a2, int* a3)
{
	int button_id = *(int*)a3 & 0xFFFF;

	int parent_screen_idx = this->get_top_most_parent_widget_index();
	int parent_screen_ui_channel = this->get_top_most_parent_widget_ui_channel();

	bool close_parent_screen = false;

	s_new_ui_screen_parameters new_ui_account_list_screen;
	new_ui_account_list_screen.data_new(0, 1 << *(int*)(*a2 + 4), parent_screen_ui_channel, 4, c_campaign_modifiers_list_menu::open);

	switch (button_id)
	{
	case 0:
		CampaignModifiers::c_campaign_modifiers::SetModifier(CampaignModifiers::campaign_modifier_ogh2);
		break;
	case 1:
		CampaignModifiers::c_campaign_modifiers::SetModifier(CampaignModifiers::campaign_modifier_randomizer);
		break;
	case 2:
		CampaignModifiers::c_campaign_modifiers::SetModifier(CampaignModifiers::campaign_modifier_jackal_snipers);
		break;
	}

	if (close_parent_screen)
		user_interface_back_out_from_channel(parent_screen_ui_channel, parent_screen_idx);
	return;
}

__int8 _fastcall c_load_level_event_handler(int This, int EDX, s_event_record* a2)
{
	typedef byte(__fastcall p_c_screen_widget__handle_event)(int This, int EDX, s_event_record* a2);
	p_c_screen_widget__handle_event* c_screen_widget__handle_event;
	c_screen_widget__handle_event = Memory::GetAddress<p_c_screen_widget__handle_event*>(0x20EB2B);

	if (a2->type != 5 || a2->field_8 != 2)
		return c_screen_widget__handle_event(This, 0, a2);
	CallWgit(c_campaign_modifiers_list_menu::open);
	return 0;
}