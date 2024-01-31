#include "puma_simulation.h"

inline bool iszero(const float& x)
{
	constexpr float EPS = 3e-07;
	return std::abs(x) < EPS;
}

inline bool isone(const float& x)
{
	return iszero(x - 1);
}

inline float angle_distance(const float& x, const float& y)
{
	const float d = std::abs(x - y);
	return std::min(d, TWO_PI - d);
}

float cyclic_angle_lerp(const float& start, const float& end, const float& t)
{
	float s = start, e = end;
	if (s > PI)
		s -= TWO_PI;
	if (s < -PI)
		s += TWO_PI;
	if (e > PI)
		e -= TWO_PI;
	if (e < -PI)
		e += TWO_PI;
	const float d = std::abs(s - e);
	if (d <= TWO_PI - d)
		return lerp(s, e, t);
	if (s < 0)
		return lerp(s + TWO_PI, e, t);
	return lerp(s, e + TWO_PI, t);
}

Vector<float, 6> lerp_parameters(const Vector<float, 6>& start, const Vector<float, 6>& end, const float& t)
{
	return {
		cyclic_angle_lerp(start[0], end[0], t), // a1
		cyclic_angle_lerp(start[1], end[1], t), // a2
		lerp(start[2], end[2], t),				// q2
		cyclic_angle_lerp(start[3], end[3], t), // a3
		cyclic_angle_lerp(start[4], end[4], t), // a4
		cyclic_angle_lerp(start[5], end[5], t)  // a5
	};
}

bool PumaSimulation::PumaSimulationTaskStep::execute(const TaskParameters& parameters)
{
	if (puma.parameters.paused)
		return true;

	t += puma.parameters.speed * parameters.delta_time;

	puma.current_parameter_interpolation = lerp_parameters(puma.start_parameters, puma.end_parameters, t);

	auto current_position = lerp(puma.parameters.start_position, puma.parameters.end_position, t);
	auto current_rotation = puma.parameters.use_slerp ? slerp(puma.parameters.start_rotation, puma.parameters.end_rotation, t)
		: lerp_shortest_path(puma.parameters.start_rotation, puma.parameters.end_rotation, t);
	current_rotation.normalize();

	puma.current_view_space_interpolation = puma.inverse_kinematics_nearest_to(current_position, current_rotation, puma.current_view_space_interpolation);

	return t < 1;
}

std::list<Vector<float, 6>> PumaSimulation::inverse_kinematics(const Vector3& position, const Quaternion<float>& rotation) // unused
{
	//auto rot = Matrix4x4::rotation(rotation);
	//// TODO s1,s2,c1,s4,c4,s23 == 0
	//float a1 = atan2f(-position.z + parameters.l4 * rot.elem[2][0], position.x - parameters.l1 * rot.elem[0][0]); // TODO sgn (l3s23+q2c2)!

	//float s1 = sinf(a1), c1 = cosf(a1);
	//float s4 = -rot.elem[2][0] * c1 - rot.elem[0][0] * s1;

	//float a23 = atan2f(rot.elem[1][0], (rot.elem[0][0] + s1 * s4) / c1); // TODO sgn c4!
	//float s23 = sinf(a23), c23 = cosf(a23);

	//float a4 = atan2f(s4, rot.elem[1][0] / s23);
	//float s4s23 = s4 * s23;

	//float s5 = (rot.elem[1][1] * s4s23 - rot.elem[1][2] * c23) / (c23 * c23 + s4s23 * s4s23);
	//float c5 = (rot.elem[1][2] + c23 * s5) / s4s23;

	//float a5 = atan2f(s5, c5);

	//float a2 = atan2f(position.y - parameters.l1 + parameters.l3 * c23 - parameters.l4 * rot.elem[1][0], (position.x - parameters.l4 * rot.elem[0][0]) / c1 - parameters.l3 * s23);
	//
	//float a3 = a23 - a2;
	//float s2 = sinf(a2);

	//float q2 = (position.y + parameters.l3 * c23 - parameters.l4 * rot.elem[1][0] - parameters.l1) / s2;
	//return { { a1,a2,q2,a3,a4,a5 } };
	auto rot = Matrix4x4::rotation(rotation);
	// TODO s1,s2,c1,s4,c4,s23 == 0
	float a1 = atan2f(-position.z + parameters.l4 * rot.elem[2][0], position.x - parameters.l1 * rot.elem[0][0]); // TODO sgn (l3s23+q2c2)!

	float s1 = sinf(a1), c1 = cosf(a1);
	float s4 = -rot.elem[2][0] * c1 - rot.elem[0][0] * s1;
	float c4sq = (rot.elem[0][0] * c1 - rot.elem[2][0] * s1) * (rot.elem[0][0] * c1 - rot.elem[2][0] * s1) + rot.elem[1][0] * rot.elem[1][0];

	float a23 = atan2f(rot.elem[1][0], (rot.elem[0][0] + s1 * s4) / c1); // TODO sgn c4!
	float s23 = sinf(a23), c23 = cosf(a23);

	float a4 = atan2f(s4, sqrt(c4sq));
	float s4s23 = s4 * s23;

	float s5 = (rot.elem[1][1] * s4s23 - rot.elem[1][2] * c23) / (c23 * c23 + s4s23 * s4s23);
	float c5 = (rot.elem[1][2] + c23 * s5) / s4s23;

	float a5 = atan2f(s5, c5);

	float a2 = atan2f(position.y - parameters.l1 + parameters.l3 * c23 - parameters.l4 * rot.elem[1][0], (position.x - parameters.l4 * rot.elem[0][0]) / c1 - parameters.l3 * s23);

	float a3 = a23 - a2;
	float s2 = sinf(a2);

	float q2 = (position.y + parameters.l3 * c23 - parameters.l4 * rot.elem[1][0] - parameters.l1) / s2;
	return { { a1,a2,q2,a3,a4,a5 } };
}

