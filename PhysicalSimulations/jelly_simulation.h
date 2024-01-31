#pragma once

#include "task.h"
#include "algebra.h"
#include "runge_kutta_4_ode_solver.h"
#include "euler_ode_solver.h"
#include "state_listener.h"
#include "bezier_cube.h"
#include "box.h"
#include <vector>

class JellySimulation {

	std::vector<Vector3> cube_points;

	RungeKutta4ODESolver<float, ODE_DIM> ode_solver;

	class JellySimulationTaskStep : public SingleTaskStep {
		JellySimulation& simulation;
	public:
		JellySimulationTaskStep(JellySimulation& sim) : simulation(sim) {}
		bool execute(const TaskParameters& parameters) override;
		void execute_immediately(const TaskParameters& parameters) override {}
	};

	void get_coords(int idx, int& i, int& j, int& k) {
		i = idx % SIDE_DIM;
		j = (idx / SIDE_DIM) % SIDE_DIM;
		k = idx / (SIDE_DIM * SIDE_DIM);
	}

	int get_idx(int i, int j, int k) {
		return i + SIDE_DIM * j + SIDE_DIM * SIDE_DIM * k;
	}

	Vector3 get_velocity(const Vector<float, ODE_DIM>& v, int idx) {
		return { v[CONFIGURATION_SPACE_DIM + 3 * idx], v[CONFIGURATION_SPACE_DIM + 3 * idx + 1], v[CONFIGURATION_SPACE_DIM + 3 * idx + 2] };
	}

	Vector3 get_velocity(const Vector<float, ODE_DIM>& v, int i, int j, int k) {
		const int idx = get_idx(i, j, k);
		return get_velocity(v, idx);
	}

	Vector3 get_point(const Vector<float, ODE_DIM>& v, int idx) {
		return { v[3 * idx], v[3 * idx + 1], v[3 * idx + 2] };
	}

	Vector3 get_point(const Vector<float, ODE_DIM>& v, int i, int j, int k) {
		const int idx = get_idx(i, j, k);
		return get_point(v, idx);
	}

	void set_velocity(Vector<float, ODE_DIM>& v, int idx, const Vector3& value) {
		v[CONFIGURATION_SPACE_DIM + 3 * idx] = value.x;
		v[CONFIGURATION_SPACE_DIM + 3 * idx + 1] = value.y;
		v[CONFIGURATION_SPACE_DIM + 3 * idx + 2] = value.z;
	}

	void set_velocity(Vector<float, ODE_DIM>& v, int i, int j, int k, const Vector3& value) {
		const int idx = get_idx(i, j, k);
		set_velocity(v, idx, value);
	}

	void set_point(Vector<float, ODE_DIM>& v, int idx, const Vector3& value) {
		v[3 * idx] = value.x;
		v[3 * idx + 1] = value.y;
		v[3 * idx + 2] = value.z;
	}

	void set_point(Vector<float, ODE_DIM>& v, int i, int j, int k, const Vector3& value) {
		const int idx = get_idx(i, j, k);
		set_point(v, idx, value);
	}

	Vector3& get_cube_point(int i, int j, int k) {
		return cube_points[get_idx(i, j, k)];
	}	

	void collide_with_box();
public:
	struct SimulationParameters {
		float side_length = 1.0f;
		float single_point_mass = 1.0f;
		float k = 2.0f;
		float c1 = 20.0f;
		float c2 = 20.0f;
		float delta_time = 0.01f;
		bool paused = false;
		bool use_control_frame = true;
		Vector3 control_frame_position = { 0.0f,0.0f,0.0f };
		Vector3 control_frame_rotation_deg = { 0.0f,0.0f,0.0f };
		Box bounding_box = { -2,2,-2,2,-2,2 };
		float punch_max = 1.0f;
		float pinch_max = 0.1f;
		int speed = 1;
		float bounce_coefficient = 1.0f;
		bool bounce_only_one_component = false;
		enum class NormalMode {
			Analytic, TriangleAverage, ScaledModel
		} normal_mode = NormalMode::Analytic;
		float normal_scale = 1.1f;
		Vector3 gravity = { 0.0f,-1.0f,0.0f };
		bool use_gravity = false;
	} parameters;

	JellySimulation();

	const Vector3* get_points() const {
		return reinterpret_cast<const Vector3*>(&ode_solver.current()[0]);
	}

	void punch();
	void pinch();

	ThreadTask get_task() {
		ThreadTask task(parameters.delta_time);
		task.add_step<JellySimulationTaskStep>(*this);
		return task;
	}

	void reset();
};
