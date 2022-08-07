#include "stdafx.h"

#include "c_display_mode_menu.h"

#include "H2MOD\GUI\GUI.h"
#include "H2MOD\Utils\Utils.h"
#include "Util\Hooks\Hook.h"

#include "..\c_brightness_menu.h"


typedef char (__cdecl p_set_display_mode)(char a1, int a2);
p_set_display_mode* set_display_mode;

static void DisplayModeSetupButtons() {
	add_cartographer_label(CMLabelMenuId_DisplayMode, 1, H2CustomLanguageGetLabel(CMLabelMenuId_DisplayMode, 0xFFFF0000), true);
	add_cartographer_label(CMLabelMenuId_DisplayMode, 2, H2CustomLanguageGetLabel(CMLabelMenuId_DisplayMode, 0xFFFF0001), true);
	add_cartographer_label(CMLabelMenuId_DisplayMode, 3, H2CustomLanguageGetLabel(CMLabelMenuId_DisplayMode, 0xFFFF0002), true);
}

void* __cdecl c_display_mode_list_menu::open(s_new_ui_screen_parameters* parameters)
{
	c_display_mode_list_menu* display_mode_list_menu = nullptr;
	BYTE* ui_buffer = (BYTE*)ui_memory_pool_allocate(sizeof(c_display_mode_list_menu), 0);

	DisplayModeSetupButtons();

	if (ui_buffer) {
		display_mode_list_menu = new (ui_buffer) c_display_mode_list_menu(
			parameters->ui_channel, 
			parameters->field_8, HIWORD(parameters->flags)
		); // manually call the constructor
	}

	display_mode_list_menu->field_6C = true;
	user_interface_register_screen_to_channel(display_mode_list_menu, parameters);
	return display_mode_list_menu;
}

c_display_mode_list_menu::c_display_mode_list_menu(int _ui_channel, int a4, __int16 _flags) :
	c_screen_with_menu(BRIGHTNESS_MENU_ID, _ui_channel, a4, _flags, &this->display_mode_list, true),
	display_mode_list(_flags)
{
}

c_display_mode_list::c_display_mode_list(int _flags) :
	c_list_widget(_flags, true)
{
	DWORD display_mode = 0;
	HKEY hKeyVideoSettings = NULL;

	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Halo 2\\Video Settings", 0, KEY_READ, &hKeyVideoSettings) == ERROR_SUCCESS)
	{
		GetDWORDRegKey(hKeyVideoSettings, L"DisplayMode", &display_mode);
		RegCloseKey(hKeyVideoSettings);
	}

	auto sub_2113C6 = Memory::GetAddress<int(__thiscall*)(void*)>(0x2113C6);
	auto sub_2113D3 = Memory::GetAddress<void* (__thiscall*)(void*, void*)>(0x2113D3);
	auto c_list_item_widget_ctor = Memory::GetAddress<void* (__thiscall*)(void*)>(0x21FFC9);
	auto c_list_item_widget_dctor = Memory::GetAddress<void(__thiscall*)(void*)>(0xA551);
	auto p_vector_constructor = Memory::GetAddress<void(__stdcall*)(void*, unsigned int, int, void* (__thiscall*)(void*), void(__thiscall*)(void*))>(0x28870B);

	// this constructs the buttons list widgets drawn inside the window
	// brightness menu can display 4 widget list buttons at once
	p_vector_constructor(this->item_list, 132, 4, c_list_item_widget_ctor, c_list_item_widget_dctor);

	this->field_2C0 = 0;
	sub_2113C6(&this->slot_2_unk.field_8);

	//this->slot_2_unk.c_slot_vtbl = &off_1229700;
	// this->slot_2_unk.c_slot_vtbl = NULL;
	// not sure why this exists, it just calls the function to the button handler
	// TODO implement virtual interface for slot_1, slot_2
	this->slot_2_unk.c_slot_vtbl = Memory::GetAddressRelative<void*>(0x7D9700);
	this->slot_2_unk.ui_screen_edit_list = this;
	this->slot_2_unk.button_handler_cb = &c_display_mode_list::button_handler;
	this->default_selected_button = display_mode;

	int button_count = 3;
	s_data_array* display_mode_list_data = allocate_list_data("display mode list", button_count, 4);
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

void SaveDisplayMode(DWORD Display_Mode)
{
	HKEY hKeyVideoSettings = NULL;
	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Halo 2\\Video Settings", 0, KEY_WRITE, &hKeyVideoSettings) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKeyVideoSettings, L"DisplayMode", 0, REG_DWORD, reinterpret_cast<BYTE *>(&Display_Mode), sizeof(Display_Mode));
		RegCloseKey(hKeyVideoSettings);
	}
}

