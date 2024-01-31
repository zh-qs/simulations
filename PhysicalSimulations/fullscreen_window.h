#pragma once

#include "window.h"
#include "dockspace.h"

class FullscreenWindow : public Window {
	bool first_time = true;

	Dockspace dockspace;

public:
	FullscreenWindow() {
		name = "FullscreenWindow";
	}

	Dockspace& get_dockspace() { return dockspace; }

	virtual void build() override;
};