#pragma once

#include <functional>
#include <utility>
#include "generic_vector.h"

template <class T, size_t DIM>
class ODE {
	using V = Vector<T, DIM>;
	using F = std::function<V(const T&, const V&)>;

	F f;
	V start_value;
	T start_argument;

public:
	ODE(const T& start_argument, const V& start_value, F&& f) : start_argument(start_argument), start_value(start_value), f(std::move(f)) {}

	const V& get_start_value() const { return start_value; }
	const T& get_start_argument() const { return start_argument; }

	V& get_start_value() { return start_value; }
	T& get_start_argument() { return start_argument; }

	Vector<T, DIM> evaluate(const T& arg, const V& v) const {
		return f(arg, v);
	}
};