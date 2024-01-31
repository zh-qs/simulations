#pragma once

#include "window.h"
#include "spinning_top_simulation.h"
#include "mesh.h"
#include "camera.h"
#include <vector>
#include <optional>

class SpinningTopViewWindow : public Window, public StateListener<float, 7> {
	friend class SpinningTopParametersWindow;

	const SpinningTopSimulation& simulation;
	TriMesh cube;
	LineMesh axes;
	LineMesh cube_diameter;
	LineStripMesh trace; // fancy stripped line with LineMesh :)
	TriMesh gravity_plane;
	Camera camera;
	FrameBuffer fbo;
	RenderTexture texture;

	Matrix4x4 initial_cube_rotation;
	Matrix4x4 current_cube_rotation;

	std::vector<Vector3> trace_points;
	std::optional<Vector3> last_trace_point = std::nullopt;

	void update_trace();
public:
	SpinningTopViewWindow(const SpinningTopSimulation& sim);

	virtual void build() override;
	virtual void notify(const float& arg, const Vector<float, 7>& value) override;
	virtual void reset() override;
};