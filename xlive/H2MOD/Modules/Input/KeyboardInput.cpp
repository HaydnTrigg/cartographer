#include "stdafx.h"

#include "KeyboardInput.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\GUI\GUI.h"
#include "H2MOD\GUI\imgui_integration\imgui_handler.h"
#include "H2MOD\GUI\imgui_integration\Console\ImGui_ConsoleImpl.h"
#include "H2MOD\Modules\Shell\Startup\Startup.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Utils\Utils.h"
#include "Util\Hooks\Hook.h"


extern LPDIRECT3DDEVICE9 pDevice;
static BYTE enableKeyboard3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
RECT rectScreenOriginal;


//Leveraging this call to unset the controller state
typedef void(__cdecl sub_B524F7_t)(signed int a1);
sub_B524F7_t* p_sub_B524F7;

__int16 last_user_index;
//Patching this call to enable keyboards to switch death targets
unsigned char* __cdecl death_cam_get_controller_input(__int16 a1)
{
	last_user_index = a1;
	unsigned char* result = ControllerInput::get_controller_input(a1);
	//Modifies the result for A button pressed if space is.
	unsigned char keyboard_space_key_state = KeyboardInput::GetGameKbState(VK_SPACE);
	if (keyboard_space_key_state > 0)
	{
		result[16] = keyboard_space_key_state;
	}
	return result;
}

void __cdecl sub_B524F7(signed int a1)
{
	unsigned char* result = ControllerInput::get_controller_input(last_user_index);
	result[16] = 0;
}

unsigned char KeyboardInput::GetGameKbState(__int16 keycode)
{
	auto input_abstraction_get_key_state_byte = Memory::GetAddress<unsigned char(__cdecl*)(__int16 key_code)>(0x2EF86);
	return input_abstraction_get_key_state_byte(keycode);
}

void KeyboardInput::ToggleKeyboardInput()
{
	if (H2Config_disable_ingame_keyboard) {
		//Allows to repeat last movement when lose focus in mp, unlocks METHOD E from point after intro vid
		BYTE getFocusB[] = { 0x00 };
		WriteBytes(H2BaseAddr + 0x2E3C5, getFocusB, 1);

		//Allows input when not in focus.
		BYTE getFocusE[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		WriteBytes(H2BaseAddr + 0x2F9EA, getFocusE, 6);
		WriteBytes(H2BaseAddr + 0x2F9FC, getFocusE, 6);
		WriteBytes(H2BaseAddr + 0x2FA09, getFocusE, 6);
	}
	else {
		//Reset them all back.
		BYTE getFocusB[] = { 0x01 };
		WriteBytes(H2BaseAddr + 0x2E3C5, getFocusB, 1);

		//dont allow input when not in focus.
		BYTE getFocusE[] = { 0x0F, 0x85, 0x02, 0x02, 0x00, 0x00 };
		WriteBytes(H2BaseAddr + 0x2F9EA, getFocusE, 6);
		getFocusE[2] = 0xF0;
		getFocusE[3] = 0x01;
		WriteBytes(H2BaseAddr + 0x2F9FC, getFocusE, 6);
		getFocusE[2] = 0xE3;
		WriteBytes(H2BaseAddr + 0x2FA09, getFocusE, 6);
	}
}
std::map<int*, std::function<void()>> hotKeyMap;
void KeyboardInput::RegisterHotkey(int* hotkey, std::function<void()> callback)
{
	if(hotKeyMap.count(hotkey) == 0)
	{
		hotKeyMap.emplace(hotkey, callback);
	} 
	else
	{
		LOG_ERROR_GAME("Hotkey {} is already in use", GetVKeyCodeString(*hotkey));
	}
}

void KeyboardInput::RemoveHotkey(int* hotkey)
{
	hotKeyMap.erase(hotkey);
}

void KeyboardInput::ExecuteHotkey(WPARAM message)
{
	for(auto &hk : hotKeyMap)
	{
		if (*hk.first == message)
			hk.second();
	}
}

void hotkeyFuncHelp() {
	addDebugText("------------------------------");
	addDebugText("Options:");
	addDebugText("%s - Print and show this help text.", GetVKeyCodeString(H2Config_hotkeyIdHelp).c_str());
	addDebugText("%s - Toggles hiding the in-game chat menu.", GetVKeyCodeString(H2Config_hotkeyIdToggleHideIngameChat).c_str());
	addDebugText("%s - Toggles hiding the Console Menu.", GetVKeyCodeString(H2Config_hotkeyIdConsole).c_str());
	addDebugText("------------------------------");

	if (!ImGuiHandler::IsWindowActive(Console::windowName))
	{
		ImGuiHandler::ToggleWindow(Console::windowName);
	}
	GetMainConsoleInstance()->SelectTab(_console_tab_logs);
}

void hotkeyFuncToggleHideIngameChat() {
	H2Config_hide_ingame_chat = !H2Config_hide_ingame_chat;
	if (H2Config_hide_ingame_chat) {
		addDebugText("Hiding in-game chat menu.");
	}
	else {
		addDebugText("Showing in-game chat menu.");
	}
}
void hotkeyFuncGuide() {
	ImGuiHandler::ToggleWindow(ImGuiHandler::ImAdvancedSettings::windowName);
}
void hotkeyFuncDebug() {
	ImGuiHandler::ToggleWindow(ImGuiHandler::ImDebugOverlay::windowName);
}
void hotkeyFuncConsole() {
	ImGuiHandler::ToggleWindow(Console::windowName);
}

int pause = VK_PRIOR;
void KeyboardInput::Initialize()
{
	p_sub_B524F7 = Memory::GetAddress<sub_B524F7_t*>(0x824F7);
	PatchCall(Memory::GetAddress(0xCDEF3), death_cam_get_controller_input);
	PatchCall(Memory::GetAddress(0xCDF5E), sub_B524F7);
	if (!enableKeyboard3[0]) {
		for (int i = 0; i < 6; i++) {
			enableKeyboard3[i] = *((BYTE*)H2BaseAddr + 0x2FA67 + i);
		}
	}
	ToggleKeyboardInput();
	addDebugText("Registering Hotkeys");
	KeyboardInput::RegisterHotkey(&H2Config_hotkeyIdHelp, hotkeyFuncHelp);
	KeyboardInput::RegisterHotkey(&H2Config_hotkeyIdGuide, hotkeyFuncGuide);
	KeyboardInput::RegisterHotkey(&H2Config_hotkeyIdConsole, hotkeyFuncConsole);
	// KeyboardInput::RegisterHotkey(&pause, hotkeyFuncDebug);
}
