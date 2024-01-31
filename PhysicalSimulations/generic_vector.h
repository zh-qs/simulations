#pragma once

template <class T, size_t DIM>
struct Vector
{
private:
	T elem[DIM];
public:
	template <class... Ts>
	inline Vector(Ts... args) : elem{ args... } {}
	inline Vector() : elem{ 0 } {}
	inline T& operator[](const int& idx) { return elem[idx]; }
	inline const T& operator[](const int& idx) const { return elem[idx]; }

	Vector<T, DIM>& operator +=(const Vector<T, DIM>& v) {
		for (int i = 0; i < DIM; ++i)
			elem[i] += v[i];
		return *this;
	}

	Vector<T, DIM>& operator *=(const T& a) {
		for (int i = 0; i < DIM; ++i)
			elem[i] *= a;
		return *this;
	}
};

template <class T, size_t DIM>
Vector<T, DIM> operator+(const Vector<T, DIM>& v1, const Vector<T, DIM>& v2) {
	Vector<T, DIM> res;
	for (int i = 0; i < DIM; ++i)
		res[i] = v1[i] + v2[i];
	return res;
}

template <class T, size_t DIM>
Vector<T, DIM> operator*(const T& a, const Vector<T, DIM>& v) {
	Vector<T, DIM> res;
	for (int i = 0; i < DIM; ++i)
		res[i] = a * v[i];
	return res;
}

template <class T, size_t DIM>
Vector<T, DIM> operator/(const Vector<T, DIM>& v, const T& a) {
	Vector<T, DIM> res;
	for (int i = 0; i < DIM; ++i)
		res[i] = v[i] / a;
	return res;
}

template <class T>
Vector<T, 3> cross(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
	return { v1[1] * v2[2] - v1[2] * v2[1],v1[0] * v2[2] - v1[2] * v2[0],v1[0] * v2[1] - v1[1] * v2[0] };
}