#include "spinning_top_parameters_window.h"
#include "algebra.h"

void SpinningTopParametersWindow::build()
{
	ImGui::Begin(get_name());
	ImGui::SeparatorText("Display");

	auto& io = ImGui::GetIO();

	ImGui::Text("FPS: %.1f", 1.0f / io.DeltaTime);

	ImGui::Checkbox("Show cube", &view.cube.visible);
	ImGui::Checkbox("Show diameter", &view.cube_diameter.visible);
	ImGui::Checkbox("Show trace", &view.trace.visible);
	ImGui::Checkbox("Show gravity plane", &view.gravity_plane.visible);

	ImGui::SeparatorText("Simulation");

	if (ImGui::Button(simulation.parameters.paused ? "Start" : "Pause"))
		simulation.parameters.paused = !simulation.parameters.paused;
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		simulation.reset();

	ImGui::SliderFloat("delta", &simulation.parameters.delta, 0.01f, 0.1f);

	ImGui::SeparatorText("Cube");
	bool dim_changed = ImGui::SliderFloat("Dimension", &simulation.parameters.cube_dim, 0.5f, 5.0f);
	bool density_changed = ImGui::SliderFloat("Density", &simulation.parameters.cube_density, 0.5f, 5.0f);
	if (dim_changed || density_changed)
	{
		simulation.inv_inertia = simulation.initial_inv_inertia / (pow5(simulation.parameters.cube_dim) * simulation.parameters.cube_density);
	}
	ImGui::BeginDisabled(simulation.is_started());

	bool dev_changed = ImGui::SliderFloat("Deviation", &simulation.parameters.initial_z_rotation_deg, -90.0f, 90.0f);
	bool anv_changed = ImGui::SliderFloat("Angular velocity", &simulation.parameters.angular_velocity_deg, -360.0f, 360.0f);
	if (dev_changed || anv_changed)
		simulation.reset_start_value();

	ImGui::EndDisabled();

	ImGui::ColorEdit3("Color", view.cube.color.data());

	ImGui::SeparatorText("Other");
	if (ImGui::SliderInt("Trace length", &simulation.parameters.trace_length, 200, 2000, NULL, ImGuiSliderFlags_NoInput))
	{
		// TODO maybe here adjust trace length (or in sim view)
	}
	ImGui::Checkbox("Turn on gravity", &simulation.parameters.gravity_present);

	ImGui::End();
}
