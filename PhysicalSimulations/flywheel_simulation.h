#pragma once

#include "algebra.h"
#include "task.h"
#include <list>
#include "state_listener.h"
#include <random>

class FlywheelSimulation {
	class FlywheelSimulationTaskStep : public SingleTaskStep {
		FlywheelSimulation& simulation;
	public:
		FlywheelSimulationTaskStep(FlywheelSimulation& sim) : simulation(sim) {}
		bool execute(const TaskParameters& parameters) override;
		void execute_immediately(const TaskParameters& parameters) override {}
	};

	int iteration = 0;

	float pprevious_x_coord = 0;

	float previous_time = 0;
	float previous_x_coord = 0;

	float current_time = 0;
	float current_angle = 0;
	float current_x_coord = 0;

	std::list<StateListener<float, 3>*> listeners;
	std::random_device dev;

public:
	struct FlywheelParameters {
		float delta_time = 0.01f;
		bool paused = false;
		float omega = 1;
		float R = 1, L = 2;
		float e0 = 0;
	} parameters;

	Vector2 get_current_point_on_circle() const {
		return { parameters.R * cosf(current_angle), parameters.R * sinf(current_angle) };
	}
	Vector2 get_current_point_on_y_axis() const {
		return { current_x_coord, 0.0f };
	}

	ThreadTask create_task();
	void add_listener(StateListener<float, 3>& listener) { listeners.push_back(&listener); }
	void notify_listeners();
	void reset();
};