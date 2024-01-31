#pragma once

#include "jelly_view_window.h"
#include "jelly_simulation.h"

class JellyParametersWindow : public Window {
	JellySimulation& simulation;
	JellyViewWindow& view;

public:
	JellyParametersWindow(JellySimulation& sim, JellyViewWindow& view);

	virtual void build() override;
};