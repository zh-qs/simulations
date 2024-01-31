#include "configurable_function.h"
#include "imgui.h"
#include <string>

const char* items[] = { "Constant", "One step", "Rectangular", "Sine" };

int ConfigurableFunction::counter = 0;

void ConfigurableFunction::build_settings()
{
	std::string suffix = "##" + std::to_string(id);
	std::string combo_name = "type" + suffix;
	std::string a_name = "A" + suffix,
		omega_name = "omega" + suffix,
		phi_name = "phi (degrees)" + suffix;
	ImGui::Combo(combo_name.c_str(), reinterpret_cast<int*>(&function_type), items, IM_ARRAYSIZE(items));
	ImGui::SliderFloat(a_name.c_str(), &value, -10.0f, 10.0f);
	if (function_type == FunctionType::Rectangular || function_type == FunctionType::Sine)
	{
		ImGui::SliderFloat(omega_name.c_str(), &omega, -10.0f, 10.0f);
		ImGui::SliderFloat(phi_name.c_str(), &phi, -180, 180);
	}
}

float ConfigurableFunction::operator()(const float& arg) const
{
	switch (function_type)
	{
	case FunctionType::Constant:
		return value;
	case FunctionType::OneStep:
		return arg > 0.0f ? value : 0.0f;
	case FunctionType::Rectangular:
	{
		float s = value * sinf(omega * arg + phi / 180.0f * PI);
		if (s > 0)
			return 1.0f;
		else if (s == 0)
			return 0.0f;
		else
			return -1.0f;
	}
	case FunctionType::Sine:
		return value * sinf(omega * arg + phi / 180.0f * PI);
	}
}
