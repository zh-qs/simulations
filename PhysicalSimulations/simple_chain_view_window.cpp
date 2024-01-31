#include "simple_chain_view_window.h"

Vector2 SimpleChainViewWindow::get_scene_point(const ImVec2& point, const ImVec2& p0, const ImVec2& size)
{
	const Vector2 clip_space_point{ (point.x - p0.x - 0.5f * size.x) / size.y, (point.y - p0.y) / size.y - 0.5f };
	return (2.0f / scale) * clip_space_point + center;
}

ImVec2 SimpleChainViewWindow::get_view_point(const Vector2& point, const ImVec2& p0, const ImVec2& size)
{
	const auto view_point = (0.5f * scale) * (point - center);

	return { p0.x + view_point.x * size.y + 0.5f * size.x, p0.y + (view_point.y + 0.5f) * size.y };
}

void SimpleChainViewWindow::draw_chain(const Vector2& angles, ImDrawList* draw_list, const ImVec2& p0, const ImVec2& size, ImU32 color)
{
	auto base = chain.get_base_position(),
		joint = chain.get_joint_position(angles.x),
		effector = chain.get_effector_position(angles.x, angles.y);

	draw_list->AddLine(get_view_point(base, p0, size), get_view_point(joint, p0, size), color);
	draw_list->AddLine(get_view_point(joint, p0, size), get_view_point(effector, p0, size), color);
}

void SimpleChainViewWindow::draw_obstacle(const Obstacle& obstacle, ImDrawList* draw_list, const ImVec2& p0, const ImVec2& size)
{
	draw_list->AddRectFilled(get_view_point(obstacle.first_corner, p0, size), get_view_point(obstacle.second_corner, p0, size), ImGui::GetColorU32({ obstacle.color.x,obstacle.color.y,obstacle.color.z,obstacle.color.w }));
}

SimpleChainViewWindow::SimpleChainViewWindow(SimpleChain& chain) : chain(chain)
{
	name = "2D Chain";
}

void SimpleChainViewWindow::build()
{
	ImGui::Begin(get_name());

	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
	//if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
	//if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
	ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
	ImGuiIO& io = ImGui::GetIO();

	// Draw background color
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(200, 190, 150, 255));

	// Draw axes
	auto view_center = get_view_point({ 0.0f,0.0f }, canvas_p0, canvas_sz);
	draw_list->AddLine({ canvas_p0.x, view_center.y }, { canvas_p1.x, view_center.y }, IM_COL32(255, 0, 0, 255));
	draw_list->AddLine({ view_center.x, canvas_p0.y }, { view_center.x, canvas_p1.y }, IM_COL32(0, 255, 0, 255));

	// mouse i/o
	if (ImGui::IsWindowHovered())
	{
		// zoom using mouse wheel
		if (io.MouseWheel != 0.0f) {
			scale *= powf(zoom_factor, io.MouseWheel);
		}
		// move scene
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
			Vector2 delta{ -2.0f * io.MouseDelta.x / (canvas_sz.x * scale), -2.0f * io.MouseDelta.y / (canvas_sz.y * scale) };
			center += delta;
		}

		// set chain position
		if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			auto point = get_scene_point(io.MousePos, canvas_p0, canvas_sz);
			auto angles = chain.get_angles(point);
			if (!angles.empty())
			{
				second_chain = !second_chain;
				show_chain = true;
				if (!edit_mode && set_end_position)
				{
					current_end_angles = second_chain ? angles.back() : angles.front();
					alternative_end_angles = second_chain ? angles.front() : angles.back();
				}
				else
				{
					current_angles = second_chain ? angles.back() : angles.front();
					alternative_angles = second_chain ? angles.front() : angles.back();
				}
			}
		}

		if (edit_mode)
		{
			// create obstacle
			if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				creating_obstacle = true;

				current_obstacle.first_corner = get_scene_point(io.MouseClickedPos[ImGuiMouseButton_Left], canvas_p0, canvas_sz);
				current_obstacle.second_corner = get_scene_point(io.MousePos, canvas_p0, canvas_sz);

				draw_list->AddRectFilled(io.MouseClickedPos[ImGuiMouseButton_Left], io.MousePos, IM_COL32(255, 255, 255, 255));
			}
			else if (creating_obstacle)
			{
				creating_obstacle = false;
				chain.obstacles.push_back(current_obstacle);
				chain.parameter_space_map.render(chain);
			}
			else
				creating_obstacle = false;

			// Context menu (under default mouse threshold)
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
			{
				auto clicked_point = get_scene_point(io.MousePos, canvas_p0, canvas_sz);
				clicked_obstacle = nullptr;
				for (auto it = chain.obstacles.rbegin(); it != chain.obstacles.rend(); ++it)
				{
					if (it->contains(clicked_point))
					{
						clicked_obstacle = &(*it);
						break;
					}
				}
				ImGui::OpenPopup("context", ImGuiPopupFlags_MouseButtonRight);
			}
		}
	}

	if (ImGui::BeginPopup("context"))
	{
		creating_obstacle = false;
		if (clicked_obstacle != nullptr)
		{
			if (ImGui::ColorEdit3("Color", clicked_obstacle->color.data()))
				chain.parameter_space_map.render(chain);
			if (ImGui::MenuItem("Remove", NULL, false))
			{
				chain.obstacles.remove(*clicked_obstacle);
				chain.parameter_space_map.render(chain);
			}
		}
		if (ImGui::MenuItem("Remove all", NULL, false, chain.obstacles.size() > 0)) 
		{ 
			chain.obstacles.clear(); 
			chain.parameter_space_map.render(chain);
		}
		ImGui::EndPopup();
	}

	// Draw obstacles
	for (const auto& o : chain.obstacles)
		draw_obstacle(o, draw_list, canvas_p0, canvas_sz);

	// Draw chain
	if (show_chain) {
		if (!edit_mode)
			draw_chain(alternative_end_angles, draw_list, canvas_p0, canvas_sz, IM_COL32(50, 50, 50, 128));
		draw_chain(alternative_angles, draw_list, canvas_p0, canvas_sz, IM_COL32(50, 50, 50, 128));
		draw_chain(current_angles, draw_list, canvas_p0, canvas_sz, IM_COL32(0, 0, 0, 255));
		if (!edit_mode)
			draw_chain(current_end_angles, draw_list, canvas_p0, canvas_sz, IM_COL32(0, 0, 0, 255));
		if (chain.is_animating())
			draw_chain(chain.animation_parameters.current_angles, draw_list, canvas_p0, canvas_sz, IM_COL32(0, 0, 0, 255));
	}

	ImGui::End();
}
