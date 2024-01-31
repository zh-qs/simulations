#pragma once

#include "imgui.h"
#include "window.h"
#include <memory>
#include <tuple>
#include <list>

class Dockspace {
	std::list<const char*> docked_window_names;
	//const char* docked_window_name = nullptr;
	bool is_leaf = true;

	ImGuiDir split_direction = ImGuiDir_None;
	float split_size_ratio;
	std::unique_ptr<Dockspace> in_dir;
	std::unique_ptr<Dockspace> out_dir;

public:
	std::pair<Dockspace*, Dockspace*> split(ImGuiDir direction, float size_ratio) {
		assert(is_leaf);
		
		in_dir = std::make_unique<Dockspace>();
		out_dir = std::make_unique<Dockspace>();

		split_direction = direction;
		split_size_ratio = size_ratio;

		is_leaf = false;

		return { in_dir.get(), out_dir.get() };
	}

	void dock(const Window& window) {
		assert(is_leaf);
		docked_window_names.push_back(window.get_name());
	}

	void create_dock_nodes(ImGuiID id) {
		if (is_leaf)
		{
			for (const auto* name : docked_window_names)
				ImGui::DockBuilderDockWindow(name, id);
		}
		else
		{
			auto in_id = ImGui::DockBuilderSplitNode(id, split_direction, split_size_ratio, nullptr, &id);
			in_dir->create_dock_nodes(in_id);
			out_dir->create_dock_nodes(id);
		}
	}
};