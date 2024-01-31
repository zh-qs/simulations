#include "flywheel_simulation.h"

bool FlywheelSimulation::FlywheelSimulationTaskStep::execute(const TaskParameters& parameters)
{
	if (simulation.parameters.paused)
		return true;

	float e;
	if (simulation.parameters.e0 <= 0)
		e = 0.0f;
	else
	{
		std::normal_distribution random(0.0f, simulation.parameters.e0);
		e = random(simulation.dev);
	}
	
	simulation.current_time += parameters.delta_time;
	simulation.current_angle = fmodf(simulation.parameters.omega * simulation.current_time, TWO_PI);
	const float Rcos = simulation.parameters.R * cosf(simulation.current_angle);
	const float L = std::max(simulation.parameters.L + e, simulation.parameters.R);
	simulation.current_x_coord = Rcos + sqrtf(L * L - simulation.parameters.R * simulation.parameters.R + Rcos * Rcos); // another solution -> minus instead of plus

	simulation.notify_listeners();
	simulation.previous_time = simulation.current_time;
	simulation.pprevious_x_coord = simulation.previous_x_coord;
	simulation.previous_x_coord = simulation.current_x_coord;

	simulation.iteration = std::min(simulation.iteration + 1, 2);

	return true;
}

ThreadTask FlywheelSimulation::create_task()
{
	ThreadTask task(parameters.delta_time);
	task.add_step<FlywheelSimulationTaskStep>(*this);
	return task;
}

void FlywheelSimulation::notify_listeners()
{
	if (iteration < 2)
		return;

	const float dt = current_time - previous_time;
	const float xt = (current_x_coord - pprevious_x_coord) / (2.0f * dt);//(current_x_coord - previous_x_coord) / (current_time - previous_time);
	const float xtt = (current_x_coord - 2 * previous_x_coord + pprevious_x_coord) / (dt * dt);
	Vector<float, 3> data = { current_x_coord, xt, xtt };
	for (auto* l : listeners)
		l->notify(current_time, data);
}

void FlywheelSimulation::reset()
{
	iteration = 0;
	pprevious_x_coord = 0;

	previous_time = 0;
	previous_x_coord = 0;

	current_angle = 0;
	current_time = 0;
	current_x_coord = 0;

	for (auto* l : listeners)
		l->reset();
}
