#pragma once

#include "algebra.h"
#include "imgui.h"
#include <cmath>

class Obstacle {
public:
	Vector2 first_corner;
	float __padding0[2];
	Vector2 second_corner;
	float __padding1[2];

	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };

	bool contains(const Vector2& point) const {
		return point.x >= std::min(first_corner.x, second_corner.x) && point.x <= std::max(first_corner.x, second_corner.x)
			&& point.y >= std::min(first_corner.y, second_corner.y) && point.y <= std::max(first_corner.y, second_corner.y);
	}

	bool intersects_segment(const Vector2& start, const Vector2& end) const {
		if (contains(start) || contains(end))
			return true;

		const auto x1 = std::min(first_corner.x, second_corner.x), x2 = std::max(first_corner.x, second_corner.x),
			y1 = std::min(first_corner.y, second_corner.y), y2 = std::max(first_corner.y, second_corner.y);

		if (start.x != end.x)
		{
			const auto t1 = (x1 - start.x) / (end.x - start.x),
				t2 = (x2 - start.x) / (end.x - start.x);
			if (t1 >= 0 && t1 <= 1)
			{
				const auto y = start.y + t1 * (end.y - start.y);
				if (y > y1 && y < y2)
					return true;
			}
			if (t2 >= 0 && t2 <= 1)
			{
				const auto y = start.y + t2 * (end.y - start.y);
				if (y > y1 && y < y2)
					return true;
			}
		}
		if (start.y != end.y)
		{
			const auto t1 = (y1 - start.y) / (end.y - start.y),
				t2 = (y2 - start.y) / (end.y - start.y);
			if (t1 >= 0 && t1 <= 1)
			{
				const auto x = start.x + t1 * (end.x - start.x);
				if (x > x1 && x < x2)
					return true;
			}
			if (t2 >= 0 && t2 <= 1)
			{
				const auto x = start.x + t2 * (end.x - start.x);
				if (x > x1 && x < x2)
					return true;
			}
		}
		return false;
	}
};

inline bool operator==(const Obstacle& o1, const Obstacle& o2) {
	return o1.first_corner == o2.first_corner && o1.second_corner == o2.second_corner;
}