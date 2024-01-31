#pragma once

#include "window.h"
#include "spring_simulation.h"

class SpringViewWindow : public Window {
	const SpringSimulation& simulation;
	float scale = 1.0f;
	float zoom_factor = 1.3f;
	bool hanger_mode = false;
public:
	SpringViewWindow(const SpringSimulation& sim) : simulation(sim) {
		name = "Spring";
	}

	virtual void build() override;
};