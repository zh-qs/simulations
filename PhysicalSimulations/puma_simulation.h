#pragma once

#include "algebra.h"
#include "generic_vector.h"
#include "task.h"
#include <list>

class PumaSimulation {
	class PumaSimulationTaskStep : public SingleTaskStep {
		PumaSimulation& puma;
		float t = 0.0f;
	public:
		PumaSimulationTaskStep(PumaSimulation& puma) : puma(puma) {}
		bool execute(const TaskParameters& parameters) override;
		void execute_immediately(const TaskParameters& parameters) override {}
	};

	Vector<float, 6> start_parameters, end_parameters;

	bool task_ended = true;

	std::list<Vector<float, 6>> inverse_kinematics(const Vector3& position, const Quaternion<float>& rotation);
	Vector<float, 6> inverse_kinematics_nearest_to(const Vector3& position, const Quaternion<float>& rotation, const Vector<float, 6>& other);

public:
	Vector<float, 6> current_parameter_interpolation;
	Vector<float, 6> current_view_space_interpolation;

	struct PumaParameters {
		float l1 = 1.0f;
		float l3 = 1.0f;
		float l4 = 1.0f;

		Vector3 start_position = { -1.0f,0.0f,0.0f };
		Vector3 end_position = { 0.0f,0.0f,0.0f };
		Quaternion<float> start_rotation = { 1.0f,0.0f,0.0f,0.0f };
		Quaternion<float> end_rotation = { 1.0f,0.0f,0.0f,0.0f };
		Vector3 start_euler_angles = { 0.0f,0.0f,0.0f };
		Vector3 end_euler_angles = { 0.0f,0.0f,0.0f };
		bool use_slerp = false;
		bool paused = false;
		float speed = 1.0f;
	} parameters;

	PumaSimulation();
	Matrix4x4 get_relative_model_for_mesh(int mesh_idx, bool interpolate_parameters) const;
	float get_q2(bool interpolate_parameters) const;

	Matrix4x4 get_start_model() const;
	Matrix4x4 get_end_model() const;

	void reset();
	void reset_end_parameters();
	inline bool is_running() const { return !task_ended; }
	Task create_task();
};