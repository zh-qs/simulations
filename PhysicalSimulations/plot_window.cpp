#include "plot_window.h"
#include "implot.h"
#include  <cstdlib>

const int MAX_PLOT_SIZE = 4000;

void PlotWindow::build()
{
	ImGui::Begin(get_name());
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
	if (ImPlot::BeginPlot(plot_name.c_str(), canvas_sz))
	{
		setup_axes();
		build_plots();
		ImPlot::EndPlot();
	}

	ImGui::End();
}

void MoveParametersPlotWindow::build_plots()
{
	ImPlot::PlotLine("x", time.data(), position.data(), time.size());
	ImPlot::PlotLine("xt", time.data(), velocity.data(), time.size());
	ImPlot::PlotLine("xtt", time.data(), acceleration.data(), time.size());
}

void MoveParametersPlotWindow::setup_axes()
{
	float min_time = time.empty() ? 0.0f : time.front();
	ImPlot::SetupAxesLimits(min_time, min_time + max_time, -max_value, max_value, ImPlotCond_Always);
	ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
}

void MoveParametersPlotWindow::update_plots()
{
	if (!last_arg.has_value() || !last_value.has_value())
		return;

	const float arg = last_arg.value();
	const auto value = last_value.value();

	float last_time = time.empty() ? 0.0f : time.back();
	if (last_time < max_time && time.size() < MAX_PLOT_SIZE)
	{
		time.push_back(arg);
		position.push_back(value[0]);
		velocity.push_back(value[1]);
		acceleration.push_back(value[2]);
	}
	else
	{
		memcpy(time.data(), time.data() + 1, sizeof(float) * (time.size() - 1));
		memcpy(position.data(), position.data() + 1, sizeof(float) * (position.size() - 1));
		memcpy(velocity.data(), velocity.data() + 1, sizeof(float) * (velocity.size() - 1));
		memcpy(acceleration.data(), acceleration.data() + 1, sizeof(float) * (acceleration.size() - 1));
		time.back() = arg;
		position.back() = value[0];
		velocity.back() = value[1];
		acceleration.back() = value[2];
	}
	for (int i = 0; i < 3; ++i)
		if (abs(value[i]) > max_value)
			max_value = abs(value[i]);

	last_arg = std::nullopt;
	last_value = std::nullopt;
}

void MoveParametersPlotWindow::notify(const float& arg, const Vector<float, 3>& value)
{
	last_arg = arg;
	last_value = value;
}

void MoveParametersPlotWindow::reset()
{
	time.clear();
	position.clear();
	velocity.clear();
	acceleration.clear();
}

void TrajectoryPlotWindow::build_plots()
{
	update_plots();
	ImPlot::PlotLine("state", position.data(), velocity.data(), position.size());
}

void TrajectoryPlotWindow::setup_axes()
{
	if (scale_up_only)
		ImPlot::SetupAxesLimits(-max_abs, max_abs, -max_abs, max_abs, ImPlotCond_Always);
	ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
}

void TrajectoryPlotWindow::update_plots()
{
	if (!last_arg.has_value() || !last_value.has_value())
		return;

	const float arg = last_arg.value();
	const auto value = last_value.value();

	max_abs = std::max(max_abs, std::max(value[0], value[1]));
	if (position.size() >= MAX_PLOT_SIZE)
	{
		memcpy(position.data(), position.data() + 1, sizeof(float) * (position.size() - 1));
		memcpy(velocity.data(), velocity.data() + 1, sizeof(float) * (velocity.size() - 1));
		position.back() = value[0];
		velocity.back() = value[1];
	}
	else
	{
		position.push_back(value[0]);
		velocity.push_back(value[1]);
	}

	last_arg = std::nullopt;
	last_value = std::nullopt;
}

void TrajectoryPlotWindow::notify(const float& arg, const Vector<float, 3>& value)
{
	last_arg = arg;
	last_value = value;
}

void TrajectoryPlotWindow::reset()
{
	position.clear();
	velocity.clear();
}

void ForcesPlotWindow::build_plots()
{
	ImPlot::PlotLine("f", time.data(), f.data(), time.size());
	ImPlot::PlotLine("g", time.data(), g.data(), time.size());
	ImPlot::PlotLine("h", time.data(), h.data(), time.size());
	ImPlot::PlotLine("w", time.data(), w.data(), time.size());
}

void ForcesPlotWindow::setup_axes()
{
	float min_time = time.empty() ? 0.0f : time.front();
	ImPlot::SetupAxesLimits(min_time, min_time + max_time, -max_value, max_value, ImPlotCond_Always);
	ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
}

