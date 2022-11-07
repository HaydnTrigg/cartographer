#include "stdafx.h"

#include "imgui.h"
#include "ImGui_NetworkStatsOverlay.h"

#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"
#include "H2MOD.h"
#include "H2MOD\Variants\HeadHunter\HeadHunter.h"

void ShowSkullCount(bool* p_open)
{
	if (p_open == nullptr
		|| !*p_open)
		return;

	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoMouseInputs
		;

	const float PAD = 10.0f;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_size.x / 2;
	window_pos.y = work_size.y / 1.25;
	window_pos_pivot.x = 0.5f;
	window_pos_pivot.y = 0.5f;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= 0
			| ImGuiWindowFlags_NoMove
			;

	ImGui::SetNextWindowBgAlpha(0.20f); // Transparent background
	if (ImGui::Begin("##HeadHunter Skull Count", p_open, window_flags))
	{
		__int8 absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0));

		//TODO Fix Skull Count for Clients
		ImGui::Text("SKULLS: %hhd", skull_count[absPlayerIdx]);
	}

	ImGui::End();
}