Vector<float, 6> PumaSimulation::inverse_kinematics_nearest_to(const Vector3& position, const Quaternion<float>& rotation, const Vector<float, 6>& other)
{
	auto rot = Matrix4x4::rotation(rotation);
	float a1;
	bool l3s23pq2c2positive = true;
	if (iszero(position.x - parameters.l1 * rot.elem[0][0]) && iszero(-position.z + parameters.l4 * rot.elem[2][0]))
	{
		// a1 dowolny
		a1 = other[0];
	}
	else
	{
		a1 = atan2f(-position.z + parameters.l4 * rot.elem[2][0], position.x - parameters.l1 * rot.elem[0][0]);
		// sgn (l3s23+q2c2)
		if (a1 < 0 && angle_distance(a1 + PI, other[0]) < angle_distance(a1, other[0]))
		{
			a1 = a1 + PI; // l3s23+q2c2 < 0
			l3s23pq2c2positive = false;
		}
		else if (a1 > 0 && angle_distance(a1 - PI, other[0]) < angle_distance(a1, other[0]))
		{
			a1 = a1 - PI; // l3s23+q2c2 < 0
			l3s23pq2c2positive = false;
		}
	}
	float s1 = sinf(a1), c1 = cosf(a1);

	float s4 = -rot.elem[2][0] * c1 - rot.elem[0][0] * s1;
	float c4sq = (rot.elem[0][0] * c1 - rot.elem[2][0] * s1) * (rot.elem[0][0] * c1 - rot.elem[2][0] * s1) + rot.elem[1][0] * rot.elem[1][0];
	float a23, a5, a4;
	float s23, c23;
	if (isone(s4) || isone(-s4)) // c4 == 0
	{
		// a5 dowolny
		a5 = other[5];
		if (isone(s4))
		{
			float a23m5 = atan2f(rot.elem[1][2], rot.elem[1][1]);
			a23 = a23m5 + a5;
			a4 = HALF_PI;
		}
		else
		{
			float a23p5 = atan2f(-rot.elem[1][2], rot.elem[1][1]);
			a23 = a23p5 - a5;
			a4 = -HALF_PI;
		}
		s23 = sinf(a23);
		c23 = cosf(a23);
	}
	else
	{
		if (iszero(c1))
			a23 = atan2f(rot.elem[1][0], (-rot.elem[2][0] + c1 * s4) / s1);
		else
			a23 = atan2f(rot.elem[1][0], (rot.elem[0][0] + s1 * s4) / c1);
		float c4 = sqrt(c4sq);
		// sgn c4
		if (a23 < 0 && angle_distance(a23 + PI, other[1] + other[3]) < angle_distance(a23, other[1] + other[3]))
		{
			a23 = a23 + PI; // c4 < 0
			c4 = -c4;
		}
		else if (a23 > 0 && angle_distance(a23 - PI, other[1] + other[3]) < angle_distance(a23, other[1] + other[3]))
		{
			a23 = a23 - PI; // c4 < 0
			c4 = -c4;
		}
		else
		{
			c4 = c4;
		}
		s23 = sinf(a23);
		c23 = cosf(a23);

		//float a4 = atan2f(s4, rot.elem[1][0] / s23);
		a4 = atan2f(s4, c4);
		float s4s23 = s4 * s23;
		float s5, c5;

		if (iszero(s4))
		{
			if (iszero(c23))
			{
				s5 = (rot.elem[0][1] * s1 + rot.elem[2][1] * c1) / c4;
				c5 = (-rot.elem[0][1] * c1 + rot.elem[2][1] * s1) / s23;
			}
			else
			{
				s5 = -rot.elem[1][2] / c23;
				c5 = rot.elem[1][1] / c23;
			}
		}
		else
		{
			if (iszero(s23))
			{
				s5 = -rot.elem[1][2] / c23;
				c5 = rot.elem[1][1] / c23;
			}
			else
			{
				s5 = (rot.elem[1][1] * s4s23 - rot.elem[1][2] * c23) / (c23 * c23 + s4s23 * s4s23);
				c5 = (rot.elem[1][2] + c23 * s5) / s4s23;
			}
		}

		a5 = atan2f(s5, c5);
	}

	float a2;
	if (iszero(c1))
	{
		if (position.y - parameters.l1 + parameters.l3 * c23 - parameters.l4 * rot.elem[1][0] == 0 && (parameters.l4 * rot.elem[2][0] - position.z) / s1 - parameters.l3 * s23 == 0) // q2 == 0
			a2 = other[1];
		else
			a2 = atan2f(position.y - parameters.l1 + parameters.l3 * c23 - parameters.l4 * rot.elem[1][0], (parameters.l4 * rot.elem[2][0] - position.z) / s1 - parameters.l3 * s23);
	}
	else
	{
		if (position.y - parameters.l1 + parameters.l3 * c23 - parameters.l4 * rot.elem[1][0] == 0 && (position.x - parameters.l4 * rot.elem[0][0]) / c1 - parameters.l3 * s23 == 0) // q2 == 0
			a2 = other[1];
		else
			a2 = atan2f(position.y - parameters.l1 + parameters.l3 * c23 - parameters.l4 * rot.elem[1][0], (position.x - parameters.l4 * rot.elem[0][0]) / c1 - parameters.l3 * s23);
	}

	float a3 = a23 - a2;
	float s2 = sinf(a2), c2 = cosf(a2);

	float q2;
	if (iszero(s2))
	{
		if (iszero(s1))
			q2 = (position.x - parameters.l3 * c1 * s23 - parameters.l4 * rot.elem[0][0]) / (c1 * c2);
		else
			q2 = (parameters.l4 * rot.elem[2][0] - parameters.l3 * s1 * s23 - position.z) / (s1 * c2);
	}
	else
		q2 = (position.y + parameters.l3 * c23 - parameters.l4 * rot.elem[1][0] - parameters.l1) / s2;
	return { a1,a2,q2,a3,a4,a5 };
}

