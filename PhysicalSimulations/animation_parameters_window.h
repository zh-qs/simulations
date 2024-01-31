#pragma once

#include "window.h"
#include "animation.h"
#include "animation_view_window.h"

class AnimationParametersWindow : public Window {
	Animation& animation;
	AnimationViewWindow& view1;
	AnimationViewWindow& view2;
	Task* current_task = nullptr;

	Quaternion<float> not_normalized_start_quaternion;
	Quaternion<float> not_normalized_end_quaternion;
public:
	AnimationParametersWindow(Animation& animation, AnimationViewWindow& view1, AnimationViewWindow& view2) : animation(animation), view1(view1), view2(view2) {
		name = "Parameters";
		not_normalized_start_quaternion = animation.parameters.start_rotation;
		not_normalized_end_quaternion = animation.parameters.end_rotation;
	}

	virtual void build() override;
};