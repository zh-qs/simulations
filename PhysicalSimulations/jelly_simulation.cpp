#include "jelly_simulation.h"
#include <random>

bool JellySimulation::JellySimulationTaskStep::execute(const TaskParameters& parameters)
{
	if (simulation.parameters.paused)
		return true;

	for (int i = 0; i < simulation.parameters.speed; ++i)
	{
		simulation.ode_solver.next(simulation.parameters.delta_time);
		simulation.collide_with_box();
	}

	return true;
}

void add_force(Vector3& l, const Vector3& point, const Vector3& pm1, const float eq_length) {
	const auto vec = pm1 - point;
	const auto nvec = normalize(vec);
	const auto add = vec - eq_length * nvec;
	l += vec - eq_length * nvec;
}

JellySimulation::JellySimulation() : ode_solver(ODE<float, ODE_DIM>(0.0f, {}, [this](const float& arg, const Vector<float, ODE_DIM>& val) {
	const float inv_m = 1.0f / parameters.single_point_mass;
	Vector<float, ODE_DIM> result;

	const float l0 = parameters.side_length / (SIDE_DIM - 1);
	const float l0diam = l0 * sqrtf(2.0f);

	// add forces between jelly control points
	for (int i = 0; i < SIDE_DIM; ++i)
		for (int j = 0; j < SIDE_DIM; ++j)
			for (int k = 0; k < SIDE_DIM; ++k)
			{
				const auto point = get_point(val, i, j, k);
				const auto velocity = get_velocity(val, i, j, k); //collide_with_box(point, get_velocity(val, i, j, k));
				set_point(result, i, j, k, velocity);

				Vector3 l{ 0.0f,0.0f,0.0f };

				if (i > 0)
				{
					add_force(l, point, get_point(val, i - 1, j, k), l0);
					if (j > 0)
						add_force(l, point, get_point(val, i - 1, j - 1, k), l0diam);
					if (k > 0)
						add_force(l, point, get_point(val, i - 1, j, k - 1), l0diam);
					if (j < SIDE_DIM - 1)
						add_force(l, point, get_point(val, i - 1, j + 1, k), l0diam);
					if (k < SIDE_DIM - 1)
						add_force(l, point, get_point(val, i - 1, j, k + 1), l0diam);
				}
				if (i < SIDE_DIM - 1)
				{
					add_force(l, point, get_point(val, i + 1, j, k), l0);
					if (j > 0)
						add_force(l, point, get_point(val, i + 1, j - 1, k), l0diam);
					if (k > 0)
						add_force(l, point, get_point(val, i + 1, j, k - 1), l0diam);
					if (j < SIDE_DIM - 1)
						add_force(l, point, get_point(val, i + 1, j + 1, k), l0diam);
					if (k < SIDE_DIM - 1)
						add_force(l, point, get_point(val, i + 1, j, k + 1), l0diam);
				}

				if (j > 0)
				{
					add_force(l, point, get_point(val, i, j - 1, k), l0);
					if (k > 0)
						add_force(l, point, get_point(val, i, j - 1, k - 1), l0diam);
					if (k < SIDE_DIM - 1)
						add_force(l, point, get_point(val, i, j - 1, k + 1), l0diam);
				}
				if (j < SIDE_DIM - 1)
				{
					add_force(l, point, get_point(val, i, j + 1, k), l0);
					if (k > 0)
						add_force(l, point, get_point(val, i, j + 1, k - 1), l0diam);
					if (k < SIDE_DIM - 1)
						add_force(l, point, get_point(val, i, j + 1, k + 1), l0diam);
				}

				if (k > 0)
				{
					add_force(l, point, get_point(val, i, j, k - 1), l0);
				}
				if (k < SIDE_DIM - 1)
				{
					add_force(l, point, get_point(val, i, j, k + 1), l0);
				}

				set_velocity(result, i, j, k, inv_m * (parameters.c1 * l - parameters.k * velocity) + (parameters.use_gravity ? parameters.gravity : Vector3{ 0.0f,0.0f,0.0f }));
			}

	// add forces between jelly corners and control frame
	if (parameters.use_control_frame)
	{
		auto rot_mat = Matrix4x4::rotation_euler((PI / 180.0f) * parameters.control_frame_rotation_deg).to_3x3();
		for (int idx = 0; idx < 8; ++idx)
		{
			const int i = (SIDE_DIM - 1) * ((idx & 0b100) >> 2), j = (SIDE_DIM - 1) * ((idx & 0b10) >> 1), k = (SIDE_DIM - 1) * (idx & 0b1);
			const auto point = get_point(val, i, j, k);
			const auto velocity = get_velocity(result, i, j, k);
			Vector3 frame_corner = parameters.side_length * Vector3{ ((idx & 0b100) >> 2) - 0.5f, ((idx & 0b10) >> 1) - 0.5f, (idx & 0b1) - 0.5f };
			frame_corner = rot_mat * frame_corner + parameters.control_frame_position;
			Vector3 force = parameters.c2 * (frame_corner - point);
			set_velocity(result, i, j, k, velocity + inv_m * force);
		}
	}

	return result;
})), cube_points(CUBE_POINT_COUNT)
{
	for (int i = 0; i < SIDE_DIM; ++i)
		for (int j = 0; j < SIDE_DIM; ++j)
			for (int k = 0; k < SIDE_DIM; ++k)
				get_cube_point(i, j, k) = parameters.side_length * Vector3{ i / 3.0f - 0.5f, j / 3.0f - 0.5f, k / 3.0f - 0.5f };

	reset();
}

