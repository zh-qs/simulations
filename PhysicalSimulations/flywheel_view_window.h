#pragma once

#include "window.h"
#include "flywheel_simulation.h"

class FlywheelViewWindow : public Window {
	const FlywheelSimulation& simulation;

	const float zoom_factor = 1.3f;

	float scale = 0.5f;
	Vector2 center = { 1.0f,0.0f };

	Vector2 get_scene_point(const ImVec2& point, const ImVec2& p0, const ImVec2& size);
	ImVec2 get_view_point(const Vector2& point, const ImVec2& p0, const ImVec2& size);
	float get_view_length(const float& l, const ImVec2& size);
public:
	FlywheelViewWindow(const FlywheelSimulation& sim) : simulation(sim) {
		name = "Flywheel";
	}

	virtual void build() override;
};