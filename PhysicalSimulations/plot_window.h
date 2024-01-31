#pragma once

#include "window.h"
#include "state_listener.h"
#include <optional>

class PlotWindow : public Window {
	std::string plot_name;
	virtual void build_plots() = 0;
	virtual void setup_axes() = 0;
public:
	PlotWindow(const std::string& name, const std::string& plot_name) : plot_name(plot_name) {
		this->name = name;
	}
	virtual void build() override;
};

class MoveParametersPlotWindow : public PlotWindow, public StateListener<float, 3> {
	float max_time = 60.0f;

	float max_value = 2.0f;

	std::vector<float> time;
	std::vector<float> position;
	std::vector<float> velocity;
	std::vector<float> acceleration;

	std::optional<float> last_arg = std::nullopt;
	std::optional<Vector<float, 3>> last_value = std::nullopt;

	virtual void build_plots() override;
	virtual void setup_axes() override;

	void update_plots();
public:
	MoveParametersPlotWindow() : PlotWindow("Position, velocity and acceleration", "##xxtxtt") {}

	virtual void notify(const float& arg, const Vector<float, 3>& value) override;
	virtual void reset() override;
};

class TrajectoryPlotWindow : public PlotWindow, public StateListener<float, 3> {
	std::vector<float> position;
	std::vector<float> velocity;
	virtual void build_plots() override;
	virtual void setup_axes() override;
	float max_abs = 0.0f;

	bool scale_up_only;

	std::optional<float> last_arg = std::nullopt;
	std::optional<Vector<float, 3>> last_value = std::nullopt;

	void update_plots();
public:
	TrajectoryPlotWindow(bool scale_up_only = true) : PlotWindow("Trajectory", "##trajectory"), scale_up_only(scale_up_only) {}

	virtual void notify(const float& arg, const Vector<float, 3>& value) override;
	virtual void reset() override;
};

class ForcesPlotWindow : public PlotWindow, public StateListener<float, 4> {
	float max_time = 60.0f;

	float max_value = 2.0f;

	std::vector<float> time;
	std::vector<float> f;
	std::vector<float> g;
	std::vector<float> h;
	std::vector<float> w;
	virtual void build_plots() override;
	virtual void setup_axes() override;
public:
	ForcesPlotWindow() : PlotWindow("Forces", "##forces") {}

	virtual void notify(const float& arg, const Vector<float, 4>& value) override;
	virtual void reset() override;
};

class Task4PlotWindow : public PlotWindow, public StateListener<float, 3>, public StateListener<float, 4> {
	float max_time = 15.0f;

	float max_value = 1.0f;

	std::vector<float> time;
	std::vector<float> x;
	std::vector<float> w;
	std::vector<float> wmx;
	std::vector<float> line_x;
	std::vector<float> up_line;
	std::vector<float> down_line;

	int customRGBMap;

	virtual void build_plots() override;
	virtual void setup_axes() override;
public:
	Task4PlotWindow();

	virtual void notify(const float& arg, const Vector<float, 3>& value) override;
	virtual void notify(const float& arg, const Vector<float, 4>& value) override;
	virtual void reset() override;
};

class SingleFunctionPlotWindow : public PlotWindow, public StateListener<float, 3> {
	float max_time = 10.0f;

	std::vector<float> time;
	std::vector<float> x;

	std::string func_name;
	int idx;
	
	std::optional<std::pair<float, float>> last_data;

	virtual void build_plots() override;
	virtual void setup_axes() override;

	void update_plots();
public:
	SingleFunctionPlotWindow(const std::string& name, const std::string& func_name, int idx);

	virtual void notify(const float& arg, const Vector<float, 3>& value) override;
	virtual void reset() override;
};