void c_display_mode_list::button_handler(int* a2, int* a3)
{
	set_display_mode = Memory::GetAddress<p_set_display_mode*>(0x26444C);

	int button_id = *(int*)a3 & 0xFFFF;

	int parent_screen_idx = this->get_top_most_parent_widget_index();
	int parent_screen_ui_channel = this->get_top_most_parent_widget_ui_channel();

	bool close_parent_screen = false;

	s_new_ui_screen_parameters new_ui_account_list_screen;
	new_ui_account_list_screen.data_new(0, 1 << *(int*)(*a2 + 4), parent_screen_ui_channel, 4, c_display_mode_list_menu::open);

	switch (button_id)
	{
	case 0:
		SaveDisplayMode(0);
		WriteValue<int>(Memory::GetAddress(0xA3D9F8), 0);
		set_display_mode(1, 1);
		break;
	case 1:
		SaveDisplayMode(1);
		WriteValue<int>(Memory::GetAddress(0xA3D9F8), 1);
		set_display_mode(1, 1);
		break;
	case 2:
		SaveDisplayMode(2);
		SetBorderlessWindowed();
		break;
	}
	user_interface_back_out_from_channel(parent_screen_ui_channel, parent_screen_idx);

	if (close_parent_screen)
		user_interface_back_out_from_channel(parent_screen_ui_channel, parent_screen_idx);
	return;
}

void SetBorderlessWindowed()
{
	// Set display mode to 1 to setup windowed then 2 for new string id in menus
	WriteValue<int>(Memory::GetAddress(0xA3D9F8), 1);

	set_display_mode = Memory::GetAddress<p_set_display_mode*>(0x26444C);
	set_display_mode(1, 1);

	WriteValue<int>(Memory::GetAddress(0xA3D9F8), 2);

	HMONITOR monitor = MonitorFromWindow(H2hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info ={ sizeof(MONITORINFO), NULL, NULL, NULL};
	GetMonitorInfo(monitor, &info);
	int monitor_width = info.rcMonitor.right - info.rcMonitor.left;
	int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;

	SetWindowLongPtr(H2hWnd, GWL_STYLE, GetWindowLong(H2hWnd, GWL_STYLE) & ~(WS_THICKFRAME | WS_BORDER | WS_DLGFRAME));
	SetWindowPos(H2hWnd, NULL, 0, 0, monitor_width, monitor_height, 0);

}

int __cdecl display_mode_index_to_string_id(int display_mode)
{
	switch (display_mode)
	{
	case 1:
		return 0x130008AB;
	case 2:
		return 0x130008AC;
	default:
		return 0x110008AA;
	}
}

void display_mode_patches()
{
	// Replace display mode menu with our own
	WritePointer(Memory::GetAddress(0x249665 + 1), c_display_mode_list_menu::open);
	WritePointer(Memory::GetAddress(0x258FD2 + 4), c_display_mode_list_menu::open);

	// Replace function for displaying display mode strings
	PatchCall(Memory::GetAddress(0x24886C), display_mode_index_to_string_id);

	// Nop setting the display mode global to 0
	NopFill(Memory::GetAddress(0x248864), 7);
	NopFill(Memory::GetAddress(0x26473E), 19);
	NopFill(Memory::GetAddress(0x039F39), 41);

	// Nop setting the display mode global to 2
	NopFill(Memory::GetAddress(0x26475D), 21);
}
