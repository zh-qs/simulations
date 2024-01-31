#pragma once
#include "imgui.h"

inline bool GuardedInputInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags flags, int guard_min, int guard_max)
{
	int value = *v;
	if (ImGui::InputInt(label, &value, step, step_fast, flags))
	{
		if (value >= guard_min && value <= guard_max)
		{
			*v = value;
			return true;
		}
		return false;
	}
	return false;
}

inline bool GuardedSliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags, float guard_min, float guard_max)
{
	float value = *v;
	if (ImGui::SliderFloat(label, &value, v_min, v_max, format, flags))
	{
		if (value >= guard_min && value <= guard_max)
		{
			*v = value;
			return true;
		}
		return false;
	}
	return false;
}

inline bool GuardedSliderFloat3(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags, float guard_min, float guard_max)
{
	float value[] = { v[0],v[1],v[2] };
	if (ImGui::SliderFloat3(label, value, v_min, v_max, format, flags))
	{
		bool good = false;
		for (int i = 0; i < 3; ++i)
		{
			if (value[i] >= guard_min && value[i] <= guard_max)
			{
				v[i] = value[i];
				good = true;
			}
		}
		return good;
	}
	return false;
}