#pragma once

#include "mesh.h"

class MeshGenerator
{
public:
	static void generate_grid(LineMesh& mesh, unsigned int half_x_count, unsigned int half_z_count, float x_length, float z_length);
	static void load_from_file(TriMesh& mesh, const char* filename, bool normalize = false);
	static void generate_cube(TriMesh& mesh);
	static void generate_inverted_cube(TriMesh& mesh);
	static void generate_cylinder(TriMesh& mesh, float radius, float height, unsigned int circle_divisions);
};