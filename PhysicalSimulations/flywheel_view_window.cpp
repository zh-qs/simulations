#include "flywheel_view_window.h"

Vector2 FlywheelViewWindow::get_scene_point(const ImVec2& point, const ImVec2& p0, const ImVec2& size)
{
	const Vector2 clip_space_point{ (point.x - p0.x - 0.5f * size.x) / size.y, (point.y - p0.y) / size.y - 0.5f };
	return (2.0f / scale) * clip_space_point + center;
}

ImVec2 FlywheelViewWindow::get_view_point(const Vector2& point, const ImVec2& p0, const ImVec2& size)
{
	const auto view_point = (0.5f * scale) * (point - center);

	return { p0.x + view_point.x * size.y + 0.5f * size.x, p0.y + (view_point.y + 0.5f) * size.y };
}

float FlywheelViewWindow::get_view_length(const float& l, const ImVec2& size)
{
	return 0.5f * scale * l * size.y;
}

void FlywheelViewWindow::build()
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
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
			Vector2 delta{ -2.0f * io.MouseDelta.x / (canvas_sz.x * scale), -2.0f * io.MouseDelta.y / (canvas_sz.y * scale) };
			center += delta;
		}
	}

	draw_list->AddCircle(view_center, get_view_length(simulation.parameters.R, canvas_sz), IM_COL32(0, 0, 0, 255), 200);

	const auto circle_point = get_view_point(simulation.get_current_point_on_circle(), canvas_p0, canvas_sz);
	const auto axis_point = get_view_point(simulation.get_current_point_on_y_axis(), canvas_p0, canvas_sz);

	draw_list->AddLine(view_center, circle_point, IM_COL32(0, 0, 0, 255));
	draw_list->AddLine(circle_point, axis_point, IM_COL32(0, 0, 0, 255));
	draw_list->AddRectFilled({ axis_point.x - 5, axis_point.y - 5 }, { axis_point.x + 5, axis_point.y + 5 }, IM_COL32(0, 0, 0, 255));

	ImGui::End();
}