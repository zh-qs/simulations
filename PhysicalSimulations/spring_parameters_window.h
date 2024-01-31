#pragma once

#include "window.h"
#include "spring_simulation.h"

class SpringParametersWindow : public Window {
	SpringSimulation& simulation;

	int item_current_idx_h = 0;
	int item_current_idx_w = 0;
public:
	SpringParametersWindow(SpringSimulation& sim) : simulation(sim) {
		name = "Parameters";
	}
	virtual void build() override;
};