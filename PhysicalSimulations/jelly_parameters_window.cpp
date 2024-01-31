#include "jelly_parameters_window.h"
#include "imgui_guards.h"

JellyParametersWindow::JellyParametersWindow(JellySimulation& sim, JellyViewWindow& view) : simulation(sim), view(view)
{
	name = "Parameters";
}

const char* normal_modes[] = { "Analytic", "Triangle normals average (TODO)", "Difference between scaled model" };

void JellyParametersWindow::build()
{
	ImGui::Begin(get_name());

	ImGui::SeparatorText("Simulation");
	if (ImGui::Button(simulation.parameters.paused ? "Start" : "Pause"))
		simulation.parameters.paused = !simulation.parameters.paused;
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		simulation.reset();

	ImGui::SliderFloat("dt", &simulation.parameters.delta_time, 0.01f, 0.1f);
	GuardedInputInt("Speed", &simulation.parameters.speed, 1, 10, ImGuiInputTextFlags_None, 1, 100);

	ImGui::SeparatorText("Visibility");
	ImGui::Checkbox("Jelly patches", &view.jelly.patches_visible);
	ImGui::Checkbox("Jelly lines", &view.jelly.line_visible);
	ImGui::Checkbox("Jelly points", &view.jelly.points_visible);
	ImGui::Checkbox("Deformed shape", &view.deformed_mesh.visible);
	ImGui::Checkbox("Control frame", &view.cube.visible);
	if (ImGui::Checkbox("Bounding box", &view.box.visible))
	{
		view.box_border.visible = view.box.visible;
	}

	ImGui::SeparatorText("Deformed shape display");
	ImGui::Combo("Normal calculation mode", reinterpret_cast<int*>(&simulation.parameters.normal_mode), normal_modes, sizeof(normal_modes) / sizeof(const char*));
	if (simulation.parameters.normal_mode == JellySimulation::SimulationParameters::NormalMode::ScaledModel)
	{
		ImGui::SliderFloat("Normal scale", &simulation.parameters.normal_scale, 0.5f, 1.5f);
		if (simulation.parameters.normal_scale == 1.0f)
			ImGui::Text("Normal scale cannot be equal to 1!");
	}

	ImGui::SeparatorText("Physical parameters");
	ImGui::SliderFloat("m (single point)", &simulation.parameters.single_point_mass, 0.0f, 10.0f);
	float jelly_mass = simulation.parameters.single_point_mass * CUBE_POINT_COUNT;
	if (ImGui::SliderFloat("M (whole jelly)", &jelly_mass, 0.0f, 1000.0f))
	{
		simulation.parameters.single_point_mass = jelly_mass / CUBE_POINT_COUNT;
	}
	ImGui::SliderFloat("c1", &simulation.parameters.c1, 0.01f, 100.0f);
	ImGui::SliderFloat("c2", &simulation.parameters.c2, 0.01f, 100.0f);
	ImGui::SliderFloat("k", &simulation.parameters.k, 0.0f, 100.0f);

	ImGui::SliderFloat("Bounce coefficient", &simulation.parameters.bounce_coefficient, 0.0f, 1.0f);
	ImGui::Checkbox("Bounce one component only", &simulation.parameters.bounce_only_one_component);

	ImGui::Checkbox("Use gravity", &simulation.parameters.use_gravity);
	ImGui::SliderFloat3("Gravity", simulation.parameters.gravity.data(), -10.0f, 10.0f);

	ImGui::SeparatorText("Control frame");
	ImGui::Checkbox("Use control frame", &simulation.parameters.use_control_frame);
	ImGui::SliderFloat3("Position", simulation.parameters.control_frame_position.data(), -5.0f, 5.0f);
	ImGui::SliderFloat3("Rotation", simulation.parameters.control_frame_rotation_deg.data(), -180.0f, 180.0f);

	ImGui::SeparatorText("Disturb jelly");
	if (ImGui::Button("Punch"))
		simulation.punch();
	ImGui::SameLine();
	if (ImGui::Button("Pinch"))
		simulation.pinch();
	GuardedSliderFloat("Max punch value", &simulation.parameters.punch_max, 0.0f, 10.0f, NULL, ImGuiSliderFlags_None, 0.0f, INFINITY);
	GuardedSliderFloat("Max pinch value", &simulation.parameters.pinch_max, 0.0f, 1.0f, NULL, ImGuiSliderFlags_None, 0.0f, INFINITY);	

	ImGui::End();
}
