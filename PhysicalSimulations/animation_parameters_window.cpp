#include "animation_parameters_window.h"
#include "imgui_guards.h"

void AnimationParametersWindow::build()
{
	ImGui::Begin(get_name());

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

	float duration = 1.0f / animation.parameters.speed;
	if (GuardedSliderFloat("Duration", &duration, 0.1f, 10.0f, nullptr, ImGuiSliderFlags_None, 0.1f, INFINITY))
		animation.parameters.speed = 1.0f / duration;
	ImGui::SameLine();
	ImGui::Checkbox("Use slerp", &animation.parameters.use_slerp);

	ImGui::BeginDisabled(animation.is_running());
	ImGui::SeparatorText("Position");
	if (ImGui::SliderFloat3("Start##pos", animation.parameters.start_position.data(), -10.0f, 10.0f, nullptr))
		animation.reset();
	ImGui::SameLine();
	ImGui::SliderFloat3("End##pos", animation.parameters.end_position.data(), -10.0f, 10.0f, nullptr);

	ImGui::SeparatorText("Rotation (quaternion)");
	if (ImGui::SliderFloat4("Start##rq", not_normalized_start_quaternion.data(), -1.0f, 1.0f, nullptr))
	{
		animation.parameters.start_rotation = not_normalized_start_quaternion;
		animation.parameters.start_rotation.normalize();
		animation.parameters.start_euler_angles = (180.0f / PI) * Vector3::euler_angles(animation.parameters.start_rotation);
		animation.reset();
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat4("End##rq", not_normalized_end_quaternion.data(), -1.0f, 1.0f, nullptr))
	{
		animation.parameters.end_rotation = not_normalized_end_quaternion;
		animation.parameters.end_rotation.normalize();
		animation.parameters.end_euler_angles = (180.0f / PI) * Vector3::euler_angles(animation.parameters.end_rotation);
	}

	ImGui::SeparatorText("Rotation (Euler angles XYZ)");
	if (GuardedSliderFloat3("Start##rang", animation.parameters.start_euler_angles.data(), -180.0f, 180.0f, nullptr, ImGuiSliderFlags_None, -180.0f, 180.0f))
	{
		animation.parameters.start_rotation = ((PI / 180.0f) * animation.parameters.start_euler_angles).to_quaternion();
		not_normalized_start_quaternion = animation.parameters.start_rotation;
		animation.reset();
	}
	ImGui::SameLine();
	if (GuardedSliderFloat3("End##rang", animation.parameters.end_euler_angles.data(), -180.0f, 180.0f, nullptr, ImGuiSliderFlags_None, -180.0f, 180.0f))
	{
		animation.parameters.end_rotation = ((PI / 180.0f) * animation.parameters.end_euler_angles).to_quaternion();
		not_normalized_end_quaternion = animation.parameters.end_rotation;
	}
	ImGui::EndDisabled();

	if (animation.is_running())
	{
		if (ImGui::Button(animation.parameters.paused ? "Resume" : "Pause"))
		{
			animation.parameters.paused = !animation.parameters.paused;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			if (current_task != nullptr)
				current_task->terminate();
			animation.reset();
		}
	}
	else
	{
		if (ImGui::Button("Start"))
		{
			current_task = &start_task(animation.create_task());
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			animation.reset();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Align views")) {
		view2.align_with(view1);
	}
	ImGui::SameLine();
	ImGui::Checkbox("Show keyframes", &animation.keyframes.enabled);
	if (animation.keyframes.enabled)
	{
		ImGui::SameLine();
		GuardedInputInt("Count", &animation.keyframes.count, 1, 1, ImGuiInputTextFlags_None, 2, 1000);
	}

	ImGui::PopItemWidth();

	ImGui::End();
}
