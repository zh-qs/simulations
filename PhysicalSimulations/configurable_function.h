#pragma once

#include <string>
#include <cmath>

class ConfigurableFunction {
	static int counter;

	enum class FunctionType {
		Constant, OneStep, Rectangular, Sine
	};

	int id;
	static constexpr float PI = 3.14159265358979323846f;
	float value = 0.0f,
		omega = 1.0f,
		phi = 0.0f;
	FunctionType function_type = FunctionType::Constant;
public:
	ConfigurableFunction() {
		id = counter++;
	}

	void build_settings();
	float operator()(const float& arg) const;
};
