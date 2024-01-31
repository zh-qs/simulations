#pragma once
#include "ode_solver.h"
#include "quaternion.h"
#include <type_traits>

template <class T, size_t DIM>
class RungeKutta4ODESolver : public ODESolver<T, DIM> {
	using V = Vector<T, DIM>;
	static constexpr T INV6 = 1.0f / 6.0f;
public:
	RungeKutta4ODESolver(ODE<T, DIM>&& ode) : ODESolver<T, DIM>(std::move(ode)) {}

	V next(const T& h) override {
		if (this->first_time) {
			this->first_time = false;
			return this->current_value;
		}

		auto k1 = h * this->ode.evaluate(this->current_arg, this->current_value);
		auto k2 = h * this->ode.evaluate(this->current_arg + 0.5f * h, this->current_value + 0.5f * k1);
		auto k3 = h * this->ode.evaluate(this->current_arg + 0.5f * h, this->current_value + 0.5f * k2);
		auto k4 = h * this->ode.evaluate(this->current_arg + h, this->current_value + k3);

		this->current_value += INV6 * (k1 + 2.0f * k2 + 2.0f * k3 + k4);
		this->current_arg += h;
		return this->current_value;
	}

	template <typename = std::enable_if_t<DIM == 7>>
	void normalize_quaternion() {
		reinterpret_cast<Quaternion<float>*>(&this->current_value[3])->normalize();
	}
};