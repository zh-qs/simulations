#include "jelly_view_window.h"
#include "mesh_generator.h"
#include <vector>

JellyViewWindow::JellyViewWindow(const JellySimulation& simulation) : simulation(simulation)
{
	name = "Jelly";

	fbo.init();
	fbo.bind();
	texture.init();
	texture.bind();
	texture.configure();
	fbo.unbind();

	std::vector<Vector3> cube_points = {
		{ -0.5f,-0.5f,-0.5f },
		{ -0.5f,-0.5f,0.5f },
		{ -0.5f,0.5f,-0.5f },
		{ 0.5f,-0.5f,-0.5f },
		{ 0.5f,0.5f,-0.5f },
		{ 0.5f,-0.5f,0.5f },
		{ -0.5f,0.5f,0.5f },
		{ 0.5f,0.5f,0.5f }
	};

	std::vector<IndexPair> lines = {
		{0,1}, {1,6}, {6,2}, {2,0},
		{7,4}, {4,3}, {3,5}, {5,7},
		{0,3}, {7,6}, {1,5}, {4,2}
	};

	cube.set_data(cube_points, lines);
	box_border.set_data(cube_points, lines);
	MeshGenerator::generate_inverted_cube(box);
	box.color = { 1.0f,1.0f,1.0f,0.5f };

	MeshGenerator::load_from_file(deformed_mesh, "models/duck.txt", true);
	deformed_mesh.color = { 0.0f,1.0f,0.0f,1.0f };
}

void JellyViewWindow::build()
{
	ImGui::Begin(get_name());

	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
	ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
	int width = canvas_sz.x, height = canvas_sz.y;

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

	// render axes
	glDepthFunc(GL_ALWAYS);

	/*grid.render(camera, width, height);
	axes.render(camera, width, height);

	mesh.model = animation.get_model_matrix(use_euler_angles) * initial_mesh_model;*/

	// render other objects
	glDepthFunc(GL_LESS);
	const auto* bezier_points = simulation.get_points();
	jelly.set_data(bezier_points);
	jelly.render_lines(camera, width, height);
	jelly.render_points(camera, width, height);
	jelly.render_bezier_patches(camera, width, height);

	deformed_mesh.render(camera, width, height, bezier_points, static_cast<int>(simulation.parameters.normal_mode), simulation.parameters.normal_scale);

	cube.model = Matrix4x4::translation(simulation.parameters.control_frame_position) * Matrix4x4::rotation_euler((PI / 180.0f) * simulation.parameters.control_frame_rotation_deg);
	cube.render(camera, width, height);

	// render bounding box
	const auto scale = Matrix4x4::scale(simulation.parameters.bounding_box.max() - simulation.parameters.bounding_box.min());
	box_border.model = Matrix4x4::translation(simulation.parameters.bounding_box.center()) * scale;
	box_border.render(camera, width, height);
	box.model = Matrix4x4::translation(simulation.parameters.bounding_box.min()) * scale;
	box.render(camera, width, height);

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
