#pragma once

#include "window.h"
#include "spinning_top_simulation.h"
#include "spinning_top_view_window.h"

class SpinningTopParametersWindow : public Window {
	SpinningTopSimulation& simulation;
	SpinningTopViewWindow& view;

	int item_current_idx_h = 0;
	int item_current_idx_w = 0;
public:
	SpinningTopParametersWindow(SpinningTopSimulation& sim, SpinningTopViewWindow& view) : simulation(sim), view(view) {
		name = "Parameters";
	}
	virtual void build() override;
};