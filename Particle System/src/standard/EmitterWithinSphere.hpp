/*
 * EmitterWithinSphere.hpp
 *
 *  Created on: 7 maj 2014
 *      Author: Erik
 */

#ifndef EMITTERWITHINSPHERE_HPP_
#define EMITTERWITHINSPHERE_HPP_

#include "../core/Emitter.h"
#include <Eigen/Eigen>
#include "StandardParticle.hpp"
#include <boost/random.hpp>
#include <time.h>

using namespace boost;

class EmitterWithinSphere : public Emitter<Vector3f> {
	static boost::mt19937 RNG;
	static boost::uniform_real<float> range;
	static boost::variate_generator<mt19937, uniform_real<float>> random;
	static bool started = false;

	Vector3f sphereMiddle, bias;
	float radius;
public:
	EmitterWithinSphere(Vector3f middle, float radius) { init(middle, radius, Vector3f{1,1,1}); }

	EmitterWithinSphere(Vector3f middle, float radius, Vector3f bias) {
		init(middle,radius,bias);
	}

	void init(Vector3f middle, float radius, Vector3f bias) {
		sphereMiddle = middle;
		this->radius = radius;
		this->bias = bias;
		if(!started) {
			started = true;
			RNG = boost::mt19937(time(NULL));
			range = boost::uniform_real<float>(0,1);
			random = boost::variate_generator<mt19937, uniform_real<float>>(RNG, range);
		}
	}

	Vector3f emit() override {
		Vector3f res;
		for(int i = 0; i < 3; i++) res[i] = random() * bias[i] * radius + sphereMiddle[i];
		return res;
	}
};



#endif /* EMITTERWITHINSPHERE_HPP_ */
