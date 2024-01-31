#include "spinning_top_view_window.h"
#include <vector>
#include "mesh_generator.h"

void SpinningTopViewWindow::update_trace()
{
	if (!last_trace_point.has_value())
		return;

	if (trace_points.size() < simulation.parameters.trace_length)
		trace_points.push_back(last_trace_point.value());
	else
	{
		memcpy(trace_points.data(), trace_points.data() + 1, (trace_points.size() - 1) * sizeof(Vector3));
		trace_points[trace_points.size() - 1] = last_trace_point.value();
	}
	trace.set_data(trace_points);
}

SpinningTopViewWindow::SpinningTopViewWindow(const SpinningTopSimulation& sim) : simulation(sim), fbo(), axes(ShaderType::Axes), cube(ShaderType::Phong)
{
	name = "Spinning top";

	initial_cube_rotation = Matrix4x4::identity(); // TODO
	float invsqrt2 = 0.5f * sqrtf(2.0f);
	float invsqrt3 = sqrtf(3.0f) / 3.0f;
	float invsqrt6 = sqrtf(6.0f) / 6.0f;

	initial_cube_rotation.elem[0][0] = invsqrt2;
	initial_cube_rotation.elem[0][1] = -invsqrt2;
	initial_cube_rotation.elem[0][2] = 0;
	initial_cube_rotation.elem[1][0] = invsqrt3;
	initial_cube_rotation.elem[1][1] = invsqrt3;
	initial_cube_rotation.elem[1][2] = invsqrt3;
	initial_cube_rotation.elem[2][0] = -invsqrt6;
	initial_cube_rotation.elem[2][1] = -invsqrt6;
	initial_cube_rotation.elem[2][2] = 2.0f * invsqrt6;

	std::vector<Vector3> axes_points = {
		{-10.0f,0.0f,0.0f}, {10.0f,0.0f,0.0f},
		{0.0f,-10.0f,0.0f}, {0.0f,10.0f,0.0f},
		{0.0f,0.0f,-10.0f}, {0.0f,0.0f,10.0f},
	};

	std::vector<Vector3> diameter_points = {
		{0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f}
	};

	std::vector<Vector3> plane_points = {
		{-2.0f, 0.0f, -2.0f}, {2.0f, 0.0f, 2.0f}, {2.0f, 0.0f, -2.0f},
		{-2.0f, 0.0f, -2.0f}, {-2.0f, 0.0f, 2.0f}, {2.0f, 0.0f, 2.0f},
		{-2.0f, 0.0f, -2.0f}, {2.0f, 0.0f, -2.0f}, {2.0f, 0.0f, 2.0f},
		{-2.0f, 0.0f, -2.0f}, {2.0f, 0.0f, 2.0f}, {-2.0f, 0.0f, 2.0f}
	};

	fbo.init();
	fbo.bind();
	texture.init();
	texture.bind();
	texture.configure();
	fbo.unbind();

	current_cube_rotation = initial_cube_rotation;
	cube.model = initial_cube_rotation;
	cube.color = { 0.2f, 0.2f, 0.8f, 0.6f };
	MeshGenerator::generate_cube(cube);

	axes.color = { 1.0f,1.0f,1.0f,1.0f };
	axes.set_data(axes_points);

	cube_diameter.model = cube.model;
	cube_diameter.color = { 1.0f,1.0f,0.0f,1.0f };
	cube_diameter.set_data(diameter_points);

	trace.color = { 1.0f,1.0f,1.0f,1.0f };

	gravity_plane.color = { 0.7f,0.7f,0.9f,0.6f };
	gravity_plane.set_data(plane_points);
}

void SpinningTopViewWindow::build()
{
	update_trace();

	ImGui::Begin(get_name());

	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
	int width = canvas_sz.x, height = canvas_sz.y;

	// set cube rotation
	if (simulation.is_started())
	{
		cube.model = current_cube_rotation;
	}
	else
	{
		cube.model = Matrix4x4::rotation_z(to_rad(simulation.parameters.initial_z_rotation_deg)) * initial_cube_rotation * Matrix4x4::uniform_scale(simulation.parameters.cube_dim);
	}
	cube_diameter.model = cube.model;

	texture.bind();
	texture.set_size(width, height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	GLint old_viewport[4];
	glGetIntegerv(GL_VIEWPORT, old_viewport);

	fbo.bind();
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render non-transparent objects
	glDepthMask(GL_TRUE);
	cube_diameter.render(camera, width, height);
	axes.render(camera, width, height);
	trace.render(camera, width, height);

	// render transparent objects 
	// TODO proper transparency!
	glDepthMask(GL_FALSE);
	if (simulation.parameters.gravity_present)
		gravity_plane.render(camera, width, height);
	cube.render(camera, width, height);
	
	fbo.unbind();

	glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);

	ImGui::Image((void*)(intptr_t)texture.get_id(), canvas_sz);
	if (ImGui::IsItemHovered())
	{
		auto& io = ImGui::GetIO();
		// zoom using mouse wheel
		if (io.MouseWheel != 0.0f) {
			camera.zoom(powf(1.3f, io.MouseWheel));
		}
		// move scene
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			camera.move_by({ -io.MouseDelta.x / width * 5.0f, -io.MouseDelta.y / height * 5.0f, 0 });
		}
		// rotate scene
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			camera.rotate(io.MouseDelta.y * 0.01f, -io.MouseDelta.x * 0.01f, 0);
		}
	}

	ImGui::End();
}

void SpinningTopViewWindow::reset()
{
	last_trace_point = std::nullopt;
	trace_points.clear();
	trace.set_data({});
}

void SpinningTopViewWindow::notify(const float& arg, const Vector<float, 7>& value)
{
	current_cube_rotation = Matrix4x4::rotation({ value[3],value[4],value[5],value[6] }) * initial_cube_rotation * Matrix4x4::uniform_scale(simulation.parameters.cube_dim);

	last_trace_point = (current_cube_rotation * Vector4{ 1.0f, 1.0f, 1.0f, 1.0f }).xyz(); // TODO mutex??
}
