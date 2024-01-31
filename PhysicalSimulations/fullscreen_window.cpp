#include "fullscreen_window.h"

void FullscreenWindow::build()
{
	// We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
		// Based on your use case you may want one of the other.
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin(get_name(), nullptr, flags);

	ImGuiID dockspace_id = ImGui::GetID(get_name());
	ImGui::DockSpace(dockspace_id);

	if (first_time)
	{
		first_time = false;
		ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		dockspace.create_dock_nodes(dockspace_id);

		/*std::vector<std::pair<const char*, ImGuiID>> dock_ids;
		dock_ids.reserve(docked_windows.size());

		for (auto dw : docked_windows)
		{
			if (dw.direction != ImGuiDir_None)
			{
				auto dock_id = ImGui::DockBuilderSplitNode(dockspace_id, dw.direction, dw.size_ratio, nullptr, &dockspace_id);
				dock_ids.push_back({ dw.name, dock_id });
			}
			else
			{
				dock_ids.push_back({ dw.name, dockspace_id });
			}
		}

		for (auto id : dock_ids)
		{
			ImGui::DockBuilderDockWindow(id.first, id.second);
		}*/
		ImGui::DockBuilderFinish(dockspace_id);
	}

	ImGui::End();
}
