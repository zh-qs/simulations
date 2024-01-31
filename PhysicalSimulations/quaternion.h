#pragma once

#include <cmath>
#include <type_traits>

template <class T>
struct Quaternion
{
	T w, x, y, z;

	T length() const { return std::sqrt(w * w + x * x + y * y + z * z); }
	Quaternion<T> conj() const { return { w,-x,-y,-z }; }
	void normalize() {
		const auto invl = 1 / length();
		w *= invl;
		x *= invl;
		y *= invl;
		z *= invl;
	}
	T* data() { return &w; }
};

static_assert(std::is_pod_v<Quaternion<float>>, "Quaternion needs to be POD, but it's not");

template <class T>
Quaternion<T> operator+(const Quaternion<T>& q1, const Quaternion<T>& q2) {
	return {
		q1.w + q2.w,
		q1.x + q2.x,
		q1.y + q2.y,
		q1.z + q2.z
	};
}

template <class T>
Quaternion<T> operator*(const Quaternion<T>& q1, const Quaternion<T>& q2) {
	return {
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
		q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w
	};
}

template <class T>
Quaternion<T> operator*(const T& a, const Quaternion<T>& q) {
	return {
		a * q.w,
		a * q.x,
		a * q.y,
		a * q.z
	};
}

template <class T>
Quaternion<T> operator-(const Quaternion<T>& q) {
	return {
		-q.w,
		-q.x,
		-q.y,
		-q.z
	};
}

template <class T>
inline T dot(const Quaternion<T>& q1, const Quaternion<T>& q2) {
	return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

// Shortest path
template <class T>
inline Quaternion<T> slerp(const Quaternion<T>& q1, const Quaternion<T>& q2, const T& t) {
	const T tt = t < 0 ? 0 : (t > 1 ? 1 : t);
	const auto cos = dot(q1, q2);
	const auto theta = std::acos(std::abs(cos));
	const auto sin = std::sin(theta);

	if (std::abs(sin) < 1e-09)
		return q1;

	const auto sint = std::sin(theta * tt) / sin, sinmt = std::sin(theta * (1 - tt)) / sin;

	if (cos < 0)
		return {
			-q1.w * sinmt + q2.w * sint,
			-q1.x * sinmt + q2.x * sint,
			-q1.y * sinmt + q2.y * sint,
			-q1.z * sinmt + q2.z * sint
		};

	return {
		q1.w * sinmt + q2.w * sint,
		q1.x * sinmt + q2.x * sint,
		q1.y * sinmt + q2.y * sint,
		q1.z * sinmt + q2.z * sint
	};
}