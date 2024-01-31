#pragma once

#include "window.h"
#include "simple_chain.h"
#include "simple_chain_view_window.h"
#include <vector>

class SimpleChainParametersWindow : public Window {
	SimpleChain& chain;
	SimpleChainViewWindow& view;
	int current_mode = 0;

	std::vector<Vector2> path;
	bool show_path_error = false;

	Task* current_task = nullptr;

public:
	SimpleChainParametersWindow(SimpleChain& chain, SimpleChainViewWindow& view);

	virtual void build() override;
};