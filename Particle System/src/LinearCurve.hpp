/*
 * LinearCurve.hpp
 *
 *  Created on: 5 maj 2014
 *      Author: Erik
 */

#ifndef LINEARCURVE_HPP_
#define LINEARCURVE_HPP_
#include "Curve.h"
namespace Curves {

/**
 * Class representing a linear curve on the form kx + m = y.
 */
template<typename type>
class LinearCurve : public Curve<type,type> {
	type m, k;
public:
	LinearCurve(type k, type m) {
		this->m = m;
		this->k = k;
	}

	type getValue(type v) override {
		return k * v + m;
	}
};

}

#endif /* LINEARCURVE_HPP_ */
