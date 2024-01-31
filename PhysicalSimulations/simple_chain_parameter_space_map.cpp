#include "simple_chain_parameter_space_map.h"
#include "simple_chain.h"
#include <queue>

SimpleChainParameterSpaceMap::SimpleChainParameterSpaceMap() : tex_map(), fbo()
{
	fbo.init();
	fbo.bind();
	tex_map.init();
	tex_map.bind();
	tex_map.configure();
	fbo.unbind();

	std::vector<Vector2> quad_vts = {
		{-1.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f},
		{1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 1.0f}
	};

	vao.init();
	vao.bind();
	vbo.init();
	vbo.bind();
	vbo.attrib_buffer(0, 2);
	vbo.set_static_data(reinterpret_cast<const float*>(quad_vts.data()), quad_vts.size() * sizeof(Vector2));
	vao.unbind();

	ssbo.init();
	ssbo.bind();
	ssbo.bind_base(1);
	ssbo.unbind();

	tex_map.set_size(SIZE, SIZE);

	shader.init("quad_vertex_shader.glsl", "scpm_fragment_shader.glsl");
	l1_location = shader.get_uniform_location("l1");
	l2_location = shader.get_uniform_location("l2");
	obstacle_count_location = shader.get_uniform_location("obstacle_count");
}

void SimpleChainParameterSpaceMap::render(const SimpleChain& chain)
{
	std::vector<Obstacle> obstacles_v(chain.obstacles.begin(), chain.obstacles.end());

	GLint old_viewport[4];
	glGetIntegerv(GL_VIEWPORT, old_viewport);

	fbo.bind();
	glViewport(0, 0, SIZE, SIZE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ssbo.bind();
	ssbo.set_static_data(reinterpret_cast<const float*>(obstacles_v.data()), obstacles_v.size() * sizeof(Obstacle));

	shader.use();
	glUniform1f(l1_location, chain.l1);
	glUniform1f(l2_location, chain.l2);
	glUniform1i(obstacle_count_location, obstacles_v.size());

	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	vao.unbind();

	fbo.unbind();
	glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
}

struct MapNode {
	int x, y;
	int distance;
	enum class Move {
		None, Up, Down, Left, Right, Unreachable
	} move;
};

std::vector<Vector2> SimpleChainParameterSpaceMap::seek_path(const Vector2& from, const Vector2& to)
{
	std::vector<Vector4> map(SIZE * SIZE);
	std::vector<MapNode> moves(SIZE * SIZE, { 0,0,0,MapNode::Move::Unreachable });
	tex_map.get_image(SIZE, SIZE, map.data());

	auto from_deg = (180.0f / PI) * from,
		to_deg = (180.0f / PI) * to;

	int from_x = lroundf(from_deg.x), from_y = lroundf(from_deg.y),
		to_x = lroundf(to_deg.x), to_y = lroundf(to_deg.y);

	// flood fill
	std::queue<MapNode> queue;
	queue.push({ from_x, from_y, 0, MapNode::Move::None });
	while (!queue.empty())
	{
		const auto coords = queue.front();
		queue.pop();

		if (map[coords.x + coords.y * SIZE].w == 1.0f)
			continue;
		
		float coef = std::max(0.0f, 1.0f - coords.distance / 255.0f);
		map[coords.x + coords.y * SIZE] = { coef, coef, coef, 1.0f };
		moves[coords.x + coords.y * SIZE] = coords;

		int left_x = coords.x == 0 ? (SIZE - 1) : (coords.x - 1),
			right_x = coords.x == SIZE - 1 ? 0 : (coords.x + 1),
			up_y = coords.y == 0 ? (SIZE - 1) : (coords.y - 1),
			down_y = coords.y == SIZE - 1 ? 0 : (coords.y + 1);

		if (map[left_x + coords.y * SIZE].w < 1.0f)
			queue.push({ left_x, coords.y, coords.distance + 1, MapNode::Move::Left });
		if (map[right_x + coords.y * SIZE].w < 1.0f)
			queue.push({ right_x, coords.y, coords.distance + 1, MapNode::Move::Right });
		if (map[coords.x + up_y * SIZE].w < 1.0f)
			queue.push({ coords.x, up_y, coords.distance + 1, MapNode::Move::Up });
		if (map[coords.x + down_y * SIZE].w < 1.0f)
			queue.push({ coords.x, down_y, coords.distance + 1, MapNode::Move::Down });
	}

	// reconstruct the path
	auto move = moves[to_x + to_y * SIZE];
	move.x = to_x;
	move.y = to_y; // this is because flood fill may not fill these fields (when unreachable)
	std::vector<Vector2> path(move.distance + 1);
	int i = path.size() - 1;
	while (move.move != MapNode::Move::None)
	{
		Vector2 coords_deg = { move.x, move.y };
		path[i--] = (PI / 180.0f) * coords_deg;
		map[move.x + SIZE * move.y] = { 1.0f, 0.0f,0.0f,1.0f }; // draw a path

		int left_x = move.x == 0 ? (SIZE - 1) : (move.x - 1),
			right_x = move.x == SIZE - 1 ? 0 : (move.x + 1),
			up_y = move.y == 0 ? (SIZE - 1) : (move.y - 1),
			down_y = move.y == SIZE - 1 ? 0 : (move.y + 1);

		switch (move.move)
		{
		case MapNode::Move::Up:
			move = moves[move.x + SIZE * down_y];
			break;
		case MapNode::Move::Down:
			move = moves[move.x + SIZE * up_y];
			break;
		case MapNode::Move::Left:
			move = moves[right_x + SIZE * move.y];
			break;
		case MapNode::Move::Right:
			move = moves[left_x + SIZE * move.y];
			break;
		case MapNode::Move::Unreachable:
			if (moves[left_x + SIZE * move.y].move != MapNode::Move::Unreachable)
				move = moves[left_x + SIZE * move.y];
			else if (moves[right_x + SIZE * move.y].move != MapNode::Move::Unreachable)
				move = moves[right_x + SIZE * move.y];
			else if (moves[move.x + SIZE * up_y].move != MapNode::Move::Unreachable)
				move = moves[move.x + SIZE * up_y];
			else if (moves[move.x + SIZE * down_y].move != MapNode::Move::Unreachable)
				move = moves[move.x + SIZE * down_y];
			else
				return {};
			break;
		}
	} 

	assert(i == 0);

	tex_map.set_image(SIZE, SIZE, map.data());

	path.front() = from;
	path.back() = to;

	return path;
}
