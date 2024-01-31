#pragma once

#include "algebra.h"
#include "task.h"
#include <list>
#include "state_listener.h"

class Animation {
	class AnimationTaskStep : public SingleTaskStep {
		Animation& animation;
		float t = 0.0f;
	public:
		AnimationTaskStep(Animation& animation) : animation(animation) {}
		bool execute(const TaskParameters& parameters) override;
		void execute_immediately(const TaskParameters& parameters) override {}
	};

	std::list<StateListener<float, 7>*> state_listeners;

	Vector3 current_position;
	Quaternion<float> current_rotation;
	Vector3 current_euler_angles; // in degrees

	bool task_ended = true;

public:
	struct AnimationParameters {
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

	struct KeyframeParameters {
		bool enabled = false;
		int count = 3;
	} keyframes;

	Animation();

	void reset();

	Matrix4x4 get_model_matrix_for_keyframe(int i, bool use_euler_angles) const;

	inline Matrix4x4 get_model_matrix(bool use_euler_angles) const { 
		if (use_euler_angles)
			return Matrix4x4::translation(current_position) * Matrix4x4::rotation_euler((PI / 180.0f) * current_euler_angles);
		else
			return Matrix4x4::translation(current_position) * Matrix4x4::rotation(current_rotation);
	}
	inline bool is_running() const { return !task_ended; }
	Task create_task();
};