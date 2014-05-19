/*
 * SampleInitializers.hpp
 *
 *  Created on: 16 maj 2014
 *      Author: Erik
 */

#ifndef SAMPLEINITIALIZERS_HPP_
#define SAMPLEINITIALIZERS_HPP_

#include "../Particle.hpp"
#include "Initializers.hpp"
#include <glm/glm.hpp>

class FountainInitializer {
	RandomSpherePositionInitializer startSphere;
	VelocitySpreadInitializer vel;

public:
	FountainInitializer(vec3 initialPosition, vec3 fountainDirection, float minRadius,
			float maxRadius, float spreadAngle, float minSpeed, float maxSpeed)
			:
			startSphere(initialPosition, minRadius, maxRadius),
			vel(fountainDirection, spreadAngle, minSpeed, maxSpeed) {
	}

	inline void init(Particle* p) {
		startSphere.init(p);
		vel.init(p);
	}
};

class ExplosionInitializer {
	VelocityFromOriginInitializer vel;
	RandomSpherePositionInitializer sphere;
	OffsetInitializer offset;
public:
	ExplosionInitializer(vec3 pos, float startRadius, float minSpeed, float maxSpeed) :
	vel(minSpeed,maxSpeed),
	sphere(vec3(0,0,0), 0, startRadius),
	offset(pos) {

	}

	inline void init(Particle* p) {
		sphere.init(p);
		vel.init(p);
		offset.init(p);
	}
};



#endif /* SAMPLEINITIALIZERS_HPP_ */
