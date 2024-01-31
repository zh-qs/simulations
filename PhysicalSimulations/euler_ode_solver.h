#pragma once
#include "ode_solver.h"

template <class T, size_t DIM>
class EulerODESolver : public ODESolver<T, DIM> {
	using V = Vector<T, DIM>;
public:
	EulerODESolver(ODE<T, DIM>&& ode) : ODESolver<T, DIM>(std::move(ode)) {}

	V next(const T& h) override {
		if (this->first_time) {
			this->first_time = false;
			return this->current_value;
		}
		this->current_value += h * this->ode.evaluate(this->current_arg, this->current_value);
		this->current_arg += h;
		return this->current_value;
	}
};