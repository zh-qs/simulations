#include "animation_view_window.h"
#include "imgui.h"
#include "mesh_generator.h"

AnimationViewWindow::AnimationViewWindow(const std::string& name, const Animation& animation, bool use_euler_angles) : mesh(ShaderType::Phong), axes(ShaderType::Axes), animation(animation), use_euler_angles(use_euler_angles)
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

	MeshGenerator::load_from_file(mesh, "models/duck.txt");
	initial_mesh_model = Matrix4x4::scale(-0.01f, 0.01f, -0.01f);
	mesh.color = { 0.1f, 0.7f, 0.2f, 1.0f };

	std::vector<Vector3> axes_points = {
		{-100.0f,0.0f,0.0f}, {100.0f,0.0f,0.0f},
		/*{0.0f,-100.0f,0.0f}, {0.0f,100.0f,0.0f},*/
		{0.0f,0.0f,-100.0f}, {0.0f,0.0f,100.0f},
	};
	axes.color = { 0.8f,0.8f,0.8f,1.0f };
	axes.set_data(axes_points);
}

void AnimationViewWindow::align_with(const AnimationViewWindow& other)
{
	camera = other.camera;
}

void AnimationViewWindow::build()
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

	mesh.model = animation.get_model_matrix(use_euler_angles) * initial_mesh_model;

	// render other objects
	glDepthFunc(GL_LESS);
	mesh.render(camera, width, height);

	// render keyframes
	if (animation.keyframes.enabled)
	{
		for (int i = 0; i < animation.keyframes.count; ++i)
		{
			mesh.model = animation.get_model_matrix_for_keyframe(i, use_euler_angles) * initial_mesh_model;
			mesh.render(camera, width, height);
		}
	}

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
