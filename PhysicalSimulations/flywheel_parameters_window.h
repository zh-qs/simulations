#pragma once

#include "window.h"
#include "flywheel_simulation.h"

class FlywheelParametersWindow : public Window {
	FlywheelSimulation& simulation;

public:
	FlywheelParametersWindow(FlywheelSimulation& sim);

	virtual void build() override;
};