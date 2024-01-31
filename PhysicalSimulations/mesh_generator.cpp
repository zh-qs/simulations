#include "mesh_generator.h"
#include <fstream>

void MeshGenerator::generate_grid(LineMesh& mesh, unsigned int half_x_count, unsigned int half_z_count, float x_length, float z_length)
{
	std::vector<Vector3> points(4 * (half_x_count + half_z_count) + 4);
	unsigned int iv = 0, il = 0;
	float total_x = x_length * half_x_count, total_z = z_length * half_z_count;
	points[iv++] = { 0.0f,0.0f,total_z };
	points[iv++] = { 0.0f,0.0f,-total_z };
	for (unsigned int x = 1; x <= half_x_count; ++x)
	{
		points[iv++] = { x * x_length,0.0f,total_z };
		points[iv++] = { x * x_length,0.0f,-total_z };

		points[iv++] = { x * -x_length,0.0f,total_z };
		points[iv++] = { x * -x_length,0.0f,-total_z };

	}
	points[iv++] = { total_x,0.0f,0.0f };
	points[iv++] = { -total_x,0.0f,0.0f };
	for (unsigned int z = 1; z <= half_z_count; ++z)
	{
		points[iv++] = { total_x,0.0f,z * z_length };
		points[iv++] = { -total_x,0.0f,z * z_length };

		points[iv++] = { total_x,0.0f,z * -z_length };
		points[iv++] = { -total_x,0.0f,z * -z_length };
	}

	mesh.set_data(points);
}

void MeshGenerator::load_from_file(TriMesh& mesh, const char* filename, bool normalize)
{
	std::ifstream s(filename);

	if (!s.good())
		throw std::invalid_argument("Couldn't open file");

	int vertexCount;
	s >> vertexCount;

	std::vector<Vector3> vertices(vertexCount);
	std::vector<Vector3> normals(vertexCount);
	std::vector<Vector2> texCoords(vertexCount);

	for (int i = 0; i < vertexCount; ++i)
	{
		s >> vertices[i].x >> vertices[i].y >> vertices[i].z
			>> normals[i].x >> normals[i].y >> normals[i].z
			>> texCoords[i].x >> texCoords[i].y;
	}

	int triangleCount;
	s >> triangleCount;

	std::vector<IndexTriple> indices(triangleCount);
	for (int i = 0; i < triangleCount; ++i)
	{
		IndexTriple t;
		s >> t.i >> t.j >> t.k;
		indices[i] = t;
	}

	if (normalize)
	{
		float max_coord = 0.0f;
		for (int i = 0; i < vertices.size(); ++i)
		{
			max_coord = std::max(std::abs(vertices[i].x), max_coord);
			max_coord = std::max(std::abs(vertices[i].y), max_coord);
			max_coord = std::max(std::abs(vertices[i].z), max_coord);
		}
		const float scale = 0.5f / max_coord;
		Vector3 half = { 0.5f,0.25f,0.5f }; // y is 0.25f only for duck model (normally should be 0.5f)
		for (int i = 0; i < vertices.size(); ++i)
		{
			vertices[i] = scale * vertices[i] + half;
		}
	}

	mesh.set_data(vertices, indices);
	mesh.set_normals(normals);
}

void MeshGenerator::generate_cube(TriMesh& mesh)
{
	std::vector<Vector3> cube_points = {
		{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},//top
		{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},//bottom

		{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},//left
		{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f},
		{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f},//right

		{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},//front
		{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f},//back
	};
	std::vector<Vector3> cube_normals = {
		{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},//top
		{0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},//bottom

		{-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},//left
		{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},//right

		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},//front
		{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},//back
	};
	mesh.set_data(cube_points);
	mesh.set_normals(cube_normals);
}

void MeshGenerator::generate_inverted_cube(TriMesh& mesh)
{
	std::vector<Vector3> cube_points = {
		{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f},
		{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, //top
		{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, //bottom

		{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, //left
		{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, //right

		{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, //front
		{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, //back
	};
	std::vector<Vector3> cube_normals = {
		{0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},//top
		{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},//bottom

		{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},//left
		{-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},//right

		{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},//front
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},//back
	};
	mesh.set_data(cube_points);
	mesh.set_normals(cube_normals);
}

void MeshGenerator::generate_cylinder(TriMesh& mesh, float radius, float height, unsigned int circle_divisions)
{
	std::vector<Vector3> points(4 * circle_divisions + 2);
	std::vector<Vector3> normals(points.size());
	std::vector<IndexTriple> triangle_indices(4 * circle_divisions);
	float step = TWO_PI / circle_divisions;

	// points
	points[0] = { 0.0f, 0.0f, 0.0f };
	normals[0] = { 0.0f, -1.0f, 0.0f };
	for (unsigned int i = 1; i <= circle_divisions; ++i)
	{
		float t = i * step;
		float cos = cosf(t), sin = sinf(t);
		points[i] = points[i + 2 * circle_divisions] = { radius * cos, 0.0f, radius * sin };
		points[i + circle_divisions] = points[i + 3 * circle_divisions] = { radius * cos, height, radius * sin };
		normals[i] = { 0.0f, -1.0f, 0.0f };
		normals[i + circle_divisions] = { 0.0f,1.0f,0.0f };
		normals[i + 2 * circle_divisions] = normals[i + 3 * circle_divisions] = { cos, 0.0f, sin };
	}
	points[points.size() - 1] = { 0.0f, height, 0.0f };
	normals[normals.size() - 1] = { 0.0f, 1.0f, 0.0f };

	// lower base faces
	for (unsigned int i = 0; i < circle_divisions - 1; ++i)
	{
		triangle_indices[i] = { 0, i + 1, i + 2 };
	}
	triangle_indices[circle_divisions - 1] = { 0, circle_divisions, 1 };

	// side faces
	for (unsigned int i = 0; i < circle_divisions - 1; ++i)
	{
		triangle_indices[circle_divisions + i] = { i + 2 * circle_divisions + 2, i + 2 * circle_divisions + 1, i + 3 * circle_divisions + 1 };
		triangle_indices[2 * circle_divisions + i] = { i + 3 * circle_divisions + 2, i + 2 * circle_divisions + 2, i + 3 * circle_divisions + 1 };
	}
	triangle_indices[2 * circle_divisions - 1] = { 1 + 2 * circle_divisions, 3 * circle_divisions, 4 * circle_divisions };
	triangle_indices[3 * circle_divisions - 1] = { 3 * circle_divisions + 1, 1 + 2 * circle_divisions, 4 * circle_divisions };

	// upper base faces
	for (unsigned int i = 0; i < circle_divisions - 1; ++i)
	{
		triangle_indices[3 * circle_divisions + i] = { 4 * circle_divisions + 1, i + circle_divisions + 2, i + circle_divisions + 1 };
	}
	triangle_indices[4 * circle_divisions - 1] = { 4 * circle_divisions + 1, circle_divisions + 1, 2 * circle_divisions };

	mesh.set_data(points, triangle_indices);
	mesh.set_normals(normals);
}
