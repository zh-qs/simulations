#include "spring_simulation.h"

constexpr bool MULTI_FRAME = false; // TODO multi-threaded

bool SpringSimulation::SpringSimulationTaskStep::execute(const TaskParameters& parameters)
{
    //simulation.ode_solver.next(parameters.delta_time);
    if (simulation.parameters.paused)
        return true;

    if constexpr (MULTI_FRAME)
    {
        float d = 0;
        while (d < parameters.delta_time)
        {
            d += simulation.parameters.delta;
            simulation.ode_solver.next(simulation.parameters.delta);
        }
        simulation.notify_listeners();
    }
    else
    {
        simulation.ode_solver.next(simulation.parameters.delta);
        simulation.notify_listeners();
    }
    return true;
}

SpringSimulation::SpringSimulation() : ode_solver(ODE<float, 2>{ 0, Vector<float, 2>{ 1.0f, 0.0f }, [this](const float& arg, const Vector<float, 2>& v) {
        return Vector<float, 2>{ v[1], (parameters.c * (parameters.w(arg) - v[0]) - parameters.k * v[1] + parameters.h(arg)) * parameters.invm };
    } }
) {}

Task SpringSimulation::get_task()
{
    Task task;
    task.add_step<SpringSimulationTaskStep>(*this);
    return task;
}
