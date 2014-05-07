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
	boost::mt19937 RNG;
	boost::uniform_real<float> range;
	boost::variate_generator<boost::mt19937, boost::uniform_real<float>> randomG;
	//static bool started = false;

	Vector3f sphereMiddle, bias;
	float radius;
public:
	EmitterWithinSphere(Vector3f& middle, float radius) : RNG(time(NULL)), range(0,1), randomG(RNG,range) {
		Vector3f noBias {1,1,1};
		init(middle, radius, noBias);
	}

	EmitterWithinSphere(Vector3f& middle, float radius, Vector3f& bias) : RNG(time(NULL)), range(0,1), randomG(RNG,range) {
		init(middle,radius,bias);
	}

	void init(Vector3f& middle, float radius, Vector3f& bias) {
		sphereMiddle = middle;
		this->radius = radius;
		this->bias = bias;
		//if(!started) {
			//started = true;
//			RNG = boost::mt19937(time(NULL));
//			range = boost::uniform_real<float>(0,1);
//			randomG = boost::variate_generator<boost::mt19937, boost::uniform_real<float>>(RNG, range);
		//}
	}

	Vector3f emit() override {
		Vector3f res;
		for(int i = 0; i < 3; i++) res[i] = (randomG()-.5) * bias[i] * radius + sphereMiddle[i];
		return res;
	}
};



#endif /* EMITTERWITHINSPHERE_HPP_ */
