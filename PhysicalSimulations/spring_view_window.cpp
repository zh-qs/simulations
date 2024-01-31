#include "spring_view_window.h"

void AddLineScaled(ImDrawList* draw_list, float x0, float y0, float x1, float y1, const ImVec2& cp0, const ImVec2& sz) {
    ImVec2 p0 = { cp0.x + x0 * sz.x, cp0.y + y0 * sz.y };
    ImVec2 p1 = { cp0.x + x1 * sz.x, cp0.y + y1 * sz.y };
    draw_list->AddLine(p0, p1, IM_COL32(0, 0, 0, 255), 2.0f);
}

void AddLineUnscaled(ImDrawList* draw_list, float x0, float y0, float xoff, float yoff, const ImVec2& cp0, const ImVec2& sz) {
    ImVec2 p0 = { cp0.x + x0 * sz.x, cp0.y + y0 * sz.y };
    ImVec2 p1 = { p0.x + xoff, p0.y + yoff };
    draw_list->AddLine(p0, p1, IM_COL32(0, 0, 0, 255), 2.0f);
}

void AddRectScaled(ImDrawList* draw_list, float x, float y, int width_px, int height_px, const ImVec2& cp0, const ImVec2& sz) {
    ImVec2 p0 = { cp0.x + x * sz.x - width_px * 0.5f, cp0.y + y * sz.y - height_px * 0.5f };
    ImVec2 p1 = { cp0.x + x * sz.x + width_px * 0.5f, cp0.y + y * sz.y + height_px * 0.5f };
    draw_list->AddRectFilled(p0, p1, IM_COL32(0, 0, 0, 255), 0.0f);
}

void SpringViewWindow::build()
{
	ImGui::Begin(get_name());

    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
    //if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    //if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));

    // ***** on middle button scroll
    if (io.MouseWheel != 0.0f)
        scale *= powf(zoom_factor, io.MouseWheel);

    // ***** context menu on right click

    // This will catch our interactions
    if (canvas_sz.x != 0 && canvas_sz.y != 0) // workaround for stand-alone version
        ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonRight);

    // Context menu (under default mouse threshold)
    ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
    if (drag_delta.x == 0.0f && drag_delta.y == 0.0f)
        ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
    if (ImGui::BeginPopup("context"))
    {
        ImGui::Checkbox("Hanger", &hanger_mode);
        ImGui::EndPopup();
    }
    // ***** end context menu on right click

    draw_list->PushClipRect(canvas_p0, canvas_p1, true);

    float y = (1.0f - simulation.get_current_state()[0] * scale) * 0.5f;

    float yoff = 0.0f;
    if (hanger_mode)
    {
        yoff = 0.1f;
        AddLineScaled(draw_list, 0.25f, 1.0f, 0.25f, yoff, canvas_p0, canvas_sz);
        AddLineScaled(draw_list, 0.25f, yoff, 0.5f, yoff, canvas_p0, canvas_sz);
        AddLineScaled(draw_list, 0.25f, yoff + 0.2f, 0.4f, yoff, canvas_p0, canvas_sz);
    }

    AddLineScaled(draw_list, 0.5f, yoff, 0.5f, y + yoff, canvas_p0, canvas_sz);
    AddRectScaled(draw_list, 0.5f, y + yoff, 20, 20, canvas_p0, canvas_sz);

    if (hanger_mode)
    {
        float arm_length = 20.0f;
        AddLineScaled(draw_list, 0.5f, y + yoff, 0.5f, y + yoff + 0.1f, canvas_p0, canvas_sz);
        AddLineUnscaled(draw_list, 0.5f, y + yoff + 0.05f, -arm_length, arm_length, canvas_p0, canvas_sz);
        AddLineUnscaled(draw_list, 0.5f, y + yoff + 0.05f, arm_length, arm_length, canvas_p0, canvas_sz);
        AddLineUnscaled(draw_list, 0.5f, y + yoff + 0.1f, -arm_length, arm_length, canvas_p0, canvas_sz);
        AddLineUnscaled(draw_list, 0.5f, y + yoff + 0.1f, arm_length, arm_length, canvas_p0, canvas_sz);
    }

	ImGui::End();
}
