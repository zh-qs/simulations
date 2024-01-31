#pragma once

#include "task.h"
#include "runge_kutta_4_ode_solver.h"
#include "state_listener.h"

class SpinningTopSimulation {
	RungeKutta4ODESolver<float, 7> ode_solver;

	class SpinningTopSimulationTaskStep : public SingleTaskStep {
		SpinningTopSimulation& simulation;
	public:
		SpinningTopSimulationTaskStep(SpinningTopSimulation& sim) : simulation(sim) {}
		bool execute(const TaskParameters& parameters) override;
		void execute_immediately(const TaskParameters& parameters) override {}
	};

	std::list<StateListener<float, 7>*> state_listeners;
	bool started = false;
public:
	struct SimulationParameters {
		bool paused = false;
		float angular_velocity_deg = 180.0f;
		float initial_z_rotation_deg = 30.0f;
		float delta = 0.05f;
		float cube_dim = 1.0f;
		float cube_density = 1.0f;
		int trace_length = 1000; // TODO
		bool gravity_present = true;
	} parameters;

	const Vector<float, 3> initial_inv_inertia = { 12.0f / 11.0f,6.0f / 5.0f,12.0f / 11.0f }; // TODO czy to jest dobre?
	//const Vector<float, 3> initial_inv_inertia = { 12.0f / 11.0f,6.0f,12.0f / 11.0f }; // czy to?
	Vector<float, 3> inv_inertia;

	SpinningTopSimulation();

	void reset();
	void reset_start_value();
	bool is_started() const { return started; }

	Vector<float, 7> get_current_state() const { return ode_solver.current(); }
	float get_current_time() const { return ode_solver.current_argument(); }
	void add_state_listener(StateListener<float, 7>& listener) { state_listeners.push_back(&listener); }
	Quaternion<float> get_current_quaternion() const { 
		auto current = get_current_state();
		return { current[3], current[4], current[5], current[6] };
	}

	void notify_listeners()
	{
		auto value_to_pass = get_current_state();
		for (auto* l : state_listeners)
			l->notify(ode_solver.current_argument(), value_to_pass);
	}
	ThreadTask get_task();
};