PumaSimulation::PumaSimulation()
{
	reset();
	reset_end_parameters();
}

Matrix4x4 PumaSimulation::get_relative_model_for_mesh(int mesh_idx, bool interpolate_parameters) const
{
	// all matrices are inversions of frame matrices!
	switch (mesh_idx)
	{
	case 0: // base
		return Matrix4x4::identity();
	case 1: // first arm
		return Matrix4x4::rotation_y(interpolate_parameters ? current_parameter_interpolation[0] : current_view_space_interpolation[0]);
	case 2: // 1-2 joint
		return Matrix4x4::translation({ 0.0f, parameters.l1, 0.0f });
	case 3: // second arm
		return Matrix4x4::rotation_z(interpolate_parameters ? current_parameter_interpolation[1] : current_view_space_interpolation[1]);
	case 4: // 2-3 joint
		return Matrix4x4::translation({ interpolate_parameters ? current_parameter_interpolation[2] : current_view_space_interpolation[2], 0.0f, 0.0f });
	case 5: // third arm
		return Matrix4x4::rotation_z(interpolate_parameters ? current_parameter_interpolation[3] : current_view_space_interpolation[3]);
	case 6: // 3-4 joint
		return Matrix4x4::translation({ 0.0f, -parameters.l3, 0.0f });
	case 7: // fourth arm
		return Matrix4x4::rotation_y(interpolate_parameters ? current_parameter_interpolation[4] : current_view_space_interpolation[4]);
	case 8: // effector
		return Matrix4x4::rotation_x(interpolate_parameters ? current_parameter_interpolation[5] : current_view_space_interpolation[5]) * Matrix4x4::translation({ parameters.l4, 0.0f, 0.0f });
	default:
		throw std::runtime_error("Wrong mesh index!");
	}
}

float PumaSimulation::get_q2(bool interpolate_parameters) const
{
	return interpolate_parameters ? current_parameter_interpolation[2] : current_view_space_interpolation[2];
}

Matrix4x4 PumaSimulation::get_start_model() const
{
	return Matrix4x4::translation(parameters.start_position) * Matrix4x4::rotation(parameters.start_rotation);
}

Matrix4x4 PumaSimulation::get_end_model() const
{
	return Matrix4x4::translation(parameters.end_position) * Matrix4x4::rotation(parameters.end_rotation);
}

void PumaSimulation::reset()
{
	start_parameters = current_parameter_interpolation = current_view_space_interpolation = inverse_kinematics_nearest_to(parameters.start_position, parameters.start_rotation, { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f });//inverse_kinematics(parameters.start_position, parameters.start_rotation).front();
	parameters.paused = false;
}

void PumaSimulation::reset_end_parameters()
{
	end_parameters = inverse_kinematics_nearest_to(parameters.end_position, parameters.end_rotation, { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f });//inverse_kinematics(parameters.start_position, parameters.start_rotation).front();
}

Task PumaSimulation::create_task()
{
	reset_end_parameters();
	Task task(task_ended);
	task.add_step<PumaSimulationTaskStep>(*this);
	return task;
}
