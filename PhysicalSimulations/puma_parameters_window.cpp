#include "puma_parameters_window.h"
#include "imgui_guards.h"

void PumaParametersWindow::build()
{
	ImGui::Begin(get_name());

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

	float duration = 1.0f / puma.parameters.speed;
	if (GuardedSliderFloat("Duration", &duration, 0.1f, 10.0f, nullptr, ImGuiSliderFlags_None, 0.1f, INFINITY))
		puma.parameters.speed = 1.0f / duration;
	ImGui::SameLine();
	ImGui::Checkbox("Use slerp", &puma.parameters.use_slerp);

	ImGui::BeginDisabled(puma.is_running());
	ImGui::SeparatorText("Position");
	if (ImGui::SliderFloat3("Start##pos", puma.parameters.start_position.data(), -10.0f, 10.0f, nullptr))
		puma.reset();
	ImGui::SameLine();
	ImGui::SliderFloat3("End##pos", puma.parameters.end_position.data(), -10.0f, 10.0f, nullptr);

	ImGui::SeparatorText("Rotation (quaternion)");
	if (ImGui::SliderFloat4("Start##rq", not_normalized_start_quaternion.data(), -1.0f, 1.0f, nullptr))
	{
		puma.parameters.start_rotation = not_normalized_start_quaternion;
		puma.parameters.start_rotation.normalize();
		puma.parameters.start_euler_angles = (180.0f / PI) * Vector3::euler_angles(puma.parameters.start_rotation);
		puma.reset();
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat4("End##rq", not_normalized_end_quaternion.data(), -1.0f, 1.0f, nullptr))
	{
		puma.parameters.end_rotation = not_normalized_end_quaternion;
		puma.parameters.end_rotation.normalize();
		puma.parameters.end_euler_angles = (180.0f / PI) * Vector3::euler_angles(puma.parameters.end_rotation);
	}

	ImGui::SeparatorText("Rotation (Euler angles XYZ)");
	if (GuardedSliderFloat3("Start##rang", puma.parameters.start_euler_angles.data(), -180.0f, 180.0f, nullptr, ImGuiSliderFlags_None, -180.0f, 180.0f))
	{
		puma.parameters.start_rotation = ((PI / 180.0f) * puma.parameters.start_euler_angles).to_quaternion();
		not_normalized_start_quaternion = puma.parameters.start_rotation;
		puma.reset();
	}
	ImGui::SameLine();
	if (GuardedSliderFloat3("End##rang", puma.parameters.end_euler_angles.data(), -180.0f, 180.0f, nullptr, ImGuiSliderFlags_None, -180.0f, 180.0f))
	{
		puma.parameters.end_rotation = ((PI / 180.0f) * puma.parameters.end_euler_angles).to_quaternion();
		not_normalized_end_quaternion = puma.parameters.end_rotation;
	}

	ImGui::SeparatorText("Robot parameters");
	GuardedSliderFloat("l1", &puma.parameters.l1, 0.0f, 5.0f, NULL, ImGuiSliderFlags_None, 0.0f, INFINITY);
	GuardedSliderFloat("l3", &puma.parameters.l3, 0.0f, 5.0f, NULL, ImGuiSliderFlags_None, 0.0f, INFINITY);
	GuardedSliderFloat("l4", &puma.parameters.l4, 0.0f, 5.0f, NULL, ImGuiSliderFlags_None, 0.0f, INFINITY);

	ImGui::SeparatorText("Internal parameters");
	ImGui::SliderFloat3("Parameters0", show_parameter_interpolation ? &puma.current_parameter_interpolation[0] : &puma.current_view_space_interpolation[0], -PI, PI);
	ImGui::SliderFloat3("Parameters1", show_parameter_interpolation ? &puma.current_parameter_interpolation[3] : &puma.current_view_space_interpolation[3], -PI, PI);
	if (puma.current_parameter_interpolation[2] < 0)
		puma.current_parameter_interpolation[2] = 0;

	ImGui::EndDisabled();

	ImGui::Checkbox("Show first window parameters", &show_parameter_interpolation);

	if (puma.is_running())
	{
		if (ImGui::Button(puma.parameters.paused ? "Resume" : "Pause"))
		{
			puma.parameters.paused = !puma.parameters.paused;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			if (current_task != nullptr)
				current_task->terminate();
			puma.reset();
		}
	}
	else
	{
		if (ImGui::Button("Start"))
		{
			puma.reset();
			current_task = &start_task(puma.create_task());
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			puma.reset();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Align views")) {
		view2.align_with(view1);
	}

	ImGui::PopItemWidth();

	ImGui::End();
}
