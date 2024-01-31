#pragma once

#include "algebra.h"
#include <list>
#include <vector>
#include "obstacle.h"
#include "simple_chain_parameter_space_map.h"
#include "task.h"

class SimpleChain {
	class SimpleChainTaskStep : public SingleTaskStep {
		SimpleChain& chain;
		const std::vector<Vector2>& path;
		float t = 0.0f;
		int idx = 0;
	public:
		SimpleChainTaskStep(SimpleChain& chain, const std::vector<Vector2>& path) : chain(chain), path(path) {}
		bool execute(const TaskParameters& parameters) override;
		void execute_immediately(const TaskParameters& parameters) override {}
	};

	const Vector2 base_position = { 0.0f,0.0f };
	bool task_ended = true;
public:
	struct AnimationParameters {
		float speed = 1.0f;
		bool paused = false;
		Vector2 current_angles = { 0.0f,0.0f };
	} animation_parameters;
	SimpleChainParameterSpaceMap parameter_space_map;
	std::list<Obstacle> obstacles;

	float l1 = 1.0f, l2 = 1.0f;

	Vector2 get_base_position() const {
		return base_position;
	}

	Vector2 get_joint_position(float alpha1) const {
		return base_position + Vector2{
			cosf(alpha1) * l1,
			sinf(alpha1) * l1
		};
	}

	Vector2 get_effector_position(float alpha1, float alpha2) const {
		return base_position + Vector2{
			cosf(alpha1) * l1 + cosf(alpha1 + alpha2) * l2,
			sinf(alpha1) * l1 + sinf(alpha1 + alpha2) * l2
		};
	}

	bool are_angles_acceptable(const Vector2& angles) const {
		for (const auto& o : obstacles)
		{
			if (o.intersects_segment(get_base_position(), get_joint_position(angles.x)) 
				|| o.intersects_segment(get_joint_position(angles.x), get_effector_position(angles.x, angles.y)))
				return false;
		}
		return true;
	}

	// returns angles from range [0, 2pi)
	std::list<Vector2> get_angles(const Vector2& position) const {
		const Vector2 relative = position - base_position;

		if (relative.length() > l1 + l2 || relative.length() < l1 - l2)
			return {};

		auto cos2 = (relative.x * relative.x + relative.y * relative.y - l1 * l1 - l2 * l2) / (2.0f * l1 * l2);
		auto alpha2 = acosf(cos2);
		auto abs_sin2 = sqrtf(1.0f - cos2 * cos2);
		auto alpha1plus = atan2f((l1 + cos2 * l2) * relative.y - relative.x * abs_sin2 * l2, (l1 + cos2 * l2) * relative.x + relative.y * abs_sin2 * l2);
		auto alpha1minus = atan2f((l1 + cos2 * l2) * relative.y + relative.x * abs_sin2 * l2, (l1 + cos2 * l2) * relative.x - relative.y * abs_sin2 * l2);

		Vector2 angles1{ alpha1plus < 0 ? (TWO_PI + alpha1plus) : alpha1plus, alpha2},
			angles2{ alpha1minus < 0 ? (TWO_PI + alpha1minus) : alpha1minus, TWO_PI - alpha2 };

		std::list<Vector2> result;
		if (are_angles_acceptable(angles1))
			result.push_back(angles1);
		if (are_angles_acceptable(angles2))
			result.push_back(angles2);
		
		return result;
	}

	Task create_animation_task(const std::vector<Vector2>& path);
	bool is_animating() const { return !task_ended; }
};