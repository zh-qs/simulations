#pragma once

#include "window.h"
#include "simple_chain.h"
#include "algebra.h"
#include "obstacle.h"
#include <tuple>

class SimpleChainViewWindow : public Window {
	friend class SimpleChainParametersWindow;

	SimpleChain& chain;

	const float zoom_factor = 1.3f;

	float scale = 1.0f;
	Vector2 center = { 0.0f,0.0f };

	bool edit_mode = true;
	bool set_end_position = false;
	bool creating_obstacle = false;
	Obstacle current_obstacle;
	
	Vector2 get_scene_point(const ImVec2& point, const ImVec2& p0, const ImVec2& size);
	ImVec2 get_view_point(const Vector2& point, const ImVec2& p0, const ImVec2& size);

	Obstacle* clicked_obstacle = nullptr;

	Vector2 current_angles = { 0.0f,0.0f };
	Vector2 alternative_angles = { 0.0f,0.0f };
	Vector2 current_end_angles = { 0.0f,0.0f };
	Vector2 alternative_end_angles = { 0.0f,0.0f };
	bool show_chain = false;
	bool second_chain = false;

	void draw_chain(const Vector2& angles, ImDrawList* draw_list, const ImVec2& p0, const ImVec2& size, ImU32 color);
	void draw_obstacle(const Obstacle& obstacle, ImDrawList* draw_list, const ImVec2& p0, const ImVec2& size);
public:
	SimpleChainViewWindow(SimpleChain& chain);

	virtual void build() override;
};