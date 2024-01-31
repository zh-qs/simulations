#pragma once

#include "ode.h"

template <class T, size_t DIM>
class StateListener {
public:
	virtual void notify(const T& arg, const Vector<T, DIM>& val) = 0;
	virtual void reset() = 0;
};