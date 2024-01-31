#include "simple_chain.h"
#include <cmath>

bool SimpleChain::SimpleChainTaskStep::execute(const TaskParameters& parameters)
{
    if (chain.animation_parameters.paused)
        return true;

    t += path.size() * chain.animation_parameters.speed * parameters.delta_time;

    float t_int;
    t = std::modf(t, &t_int);
    idx += static_cast<int>(t_int);

    if (idx >= path.size() - 1)
        return false;

    auto current = path[idx];
    auto next = path[idx + 1];

    // correct lerp issues (jumping off [0,2pi])
    if (current.x > 1.5f * PI && next.x < HALF_PI)
        current.x -= TWO_PI;
    else if (current.x < HALF_PI && next.x > 1.5f * PI)
        next.x -= TWO_PI;

    if (current.y > 1.5f * PI && next.y < HALF_PI)
        current.y -= TWO_PI;
    else if (current.y < HALF_PI && next.y > 1.5f * PI)
        next.y -= TWO_PI;
    
    chain.animation_parameters.current_angles = lerp(current, next, t);
    return true;
}

Task SimpleChain::create_animation_task(const std::vector<Vector2>& path)
{
    Task task(task_ended);
    task.add_step<SimpleChainTaskStep>(*this, path);
    return task;
}