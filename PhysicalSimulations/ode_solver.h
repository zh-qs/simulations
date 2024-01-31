#pragma once

#include "ode.h"

template <class T, size_t DIM>
class ODESolver {
protected:
	using V = Vector<T, DIM>;

	ODE<T, DIM> ode;
	T current_arg;
	V current_value;

	bool first_time = true;

public:
	ODESolver(ODE<T, DIM>&& ode) : ode(std::move(ode)) {
		current_arg = this->ode.get_start_argument();
		current_value = this->ode.get_start_value();
	}

	void reset() {
		current_arg = this->ode.get_start_argument();
		current_value = this->ode.get_start_value();
		first_time = true;
	}

	virtual V next(const T& h) = 0;

	const V& current() const {
		return current_value;
	}

	V& current() {
		return current_value;
	}

	V current_derivative() const {
		return ode.evaluate(current_arg, current_value);
	}

	const T& current_argument() const {
		return current_arg;
	}

	ODE<T, DIM>& get_ode() { return ode; }
};