void JellySimulation::punch()
{
	std::uniform_real_distribution<float> random(-parameters.punch_max, parameters.punch_max);
	std::random_device dev;
	for (int i = 0; i < CUBE_POINT_COUNT; ++i)
	{
		auto velocity = get_velocity(ode_solver.current(), i);
		set_velocity(ode_solver.current(), i, velocity + Vector3{ random(dev),random(dev),random(dev) });
	}
}

void JellySimulation::pinch()
{
	std::uniform_real_distribution<float> random(-parameters.pinch_max, parameters.pinch_max);
	std::random_device dev;
	for (int i = 0; i < CUBE_POINT_COUNT; ++i)
	{
		const auto point = get_point(ode_solver.current(), i);
		set_point(ode_solver.current(), i, point + Vector3{ random(dev),random(dev),random(dev) });
	}
}

void JellySimulation::collide_with_box()
{
	for (int i = 0; i < CUBE_POINT_COUNT; ++i)
	{
		auto point = get_point(ode_solver.current(), i);
		auto velocity = get_velocity(ode_solver.current(), i);
		if (point.x < parameters.bounding_box.x_min || point.x > parameters.bounding_box.x_max)
		{
			velocity.x = -velocity.x;
			if (parameters.bounce_only_one_component)
				velocity.x *= parameters.bounce_coefficient;
			else
				velocity *= parameters.bounce_coefficient;
			point.x = point.x < parameters.bounding_box.x_min ? (2.0f * parameters.bounding_box.x_min - point.x) : (2.0f * parameters.bounding_box.x_max - point.x);
		}
		if (point.y < parameters.bounding_box.y_min || point.y > parameters.bounding_box.y_max)
		{
			velocity.y = -velocity.y;
			if (parameters.bounce_only_one_component)
				velocity.y *= parameters.bounce_coefficient;
			else
				velocity *= parameters.bounce_coefficient;
			point.y = point.y < parameters.bounding_box.y_min ? (2.0f * parameters.bounding_box.y_min - point.y) : (2.0f * parameters.bounding_box.y_max - point.y);
		}
		if (point.z < parameters.bounding_box.z_min || point.z > parameters.bounding_box.z_max)
		{
			velocity.z = -velocity.z;
			if (parameters.bounce_only_one_component)
				velocity.z *= parameters.bounce_coefficient;
			else
				velocity *= parameters.bounce_coefficient;
			point.z = point.z < parameters.bounding_box.z_min ? (2.0f * parameters.bounding_box.z_min - point.z) : (2.0f * parameters.bounding_box.z_max - point.z);
		}
		set_point(ode_solver.current(), i, point);
		set_velocity(ode_solver.current(), i, velocity);
	}
}

void JellySimulation::reset()
{
	for (int i = 0; i < CUBE_POINT_COUNT; ++i)
	{
		set_point(ode_solver.get_ode().get_start_value(), i, cube_points[i]);
	}

	ode_solver.reset();
}