void ForcesPlotWindow::notify(const float& arg, const Vector<float, 4>& value)
{
	float last_time = time.empty() ? 0.0f : time.back();
	if (last_time < max_time && time.size() < MAX_PLOT_SIZE)
	{
		time.push_back(arg);
		f.push_back(value[0]);
		g.push_back(value[1]);
		h.push_back(value[2]);
		w.push_back(value[3]);
	}
	else
	{
		memcpy(time.data(), time.data() + 1, sizeof(float) * (time.size() - 1));
		memcpy(f.data(), f.data() + 1, sizeof(float) * (f.size() - 1));
		memcpy(g.data(), g.data() + 1, sizeof(float) * (g.size() - 1));
		memcpy(h.data(), h.data() + 1, sizeof(float) * (h.size() - 1));
		memcpy(w.data(), w.data() + 1, sizeof(float) * (w.size() - 1));
		time.back() = arg;
		f.back() = value[0];
		g.back() = value[1];
		h.back() = value[2];
		w.back() = value[3];
	}
	for (int i = 0; i < 4; ++i)
		if (abs(value[i]) > max_value)
			max_value = abs(value[i]);
}

void ForcesPlotWindow::reset()
{
	time.clear();
	f.clear();
	g.clear();
	h.clear();
	w.clear();
}

void Task4PlotWindow::build_plots()
{
	ImPlot::PushColormap(customRGBMap);
	ImPlot::PlotLine("u", line_x.data(), up_line.data(), line_x.size());
	ImPlot::PlotLine("d", line_x.data(), down_line.data(), line_x.size());
	ImPlot::PopColormap();

	ImPlot::PlotLine("x", time.data(), x.data(), time.size());
	ImPlot::PlotLine("w", time.data(), w.data(), time.size());
	ImPlot::PlotLine("w-x", time.data(), wmx.data(), time.size());
}

void Task4PlotWindow::setup_axes()
{
	float min_time = time.empty() ? 0.0f : time.front();
	ImPlot::SetupAxesLimits(min_time, min_time + max_time, -max_value, max_value, ImPlotCond_Always);
	ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
	line_x = { min_time,min_time + max_time };
}

Task4PlotWindow::Task4PlotWindow() : PlotWindow("Task 4", "##task4") {
	up_line = { 0.1f,0.1f };
	down_line = { -0.1f,-0.1f };
	static ImU32 colorDataRGB[2] = { 0x969696FF, 0x969696FF };
	customRGBMap = ImPlot::AddColormap("RGBColors", colorDataRGB, 2);
}

void Task4PlotWindow::notify(const float& arg, const Vector<float, 3>& value)
{
	float last_time = time.empty() ? 0.0f : time.back();
	if (last_time < max_time && time.size() < MAX_PLOT_SIZE)
	{
		time.push_back(arg);
		x.push_back(value[0]);
		wmx.push_back(-value[0]);
	}
	else
	{
		memcpy(time.data(), time.data() + 1, sizeof(float) * (time.size() - 1));
		memcpy(x.data(), x.data() + 1, sizeof(float) * (x.size() - 1));
		memcpy(wmx.data(), wmx.data() + 1, sizeof(float) * (wmx.size() - 1));
		time.back() = arg;
		x.back() = value[0];
		wmx.back() = -value[0];
	}
	if (abs(value[0]) > max_value)
		max_value = abs(value[0]);
}

void Task4PlotWindow::notify(const float& arg, const Vector<float, 4>& value)
{
	float current_w = value[3];
	float last_time = time.empty() ? 0.0f : time.back();
	if (last_time < max_time && time.size() < MAX_PLOT_SIZE)
	{
		w.push_back(current_w);
	}
	else
	{
		memcpy(w.data(), w.data() + 1, sizeof(float) * (w.size() - 1));
		w.back() = current_w;
	}
	wmx.back() += current_w;
	if (abs(current_w) > max_value)
		max_value = abs(current_w);
}

void Task4PlotWindow::reset()
{
	time.clear();
	x.clear();
	w.clear();
	wmx.clear();
}

void SingleFunctionPlotWindow::build_plots()
{
	update_plots();
	ImPlot::PlotLine(func_name.c_str(), time.data(), x.data(), time.size());
}

void SingleFunctionPlotWindow::setup_axes()
{
	ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
}

void SingleFunctionPlotWindow::update_plots()
{
	if (!last_data.has_value())
		return;

	auto [t, val] = last_data.value();
	float last_time = time.empty() ? 0.0f : time.back();
	if (last_time < max_time && time.size() < MAX_PLOT_SIZE)
	{
		time.push_back(t);
		x.push_back(val);
	}
	else
	{
		memcpy(time.data(), time.data() + 1, sizeof(float) * (time.size() - 1));
		memcpy(x.data(), x.data() + 1, sizeof(float) * (x.size() - 1));
		time.back() = t;
		x.back() = val;
	}

	last_data = std::nullopt;
}

SingleFunctionPlotWindow::SingleFunctionPlotWindow(const std::string& name, const std::string& func_name, int idx) : PlotWindow(name, "##sfp" + std::to_string(idx)), func_name(func_name), idx(idx)
{
}

void SingleFunctionPlotWindow::notify(const float& arg, const Vector<float, 3>& value)
{
	last_data = { arg, value[idx] };
}

void SingleFunctionPlotWindow::reset()
{
	time.clear();
	x.clear();
}
