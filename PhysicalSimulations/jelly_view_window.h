#pragma once

#include "jelly_simulation.h"
#include "window.h"
#include "mesh.h"
#include "deformed_mesh.h"

class JellyViewWindow : public Window {
	friend class JellyParametersWindow;

	const JellySimulation& simulation;

	BezierCube jelly;
	LineMesh cube;
	LineMesh box_border;
	TriMesh box;
	DeformedTriMesh deformed_mesh;

	FrameBuffer fbo;
	Camera camera;
	RenderTexture texture;

public:
	JellyViewWindow(const JellySimulation& simulation);
	virtual void build() override;
};