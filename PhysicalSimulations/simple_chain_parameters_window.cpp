#include "simple_chain_parameters_window.h"
#include "imgui_guards.h"

SimpleChainParametersWindow::SimpleChainParametersWindow(SimpleChain& chain, SimpleChainViewWindow& view) : chain(chain), view(view)
{
	name = "Parameters";
}

const char* modes[] = { "Edit", "Pathfinding" };
const char* set_modes[] = { "Start", "End" };

void SimpleChainParametersWindow::build()
{
	ImGui::Begin(get_name());
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();

	if (ImGui::Combo("Mode", &current_mode, modes, 2))
	{
		view.edit_mode = current_mode == 0;
		show_path_error = false;
	}

	ImGui::SeparatorText("Chain");
	if (view.edit_mode)
	{
		ImGui::SliderFloat("l1", &chain.l1, 0.0f, 10.0f, NULL, ImGuiSliderFlags_NoInput);
		ImGui::SliderFloat("l2", &chain.l2, 0.0f, 10.0f, NULL, ImGuiSliderFlags_NoInput);
	}
	else
	{
		static int set_mode = 0;
		if (ImGui::Combo("Set position", &set_mode, set_modes, 2))
			view.set_end_position = set_mode == 1;
	}

	ImGui::SeparatorText("Parameter map");
	ImGui::Image(chain.parameter_space_map.get_texture_pointer(), ImVec2(canvas_sz.x, canvas_sz.x));

	if (!view.edit_mode)
	{
		if (ImGui::Button("Seek path")) {
			chain.parameter_space_map.render(chain);
			path = chain.parameter_space_map.seek_path(view.current_angles, view.current_end_angles);
			show_path_error = path.empty();
		}
		if (show_path_error)
			ImGui::Text("Path not found!");
		if (!path.empty()) {
			ImGui::SeparatorText("Animation");
			float duration = 1.0f / chain.animation_parameters.speed;
			if (GuardedSliderFloat("Duration", &duration, 0.1f, 10.0f, nullptr, ImGuiSliderFlags_None, 0.1f, INFINITY))
				chain.animation_parameters.speed = 1.0f / duration;
			if (chain.is_animating())
			{
				if (ImGui::Button(chain.animation_parameters.paused ? "Resume" : "Pause"))
				{
					chain.animation_parameters.paused = !chain.animation_parameters.paused;
				}
				ImGui::SameLine();
				if (ImGui::Button("Stop"))
				{
					if (current_task != nullptr)
						current_task->terminate();
					chain.animation_parameters.current_angles = view.current_angles; // reset chain
					chain.animation_parameters.paused = false;
				}
				ImGui::Text("Current angles: (%.0f,%.0f)", chain.animation_parameters.current_angles.x * (180.0f / PI), chain.animation_parameters.current_angles.y * (180.0f / PI));
			}
			else
			{
				if (ImGui::Button("Start"))
				{
					current_task = &start_task(chain.create_animation_task(path));
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					chain.animation_parameters.current_angles = view.current_angles; // reset chain
					chain.animation_parameters.paused = false;
				}
			}
		}
	}

	ImGui::End();
}
