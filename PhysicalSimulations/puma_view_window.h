#pragma once

#include "window.h"
#include "mesh.h"
#include "puma_simulation.h"

class PumaViewWindow : public Window {
	static constexpr int PUMA_MESH_COUNT = 9;

	const PumaSimulation& puma;

	TriMesh puma_meshes[PUMA_MESH_COUNT];
	TriMesh gimbal_stick;
	LineMesh grid;
	LineMesh axes;
	Camera camera;
	FrameBuffer fbo;
	RenderTexture texture;

	Matrix4x4 initial_mesh_models[PUMA_MESH_COUNT];

	bool interpolate_parameters;
	void draw_gimbal(const Matrix4x4& model, const Camera& camera, int width, int height);
public:
	PumaViewWindow(const std::string& name, const PumaSimulation& puma, bool interpolate_parameters);
	void align_with(const PumaViewWindow& other);
	virtual void build() override;
};