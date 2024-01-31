#include "animation.h"

Vector3 correct_euler_angles(const Vector3& start, const Vector3& end)
{
    Vector3 result;
    if (start.x - end.x > 180.0f)
        result.x = end.x + 360.0f;
    else if (end.x - start.x > 180.0f)
        result.x = end.x - 360.0f;
    else
        result.x = end.x;

    if (start.y - end.y > 180.0f)
        result.y = end.y + 360.0f;
    else if (end.y - start.y > 180.0f)
        result.y = end.y - 360.0f;
    else
        result.y = end.y;

    if (start.z - end.z > 180.0f)
        result.z = end.z + 360.0f;
    else if (end.z - start.z > 180.0f)
        result.z = end.z - 360.0f;
    else
        result.z = end.z;

    return result;
}

bool Animation::AnimationTaskStep::execute(const TaskParameters& parameters)
{
    if (animation.parameters.paused)
        return true;

    t += animation.parameters.speed * parameters.delta_time;

    animation.current_position = lerp(animation.parameters.start_position, animation.parameters.end_position, t);

    // shortest path lerp
    Vector3 end_euler_corrected = correct_euler_angles(animation.parameters.start_euler_angles, animation.parameters.end_euler_angles);
    animation.current_euler_angles = lerp(animation.parameters.start_euler_angles, end_euler_corrected, t);

    if (animation.parameters.use_slerp)
        animation.current_rotation = slerp(animation.parameters.start_rotation, animation.parameters.end_rotation, t);
    else
    {
        //// lerp shortest path
        //const auto d = dot(animation.parameters.start_rotation, animation.parameters.end_rotation);
        //if (d < 0)
        //    animation.current_rotation = lerp(-animation.parameters.start_rotation, animation.parameters.end_rotation, t);
        //else
        //    animation.current_rotation = lerp(animation.parameters.start_rotation, animation.parameters.end_rotation, t);
        animation.current_rotation = lerp_shortest_path(animation.parameters.start_rotation, animation.parameters.end_rotation, t);
    }
        
    animation.current_rotation.normalize();

    return t < 1;
}

Animation::Animation()
{
    reset();
}

void Animation::reset()
{
    current_position = parameters.start_position;
    current_rotation = parameters.start_rotation;
    current_euler_angles = parameters.start_euler_angles;
    parameters.paused = false;
}

Matrix4x4 Animation::get_model_matrix_for_keyframe(int i, bool use_euler_angles) const
{
    float t = 0.0f;
    if (keyframes.count > 1)
        t = static_cast<float>(i) / (keyframes.count - 1);

    Vector3 position = lerp(parameters.start_position, parameters.end_position, t);
    if (use_euler_angles)
    {
        Vector3 end_euler_corrected = correct_euler_angles(parameters.start_euler_angles, parameters.end_euler_angles);
        Vector3 euler_angles = lerp(parameters.start_euler_angles, end_euler_corrected, t);
        return Matrix4x4::translation(position) * Matrix4x4::rotation_euler((PI / 180.0f) * euler_angles);
    }
    else
    {
        Quaternion<float> rotation;
        if (parameters.use_slerp)
            rotation = slerp(parameters.start_rotation, parameters.end_rotation, t);
        else
        {
            // lerp shortest path
            const auto d = dot(parameters.start_rotation, parameters.end_rotation);
            if (d < 0)
                rotation = lerp(-parameters.start_rotation, parameters.end_rotation, t);
            else
                rotation = lerp(parameters.start_rotation, parameters.end_rotation, t);
        }
        rotation.normalize();
        return Matrix4x4::translation(position) * Matrix4x4::rotation(rotation);
    }
}

Task Animation::create_task()
{
    Task task(task_ended);
    task.add_step<AnimationTaskStep>(*this);
    return task;
}
