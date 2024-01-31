#include "flywheel_parameters_window.h"
#include "imgui_guards.h"

FlywheelParametersWindow::FlywheelParametersWindow(FlywheelSimulation& sim) : simulation(sim)
{
	name = "Parameters##flywheel";
}

void FlywheelParametersWindow::build()
{
	ImGui::Begin(get_name());

	ImGui::SeparatorText("Simulation");
	if (ImGui::Button(simulation.parameters.paused ? "Start" : "Pause"))
	{
		simulation.parameters.paused = !simulation.parameters.paused;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		simulation.reset();
	}
	GuardedSliderFloat("dt", &simulation.parameters.delta_time, 0.0f, 0.2f, NULL, ImGuiSliderFlags_None, 0.01f, INFINITY);

	ImGui::SeparatorText("Parameters");
	GuardedSliderFloat("omega", &simulation.parameters.omega, -10, 10, NULL, ImGuiSliderFlags_None, -INFINITY, INFINITY);
	GuardedSliderFloat("R", &simulation.parameters.R, 0.0f, 5.0f, NULL, ImGuiSliderFlags_None, 0.01f, INFINITY);
	GuardedSliderFloat("L", &simulation.parameters.L, 0.0f, 20.0f, NULL, ImGuiSliderFlags_None, simulation.parameters.R, INFINITY);
	GuardedSliderFloat("e0", &simulation.parameters.e0, 0.0f, 0.05f, "%.6f", ImGuiSliderFlags_None, 0.0f, 0.5f);

	ImGui::End();
}
