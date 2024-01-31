#pragma once

#include "window.h"
#include "mesh.h"
#include "camera.h"
#include "ode.h"
#include "state_listener.h"
#include "animation.h"
#include <string>

class AnimationViewWindow : public Window {
	const Animation& animation;

	TriMesh mesh;
	LineMesh grid;
	LineMesh axes;
	Camera camera;
	FrameBuffer fbo;
	RenderTexture texture;

	Matrix4x4 initial_mesh_model;

	bool use_euler_angles;
public:
	AnimationViewWindow(const std::string& name, const Animation& animation, bool use_euler_angles);
	void align_with(const AnimationViewWindow& other);
	virtual void build() override;
};