#include "spinning_top_simulation.h"
#include "quaternion.h"
#include "algebra.h"
#include <cmath>

static const float HALF_SQRT3 = sqrtf(3) * 0.5f;
constexpr bool MULTI_FRAME = false;

bool SpinningTopSimulation::SpinningTopSimulationTaskStep::execute(const TaskParameters& parameters)
{
	if (simulation.parameters.paused)
		return true;

	simulation.started = true;

	if constexpr (MULTI_FRAME)
	{
		float d = 0;
		while (d < parameters.delta_time)
		{
			d += simulation.parameters.delta;
			simulation.ode_solver.next(simulation.parameters.delta);
			simulation.ode_solver.normalize_quaternion();
		}
	}
	else
	{
		simulation.ode_solver.next(simulation.parameters.delta);
		simulation.ode_solver.normalize_quaternion();
	}
	simulation.notify_listeners();
	return true;
}

SpinningTopSimulation::SpinningTopSimulation() : ode_solver(ODE<float, 7>{0, {}, [this](const float& arg, const Vector<float, 7>& val) {
	Quaternion<float> W{ 0.0f, 0.5f * val[0], 0.5f * val[1], 0.5f * val[2] };
	Quaternion<float> Q{ val[3],val[4],val[5],val[6] };

	auto Qt = Q * W;

	float mass = pow3(parameters.cube_dim) * parameters.cube_density;
	Quaternion<float> fg = { 0.0f, 0.0f, parameters.gravity_present ? -1.0f * mass : 0.0f, 0.0f };
	auto Fg = Q.conj() * fg * Q;
	Vector<float, 3> Fgv = { Fg.x,Fg.y,Fg.z };

	Vector<float, 3> R = { 0.0f, HALF_SQRT3 * parameters.cube_dim, 0.0f };
	auto N = cross(R, Fgv);
	N = { inv_inertia[0] * N[0],inv_inertia[1] * N[1], inv_inertia[2] * N[2] };
	auto Wt = N + cross(Vector<float, 3>{ W.x,W.y,W.z }, Vector<float, 3>{ W.x * inv_inertia[0],W.y * inv_inertia[1],W.z * inv_inertia[2] });

	// TODO mo¿na to zoptymalizowaæ bo jest najpierw mno¿enie przez a^3*ro a potem dzielimy przez to w inv_inertia (tylko wtedy mo¿e nie byæ przeliczania tensora bezw³adnoœci i kod bêdzie mniej czytelny od strony teoretycznej)

	return Vector<float, 7>{ Wt[0],Wt[1],Wt[2],Qt.w,Qt.x,Qt.y,Qt.z };
}})
{
	reset();

	inv_inertia = initial_inv_inertia / (pow5(parameters.cube_dim) * parameters.cube_density);
}

ThreadTask SpinningTopSimulation::get_task()
{
	ThreadTask task(parameters.delta);
	task.add_step<SpinningTopSimulationTaskStep>(*this);
	return task;
}

void SpinningTopSimulation::reset() 
{
	reset_start_value();
	for (auto* l : state_listeners)
		l->reset();

	started = false;
}

void SpinningTopSimulation::reset_start_value()
{
	ode_solver.get_ode().get_start_value() = {
		0.0f,
		to_rad(parameters.angular_velocity_deg),
		0.0f,
		cosf(0.5f * to_rad(parameters.initial_z_rotation_deg)),
		0.0f,
		0.0f,
		sinf(0.5f * to_rad(parameters.initial_z_rotation_deg))
	};
	ode_solver.reset();
}
