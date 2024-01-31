#include "puma_view_window.h"
#include "mesh_generator.h"

void PumaViewWindow::draw_gimbal(const Matrix4x4& model, const Camera& camera, int width, int height)
{
	constexpr float big_scale = 1.0f, small_scale = 0.1f;

	// X axis
	gimbal_stick.model = model * Matrix4x4::scale(big_scale, small_scale, small_scale) * Matrix4x4::translation({ 0.0f,-0.5f,-0.5f });
	gimbal_stick.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	gimbal_stick.render(camera, width, height);

	// Y axis
	gimbal_stick.model = model * Matrix4x4::scale(small_scale, big_scale, small_scale) * Matrix4x4::translation({ -0.5f,0.0f,-0.5f });
	gimbal_stick.color = { 0.0f, 1.0f, 0.0f, 1.0f };
	gimbal_stick.render(camera, width, height);

	// Z axis
	gimbal_stick.model = model * Matrix4x4::scale(small_scale, small_scale, big_scale) * Matrix4x4::translation({ -0.5f,-0.5f,0.0f });
	gimbal_stick.color = { 0.0f, 0.0f, 1.0f, 1.0f };
	gimbal_stick.render(camera, width, height);
}

PumaViewWindow::PumaViewWindow(const std::string& name, const PumaSimulation& puma, bool interpolate_parameters) : gimbal_stick(ShaderType::Phong), puma_meshes{ TriMesh(ShaderType::Phong), TriMesh(ShaderType::Phong), TriMesh(ShaderType::Phong), TriMesh(ShaderType::Phong), TriMesh(ShaderType::Phong), TriMesh(ShaderType::Phong), TriMesh(ShaderType::Phong), TriMesh(ShaderType::Phong), TriMesh(ShaderType::Phong) }, axes(ShaderType::Axes), puma(puma), interpolate_parameters(interpolate_parameters)
{
	this->name = name;

	fbo.init();
	fbo.bind();
	texture.init();
	texture.bind();
	texture.configure();
	fbo.unbind();

	MeshGenerator::generate_grid(grid, 100, 100, 1.0f, 1.0f);
	grid.color = { 0.5f,0.5f,0.5f,1.0f };

	for (int i = 0; i < PUMA_MESH_COUNT; ++i)
		MeshGenerator::generate_cylinder(puma_meshes[i], 0.2f, 1.0f, 25);

	MeshGenerator::generate_cube(gimbal_stick);

	std::vector<Vector3> axes_points = {
		{-100.0f,0.0f,0.0f}, {100.0f,0.0f,0.0f},
		/*{0.0f,-100.0f,0.0f}, {0.0f,100.0f,0.0f},*/
		{0.0f,0.0f,-100.0f}, {0.0f,0.0f,100.0f},
	};
	axes.color = { 0.8f,0.8f,0.8f,1.0f };
	axes.set_data(axes_points);

	initial_mesh_models[0] = Matrix4x4::scale(1.1f, 0.45f, 1.1f) * Matrix4x4::translation({ 0.0f, -0.5f, 0.0f });
	initial_mesh_models[1] = Matrix4x4::identity();
	initial_mesh_models[2] = Matrix4x4::rotation_x(HALF_PI) * Matrix4x4::scale(1.1f, 0.45f, 1.1f) * Matrix4x4::translation({ 0.0f, -0.5f, 0.0f });
	initial_mesh_models[3] = Matrix4x4::rotation_z(-HALF_PI);
	initial_mesh_models[4] = Matrix4x4::rotation_x(HALF_PI) * Matrix4x4::scale(1.1f, 0.45f, 1.1f) * Matrix4x4::translation({ 0.0f, -0.5f, 0.0f });
	initial_mesh_models[5] = Matrix4x4::rotation_z(PI);
	initial_mesh_models[6] = Matrix4x4::scale(1.1f, 0.45f, 1.1f) * Matrix4x4::translation({ 0.0f, -0.5f, 0.0f });
	initial_mesh_models[7] = Matrix4x4::rotation_z(-HALF_PI);
	initial_mesh_models[8] = Matrix4x4::identity();
	puma_meshes[0].color = puma_meshes[2].color = puma_meshes[4].color = puma_meshes[6].color = { 1.0f, 0.0f, 0.0f, 1.0f };
	puma_meshes[1].color = puma_meshes[5].color = puma_meshes[7].color = { 1.0f, 1.0f, 0.0f, 1.0f };
	puma_meshes[3].color = { 0.0f, 1.0f, 0.0f, 1.0f };
}

void PumaViewWindow::align_with(const PumaViewWindow& other)
{
	camera = other.camera;
}

void PumaViewWindow::build()
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

	grid.render(camera, width, height);
	axes.render(camera, width, height);

	glDepthFunc(GL_LESS);

	puma_meshes[0].model = puma.get_relative_model_for_mesh(0, interpolate_parameters);
	for (int i = 1; i < PUMA_MESH_COUNT; ++i)
	{
		puma_meshes[i].model = puma_meshes[i - 1].model * puma.get_relative_model_for_mesh(i, interpolate_parameters);
	}
	draw_gimbal(puma_meshes[PUMA_MESH_COUNT - 1].model, camera, width, height);
	for (int i = 0; i < PUMA_MESH_COUNT; ++i)
		puma_meshes[i].model = puma_meshes[i].model * initial_mesh_models[i];
	puma_meshes[1].model = puma_meshes[1].model * Matrix4x4::scale(1.0f, puma.parameters.l1, 1.0f);
	puma_meshes[3].model = puma_meshes[3].model * Matrix4x4::scale(1.0f, puma.get_q2(interpolate_parameters), 1.0f);
	puma_meshes[5].model = puma_meshes[5].model * Matrix4x4::scale(1.0f, puma.parameters.l3, 1.0f);
	puma_meshes[7].model = puma_meshes[7].model * Matrix4x4::scale(1.0f, puma.parameters.l4, 1.0f);

	// render other objects
	for (int i = 0; i < PUMA_MESH_COUNT - 1; ++i)
		puma_meshes[i].render(camera, width, height);

	draw_gimbal(puma.get_start_model(), camera, width, height);
	draw_gimbal(puma.get_end_model(), camera, width, height);

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
