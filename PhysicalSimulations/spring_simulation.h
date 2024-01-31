#pragma once

#include "task.h"
#include "euler_ode_solver.h"
#include <list>
#include "state_listener.h"
#include "configurable_function.h"

class SpringSimulation {
	EulerODESolver<float, 2> ode_solver;

	class SpringSimulationTaskStep : public SingleTaskStep {
		SpringSimulation& simulation;
	public:
		SpringSimulationTaskStep(SpringSimulation& sim) : simulation(sim) {}
		bool execute(const TaskParameters& parameters) override;
		void execute_immediately(const TaskParameters& parameters) override {}
	};

	std::list<StateListener<float, 3>*> state_listeners;
	std::list<StateListener<float, 4>*> force_listeners;
public:
	struct SimulationParameters {
		bool paused = false;
		float m = 1.0f,
			invm = 1.0f / m,
			c = 1.0f,
			k = 2.0f,
			delta = 0.005f;
		ConfigurableFunction w, h;
	} parameters;

	SpringSimulation();
	Vector<float, 2> get_current_state() const { return ode_solver.current(); }
	Vector<float, 3> get_current_state_with_acceleration() const {
		auto current_state = ode_solver.current();
		return { current_state[0], current_state[1], ode_solver.current_derivative()[1] };
	}

	Vector<float, 3> get_forces() const {
		const auto state = get_current_state();
		const auto t = get_current_time();
		return {
			parameters.c * (parameters.w(t) - state[0]),
			-parameters.k * state[1],
			parameters.h(t)
		};
	}

	float get_equilibrium() const {
		return parameters.w(get_current_time());
	}

	Vector<float, 2>& get_start_state() {
		return ode_solver.get_ode().get_start_value();
	}

	void reset() {
		ode_solver.reset();
		for (auto* l : state_listeners)
			l->reset();
		for (auto* l : force_listeners)
			l->reset();
	}

	float get_current_time() const { return ode_solver.current_argument(); }
	void add_state_listener(StateListener<float, 3>& listener) { state_listeners.push_back(&listener); }
	void add_force_listener(StateListener<float, 4>& listener) { force_listeners.push_back(&listener); }
	void notify_listeners()
	{
		auto value_to_pass = get_current_state_with_acceleration();
		auto force = get_forces();
		Vector<float, 4> value_to_pass_forces = { force[0],force[1],force[2],get_equilibrium() };
		for (auto* l : state_listeners)
			l->notify(ode_solver.current_argument(), value_to_pass);
		for (auto* l : force_listeners)
			l->notify(ode_solver.current_argument(), value_to_pass_forces);
	}
	Task get_task();
};