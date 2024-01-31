#pragma once

#include "window.h"
#include "puma_simulation.h"
#include "puma_view_window.h"

class PumaParametersWindow : public Window {
	PumaSimulation& puma;
	PumaViewWindow& view1;
	PumaViewWindow& view2;
	Task* current_task = nullptr;

	Quaternion<float> not_normalized_start_quaternion;
	Quaternion<float> not_normalized_end_quaternion;

	bool show_parameter_interpolation = true;
public:
	PumaParametersWindow(PumaSimulation& puma, PumaViewWindow& view1, PumaViewWindow& view2) : puma(puma), view1(view1), view2(view2) {
		name = "Parameters";
		not_normalized_start_quaternion = puma.parameters.start_rotation;
		not_normalized_end_quaternion = puma.parameters.end_rotation;
	}

	virtual void build() override;
};