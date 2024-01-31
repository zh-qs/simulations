#include "spring_parameters_window.h"

void SpringParametersWindow::build()
{
	ImGui::Begin(get_name());
	ImGui::SeparatorText("Display");

	auto& io = ImGui::GetIO();

	ImGui::Text("FPS: %.1f", 1.0f / io.DeltaTime);

	ImGui::SeparatorText("Simulation");

	if (ImGui::Button(simulation.parameters.paused ? "Start" : "Pause"))
		simulation.parameters.paused = !simulation.parameters.paused;
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		simulation.reset();

	ImGui::SeparatorText("Kinematic");

	auto state = simulation.get_current_state_with_acceleration();
	ImGui::Text("x = %f", state[0]);
	ImGui::Text("xt = %f", state[1]);
	ImGui::Text("xtt = %f", state[2]);

	ImGui::SeparatorText("Forces");

	auto forces = simulation.get_forces();
	ImGui::Text("f(t) = %f", forces[0]);
	ImGui::Text("g(t) = %f", forces[1]);
	ImGui::Text("h(t) = %f", forces[2]);

	ImGui::SeparatorText("Other");
	ImGui::Text("w(t) = %f", simulation.get_equilibrium());

	ImGui::SeparatorText("Properties");
	ImGui::SliderFloat("x0", &simulation.get_start_state()[0], -10.0f, 10.0f);
	ImGui::SliderFloat("v0", &simulation.get_start_state()[1], -10.0f, 10.0f);
	if (ImGui::SliderFloat("m", &simulation.parameters.m, 0.01f, 10.0f))
		simulation.parameters.invm = 1.0f / simulation.parameters.m;
	ImGui::SliderFloat("k", &simulation.parameters.k, 0.0f, 10.0f);
	ImGui::SliderFloat("c", &simulation.parameters.c, 0.0f, 10.0f);
	ImGui::SliderFloat("delta", &simulation.parameters.delta, 0.01f, 0.1f);

	ImGui::Text("h(t)");
	simulation.parameters.h.build_settings();

	ImGui::Text("w(t)");
	simulation.parameters.w.build_settings();

	ImGui::End();
}
